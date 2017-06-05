
template <typename FieldT>
void Node<FieldT>::walkNode(
    Node<FieldT>* parent,
    Node<FieldT>& node,
    int& indent,
    StartNodeFn<FieldT>& startNodeFn,
    uint32_t indentInc,
    map<int, Node<FieldT>*>& endNodesMap) {
  startNodeFn(indent, node, parent);
  endNodesMap.emplace(-indent, &node);
  for (auto& e : node.children) {
    Node& child = e.second;
    indent += indentInc;
    walkNode(&node, child, indent, startNodeFn, indentInc, endNodesMap);
  }
}

template <typename FieldT>
void Node<FieldT>::walkNode(
    Node<FieldT>& root,
    StartNodeFn<FieldT>& startNodeFn,
    EndNodeFn<FieldT>& endNodeFn,
    uint32_t indentInc) {
  int indent = 0;
  map<int, Node<FieldT>*> endNodesMap;
  walkNode(nullptr, root, indent, startNodeFn, indentInc, endNodesMap);
  for (auto& e : endNodesMap) {
    int indent = -e.first;
    Node* node = e.second;
    endNodeFn(indent, *node);
  }
}

template <typename FieldT>
void QueryTree<FieldT>::process(const SelectQuery& query) {
  root.objName = getRootName();
  processSelect(query.selectStmt);
  processWhere(query.whereStmt);
  processOrderBy(query.orderByStmt);
}

template <typename FieldT>
void QueryTree<FieldT>::addReadIdentifier(const string& identifier) {
  if (idFieldMap.find(identifier) != idFieldMap.end()) {
    return;
  }
  FieldT field = newField();
  Node<FieldT>* parent = &root;
  vector<string> fieldParts = Utils::splitDotIdentifier(identifier);
  for (size_t j=0; j<fieldParts.size(); j++) {
    const string& fieldPart = fieldParts[j];
    field.addFieldPart(fieldPart);
    if (j != (fieldParts.size()-1)) {
      if (field.repeated()) {
        Node<FieldT>& child = parent->children[field];
        child.objName = Utils::makeSingular(fieldPart);
        child.repeatedField = field;
        parent = &child;
      }
    } else {
      if (field.repeated()) {
        Node<FieldT>& child = parent->children[field];
        child.objName = Utils::makeSingular(fieldPart);
        child.repeatedField = field;
        parent = &child;
        parent->allFields.emplace(field, true);
      } else {
        parent->allFields.emplace(field, false);
      }
      idFieldMap.emplace(identifier, field);
    }
  }
}

template <typename FieldT>
void QueryTree<FieldT>::processSelect(const SelectStmt& selectStmt) {
  for (const SelectField& selectField : selectStmt.getSelectFields()) {
    set<string> identifiers;
    selectField.getAllIdentifiers(identifiers);
    auto callback = [&selectField] (Node<FieldT>& node) {
      QueryTree<FieldT>::addExpr(
          node.selectAndOrderByExprs, &(selectField.getExpr()));
    };
    processExpr(identifiers, callback);
  }
}

template <typename FieldT>
void QueryTree<FieldT>::processWhere(const WhereStmt& whereStmt) {
  vector<const BooleanExpr*> andClauses;
  whereStmt.canoncialize(andClauses);
  for (const BooleanExpr* clause : andClauses) {
    set<string> identifiers;
    clause->getAllIdentifiers(identifiers);
    auto callback = [clause] (Node<FieldT>& node) {
      node.whereClauses.push_back(clause);
    };
    processExpr(identifiers, callback);
  }
}

template <typename FieldT>
void QueryTree<FieldT>::processOrderBy(const OrderByStmt& orderByStmt) {
  for (const OrderByField& orderByField : orderByStmt.getOrderByFields()) {
    set<string> identifiers;
    orderByField.getAllIdentifiers(identifiers);
    auto callback = [&orderByField] (Node<FieldT>& node) {
      QueryTree<FieldT>::addExpr(
          node.selectAndOrderByExprs, &(orderByField.getExpr()));
    };
    processExpr(identifiers, callback);
  }
}

template <typename FieldT>
void QueryTree<FieldT>::processExpr(
    const set<string>& identifiers,
    const function<void(Node<FieldT>& node)>& callback) {
  set<FieldT> fields;
  for (const string& identifier : identifiers) {
    addReadIdentifier(identifier);
    fields.insert(idFieldMap[identifier]);
  }
  StartNodeFn<FieldT> startNodeFn =
      [&](int, Node<FieldT>& node, Node<FieldT>*){
        if (fields.empty()) {
          return;
        }
        for(auto it = fields.begin(); it != fields.end();) {
          if (node.allFields.find(*it) != node.allFields.end()) {
            it = fields.erase(it);
          } else {
            ++it;
          }
        }
        if (fields.empty()) {
          callback(node);
        }
      };
  EndNodeFn<FieldT> endNodeFn = [](int, Node<FieldT>&) {};
  Node<FieldT>::walkNode(root, startNodeFn, endNodeFn);
}

template <typename FieldT>
void QueryTree<FieldT>::addExpr(vector<const Expr*>& exprs, const Expr* expr) {
  string exprStr = expr->str();
  auto f = std::find_if(exprs.begin(), exprs.end(),
      [&] (const Expr* expr) {return expr->str() == exprStr;});
  if (f == exprs.end()) {
    exprs.push_back(expr);
  }
}

template <typename FieldT>
void QueryTree<FieldT>::printPlan(const SelectQuery& query, ostream& out) {
  StartNodeFn<FieldT> startNodeFn =
      [this, &out] (int indent, const Node<FieldT>& node, const Node<FieldT>* parent) {
        if (!parent) { // root
          out << string(indent, ' ') << "with (" << root.objName
              << " = parseFromFile()) {" << endl;
        } else {
          out << string(indent, ' ') << "for each " << node.objName << " in "
              << parent->objName << "." << node.repeatedField.accessor()
              << " {" << endl;
        }
        indent+=2;
        for (const BooleanExpr* expr : node.whereClauses) {
          out << string(indent, ' ') << "if (!" << expr->str()
              << ") { continue; }" << endl;
        }
        for (const Expr* expr : node.selectAndOrderByExprs) {
          out << string(indent, ' ') << "tuples.add(" << expr->str() << ")" << endl;
        }
      };
  bool firstEnd = true;
  EndNodeFn<FieldT> endNodeFn =
      [this, &firstEnd, &out] (int indent, const Node<FieldT>& node) {
        if (firstEnd) {
          out << string(indent+2, ' ') << "tuples.record()" << endl;
          firstEnd = false;
        }
        out << string(indent, ' ') << "} //" << node.objName << endl;
      };
  Node<FieldT>::walkNode(root, startNodeFn, endNodeFn, 2);
  if (!query.orderByStmt.getOrderByFields().empty()) {
    out << "tuples.sortBy("
        << Utils::joinVec<OrderByField>(
            ", ", query.orderByStmt.getOrderByFields(),
            [](const OrderByField& orderByField) {
              return "'" + orderByField.getExpr().str() + "'";
            })
        << ")" << endl;
  }
  out << "tuples.print("
      << Utils::joinVec<SelectField>(
          ", ", query.selectStmt.getSelectFields(),
          [](const SelectField& selectField) {
            return "'" + selectField.getExpr().str() + "'";
          })
      << ")" << endl;
}
