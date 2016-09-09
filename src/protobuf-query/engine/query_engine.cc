
#include <regex>
#include <algorithm>
#include "utils.h"
#include "query_engine.h"

using namespace std;
using namespace google::protobuf;

void FieldPart::parseFrom(const Descriptor& parentDescriptor,
                          const string& partName) {
  descriptor = parentDescriptor.FindFieldByName(partName);
  if (descriptor != nullptr) {
    part_type = NORMAL;
    return;
  }
  static std::regex size_regex("^(.+)_size$");
  std::smatch size_match;
  if (std::regex_match(partName, size_match, size_regex)) {
    std::ssub_match base_match = size_match[1];
    string base_part = base_match.str();
    descriptor = parentDescriptor.FindFieldByName(base_part);
    if (descriptor != nullptr) {
      part_type = SIZE;
      return;
    }
  }
  static std::regex has_regex("^has_(.+)$");
  std::smatch has_match;
  if (std::regex_match(partName, has_match, has_regex)) {
    std::ssub_match base_match = has_match[1];
    string base_part = base_match.str();
    descriptor = parentDescriptor.FindFieldByName(base_part);
    if (descriptor != nullptr) {
      part_type = HAS;
      return;
    }
  }
  THROW("No fieldPart by name", partName);
}

bool FieldPart::operator<(const FieldPart& other) const {
  if (part_type < other.part_type) {
    return true;
  } else if (part_type == other.part_type) {
    return descriptor < other.descriptor;
  } else {
    return false;
  }
}

bool FieldPart::operator==(const FieldPart& other) const {
  return (part_type == other.part_type) && (descriptor == other.descriptor);
}

string FieldPart::name() const {
  switch (part_type) {
  case NORMAL : return descriptor->name();
  case SIZE : return descriptor->name() + "_size";
  case HAS : return "has_" + descriptor->name();
  default : THROW("Unhandled type");
  }
}

FieldDescriptor::Type FieldPart::type() const {
  switch (part_type) {
  case NORMAL : return descriptor->type();
  case SIZE : return FieldDescriptor::Type::TYPE_INT32;
  case HAS : return FieldDescriptor::Type::TYPE_BOOL;
  default : THROW("Unhandled type");
  }
}

string FieldPart::type_name() const {
  return FieldDescriptor::TypeName(type());
}

FieldDescriptor::CppType FieldPart::cpp_type() const {
  return FieldDescriptor::TypeToCppType(type());
}

string FieldPart::cpp_type_name() const {
  return FieldDescriptor::CppTypeName(cpp_type());
}

bool FieldPart::is_message() const {
  return type() == FieldDescriptor::Type::TYPE_MESSAGE;
}

const Descriptor* FieldPart::message_descriptor() const {
  ASSERT(is_message());
  return descriptor->message_type();
}

bool FieldPart::is_repeated() const {
  return (part_type == NORMAL) &&
      (descriptor->label() == FieldDescriptor::LABEL_REPEATED);
}

bool FieldPart::is_enum() const {
  return type() == FieldDescriptor::Type::TYPE_ENUM;
}

const EnumDescriptor* FieldPart::enum_descriptor() const {
  ASSERT(is_enum());
  return descriptor->enum_type();
}


string FieldPart::code_type() const {
  if (is_message()) {
    return message_descriptor()->name();
  } else if (is_enum()) {
    return "string";
  } else {
    return cpp_type_name();
  }
}

string FieldPart::accessor() const {
  switch (part_type) {
  case NORMAL : return descriptor->name() + "()";
  case SIZE : return descriptor->name() + "_size()";
  case HAS : return "has_" + descriptor->name() + "()";
  default : THROW("Unhandled type");
  }
}

bool FieldPart::needs_has_check() const {
  return part_type == NORMAL;
}

string FieldPart::has_accessor() const {
  ASSERT(needs_has_check());
  return "has_" + descriptor->name() + "()";
}

bool Field::operator<(const Field& other) const {
  return fieldParts < other.fieldParts;
}

bool Field::operator==(const Field& other) const {
  return fieldParts == other.fieldParts;
}

string Field::code_type() const {
  ASSERT(!fieldParts.empty(), "Can't determine type of empty field");
  return fieldParts.back().code_type();
}

string Field::accessor(const string& objName) const {
  string str = objName;
  str += joinVec<FieldPart>(".", fieldParts,
      [] (const FieldPart& part) {return part.accessor();});
  if (!fieldParts.empty() && fieldParts.back().is_enum()) {
    str = fieldParts.back().enum_descriptor()->name() + "_Name(" + str + ")";
  }
  return str;
}

string Field::has_check(const string& objName) const {
  vector<string> checks;
  for (uint32_t i=0; i<fieldParts.size(); i++) {
    if (!fieldParts[i].needs_has_check()) {
      continue;
    }
    string check = objName;
    for (uint32_t j=0; j<i; j++) {
      if (j != 0) check += ".";
      check += fieldParts[j].accessor();
    }
    if (i != 0) check += ".";
    check += fieldParts[i].has_accessor();
    checks.push_back(check);
  }
  return joinVec(" && ", checks, string2str);
}

void Node::walkNode(Node* parent,
                    Node& node,
                    int& indent,
                    StartNodeFn& startNodeFn,
                    uint32_t indentInc,
                    map<int, Node*>& endNodesMap) {
  startNodeFn(indent, node, parent);
  endNodesMap.emplace(-indent, &node);
  for (auto& e : node.children) {
    Node& child = e.second;
    indent += indentInc;
    walkNode(&node, child, indent, startNodeFn, indentInc, endNodesMap);
  }
}

void Node::walkNode(Node& root,
                    StartNodeFn& startNodeFn,
                    EndNodeFn& endNodeFn,
                    uint32_t indentInc) {
  int indent = 0;
  map<int, Node*> endNodesMap;
  walkNode(nullptr, root, indent, startNodeFn, indentInc, endNodesMap);
  for (auto& e : endNodesMap) {
    int indent = -e.first;
    Node* node = e.second;
    endNodeFn(indent, *node);
  }
}

string QueryGraph::makePlural(const string& name) {
  ASSERT(!name.empty());
  return name + "s";
}

string QueryGraph::makeSingular(const string& name) {
  ASSERT(!name.empty());
  if ((name.size() > 1) && (name.substr(name.size()-1, 1) == "s")) {
    return name.substr(0, name.size()-1);
  } else {
    return "each_" + name;
  }
}

void QueryGraph::addReadIdentifier(const string& identifier) {
  if (idFieldMap.find(identifier) != idFieldMap.end()) {
    return;
  }
  const Descriptor* parentDescriptor = proto.protoDescriptor;
  Node* parent = &root;
  Field field;
  vector<string> selectFieldParts = splitString(identifier, '.');
  for (size_t j=0; j<selectFieldParts.size(); j++) {
    FieldPart fieldPart;
    fieldPart.parseFrom(*parentDescriptor, selectFieldParts[j]);
    field.fieldParts.push_back(fieldPart);
    if (j != (selectFieldParts.size()-1)) {
      ASSERT(fieldPart.is_message(),
             "FieldPart", selectFieldParts[j], "expected to be message but is",
             fieldPart.type_name());
      if (fieldPart.is_repeated()) {
        Node& child = parent->children[field];
        child.type = REPEATED_MESSAGE;
        child.objName = makeSingular(fieldPart.name());
        child.repeatedField = field;
        parent = &child;
        field.fieldParts.clear();
      }
      parentDescriptor = fieldPart.message_descriptor();
    } else {
      if (fieldPart.is_repeated()) {
        Node& child = parent->children[field];
        child.type = fieldPart.is_message() ? REPEATED_MESSAGE : REPEATED_PRIMITIVE;
        child.objName = makeSingular(fieldPart.name());
        child.repeatedField = field;
        parent = &child;
      }
      parent->allFields.insert(field);
    }
  }
  idFieldMap.emplace(identifier, field);
}

void QueryGraph::processSelect(const SelectStmt& selectStmt) {
  for (const SelectField& selectField : selectStmt.selectFields) {
    set<string> identifiers;
    selectField.expr.getAllIdentifiers(identifiers);
    auto callback = [&selectField](Node& node) {
      QueryGraph::addExpr(node.selectAndOrderByExprs, &(selectField.expr));
    };
    processExpr(identifiers, callback);
  }
}

void QueryGraph::processWhere(const WhereStmt& whereStmt) {
  vector<const BooleanExpr*> andClauses;
  whereStmt.canoncialize(andClauses);
  for (const BooleanExpr* clause : andClauses) {
    set<string> identifiers;
    clause->getAllIdentifiers(identifiers);
    auto callback = [clause](Node& node) {
      node.whereClauses.push_back(clause);
    };
    processExpr(identifiers, callback);
  }
}

void QueryGraph::processOrderBy(const OrderByStmt& orderByStmt) {
  for (const OrderByField& orderByField : orderByStmt.orderByFields) {
    set<string> identifiers;
    orderByField.expr.getAllIdentifiers(identifiers);
    auto callback = [&orderByField](Node& node) {
      QueryGraph::addExpr(node.selectAndOrderByExprs, &(orderByField.expr));
    };
    processExpr(identifiers, callback);
  }
}

void QueryGraph::processExpr(
    const set<string>& identifiers, function<void(Node& node)> callback) {
  set<Field> fields;
  for (const string& identifier : identifiers) {
    addReadIdentifier(identifier);
    fields.insert(idFieldMap[identifier]);
  }
  StartNodeFn startNodeFn = [&](int indent, Node& node, Node* parent){
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
  EndNodeFn endNodeFn = [](int indent, Node& node) {};
  Node::walkNode(root, startNodeFn, endNodeFn);
}

void QueryGraph::calculateGraph(const SelectQuery& query) {
  Proto::initProto(query.fromStmt.fromRootProto, proto);
  const Descriptor* rootDescriptor = proto.protoDescriptor;
  root.type = ROOT;
  root.objName = rootDescriptor->name();
  std::transform(root.objName.begin(), root.objName.end(),
                 root.objName.begin(), ::tolower);
  processSelect(query.selectStmt);
  processWhere(query.whereStmt);
  processOrderBy(query.orderByStmt);
}

void QueryGraph::addExpr(vector<const Expr*>& exprs, const Expr* expr) {
  string exprStr = expr->str();
  auto f = std::find_if(exprs.begin(), exprs.end(),
      [&] (const Expr* expr) {return expr->str() == exprStr;});
  if (f == exprs.end()) {
    exprs.push_back(expr);
  }
}

QueryEngine::QueryEngine(const string& rawSql, ostream& out) :
    query(SelectQuery(rawSql)), out(out) {}

void QueryEngine::printPlan() {
  StartNodeFn startNodeFn = [this](int indent, const Node& node, const Node* parent) {
    if (!parent) { //root
      out << string(indent, ' ') << "with (" << queryGraph.root.objName
          << " = parseFromFile()) {" << endl;
    } else {
      out << string(indent, ' ') << "for each " << node.objName << " in "
          << node.repeatedField.accessor(parent->objName + ".") << " {" << endl;
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
  EndNodeFn endNodeFn = [this, &firstEnd](int indent, const Node& node) {
    if (firstEnd) {
      out << string(indent+2, ' ') << "tuples.record()" << endl;
      firstEnd = false;
    }
    out << string(indent, ' ') << "} //" << node.objName << endl;
  };
  Node::walkNode(queryGraph.root, startNodeFn, endNodeFn, 2);
  if (!query.orderByStmt.orderByFields.empty()) {
    out << "tuples.sortBy("
        << joinVec<OrderByField>(
            ", ", query.orderByStmt.orderByFields,
            [](const OrderByField& orderByField) {
              return "'" + orderByField.expr.str() + "'";
            })
        << ")" << endl;
  }
  out << "tuples.print("
      << joinVec<SelectField>(
          ", ", query.selectStmt.selectFields,
          [](const SelectField& selectField) {
            return "'" + selectField.expr.str() + "'";
          })
      << ")" << endl;
}

void QueryEngine::printCode() {
  out << "#include \"" << queryGraph.proto.protoHeaderInclude << "\"" << endl;
  if (!queryGraph.proto.extraInclude.empty()) {
    out << "#include \"" << queryGraph.proto.extraInclude << "\"" << endl;
  }
  out << "#include \"generated_common.h\"" << endl << endl;
  out << "using namespace std;" << endl;
  out << "using namespace " << queryGraph.proto.protoNamespace << ";" << endl << endl;

  // select fields header
  string header = "vector<string> header = {\n";
  header += joinVec<SelectField>(
      "\n", query.selectStmt.selectFields,
      [](const SelectField& sf) {
        return "  \"" + (sf.alias.empty() ? sf.str() : sf.alias) + "\",";
      });
  header += "\n};";
  out << header << endl;

  CodeGenReqs cgr;
  query.whereStmt.extractStatics(cgr);

  map<Field, string> fieldVarMap;
  map<Field, string> fieldTypeMap;
  map<Field, string> fieldDefaultMap;
  uint32_t varIdx = 0;
  for (const auto& e : queryGraph.idFieldMap) {
    const string& id = e.first;
    const Field& field = e.second;
    fieldVarMap[field] = "s" + to_string(varIdx);
    fieldTypeMap[field] = "S" + to_string(varIdx);
    fieldDefaultMap[field] = "S" + to_string(varIdx) + "()";
    varIdx++;
    string type = "optional<" + field.code_type() + ">";
    string spaces(((type.size() < 16) ? (16-type.size()) : 0), ' ');
    out << "using " << fieldTypeMap[field] << " = " << type << ";"
        << spaces << " /*" << field.accessor(".") << "*/" << endl;
    cgr.idVarMap[id] = fieldVarMap[field];
    cgr.idDefaultMap[id] = fieldDefaultMap[field];
  }

  vector<const Expr*> selectAndOrderByExprs;
  for (const SelectField& selectField : query.selectStmt.selectFields) {
    QueryGraph::addExpr(selectAndOrderByExprs, &(selectField.expr));
  }
  for (const OrderByField& orderByField : query.orderByStmt.orderByFields) {
    QueryGraph::addExpr(selectAndOrderByExprs, &(orderByField.expr));
  }

  map<string, string> exprVarMap;
  map<string, string> exprTypeMap;
  map<string, string> exprDefaultMap;
  for (const Expr* expr : selectAndOrderByExprs) {
    string exprStr = expr->str();
    if (expr->type == IDENTIFIER) {
      Field f = queryGraph.idFieldMap[expr->identifier];
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
          << spaces << " /*" << exprStr << "*/" << endl;
    }
  }

  string tupleType = "using TupleType = tuple<";
  tupleType += joinVec<const Expr*>(
      ", ", selectAndOrderByExprs,
      [&](const Expr* expr) {return exprTypeMap[expr->str()];});
  tupleType += ">;";
  out << tupleType << endl;
  out << endl;

  if (!cgr.regexMap.empty()) {
    for (auto& e : cgr.regexMap) {
      string regexVar = "r" + to_string(varIdx);
      varIdx++;
      out << "std::regex " << regexVar << "(\"" << e.first << "\", "
          << "std::regex::optimize);" << endl;
      e.second = regexVar;
    }
    out << endl;
  }

  out << "void runSelect(const vector<"
      << queryGraph.proto.protoDescriptor->name() << ">& "
      << QueryGraph::makePlural(queryGraph.root.objName)
      << ", vector<TupleType>& tuples) {" << endl;
  unsigned numSelectAndOrderByFieldsProcessed = 0;
  bool allSelectAndOrderByFieldsProcessed = false;
  StartNodeFn startNodeFn = [&](int indent, const Node& node, const Node* parent) {
    string ind = string(indent+2, ' ');
    if (!parent) { //root
      out << ind << "for (const "
          << queryGraph.proto.protoDescriptor->name()
          << "* " << node.objName << " : Iterators::mk_iterator(&"
          << QueryGraph::makePlural(queryGraph.root.objName) << ")) {" << endl;
    } else {
      out << ind << "for (const " << node.repeatedField.code_type() << "* "
          << node.objName << " : Iterators::mk_iterator(" << parent->objName
          << " ? &" << node.repeatedField.accessor(parent->objName + "->")
          << " : nullptr)) {" << endl;
    }
    ind += "  ";
    for (const Field& field : node.allFields) {
      out << ind << fieldTypeMap[field] << " " << fieldVarMap[field] << " = "
          << fieldDefaultMap[field] << ";" << endl;
      if (node.type == REPEATED_PRIMITIVE) {
        out << ind << "if (" << node.objName << ") {" << endl;
        out << ind << "  " << fieldVarMap[field] << " = *"
            << node.objName << ";" << endl;
        out << ind << "}" << endl;
      } else {
        string checks = field.has_check(node.objName + "->");
        out << ind << "if (" << node.objName
            << (checks.empty() ? "" : " && " + checks) << ") {" << endl;
        out << ind << "  " << fieldVarMap[field] << " = "
            << field.accessor(node.objName + "->") << ";" << endl;
        out << ind << "}" << endl;
      }
    }
    // TODO(sanchay): print variable assignment and where clauses in optimal order
    for (const BooleanExpr* whereClause : node.whereClauses) {
      out << ind << "if (!" << whereClause->code(cgr)
          << ") { continue; }" << endl;
    }
    for (const Expr* expr : node.selectAndOrderByExprs) {
      if (expr->type != IDENTIFIER) {
        out << ind << exprTypeMap[expr->str()] << " "
            << exprVarMap[expr->str()] << " = "
            << expr->code(cgr) << ";" << endl;
      }
    }
    numSelectAndOrderByFieldsProcessed += node.selectAndOrderByExprs.size();
    if (!allSelectAndOrderByFieldsProcessed &&
        (numSelectAndOrderByFieldsProcessed == selectAndOrderByExprs.size())) {
      string tuplesList = joinVec<const Expr*>(
          ", ", selectAndOrderByExprs,
          [&](const Expr* expr) {return exprVarMap[expr->str()];});
      out << ind << "tuples.emplace_back(" + tuplesList + ");" << endl;
      allSelectAndOrderByFieldsProcessed = true;
    }
  };
  EndNodeFn endNodeFn = [&](int indent, const Node& node) {
    string ind = string(indent+2, ' ');
    out << ind << "}" << endl;
  };
  Node::walkNode(queryGraph.root, startNodeFn, endNodeFn);
  out << "}" << endl;

  if (!query.orderByStmt.orderByFields.empty()) {
    out << endl;
    out << "bool compareTuples(const TupleType& t1, const TupleType& t2) {" << endl;
    out << "  int c;" << endl;
    for (const OrderByField& orderByField : query.orderByStmt.orderByFields) {
      int idx = -1;
      string exprStr = orderByField.expr.str();
      for (size_t j=0; j<selectAndOrderByExprs.size(); j++) {
        if (exprStr == selectAndOrderByExprs[j]->str()) {
          idx = j;
          break;
        }
      }
      ASSERT(idx != -1);
      out << "  c = " << (orderByField.desc ? "-" : "")
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
  for (size_t i=0; i<query.selectStmt.selectFields.size(); i++) {
    out << "    sizes[" << i << "] = max(sizes[" << i << "], Stringify(get<"
        << i << ">(t)).size());" << endl;
  }
  out << "  }" << endl;
  out << "  cout << left;";
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
  for (size_t i=0; i<query.selectStmt.selectFields.size(); i++) {
    out << "    cout << " << ((i==0) ? "         " : "\" | \" << ")
        << "setw(sizes[" << i << "]) << "
        << "Stringify(get<" << i << ">(t));" << endl;
  }
  out << "    cout << endl;" << endl;
  out << "  }" << endl;
  out << "}" << endl << endl;

  // main
  out << "int main(int argc, char** argv) {" << endl;
  out << "  " << queryGraph.proto.protoDescriptor->name()
      << " " << queryGraph.root.objName << ";" << endl;
  string fromFile = (query.fromStmt.fromFile.find("argv") == 0) ?
      query.fromStmt.fromFile : ("\"" + query.fromStmt.fromFile + "\"");
  out << "  ParsePbFromFile(" << fromFile << ", " << queryGraph.root.objName
      << ");" << endl;
  out << "  vector<TupleType> tuples;" << endl;
  out << "  runSelect({" << queryGraph.root.objName << "}, tuples);" << endl;
  if (!query.orderByStmt.orderByFields.empty()) {
    out << "  std::sort(tuples.begin(), tuples.end(), compareTuples);" << endl;
  }
  out << "  printTuples(tuples);" << endl;
  out << "}" << endl;
}

void QueryEngine::process() {
  ASSERT(query.parse(), "Parsing select query failed");
  out << "/*" << endl;
  out << query.str() << endl << endl;
  query.preProcess();
  queryGraph.calculateGraph(query);
  printPlan();
  out << "*/" << endl;
  printCode();
}
