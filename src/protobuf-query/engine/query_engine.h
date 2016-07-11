
#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <iostream>
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

struct Node;

// Function invoked when node is visited. Parent will be null and indent=0 for root.
using StartNodeFn = function<void(int indent, const Node& node, const Node* parent)>;
// Function invoked to mark node ending.
using EndNodeFn = function<void(int indent, const Node& node)>;

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
  // list of non-repeating read fields for this node.
  set<Field> selectFields;
  set<Field> nonSelectFields;

  // Node tree walk, modified DFS which vists each node twice,
  // one in depth first order, second in reverse order.
  static void walkNode(const Node& root,
                       const StartNodeFn& startNodeFn,
                       const EndNodeFn& endNodeFn,
                       const uint32_t indentInc = 2);
  static void walkNode(const Node* parent,
                       const Node& node,
                       int& indent,
                       const StartNodeFn& startNodeFn,
                       const uint32_t indentInc,
                       map<int, const Node*>& endNodesMap);
};

struct QueryGraph {
  Proto proto;
  Node root;
  // map of idx in query.selectFields => pointer in readFields
  map<size_t, Field> selectFieldIdxReadFieldMap;

  static string constructObjNameForRepeated(const FieldDescriptor* field);
  Field addReadIdentifier(const string& identifier, bool partOfSelect);
  void readFieldsFromSelect(const SelectStmt& selectStmt);
  void readFieldsFromWhere(const WhereStmt& whereStmt);
  void calculateGraph(const SelectQuery& query);
};

class QueryEngine {
public:
  QueryEngine(const string& rawSql, ostream& out);
  void process();

private:
  SelectQuery query;
  QueryGraph queryGraph;
  ostream& out;
  void printPlan();
  void printCode();
};
