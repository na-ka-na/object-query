
#include "select_query.h"
#include "utils.h"

using namespace std;

yy::SqlParser::symbol_type yylex(SelectQuery& query) {
  return yylex_flex(query, query.yyscanner);
}

bool SelectQuery::parse() {
  if (yylex_init(&yyscanner) != 0) {
    cerr << "yylex_init failed with errno=" << errno << endl;
    return false;
  }
  YY_BUFFER_STATE buffer = yy_scan_string(rawSql.c_str(), yyscanner);

//  yy::SqlParser::token_type token;
//  do {
//    yy::SqlParser::symbol_type symbol = yylex_flex(*this, yyscanner);
//    token = symbol.token();
//    cout << "TOKEN: " << token << " location: " << symbol.location << endl;
//  } while (token != yy::SqlParser::token_type::TOK_END);

  yy::SqlParser parser(*this);
  int res = parser.parse();

  yy_delete_buffer(buffer, yyscanner);
  if (yylex_destroy(yyscanner) != 0) {
    cerr << "yylex_destroy failed with errno=" << errno << endl;
    return false;
  }
  return res==0;
}

void SelectQuery::mark_lexer_invalid_char(char c) {
  cerr << "parse error at " << loc << ": ignoring invalid char '"
       << c << "'" << endl;
}

void SelectQuery::mark_parse_error(
    const yy::SqlParser::location_type& loc,
    const std::string& msg) {
  cerr << "parse error at " << loc << ": " << msg << endl;
}

Expr Expr::create(BinaryExprOp op, const Expr& lhs, const Expr& rhs) {
  Expr expr;
  expr.type = BINARY_EXPR;
  expr.binaryExpr.op = op;
  expr.binaryExpr.lhs.reset(new Expr());
  expr.binaryExpr.rhs.reset(new Expr());
  *(expr.binaryExpr.lhs) = lhs;
  *(expr.binaryExpr.rhs) = rhs;
  return expr;
}

Expr Expr::create(UnaryExprOp op, const Expr& uexpr) {
  Expr expr;
  expr.type = UNARY_EXPR;
  expr.unaryExpr.op = op;
  expr.unaryExpr.expr.reset(new Expr());
  *(expr.unaryExpr.expr) = uexpr;
  return expr;
}

Expr Expr::create(Fn1 fn1, const Expr& fexpr) {
  Expr expr;
  expr.type = FN1_CALL_EXPR;
  expr.fn1CallExpr.fn1 = fn1;
  expr.fn1CallExpr.expr.reset(new Expr());
  *(expr.fn1CallExpr.expr) = fexpr;
  return expr;
}

Expr Expr::create(Fn3 fn3, const Expr& expr1, const Expr& expr2,
                  const Expr& expr3) {
  Expr expr;
  expr.type = FN3_CALL_EXPR;
  expr.fn3CallExpr.fn3 = fn3;
  expr.fn3CallExpr.expr1.reset(new Expr());
  expr.fn3CallExpr.expr2.reset(new Expr());
  expr.fn3CallExpr.expr3.reset(new Expr());
  *(expr.fn3CallExpr.expr1) = expr1;
  *(expr.fn3CallExpr.expr2) = expr2;
  *(expr.fn3CallExpr.expr3) = expr3;
  return expr;
}

Expr Expr::createIdentifier(const string& identifier) {
  Expr expr;
  expr.type = IDENTIFIER;
  expr.identifier = identifier;
  return expr;
}

Expr Expr::createPrimitive(const string& value) {
  Expr expr;
  expr.type = STRING;
  expr.stringValue = value;
  return expr;
}

Expr Expr::createPrimitive(long value) {
  Expr expr;
  expr.type = LONG;
  expr.longValue = value;
  return expr;
}

Expr Expr::createPrimitive(double value) {
  Expr expr;
  expr.type = DOUBLE;
  expr.doubleValue = value;
  return expr;
}

Expr Expr::createPrimitive(bool value) {
  Expr expr;
  expr.type = BOOL;
  expr.boolValue = value;
  return expr;
}

BooleanExpr BooleanExpr::create(
    CompoundBooleanOp op, const BooleanExpr& lhs, const BooleanExpr& rhs) {
  BooleanExpr bexpr;
  bexpr.type = BOOLEAN;
  bexpr.compoundBooleanExpr.op = op;
  bexpr.compoundBooleanExpr.lhs.reset(new BooleanExpr());
  bexpr.compoundBooleanExpr.rhs.reset(new BooleanExpr());
  *(bexpr.compoundBooleanExpr.lhs) = lhs;
  *(bexpr.compoundBooleanExpr.rhs) = rhs;
  return bexpr;
}

BooleanExpr BooleanExpr::create(
    SimpleBooleanOp op, const Expr& lhs, const Expr& rhs) {
  BooleanExpr bexpr;
  bexpr.type = SIMPLE;
  bexpr.simpleBooleanExpr.op = op;
  bexpr.simpleBooleanExpr.lhs = lhs;
  bexpr.simpleBooleanExpr.rhs = rhs;
  return bexpr;
}

BooleanExpr BooleanExpr::createNullary(bool isNull, const string& identifier) {
  BooleanExpr bexpr;
  bexpr.type = NULLARY;
  bexpr.nullaryBooleanExpr.isNull = isNull;
  bexpr.nullaryBooleanExpr.identifier = identifier;
  return bexpr;
}

void BinaryExpr::getAllIdentifiers(set<string>& identifiers) const {
  lhs->getAllIdentifiers(identifiers);
  rhs->getAllIdentifiers(identifiers);
}

void UnaryExpr::getAllIdentifiers(set<string>& identifiers) const {
  expr->getAllIdentifiers(identifiers);
}

void Fn1CallExpr::getAllIdentifiers(set<string>& identifiers) const {
  expr->getAllIdentifiers(identifiers);
}

void Fn3CallExpr::getAllIdentifiers(set<string>& identifiers) const {
  expr1->getAllIdentifiers(identifiers);
  expr2->getAllIdentifiers(identifiers);
  expr3->getAllIdentifiers(identifiers);
}

void Expr::getAllIdentifiers(set<string>& identifiers) const {
  switch (type) {
  case BINARY_EXPR: binaryExpr.getAllIdentifiers(identifiers); break;
  case UNARY_EXPR: unaryExpr.getAllIdentifiers(identifiers); break;
  case FN1_CALL_EXPR: fn1CallExpr.getAllIdentifiers(identifiers); break;
  case FN3_CALL_EXPR: fn3CallExpr.getAllIdentifiers(identifiers); break;
  case IDENTIFIER: identifiers.insert(identifier); break;
  case STRING: break;
  case LONG: break;
  case DOUBLE: break;
  case BOOL: break;
  default: THROW("Unhandled Expr case");
  }
}

void CompoundBooleanExpr::getAllIdentifiers(set<string>& identifiers) const {
  lhs->getAllIdentifiers(identifiers);
  rhs->getAllIdentifiers(identifiers);
}

void SimpleBooleanExpr::getAllIdentifiers(set<string>& identifiers) const {
  lhs.getAllIdentifiers(identifiers);
  rhs.getAllIdentifiers(identifiers);
}

void BooleanExpr::getAllIdentifiers(set<string>& identifiers) const {
  switch (type) {
  case BOOLEAN: compoundBooleanExpr.getAllIdentifiers(identifiers); break;
  case SIMPLE: simpleBooleanExpr.getAllIdentifiers(identifiers); break;
  case NULLARY: nullaryBooleanExpr.getAllIdentifiers(identifiers); break;
  default: THROW("Unhandled BooleanExpr case");
  }
}

void NullaryBooleanExpr::getAllIdentifiers(set<string>& identifiers) const {
  identifiers.insert(identifier);
}

void SelectField::getAllIdentifiers(set<string>& identifiers) const {
  expr.getAllIdentifiers(identifiers);
}

void OrderByField::getAllIdentifiers(set<string>& identifiers) const {
  expr.getAllIdentifiers(identifiers);
}

void WhereStmt::getAllIdentifiers(set<string>& identifiers) const {
  if (booleanExpr) {
    booleanExpr->getAllIdentifiers(identifiers);
  }
}

void BooleanExpr::canoncialize(vector<const BooleanExpr*>& andClauses) const {
  if ((type == BOOLEAN) && (compoundBooleanExpr.op == AND)) {
    compoundBooleanExpr.lhs->canoncialize(andClauses);
    compoundBooleanExpr.rhs->canoncialize(andClauses);
  } else {
    andClauses.push_back(this);
  }
}

void WhereStmt::canoncialize(vector<const BooleanExpr*>& andClauses) const {
  if (booleanExpr) {
    booleanExpr->canoncialize(andClauses);
  }
}

string BinaryExpr::str() const {
  string opStr;
  switch (op) {
  case PLUS:   opStr = "+"; break;
  case MINUS:  opStr = "-"; break;
  case MULT:   opStr = "*"; break;
  case DIVIDE: opStr = "/"; break;
  default:     opStr = "<BinaryExpOp>";
  }
  return "(" + lhs->str() + opStr + rhs->str() + ")";
}

string UnaryExpr::str() const {
  string prefix;
  switch (op) {
  case UMINUS: prefix = "-"; break;
  default:     prefix = "<UnaryExpOp>";
  }
  return prefix + expr->str();
}

string Fn1CallExpr::str() const {
  string fnStr;
  switch (fn1) {
  case STR:   fnStr = "STR"; break;
  case INT:   fnStr = "INT"; break;
  case SUM:   fnStr = "SUM"; break;
  case COUNT: fnStr = "COUNT"; break;
  default:    fnStr = "<fn1>";
  }
  return fnStr + "(" + expr->str() + ")";
}

string Fn3CallExpr::str() const {
  string fnStr;
  switch (fn3) {
  case SUBSTR: fnStr = "SUBSTR"; break;
  default:     fnStr = "<fn3>";
  }
  return fnStr + "(" + expr1->str() + "," + expr2->str() + "," +
         expr3->str() + ")";
}

string Expr::str() const {
  switch (type) {
  case BINARY_EXPR:   return binaryExpr.str();
  case UNARY_EXPR:    return unaryExpr.str();
  case FN1_CALL_EXPR: return fn1CallExpr.str();
  case FN3_CALL_EXPR: return fn3CallExpr.str();
  case IDENTIFIER:    return identifier;
  case STRING:        return "'"+stringValue+"'";
  case LONG:          return to_string(longValue);
  case DOUBLE:        return to_string(doubleValue);
  case BOOL:          return boolValue ? "TRUE" : "FALSE";
  default:            return "<Expr>";
  }
}

string CompoundBooleanExpr::str() const {
  string opStr;
  switch (op) {
  case AND: opStr = "AND"; break;
  case OR:  opStr = "OR"; break;
  default:  opStr = "<CompoundBooleanExprOp>";
  }
  return "(" + lhs->str() + " " + opStr + " " + rhs->str() + ")";
}

string SimpleBooleanExpr::str() const {
  string opStr;
  switch (op) {
  case EQ:   opStr = "="; break;
  case NE:   opStr = "!="; break;
  case LT:   opStr = "<"; break;
  case GT:   opStr = ">"; break;
  case LE:   opStr = "<="; break;
  case GE:   opStr = ">="; break;
  case LIKE: opStr = "LIKE"; break;
  default:   opStr = "<SimpleBooleanExprOp>";
  }
  return "(" + lhs.str() + " " + opStr + " " + rhs.str() + ")";
}

string NullaryBooleanExpr::str() const {
  Expr expr;
  expr.type = IDENTIFIER;
  expr.identifier = identifier;
  return expr.str() + " IS" + (isNull ? "" : " NOT") + " NULL";
}

string BooleanExpr::str() const {
  switch (type) {
  case BOOLEAN: return compoundBooleanExpr.str();
  case SIMPLE:  return simpleBooleanExpr.str();
  case NULLARY: return nullaryBooleanExpr.str();
  default:      return "<BooleanExpr>";
  }
}

string SelectField::str() const {
  return expr.str() + (alias.empty() ? "" : (" AS " + alias));
}

string SelectStmt::str() const {
  return "SELECT " + string(distinct ? "DISTINCT " : "") +
         joinVec(", ", selectFields, strfn<SelectField>());
}

string OrderByField::str() const {
  return expr.str() + (desc ? " DESC" : "");
}

string OrderByStmt::str() const {
  return orderByFields.empty() ? "" :
      ("ORDER BY " + joinVec(", ", orderByFields, strfn<OrderByField>()));
}

string FromStmt::str() const {
  return "FROM ('" + fromFile + "', '" + fromRootProto + "')";
}

string WhereStmt::str() const {
  return booleanExpr ? ("WHERE " + booleanExpr->str()) : "";
}

string SelectQuery::str() const {
  string selectStr = selectStmt.str();
  string fromStr = fromStmt.str();
  string whereStr = whereStmt.str();
  string orderByStr = orderByStmt.str();
  return selectStr + " " + fromStr +
         (whereStr.empty() ? "" : (" " + whereStr)) +
         (orderByStr.empty() ? "" : (" " + orderByStr));
}

void CompoundBooleanExpr::extractStatics(CodeGenReqs& cgr) const {
  lhs->extractStatics(cgr);
  rhs->extractStatics(cgr);
}

void SimpleBooleanExpr::extractStatics(CodeGenReqs& cgr) const {
  if ((op == LIKE) && (rhs.type == STRING)) {
    cgr.regexMap.emplace(rhs.stringValue, "");
  }
}

void BooleanExpr::extractStatics(CodeGenReqs& cgr) const {
  switch (type) {
  case BOOLEAN: compoundBooleanExpr.extractStatics(cgr); break;
  case SIMPLE: simpleBooleanExpr.extractStatics(cgr); break;
  case NULLARY: break;
  default: THROW("Unhandled BooleanExpr case");
  }
}

void WhereStmt::extractStatics(CodeGenReqs& cgr) const {
  if (booleanExpr) {
    booleanExpr->extractStatics(cgr);
  }
}

string BinaryExpr::code(const CodeGenReqs& cgr) const {
  string opFn;
  switch (op) {
  case PLUS:   opFn = "Plus"; break;
  case MINUS:  opFn = "Minus"; break;
  case MULT:   opFn = "Mult"; break;
  case DIVIDE: opFn = "Divide"; break;
  default:     opFn = "<BinaryExpOp>";
  }
  return opFn + "(" + lhs->code(cgr) + ", " + rhs->code(cgr) + ")";
}

string UnaryExpr::code(const CodeGenReqs& cgr) const {
  string unaryFn;
  switch (op) {
  case UMINUS: unaryFn = "Uminus"; break;
  default:     unaryFn = "<UnaryExpOp>";
  }
  return unaryFn + "(" + expr->code(cgr) + ")";
}

string Fn1CallExpr::code(const CodeGenReqs& cgr) const {
  string fnStr;
  switch (fn1) {
  case STR:   fnStr = "ToStr"; break;
  case INT:   fnStr = "ToInt"; break;
  default:    fnStr = "<fn1>";
  }
  return fnStr + "(" + expr->code(cgr) + ")";
}

string Fn3CallExpr::code(const CodeGenReqs& cgr) const {
  string fnStr;
  switch (fn3) {
  case SUBSTR: fnStr = "substr"; break;
  default:     fnStr = "<fn3>";
  }
  return fnStr + "(" + expr1->code(cgr) + "," + expr2->code(cgr) + "," +
         expr3->code(cgr) + ")";
}

string Expr::code(const CodeGenReqs& cgr) const {
  switch (type) {
  case BINARY_EXPR:   return binaryExpr.code(cgr);
  case UNARY_EXPR:    return unaryExpr.code(cgr);
  case FN1_CALL_EXPR: return fn1CallExpr.code(cgr);
  case FN3_CALL_EXPR: return fn3CallExpr.code(cgr);
  case IDENTIFIER:    {auto f = cgr.idVarMap.find(identifier);
                       return f==cgr.idVarMap.end() ? identifier : f->second;}
  case STRING:        return string("optional<string>(") + "\"" + stringValue + "\")";
  case LONG:          return string("optional<int64>(") + to_string(longValue) + ")";
  case DOUBLE:        return string("optional<double>(") + to_string(doubleValue) + ")";
  case BOOL:          return string("optional<bool>(") + (boolValue ? "true" : "false") + ")";
  default:            return "<Expr>";
  }
}

string CompoundBooleanExpr::code(const CodeGenReqs& cgr) const {
  string opStr;
  switch (op) {
  case AND: opStr = "&&"; break;
  case OR:  opStr = "||"; break;
  default:  opStr = "<CompoundBooleanExprOp>";
  }
  return "(" + lhs->code(cgr) + " " + opStr + " " + rhs->code(cgr) + ")";
}

string SimpleBooleanExpr::code(const CodeGenReqs& cgr) const {
  string opFn;
  switch (op) {
  case EQ:   opFn = "Eq"; break;
  case NE:   opFn = "Ne"; break;
  case LT:   opFn = "Lt"; break;
  case GT:   opFn = "Gt"; break;
  case LE:   opFn = "Le"; break;
  case GE:   opFn = "Ge"; break;
  case LIKE: opFn = "Like"; break;
  default:   opFn = "<SimpleBooleanExprOp>";
  }
  string lhs_code = lhs.code(cgr);
  string rhs_code;
  if ((op == LIKE) && (rhs.type == STRING)) {
    auto f = cgr.regexMap.find(rhs.stringValue);
    ASSERT(f != cgr.regexMap.end());
    rhs_code = f->second;
  } else {
    rhs_code = rhs.code(cgr);
  }
  return opFn + "(" + lhs_code + ", " + rhs_code + ")";
}

string NullaryBooleanExpr::code(const CodeGenReqs& cgr) const {
  Expr expr;
  expr.type = IDENTIFIER;
  expr.identifier = identifier;
  return string(isNull ? "IsNull" : "IsNotNull") + "(" + expr.code(cgr) + ")";
}

string BooleanExpr::code(const CodeGenReqs& cgr) const {
  switch (type) {
  case BOOLEAN: return compoundBooleanExpr.code(cgr);
  case SIMPLE:  return simpleBooleanExpr.code(cgr);
  case NULLARY: return nullaryBooleanExpr.code(cgr);
  default:      return "<BooleanExpr>";
  }
}

string OrderByField::code(const CodeGenReqs& cgr) const {
  return expr.code(cgr);
}

string SelectField::code(const CodeGenReqs& cgr) const {
  return expr.code(cgr);
}

string Expr::cppType(const CodeGenReqs& cgr) const {
  CodeGenReqs copy = cgr;
  copy.idVarMap = copy.idDefaultMap;
  return "decltype(" + code(copy) + ")";
}

void BinaryExpr::removeSelectAliases(const map<string, const Expr*>& aliases) {
  lhs->removeSelectAliases(aliases);
  rhs->removeSelectAliases(aliases);
}

void UnaryExpr::removeSelectAliases(const map<string, const Expr*>& aliases) {
  expr->removeSelectAliases(aliases);
}

void Fn1CallExpr::removeSelectAliases(const map<string, const Expr*>& aliases) {
  expr->removeSelectAliases(aliases);
}

void Fn3CallExpr::removeSelectAliases(const map<string, const Expr*>& aliases) {
  expr1->removeSelectAliases(aliases);
  expr2->removeSelectAliases(aliases);
  expr3->removeSelectAliases(aliases);
}

void Expr::removeSelectAliases(const map<string, const Expr*>& aliases) {
  switch (type) {
  case BINARY_EXPR:   binaryExpr.removeSelectAliases(aliases); break;
  case UNARY_EXPR:    unaryExpr.removeSelectAliases(aliases); break;
  case FN1_CALL_EXPR: fn1CallExpr.removeSelectAliases(aliases); break;
  case FN3_CALL_EXPR: fn3CallExpr.removeSelectAliases(aliases); break;
  case IDENTIFIER:    {
                       auto f = aliases.find(identifier);
                       if (f != aliases.end()) {
                         *this = *(f->second);
                       }
                      }
                      break;
  case STRING:        break;
  case LONG:          break;
  case DOUBLE:        break;
  case BOOL:          break;
  default:            THROW("Unhandled Expr case");
  }
}

void CompoundBooleanExpr::removeSelectAliases(const map<string, const Expr*>& aliases) {
  lhs->removeSelectAliases(aliases);
  rhs->removeSelectAliases(aliases);
}

void SimpleBooleanExpr::removeSelectAliases(const map<string, const Expr*>& aliases) {
  lhs.removeSelectAliases(aliases);
  rhs.removeSelectAliases(aliases);
}

void BooleanExpr::removeSelectAliases(const map<string, const Expr*>& aliases) {
  switch (type) {
  case BOOLEAN: compoundBooleanExpr.removeSelectAliases(aliases); break;
  case SIMPLE: simpleBooleanExpr.removeSelectAliases(aliases); break;
  case NULLARY: break;
  default: THROW("Unhandled BooleanExpr case");
  }
}

void SelectQuery::removeSelectAliases() {
  map<string, const Expr*> aliases;
  for (SelectField& selectField : selectStmt.selectFields) {
    if (!aliases.empty()) {
      selectField.expr.removeSelectAliases(aliases);
    }
    if (!selectField.alias.empty()) {
      aliases.emplace(selectField.alias, &(selectField.expr));
    }
  }
  if (!aliases.empty()) {
    if (whereStmt.booleanExpr) {
      whereStmt.booleanExpr->removeSelectAliases(aliases);
    }
    for (OrderByField& orderByField : orderByStmt.orderByFields) {
      orderByField.expr.removeSelectAliases(aliases);
    }
  }
}

void SelectQuery::preProcess() {
  removeSelectAliases();
}
