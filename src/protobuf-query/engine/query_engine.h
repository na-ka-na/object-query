
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
  string protoNamespace;
  string protoHeaderInclude;
  string extraInclude;

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
using StartNodeFn = function<void(int indent, Node& node, Node* parent)>;
// Function invoked to mark node ending.
using EndNodeFn = function<void(int indent, Node& node)>;

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
  set<Field> allFields;
  // list of all canonical where clauses
  vector<const BooleanExpr*> whereClauses;
  // list of all select and order by exprs
  vector<const Expr*> selectAndOrderByExprs;

  // Node tree walk, modified DFS which vists each node twice,
  // one in depth first order, second in reverse order.
  static void walkNode(Node& root,
                       StartNodeFn& startNodeFn,
                       EndNodeFn& endNodeFn,
                       uint32_t indentInc = 2);
  static void walkNode(Node* parent,
                       Node& node,
                       int& indent,
                       StartNodeFn& startNodeFn,
                       uint32_t indentInc,
                       map<int, Node*>& endNodesMap);
};

struct QueryGraph {
  Proto proto;
  Node root;
  map<string, Field> idFieldMap;

  static string makePlural(const string& name);
  static string makeSingular(const string& name);
  void addReadIdentifier(const string& identifier);
  void processSelect(const SelectStmt& selectStmt);
  void processWhere(const WhereStmt& whereStmt);
  void processOrderBy(const OrderByStmt& orderByStmt);
  void processExpr(const set<string>& identifiers, function<void(Node& node)>);
  void calculateGraph(const SelectQuery& query);
  static void addExpr(vector<const Expr*>& exprs, const Expr* expr);
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
