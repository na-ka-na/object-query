
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
  default: ASSERT(false);
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
  default: ASSERT(false);
  }
}

void NullaryBooleanExpr::getAllIdentifiers(set<string>& identifiers) const {
  identifiers.insert(identifier);
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

string SelectField::str() const {
  return identifier;
}

string SelectStmt::str() const {
  return "SELECT " + string(distinct ? "DISTINCT " : "") +
         joinVec(", ", selectFields, strfn<SelectField>());
}

string FromStmt::str() const {
  return "FROM ('" + fromFile + "', '" + fromRootProto + "')";
}

string BinaryExpr::str(const map<string, string>& idMap) const {
  string opStr;
  switch (op) {
  case PLUS:   opStr = "+"; break;
  case MINUS:  opStr = "-"; break;
  case MULT:   opStr = "*"; break;
  case DIVIDE: opStr = "/"; break;
  default:     opStr = "<BinaryExpOp>";
  }
  return "(" + lhs->str(idMap) + opStr + rhs->str(idMap) + ")";
}

string UnaryExpr::str(const map<string, string>& idMap) const {
  string prefix;
  switch (op) {
  case UMINUS: prefix = "-"; break;
  default:     prefix = "<UnaryExpOp>";
  }
  return prefix + expr->str(idMap);
}

string Fn1CallExpr::str(const map<string, string>& idMap) const {
  string fnStr;
  switch (fn1) {
  case STR:   fnStr = "STR"; break;
  case INT:   fnStr = "INT"; break;
  case SUM:   fnStr = "SUM"; break;
  case COUNT: fnStr = "COUNT"; break;
  default:    fnStr = "<fn1>";
  }
  return fnStr + "(" + expr->str(idMap) + ")";
}

string Fn3CallExpr::str(const map<string, string>& idMap) const {
  string fnStr;
  switch (fn3) {
  case SUBSTR: fnStr = "SUBSTR"; break;
  default:     fnStr = "<fn3>";
  }
  return fnStr + "(" + expr1->str(idMap) + "," + expr2->str(idMap) + "," +
         expr3->str(idMap) + ")";
}

string Expr::str(const map<string, string>& idMap) const {
  switch (type) {
  case BINARY_EXPR:   return binaryExpr.str(idMap);
  case UNARY_EXPR:    return unaryExpr.str(idMap);
  case FN1_CALL_EXPR: return fn1CallExpr.str(idMap);
  case FN3_CALL_EXPR: return fn3CallExpr.str(idMap);
  case IDENTIFIER:    {auto f = idMap.find(identifier);
                       return f==idMap.end() ? identifier : f->second;}
  case STRING:        return "\""+stringValue+"\"";
  case LONG:          return to_string(longValue);
  case DOUBLE:        return to_string(doubleValue);
  default:            return "<Expr>";
  }
}

string CompoundBooleanExpr::str(const map<string, string>& idMap) const {
  string opStr;
  switch (op) {
  case AND: opStr = "AND"; break;
  case OR:  opStr = "OR"; break;
  default:  opStr = "<CompoundBooleanExprOp>";
  }
  return "(" + lhs->str(idMap) + " " + opStr + " " + rhs->str(idMap) + ")";
}

string SimpleBooleanExpr::str(const map<string, string>& idMap) const {
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
  return "(" + lhs.str(idMap) + " " + opStr + " " + rhs.str(idMap) + ")";
}

string BooleanExpr::str(const map<string, string>& idMap) const {
  switch (type) {
  case BOOLEAN: return compoundBooleanExpr.str(idMap);
  case SIMPLE:  return simpleBooleanExpr.str(idMap);
  case NULLARY: return nullaryBooleanExpr.str(idMap);
  default:      return "<BooleanExpr>";
  }
}

string NullaryBooleanExpr::str(const map<string, string>& idMap) const {
  Expr expr;
  expr.type = IDENTIFIER;
  expr.identifier = identifier;
  return expr.str(idMap) + " IS" + (isNull ? "" : " NOT") + " NULL";
}

string WhereStmt::str(const map<string, string>& idMap) const {
  return booleanExpr ? ("WHERE " + booleanExpr->str(idMap)) : "";
}

string SelectQuery::str() const {
  string whereStr = whereStmt.str({});
  return selectStmt.str() + " " + fromStmt.str() +
         (whereStr.empty() ? "" : (" " + whereStr));
}

