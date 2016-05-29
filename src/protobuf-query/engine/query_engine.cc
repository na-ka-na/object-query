
#include <fcntl.h>
#include <string>
#include <tuple>
#include <algorithm>
#include <map>
#include <iostream>
#include <functional>
#include <google/protobuf/message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/gzip_stream.h>
#include "example1.pb.h"

using namespace std;
using namespace google::protobuf;

template <typename T, typename It>
static string join(const string& delim, It begin, It end,
    const function<string(const T&)>& to_str) {
  string joined = "[";
  if (begin != end) {
     joined += to_str(*begin);
     while ((++begin) != end) {
       joined += delim;
       joined += to_str(*begin);
     }
  }
  return joined + "]";
}

template <typename T>
static string joinVec(const string& delim, const vector<T>& ts,
    const function<string(const T&)>& to_str) {
  return join<T>(delim, ts.cbegin(), ts.cend(), to_str);
}

const Message& getDefaultPbInstance(const string& className) {
  if (className == "Example1.Company") {
    static Example1::Company proto;
    return proto;
  }
  throw runtime_error("No mapping defined for className: " + className);
}

struct Field {
  vector<const FieldDescriptor*> fieldParts;

  bool operator<(const Field& other) const {
    return fieldParts < other.fieldParts;
  }

  string accessor() const {
    string str;
    for (const FieldDescriptor* part : fieldParts) {
      str += "." + part->name();
    }
    return str;
  }
};

enum NodeType {
  ROOT, REPEATED_MESSAGE, REPEATED_PRIMITIVE
};

// Root will be a node and all repeated fields will be nodes
struct Node {
  // only the ROOT node will be marked ROOT
  NodeType type;
  // this is the variable name in the generated code
  string objName;
  // this will be set for non root nodes
  Field repeatedField;
  // children of this node, repeated fields, repeated field => Node.
  map<Field, Node> children;
  // list of non-repeating select fields for this node.
  // Only populated for ROOT, REPEATED_MESSAGE
  vector<Field> selectFields;
};

using SelectFieldsFn = function<void(int indent, const Node& node)>;
using StartForAllFn = function<void(int indent, const Node& node, const Node& parent)>;
using EndForAllFn = function<void(int indent, const Node& node)>;

struct QueryGraph {
  Node root;
};

struct Query {
  vector<vector<string>> selects;
  string fromFile;
  string fromRootProto;
  vector<vector<string>> wheres;
  vector<vector<string>> groupbys;
  vector<vector<string>> havings;
  vector<vector<string>> orderbys;
};

void walkNode(const Node& node,
              int& indent,
              const SelectFieldsFn& selectFieldsFn,
              const StartForAllFn& startForAllFn,
              map<int, const Node*>& endFieldsMap) {
  selectFieldsFn(indent, node);
  for (const auto& e : node.children) {
    const Node& child = e.second;
    startForAllFn(indent, child, node);
    endFieldsMap.emplace(-indent, &child);
    indent += 2;
    walkNode(child, indent, selectFieldsFn, startForAllFn, endFieldsMap);
  }
}

void walkNode(const Node& root,
              const SelectFieldsFn& selectFieldsFn,
              const StartForAllFn& startForAllFn,
              const EndForAllFn& endForAllFn) {
  int indent = 0;
  map<int, const Node*> endNodeMap;
  walkNode(root, indent, selectFieldsFn, startForAllFn, endNodeMap);
  for (const auto& e : endNodeMap) {
    int indent = -e.first;
    const Node* node = e.second;
    endForAllFn(indent, *node);
  }
}

void printPlan(QueryGraph& queryGraph) {
  SelectFieldsFn selectFieldsFn = [](int indent, const Node& node) {
    if (node.type == REPEATED_PRIMITIVE) {
      cout << string(indent, ' ') << "print " << node.objName << endl;
    } else {
      for (const Field& field : node.selectFields) {
        cout << string(indent, ' ') << "print " << node.objName << field.accessor() << endl;
      }
    }
  };
  StartForAllFn startForAllFn = [](int indent, const Node& node, const Node& parent) {
    cout << string(indent, ' ') << "for each " << node.objName << " in "
        << parent.objName << node.repeatedField.accessor() << " {" << endl;
  };
  EndForAllFn endForAllFn = [](int indent, const Node& node) {
    cout << string(indent, ' ') << "} //" << node.repeatedField.accessor() << endl;
  };
  walkNode(queryGraph.root, selectFieldsFn, startForAllFn, endForAllFn);
}

void printCode(QueryGraph& queryGraph) {
  cout << "--------------------------------------" << endl;
  SelectFieldsFn selectFieldsFn = [](int indent, const Node& node) {};
  StartForAllFn startForAllFn = [](int indent, const Node& node, const Node& parent) {};
  EndForAllFn endForAllFn = [](int indent, const Node& node) {};

  vector<const Field*> allSelectFields;
  selectFieldsFn = [&](int indent, const Node& node) {
    if (node.type == REPEATED_PRIMITIVE) {
      allSelectFields.push_back(&node.repeatedField);
    } else {
      for (const Field& field : node.selectFields) {
        allSelectFields.push_back(&field);
      }
    }
  };
  walkNode(queryGraph.root, selectFieldsFn, startForAllFn, endForAllFn);

  string tupleType = "using TupleType = tuple<";
  tupleType += joinVec<const Field*>(", ", allSelectFields, [](const Field* field) {
    return field->fieldParts.back()->cpp_type_name();
  });
  tupleType += ">;";
  cout << tupleType << endl;
}

string constructObjNameForRepeated(const FieldDescriptor* field) {
  string fieldName = field->name();
  return (fieldName[fieldName.size()-1] == 's') ?
      fieldName.substr(0, fieldName.size()-1) : fieldName;
}

void calculateQueryGraph(const Query& query, QueryGraph& queryGraph) {
  const Message& rootProto = getDefaultPbInstance(query.fromRootProto);
  const Descriptor* rootDescriptor = rootProto.GetDescriptor();
  queryGraph.root.type = ROOT;
  queryGraph.root.objName = rootDescriptor->name();
  std::transform(queryGraph.root.objName.begin(), queryGraph.root.objName.end(),
                 queryGraph.root.objName.begin(), ::tolower);
  for (const vector<string>& select : query.selects) {
    const Descriptor* parentDescriptor = rootDescriptor;
    Node* parent = &(queryGraph.root);
    Field field;
    for (size_t i=0; i<select.size(); i++) {
      const FieldDescriptor* fieldPart =
          parentDescriptor->FindFieldByName(select[i]);
      if (fieldPart == nullptr) {
        throw runtime_error("No fieldPart by name " + select[i]);
      }
      field.fieldParts.push_back(fieldPart);
      if (i != (select.size()-1)) {
        if (fieldPart->type() != FieldDescriptor::Type::TYPE_MESSAGE) {
          throw runtime_error(
              "FieldPart " + select[i] + " expected to be message but found" +
              fieldPart->type_name());
        }
        if (fieldPart->label() == FieldDescriptor::LABEL_REPEATED) {
          Node& child = parent->children[field];
          child.type = REPEATED_MESSAGE;
          child.objName = constructObjNameForRepeated(fieldPart);
          child.repeatedField = field;
          parent = &child;
          field.fieldParts.clear();
        }
        parentDescriptor = fieldPart->message_type();
      } else {
        if (fieldPart->type() == FieldDescriptor::Type::TYPE_MESSAGE) {
          throw runtime_error(
              "FieldPart " + select[i] + " not expected to be message ");
        }
        if (fieldPart->label() == FieldDescriptor::LABEL_REPEATED) {
          Node& child = parent->children[field];
          child.type = REPEATED_PRIMITIVE;
          child.objName = constructObjNameForRepeated(fieldPart);
          child.repeatedField = field;
        } else {
          parent->selectFields.push_back(field);
        }
      }
    }
  }
}

void parseQuery(const string&, Query& query) {
  query.selects = {
      {"founded"},
      {"board_of_directors"},
      {"financial", "quarterly_profits"},
      {"financial", "quarterly_revenues"},
      {"all_employees", "id"},
      {"all_employees", "name"},
      {"all_employees", "active"},
      {"all_employees", "active_direct_reports"},
  };
  query.fromFile = "/tmp/example1.proto";
  query.fromRootProto = "Example1.Company";
  // TODO: do some validation on query
}

int main(int /*argc*/, char** /*argv*/) {
  Query query;
  parseQuery("", query);
  QueryGraph queryGraph;
  calculateQueryGraph(query, queryGraph);
  printPlan(queryGraph);
  printCode(queryGraph);
}

