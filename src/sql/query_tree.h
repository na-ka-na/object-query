/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

#pragma once

#include <functional>
#include <map>
#include <string>
#include <type_traits>
#include "select_query.h"

using namespace std;

namespace oq {

enum NodeType {
  ROOT, REPEATED_MESSAGE, REPEATED_LEAF
};

template <typename FieldT>
struct Node;

// Function invoked when node is visited. Parent will be null and indent=0 for root.
template <typename FieldT>
using StartNodeFn = function<void(int indent, Node<FieldT>& node, Node<FieldT>* parent)>;

// Function invoked to mark node ending.
template <typename FieldT>
using EndNodeFn = function<void(int indent, Node<FieldT>& node)>;

// Root will be a node and all repeated fields will be nodes
template <typename FieldT>
struct Node {
  // this is the variable name in the generated code
  string objName;
  // this will be set for non root nodes
  FieldT repeatedField;
  // children of this node, repeated fields, repeated field => Node.
  map<FieldT, Node> children;
  // list of all (repeating + non-repeating) read fields for this node.
  map<FieldT, bool> allFields; // value is if the field is repeating
  // list of all canonical where clauses
  vector<const BooleanExpr*> whereClauses;
  // list of all select and order by exprs
  vector<const Expr*> selectAndOrderByExprs;

  // Node tree walk, modified DFS which vists each node twice,
  // one in depth first order, second in reverse order.
  static void walkNode(Node<FieldT>& root,
                       StartNodeFn<FieldT>& startNodeFn,
                       EndNodeFn<FieldT>& endNodeFn,
                       uint32_t indentInc = 2);

  static void walkNode(Node<FieldT>* parent,
                       Node<FieldT>& node,
                       int& indent,
                       StartNodeFn<FieldT>& startNodeFn,
                       uint32_t indentInc,
                       map<int, Node<FieldT>*>& endNodesMap);
};

// Fields should also implement ==, <
class Field {
public:
  virtual ~Field() {};
  virtual void addFieldPart(const string&) = 0;
  virtual bool repeated() const = 0;
  virtual string code_type() const = 0;
  virtual string accessor() const = 0;
};

template <typename FieldT>
struct QueryTree {
  static_assert(std::is_base_of<Field, FieldT>::value,
                "FieldT must inherit from Field");

  QueryTree() {};
  virtual ~QueryTree() {};

  Node<FieldT> root;
  map<string, FieldT> idFieldMap;

  virtual string getRootName() const = 0;
  virtual string getRootType() const = 0;
  virtual FieldT newField() const = 0;

  virtual string generatedCommonHeader() const = 0;
  virtual void printRootForLoop(ostream& out, const string& ind,
                                const Node<FieldT>& node) const = 0;
  virtual void printNonRootForLoop(ostream& out, const string& ind,
                                   const Node<FieldT>& node,
                                   const Node<FieldT>& parent) const = 0;
  virtual void printFieldAssignment(ostream& out, const string& ind,
                                    const Node<FieldT>& node,
                                    const FieldT& field,
                                    bool repeating,
                                    const string& fieldType,
                                    const string& fieldVar,
                                    const string& fieldDefault) const = 0;

  void process(const SelectQuery& query);
  void printPlan(const SelectQuery& query, ostream& out);
  void printCode(const SelectQuery& query, ostream& out,
                 const CodeGenSpec& spec);

  void addReadIdentifier(const string& identifier);
  void processSelect(const SelectStmt& selectStmt);
  void processWhere(const WhereStmt& whereStmt);
  void processOrderBy(const OrderByStmt& orderByStmt);
  void processExpr(const set<string>& identifiers,
                   const function<void(Node<FieldT>& node)>& callback);
  static void addExpr(vector<const Expr*>& exprs, const Expr* expr);
};

} // namespace oq

#include "query_tree.hpp"
