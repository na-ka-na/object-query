
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
#include "select_query.h"

using namespace std;
using namespace google::protobuf;

template <typename T, typename It>
static string join(const string& delim, It begin, It end,
    const function<string(const T&)>& to_str) {
  string joined;
  if (begin != end) {
     joined += to_str(*begin);
     while ((++begin) != end) {
       joined += delim;
       joined += to_str(*begin);
     }
  }
  return joined;
}

template <typename T>
static string joinVec(const string& delim, const vector<T>& ts,
    const function<string(const T&)>& to_str) {
  return join<T>(delim, ts.cbegin(), ts.cend(), to_str);
}

const function<string(const string&)> string2str = [](const string& str) {return str;};

struct Proto {
  const Message* defaultInstance;
  const char* protoNamespace;
  const char* protoHeaderInclude;
};

void getProtoDetails(const string& protoName, Proto& proto) {
  if (protoName == "Example1.Company") {
    static Example1::Company defaultInstance;
    proto.defaultInstance = &defaultInstance;
    proto.protoNamespace = "Example1";
    proto.protoHeaderInclude = "example1.pb.h";
  } else {
    throw runtime_error("No mapping defined for protoName: " + protoName);
  }
}

struct Field {
  vector<const FieldDescriptor*> fieldParts;

  bool operator<(const Field& other) const {
    return fieldParts < other.fieldParts;
  }

  bool operator==(const Field& other) const {
    return fieldParts == other.fieldParts;
  }

  string type() const {
    if (fieldParts.empty()) {
      throw runtime_error("Can't determine type of empty field");
    }
    const FieldDescriptor* lastPart = fieldParts.back();
    if (lastPart->type() == FieldDescriptor::Type::TYPE_MESSAGE) {
      const Descriptor* msgType = lastPart->message_type();
      return msgType->name();
    } else {
      return lastPart->cpp_type_name();
    }
  }

  string accessor(const string& objName) const {
    string str = objName;
    for (const FieldDescriptor* part : fieldParts) {
      str += "." + part->name() + "()";
    }
    return str;
  }

  string hasAccessor(const string& objName, uint32_t end) const {
    string str = objName;
    for (uint32_t i=0; i<end; i++) {
      str += string(".") + ((i==(end-1)) ? "has_" : "") +
             fieldParts[i]->name() + "()";
    }
    return str;
  }

  string sizeAccessor(const string& objName) const {
    string str = objName;
    for (uint32_t i=0; i<fieldParts.size(); i++) {
      str += string(".") + fieldParts[i]->name() +
             ((i==(fieldParts.size()-1)) ? "_size" : "") + "()";
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
  Proto proto;
  Node root;
  // 1:1 with query.selects, order is maintained
  vector<Field> querySelects;
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
              const uint32_t indentInc,
              map<int, const Node*>& endFieldsMap) {
  selectFieldsFn(indent, node);
  for (const auto& e : node.children) {
    const Node& child = e.second;
    startForAllFn(indent, child, node);
    endFieldsMap.emplace(-indent, &child);
    indent += indentInc;
    walkNode(child, indent, selectFieldsFn, startForAllFn,
             indentInc, endFieldsMap);
  }
}

void walkNode(const Node& root,
              const SelectFieldsFn& selectFieldsFn,
              const StartForAllFn& startForAllFn,
              const EndForAllFn& endForAllFn,
              const uint32_t indentInc = 2) {
  int indent = 0;
  map<int, const Node*> endNodeMap;
  walkNode(root, indent, selectFieldsFn, startForAllFn,
           indentInc, endNodeMap);
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
        cout << string(indent, ' ') << "print " << field.accessor(node.objName) << endl;
      }
    }
  };
  StartForAllFn startForAllFn = [](int indent, const Node& node, const Node& parent) {
    cout << string(indent, ' ') << "for each " << node.objName << " in "
        << node.repeatedField.accessor(parent.objName) << " {" << endl;
  };
  EndForAllFn endForAllFn = [](int indent, const Node& node) {
    cout << string(indent, ' ') << "} //" << node.objName << endl;
  };
  walkNode(queryGraph.root, selectFieldsFn, startForAllFn, endForAllFn);
}

void printCode(const Query& query, const QueryGraph& queryGraph) {
  cout << "#include \"" << queryGraph.proto.protoHeaderInclude << "\"" << endl;
  cout << "#include \"generated_common.h\"" << endl << endl;
  cout << "using namespace std;" << endl;
  cout << "using namespace " << queryGraph.proto.protoNamespace << ";" << endl << endl;
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

  // select fields header
  if (queryGraph.querySelects.size() != query.selects.size()) {
    throw runtime_error(
        "queryGraph.querySelects.size()=" + to_string(queryGraph.querySelects.size()) +
        "query.selects.size()=" + to_string(query.selects.size()));
  }
  string header = "vector<string> header = {\n";
  map<uint32_t, uint32_t> querySelectsIdxToTupleIdxMap;
  for (size_t i=0; i<queryGraph.querySelects.size(); i++) {
    int idx = -1;
    for (size_t j=0; j<allSelectFields.size(); j++) {
      if (*allSelectFields[j] == queryGraph.querySelects[i]) {
        idx = j;
        break;
      }
    }
    if (idx == -1) {
      throw runtime_error(
          "Unable to find " + queryGraph.querySelects[i].accessor("") +
          " in tuples list");
    }
    querySelectsIdxToTupleIdxMap[i] = idx;
    header += "  \"" + joinVec(".", query.selects[i], string2str) + "\",\n";
  }
  header += "};";
  cout << header << endl;

  map<const Field*, string> selectFieldVarMap;
  map<const Field*, string> selectFieldTypeMap;
  map<const Field*, string> selectFieldDefaultMap;
  for (uint32_t i=0; i<allSelectFields.size(); i++) {
    const Field* field = allSelectFields[i];
    selectFieldVarMap[field] = "s" + to_string(i);
    selectFieldTypeMap[field] = "S" + to_string(i);
    selectFieldDefaultMap[field] = selectFieldTypeMap[field] + "()";
    string type = "optional<" + field->type() + ">";
    string spaces(((type.size() < 16) ? (16-type.size()) : 0), ' ');
    cout << "using " << selectFieldTypeMap[field] << " = " << type << ";"
        << spaces << " /*" << field->accessor("") << "*/" << endl;
  }
  string tupleType = "using TupleType = tuple<";
  tupleType += joinVec<const Field*>(
      ", ", allSelectFields,
      [&](const Field* field) {return selectFieldTypeMap[field];});
  tupleType += ">;";
  cout << tupleType << endl;
  cout << endl;

  cout << "void runSelect(const "
      << queryGraph.proto.defaultInstance->GetDescriptor()->name() << "& "
      << queryGraph.root.objName << ", vector<TupleType>& tuples) {" << endl;
  vector<const Field*> selectFieldsProcessed;
  selectFieldsFn = [&](int indent, const Node& node) {
    string ind = string(indent+2, ' ');
    if (node.type == REPEATED_PRIMITIVE) {
      const Field* field = &node.repeatedField;
      cout << ind << selectFieldTypeMap[field] << " "
          << selectFieldVarMap[field] << " = " << node.objName << ";" << endl;
      selectFieldsProcessed.push_back(field);
    } else {
      for (const Field& field : node.selectFields) {
        cout << ind << selectFieldTypeMap[&field] << " "
            << selectFieldVarMap[&field] << " = "
            << selectFieldDefaultMap[&field] << ";" << endl;
        vector<string> checks;
        for (uint32_t i=0; i<field.fieldParts.size(); i++) {
          checks.push_back(field.hasAccessor(node.objName, i+1));
        }
        cout << ind << "if(" << joinVec<string>(" && ", checks, string2str)
            << ") {" << endl;
        cout << ind << "  " << selectFieldVarMap[&field] << " = "
            << field.accessor(node.objName) << ";" << endl;
        cout << ind << "}" << endl;
        selectFieldsProcessed.push_back(&field);
      }
    }
    if (selectFieldsProcessed.size() == allSelectFields.size()) {
      string selectList = joinVec<const Field*>(", ", allSelectFields,
          [&](const Field* field) {return selectFieldVarMap[field];});
      cout << ind << "tuples.emplace_back(" + selectList + ");" << endl;
    }
  };
  startForAllFn = [](int indent, const Node& node, const Node& parent) {
    string ind = string(indent+2, ' ');
    cout << ind << "if (" << node.repeatedField.sizeAccessor(parent.objName)
        << " > 0) {" << endl;
    cout << ind << "  for (const " << node.repeatedField.type() << "& " <<
        node.objName << " : " << node.repeatedField.accessor(parent.objName) <<
        ") {" << endl;
  };
  set<const Field*> endedFieldSet;
  endForAllFn = [&](int indent, const Node& node) {
    string ind = string(indent+2, ' ');
    cout << ind << "  }" << endl;
    cout << ind << "} else { // no " << node.objName << endl;
    if (node.type == REPEATED_PRIMITIVE) {
      endedFieldSet.insert(&node.repeatedField);
    } else {
      for (const Field& field : node.selectFields) {
        endedFieldSet.insert(&field);
      }
    }
    string selectList = joinVec<const Field*>(
        ", ", selectFieldsProcessed,
        [&](const Field* field) {
            return endedFieldSet.find(field) == endedFieldSet.end() ?
                selectFieldVarMap[field] : selectFieldDefaultMap[field];
        });
    cout << ind << "  " << "tuples.emplace_back(" + selectList + ");" << endl;
    cout << ind << "}" << endl;
  };
  walkNode(queryGraph.root, selectFieldsFn, startForAllFn, endForAllFn, 4);
  cout << "}" << endl;

  // print tuples
  cout << R"fff(
void printTuples(const vector<TupleType>& tuples) {
  vector<size_t> sizes;
  for (size_t i=0; i<header.size(); i++) {
    sizes.push_back(header[i].size());
  }
)fff";
  cout << "  for (const TupleType& t : tuples) {" << endl;
  for (size_t i=0; i<query.selects.size(); i++) {
    cout << "    sizes[" << i << "] = max(sizes[" << i << "], stringify(get<"
        << querySelectsIdxToTupleIdxMap[i] << ">(t)).size());" << endl;
  }
  cout << "  }" << endl;
  cout << "  cout << left;";
  cout << R"fff(
  for (size_t i=0; i<header.size(); i++) {
    cout << ((i==0) ? "" : " | ") << setw(sizes[i]) << header[i];
  }
  cout << endl;
  for (size_t i=0; i<header.size(); i++) {
    cout << ((i==0) ? "" : " | ") << string(sizes[i], '-');
  }
  cout << endl;
)fff";
  cout << "  for(const TupleType& t : tuples) {" << endl;
  for (size_t i=0; i<query.selects.size(); i++) {
    cout << "    cout << " << ((i==0) ? "         " : "\" | \" << ")
        << "setw(sizes[" << i << "]) << "
        << "stringify(get<" << querySelectsIdxToTupleIdxMap[i]
        << ">(t));" << endl;
  }
  cout << "    cout << endl;" << endl;
  cout << "  }" << endl;
  cout << "}" << endl << endl;

  // main
  cout << "int main(int argc, char** argv) {" << endl;
  cout << "  " << queryGraph.proto.defaultInstance->GetDescriptor()->name()
      << " " << queryGraph.root.objName << ";" << endl;
  cout << "  parsePbFromFile(argv[1], " << queryGraph.root.objName << ");"
      << endl;
  cout << "  vector<TupleType> tuples;" << endl;
  cout << "  runSelect(company, tuples);" << endl;
  cout << "  printTuples(tuples);" << endl;
  cout << "}" << endl;
}

string constructObjNameForRepeated(const FieldDescriptor* field) {
  string fieldName = field->name();
  return (fieldName[fieldName.size()-1] == 's') ?
      fieldName.substr(0, fieldName.size()-1) : fieldName;
}

void calculateQueryGraph(const Query& query, QueryGraph& queryGraph) {
  getProtoDetails(query.fromRootProto, queryGraph.proto);
  const Descriptor* rootDescriptor =
      queryGraph.proto.defaultInstance->GetDescriptor();
  queryGraph.root.type = ROOT;
  queryGraph.root.objName = rootDescriptor->name();
  std::transform(queryGraph.root.objName.begin(), queryGraph.root.objName.end(),
                 queryGraph.root.objName.begin(), ::tolower);
  for (size_t i=0; i<query.selects.size(); i++) {
    const Descriptor* parentDescriptor = rootDescriptor;
    Node* parent = &(queryGraph.root);
    Field field;
    for (size_t j=0; j<query.selects[i].size(); j++) {
      const FieldDescriptor* fieldPart =
          parentDescriptor->FindFieldByName(query.selects[i][j]);
      if (fieldPart == nullptr) {
        throw runtime_error("No fieldPart by name " + query.selects[i][j]);
      }
      field.fieldParts.push_back(fieldPart);
      if (j != (query.selects[i].size()-1)) {
        if (fieldPart->type() != FieldDescriptor::Type::TYPE_MESSAGE) {
          throw runtime_error(
              "FieldPart " + query.selects[i][j] +
              " expected to be message but found" + fieldPart->type_name());
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
          throw runtime_error("FieldPart " + query.selects[i][j] +
                              " not expected to be message ");
        }
        if (fieldPart->label() == FieldDescriptor::LABEL_REPEATED) {
          Node& child = parent->children[field];
          child.type = REPEATED_PRIMITIVE;
          child.objName = constructObjNameForRepeated(fieldPart);
          child.repeatedField = field;
          queryGraph.querySelects.push_back(field);
        } else {
          parent->selectFields.push_back(field);
          queryGraph.querySelects.push_back(field);
        }
      }
    }
  }
}

void parseQuery(const string&, Query& query) {
  query.selects = {
      {"financial", "quarterly_profits"},
      {"financial", "quarterly_revenues"},
      {"all_employees", "id"},
      {"all_employees", "name"},
      {"all_employees", "active"},
      {"all_employees", "active_direct_reports"},
      {"founded"},
      {"board_of_directors"},
  };
  query.fromFile = "/tmp/example1.proto";
  query.fromRootProto = "Example1.Company";
  // TODO: do some validation on query
}

int main(int argc, char** argv) {
  if (argc < 2) {
    cerr << "Usage: ./QueryEngine <sql-query>" << endl;
    exit(1);
  }
  SelectQuery query(argv[1]);
  if (!query.parse()) {
    cerr << "parsing select query failed" << endl;
    exit(1);
  }

//  Query query;
//  parseQuery("", query);
//  QueryGraph queryGraph;
//  calculateQueryGraph(query, queryGraph);
//  cout << "/*" << endl;
//  printPlan(queryGraph);
//  cout << "*/" << endl;
//  printCode(query, queryGraph);
}

