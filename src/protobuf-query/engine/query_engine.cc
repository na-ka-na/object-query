
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

const Message& getDefaultPbInstance(const string& className) {
  if (className == "Example1.Aaa") {
    static Example1::Aaa proto;
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

struct Node {
  string objName;
  map<Field, Node> children; // repeated fields
  vector<Field> selectFields;
};

using SelectFieldsFn = function<void(int indent, const string& objName, vector<Field> selectFields)>;
using StartForAllFn = function<void(int indent, const string& name, const string& objName, const Field& repeatedField)>;
using EndForAllFn = function<void(int indent, const Field& repeatedField)>;

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

void walkNode(const Node& root,
              int& indent,
              const SelectFieldsFn& selectFieldsFn,
              const StartForAllFn& startForAllFn,
              map<int, Field>& endFieldsMap) {
  selectFieldsFn(indent, root.objName, root.selectFields);
  for (const auto& e : root.children) {
    const Field& repeatedField = e.first;
    const Node& childNode = e.second;
    startForAllFn(indent, childNode.objName, root.objName, repeatedField);
    endFieldsMap.emplace(-indent, repeatedField);
    indent += 2;
    walkNode(childNode, indent, selectFieldsFn, startForAllFn, endFieldsMap);
  }
}

void walkNode(const Node& root,
              const SelectFieldsFn& selectFieldsFn,
              const StartForAllFn& startForAllFn,
              const EndForAllFn& endForAllFn) {
  int indent = 0;
  map<int, Field> endFieldsMap;
  walkNode(root, indent, selectFieldsFn, startForAllFn, endFieldsMap);
  for (const auto& e : endFieldsMap) {
    int indent = -e.first;
    const Field& repeatedField = e.second;
    endForAllFn(indent, repeatedField);
  }
}

void printPlan(QueryGraph& queryGraph) {
  SelectFieldsFn selectFieldsFn = [](int indent, const string& objName, vector<Field> selectFields) {
    for (const Field& field : selectFields) {
      cout << string(indent, ' ') << "print " << objName << field.accessor() << endl;
    }
  };
  StartForAllFn startForAllFn = [](int indent, const string& name, const string& objName, const Field& repeatedField) {
    cout << string(indent, ' ') << "for all " << name << " in " << objName << repeatedField.accessor()
        << " {" << endl;
  };
  EndForAllFn endForAllFn = [](int indent, const Field& repeatedField) {
    cout << string(indent, ' ') << "} //" << repeatedField.accessor()
        << endl;
  };
  walkNode(queryGraph.root, selectFieldsFn, startForAllFn, endForAllFn);
}

void calculateQueryGraph(const Query& query, QueryGraph& queryGraph) {
  queryGraph.root.objName = "root";
  const Message& rootProto = getDefaultPbInstance(query.fromRootProto);
  for (const vector<string>& select : query.selects) {
    const Descriptor* parentDescriptor = rootProto.GetDescriptor();
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
          child.objName = fieldPart->name();
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
          child.objName = fieldPart->name();
          parent = &child;
          field.fieldParts.clear();
        } else {
          parent->selectFields.push_back(field);
          field.fieldParts.clear();
        }
      }
    }
  }
}

void parseQuery(const string&, Query& query) {
  /**
   * SELECT a, aa, b.b, b.bb, c.c, c.cc
   * FROM ($file, 'Example1.Aaa')
   */
  query.selects = {
      {"a"},
      {"aa"},
      {"b", "b"},
      {"b", "bb"},
      {"c", "c"},
      {"c", "cc"},
  };
  query.fromFile = "/tmp/example1.proto";
  query.fromRootProto = "Example1.Aaa";
  // TODO: do some validation on query
}

int main(int /*argc*/, char** /*argv*/) {
  Query query;
  parseQuery("", query);
  QueryGraph queryGraph;
  calculateQueryGraph(query, queryGraph);
  printPlan(queryGraph);
}

