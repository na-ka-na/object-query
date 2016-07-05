
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
  expr.type = UNINARY_EXPR;
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

string SelectField::str() const {
  return identifier;
}

string SelectStmt::str() const {
  return (distinct ? "DISTINCT " : "") +
         joinVec(", ", selectFields, strfn<SelectField>());
}

string FromStmt::str() const {
  return "('" + fromFile + "', '" + fromRootProto + "')";
}

string SelectQuery::str() const {
  return "SELECT " + selectStmt.str() + " FROM " + fromStmt.str();
}

