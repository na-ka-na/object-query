
namespace oq {

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

template <typename FieldT>
void QueryTree<FieldT>::printCode(
    const SelectQuery& query, ostream& out, const CodeGenSpec& spec) {
  for (const string& headerInclude : spec.headerIncludes) {
    out << "#include \"" << headerInclude << "\"" << endl;
  }
  out << "#include \"" << generatedCommonHeader() << "\"" << endl << endl;
  out << "using namespace std;" << endl;
  out << "using namespace oq;" << endl;
  out << endl;

  // select fields header
  string header = "vector<string> header = {\n";
  header += Utils::joinVec<SelectField>(
      "\n", query.selectStmt.getSelectFields(),
      [](const SelectField& sf) {
        return "  \"" + sf.getHeader() + "\",";
      });
  header += "\n};";
  out << header << endl << endl;

  CodeGenReqs cgr;
  query.extractStatics(cgr);

  if (!cgr.constExprs.empty()) {
    for (const auto& e : cgr.constExprs) {
      auto expression = e.first;
      if (e.second.isRegex) {
        expression = "regex(" + expression + ", regex::optimize)";
      }
      out << e.second.varType << " " << e.second.varName << " = "
          << expression << ";" << endl;
    }
    out << endl;
  }

  if (!cgr.fnMap.empty()) {
    for (const auto& f : cgr.fnMap) {
      const string& fnname = f.first;
      const FnExpr& fnexpr = f.second;
      out << "template<";
      for (unsigned i=0; i<fnexpr.numParams; i++) {
        out << "typename Arg" << i << ", ";
      }
      out << "typename Ret=decltype(" << fnexpr.origFnName << "(";
      for (unsigned i=0; i<fnexpr.numParams; i++) {
        out << "Arg" << i << "()";
        if (i != (fnexpr.numParams-1)) out << ", ";
      }
      out << "))>\n";
      out << "optional<Ret> $" << fnname << "(";
      for (unsigned i=0; i<fnexpr.numParams; i++) {
        out << "const optional<Arg" << i << ">& arg" << i;
        if (i != (fnexpr.numParams-1)) out << ", ";
      }
      out << ") {\n";
      out << "  if (";
      for (unsigned i=0; i<fnexpr.numParams; i++) {
        out << "arg" << i;
        if (i != (fnexpr.numParams-1)) out << " && ";
      }
      out << ") {\n";
      out << "    return optional<Ret>(" << fnexpr.origFnName << "(";
      for (unsigned i=0; i<fnexpr.numParams; i++) {
        out << "*arg" << i;
        if (i != (fnexpr.numParams-1)) out << ", ";
      }
      out << "));\n";
      out << "  } else {\n";
      out << "    return optional<Ret>();\n";
      out << "  }\n";
      out << "}\n";
    }
    out << endl;
  }

  map<FieldT, string> fieldVarMap;
  map<FieldT, string> fieldTypeMap;
  map<FieldT, string> fieldDefaultMap;
  uint32_t varIdx = 0;
  for (const auto& e : idFieldMap) {
    const string& id = e.first;
    const FieldT& field = e.second;
    fieldVarMap[field] = "s" + to_string(varIdx);
    fieldTypeMap[field] = "S" + to_string(varIdx);
    fieldDefaultMap[field] = "S" + to_string(varIdx) + "()";
    varIdx++;
    string type = "optional<" + field.code_type() + ">";
    string spaces(((type.size() < 16) ? (16-type.size()) : 0), ' ');
    out << "using " << fieldTypeMap[field] << " = " << type << ";"
        << spaces << " /* " << field.accessor() << " */" << endl;
    cgr.idVarMap[id] = fieldVarMap[field];
    cgr.idDefaultMap[id] = fieldDefaultMap[field];
  }

  vector<const Expr*> selectAndOrderByExprs;
  for (const SelectField& selectField : query.selectStmt.getSelectFields()) {
    addExpr(selectAndOrderByExprs, &(selectField.getExpr()));
  }
  for (const OrderByField& orderByField : query.orderByStmt.getOrderByFields()) {
    addExpr(selectAndOrderByExprs, &(orderByField.getExpr()));
  }

  map<string, string> exprVarMap;
  map<string, string> exprTypeMap;
  map<string, string> exprDefaultMap;
  for (const Expr* expr : selectAndOrderByExprs) {
    string exprStr = expr->str();
    if (expr->isIdentifier()) {
      FieldT f = idFieldMap[expr->getIdentifier()];
      exprVarMap[exprStr] = fieldVarMap[f];
      exprTypeMap[exprStr] = fieldTypeMap[f];
      exprDefaultMap[exprStr] = fieldDefaultMap[f];
    } else {
      exprVarMap[exprStr] = "s" + to_string(varIdx);
      exprTypeMap[exprStr] = "S" + to_string(varIdx);
      exprDefaultMap[exprStr] = "S" + to_string(varIdx) + "()";
      varIdx++;
      string type = expr->cppType(cgr);
      string spaces(((type.size() < 16) ? (16-type.size()) : 0), ' ');
      out << "using " << exprTypeMap[exprStr] << " = " << type << ";"
          << spaces << " /* " << exprStr << " */" << endl;
    }
  }

  string tupleType = "using TupleType = tuple<";
  tupleType += Utils::joinVec<const Expr*>(
      ", ", selectAndOrderByExprs,
      [&](const Expr* expr) {return exprTypeMap[expr->str()];});
  tupleType += ">;";
  out << tupleType << endl;
  out << endl;

  out << "void runSelect(const vector<" << getRootType() << ">& "
      << Utils::makePlural(root.objName) << ", vector<TupleType>& tuples) {"
      << endl;
  int maxIndent = -1;
  StartNodeFn<FieldT> startNodeFn =
      [&](int indent, const Node<FieldT>& node, const Node<FieldT>* parent) {
        string ind = string(indent+2, ' ');
        if (!parent) { //root
          printRootForLoop(out, ind, node);
        } else {
          printNonRootForLoop(out, ind, node, *parent);
        }
        ind += "  ";
        for (const auto& f : node.allFields) {
          const FieldT& field = f.first;
          printFieldAssignment(
              out, ind, node, field, f.second /*repeating*/,
              fieldTypeMap[field], fieldVarMap[field],
              fieldDefaultMap[field]);
        }
        // TODO(sanchay): print variable assignment and where clauses in optimal order
        for (const BooleanExpr* whereClause : node.whereClauses) {
          out << ind << "if (!" << whereClause->code(cgr)
              << ") { continue; }" << endl;
        }
        for (const Expr* expr : node.selectAndOrderByExprs) {
          if (!expr->isIdentifier()) {
            out << ind << exprTypeMap[expr->str()] << " "
                << exprVarMap[expr->str()] << " = "
                << expr->code(cgr) << ";" << endl;
          }
        }
        maxIndent = max(maxIndent, indent);
      };
  EndNodeFn<FieldT> endNodeFn =
      [&](int indent, const Node<FieldT>&) {
        string ind = string(indent+2, ' ');
        if (indent == maxIndent) {
          string tuplesList = Utils::joinVec<const Expr*>(
              ", ", selectAndOrderByExprs,
              [&](const Expr* expr) {return exprVarMap[expr->str()];});
          out << ind << "  tuples.emplace_back(" + tuplesList + ");" << endl;
        }
        out << ind << "}" << endl;
      };
  Node<FieldT>::walkNode(root, startNodeFn, endNodeFn);
  out << "}" << endl;

  if (!query.orderByStmt.getOrderByFields().empty()) {
    out << endl;
    out << "bool compareTuples(const TupleType& t1, const TupleType& t2) {" << endl;
    out << "  int c;" << endl;
    for (const OrderByField& orderByField : query.orderByStmt.getOrderByFields()) {
      int idx = -1;
      string exprStr = orderByField.getExpr().str();
      for (size_t j=0; j<selectAndOrderByExprs.size(); j++) {
        if (exprStr == selectAndOrderByExprs[j]->str()) {
          idx = j;
          break;
        }
      }
      ASSERT(idx != -1);
      out << "  c = " << (orderByField.isDesc() ? "-" : "")
          << "Compare(get<" << idx << ">(t1), get<" << idx << ">(t2));" << endl;
      out << "  if (c < 0) {return true;} else if (c > 0) {return false;}" << endl;
    }
    out << "  return false;" << endl;
    out << "}" << endl;
  }

  out << R"fff(
void printTuples(const vector<TupleType>& tuples) {
  vector<size_t> sizes;
  for (size_t i=0; i<header.size(); i++) {
    sizes.push_back(header[i].size());
  }
)fff";
  out << "  for (const TupleType& t : tuples) {" << endl;
  for (size_t i=0; i<query.selectStmt.getSelectFields().size(); i++) {
    out << "    sizes[" << i << "] = max(sizes[" << i << "], PrintSize(get<"
        << i << ">(t)));" << endl;
  }
  out << "  }" << endl;
  out << "  cout << std::left;";
  out << R"fff(
  for (size_t i=0; i<header.size(); i++) {
    cout << ((i==0) ? "" : " | ") << setw(sizes[i]) << header[i];
  }
  cout << endl;
  for (size_t i=0; i<header.size(); i++) {
    cout << ((i==0) ? "" : " | ") << string(sizes[i], '-');
  }
  cout << endl;
)fff";
  out << "  for(const TupleType& t : tuples) {" << endl;
  for (size_t i=0; i<query.selectStmt.getSelectFields().size(); i++) {
    out << "    Print(cout << " << ((i==0) ? "         " : "\" | \" << ")
        << "setw(sizes[" << i << "]), " << "get<" << i << ">(t));" << endl;
  }
  out << "    cout << endl;" << endl;
  out << "  }" << endl;
  out << "}" << endl << endl;

  // main
  out << "int main(int argc, char** argv) {" << endl;
  string protosVecIden = Utils::makePlural(root.objName);
  out << "  vector<" << getRootType() << "> " << protosVecIden << ";" << endl;
  out << "  FROM(argc, argv, " << protosVecIden << ");" << endl;
  out << "  vector<TupleType> tuples;" << endl;
  out << "  runSelect(" << protosVecIden << ", tuples);" << endl;
  if (!query.orderByStmt.getOrderByFields().empty()) {
    out << "  std::sort(tuples.begin(), tuples.end(), compareTuples);" << endl;
  }
  out << "  printTuples(tuples);" << endl;
  out << "}" << endl;
}

} // namespace oq
