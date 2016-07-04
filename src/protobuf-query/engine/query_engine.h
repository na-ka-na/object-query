
#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <ostream>
#include <google/protobuf/message.h>
#include "select_query.h"

using namespace std;
using namespace google::protobuf;

struct Proto {
  const Message* defaultInstance;
  const char* protoNamespace;
  const char* protoHeaderInclude;

  static void initProto(const string& protoName, Proto& proto);
};

struct Field {
  vector<const FieldDescriptor*> fieldParts;

  bool operator<(const Field& other) const;
  bool operator==(const Field& other) const;
  string type() const;
  string accessor(const string& objName) const;
  string hasAccessor(const string& objName, uint32_t end) const;
  string sizeAccessor(const string& objName) const;
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
  vector<Field> selectFields;
};

class QueryEngine {
public:
  QueryEngine(const string& rawSql, ostream& out);
  void process();

private:
  SelectQuery query;
  QueryGraph queryGraph;
  ostream& out;

  static string constructObjNameForRepeated(const FieldDescriptor* field);
  void calculateQueryGraph();
  void walkNode(const Node& node,
                int& indent,
                const SelectFieldsFn& selectFieldsFn,
                const StartForAllFn& startForAllFn,
                const uint32_t indentInc,
                map<int, const Node*>& endFieldsMap);
  void walkNode(const Node& root,
                const SelectFieldsFn& selectFieldsFn,
                const StartForAllFn& startForAllFn,
                const EndForAllFn& endForAllFn,
                const uint32_t indentInc = 2);
  void printPlan();
  void printCode();
};
