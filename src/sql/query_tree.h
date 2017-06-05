/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

#include <functional>
#include <map>
#include <string>
#include "select_parts.h"

using namespace std;

enum NodeType {
  ROOT, REPEATED_MESSAGE, REPEATED_LEAF
};

template <typename Field>
struct Node;

// Function invoked when node is visited. Parent will be null and indent=0 for root.
template <typename Field>
using StartNodeFn = function<void(int indent, Node<Field>& node, Node<Field>* parent)>;

// Function invoked to mark node ending.
template <typename Field>
using EndNodeFn = function<void(int indent, Node<Field>& node)>;

// Root will be a node and all repeated fields will be nodes
template <typename Field>
struct Node {
  // this is the variable name in the generated code
  string objName;
  // this will be set for non root nodes
  Field repeatedField;
  // children of this node, repeated fields, repeated field => Node.
  map<Field, Node> children;
  // list of all (repeating + non-repeating) read fields for this node.
  map<Field, bool> allFields; // value is if the field is repeating
  // list of all canonical where clauses
  vector<const BooleanExpr*> whereClauses;
  // list of all select and order by exprs
  vector<const Expr*> selectAndOrderByExprs;

  // Node tree walk, modified DFS which vists each node twice,
  // one in depth first order, second in reverse order.
  static void walkNode(Node<Field>& root,
                       StartNodeFn<Field>& startNodeFn,
                       EndNodeFn<Field>& endNodeFn,
                       uint32_t indentInc = 2);

  static void walkNode(Node<Field>* parent,
                       Node<Field>& node,
                       int& indent,
                       StartNodeFn<Field>& startNodeFn,
                       uint32_t indentInc,
                       map<int, Node<Field>*>& endNodesMap);
};

#include "query_tree.hpp"
