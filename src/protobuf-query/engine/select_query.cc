/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

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
  expr.type_ = BINARY_EXPR;
  expr.binaryExpr.op = op;
  expr.binaryExpr.lhs.reset(new Expr());
  expr.binaryExpr.rhs.reset(new Expr());
  *(expr.binaryExpr.lhs) = lhs;
  *(expr.binaryExpr.rhs) = rhs;
  return expr;
}

Expr Expr::create(UnaryExprOp op, const Expr& uexpr) {
  Expr expr;
  expr.type_ = UNARY_EXPR;
  expr.unaryExpr.op = op;
  expr.unaryExpr.expr.reset(new Expr());
  *(expr.unaryExpr.expr) = uexpr;
  return expr;
}

Expr Expr::create(Fn1 fn1, const Expr& fexpr) {
  Expr expr;
  expr.type_ = FN1_CALL_EXPR;
  expr.fn1CallExpr.fn1 = fn1;
  expr.fn1CallExpr.expr.reset(new Expr());
  *(expr.fn1CallExpr.expr) = fexpr;
  return expr;
}

Expr Expr::create(Fn3 fn3, const Expr& expr1, const Expr& expr2,
                  const Expr& expr3) {
  Expr expr;
  expr.type_ = FN3_CALL_EXPR;
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
  expr.type_ = IDENTIFIER;
  expr.identifier = identifier;
  return expr;
}

Expr Expr::createPrimitive(const string& value) {
  Expr expr;
  expr.type_ = STRING;
  expr.stringValue = value;
  return expr;
}

Expr Expr::createPrimitive(long value) {
  Expr expr;
  expr.type_ = LONG;
  expr.longValue = value;
  return expr;
}

Expr Expr::createPrimitive(double value) {
  Expr expr;
  expr.type_ = DOUBLE;
  expr.doubleValue = value;
  return expr;
}

Expr Expr::createPrimitive(bool value) {
  Expr expr;
  expr.type_ = BOOL;
  expr.boolValue = value;
  return expr;
}

BooleanExpr BooleanExpr::create(
    CompoundBooleanOp op, const BooleanExpr& lhs, const BooleanExpr& rhs) {
  BooleanExpr bexpr;
  bexpr.type_ = BOOLEAN;
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
  bexpr.type_ = SIMPLE;
  bexpr.simpleBooleanExpr.op = op;
  bexpr.simpleBooleanExpr.lhs = lhs;
  bexpr.simpleBooleanExpr.rhs = rhs;
  return bexpr;
}

BooleanExpr BooleanExpr::createNullary(bool isNull, const string& identifier) {
  BooleanExpr bexpr;
  bexpr.type_ = NULLARY;
  bexpr.nullaryBooleanExpr.isNull = isNull;
  bexpr.nullaryBooleanExpr.identifier = identifier;
  return bexpr;
}

SelectField SelectField::create(const Expr& expr, const string& alias) {
  SelectField select_field;
  select_field.expr = expr;
  select_field.alias = alias;
  return select_field;
}

SelectStmt SelectStmt::create(const vector<SelectField>& select_fields,
                              bool distinct) {
  SelectStmt select_stmt;
  select_stmt.distinct = distinct;
  select_stmt.selectFields = select_fields;
  return select_stmt;
}

FromStmt FromStmt::create(const string& proto_name) {
  FromStmt from_stmt;
  from_stmt.protoName = proto_name;
  return from_stmt;
}

WhereStmt WhereStmt::create() {
  return WhereStmt();
}

WhereStmt WhereStmt::create(BooleanExpr boolean_expr) {
  WhereStmt where_stmt;
  where_stmt.booleanExpr = boolean_expr;
  return where_stmt;
}

OrderByField OrderByField::create(const Expr& expr, bool desc) {
  OrderByField order_by_field;
  order_by_field.expr = expr;
  order_by_field.desc = desc;
  return order_by_field;
}

OrderByStmt OrderByStmt::create() {
  return OrderByStmt();
}

OrderByStmt OrderByStmt::create(const vector<OrderByField>& order_by_fields) {
  OrderByStmt order_by_stmt;
  order_by_stmt.orderByFields = order_by_fields;
  return order_by_stmt;
}

bool Expr::isIdentifier() const {
  return type_ == IDENTIFIER;
}

bool Expr::isString() const {
  return type_ == STRING;
}

const string& Expr::getIdentifier() const {
  ASSERT(isIdentifier());
  return identifier;
}

const string& Expr::getStringValue() const {
  ASSERT(isString());
  return stringValue;
}

const Expr& SelectField::getExpr() const {
  return expr;
}

string SelectField::getHeader() const {
  return alias.empty() ? str() : alias;
}

const Expr& OrderByField::getExpr() const {
  return expr;
}

bool OrderByField::isDesc() const {
  return desc;
}

const vector<SelectField>& SelectStmt::getSelectFields() const {
  return selectFields;
}

const string& FromStmt::getProtoName() const {
  return protoName;
}

const vector<OrderByField>& OrderByStmt::getOrderByFields() const {
  return orderByFields;
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
  switch (type_) {
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
  switch (type_) {
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
  if ((type_ == BOOLEAN) && (compoundBooleanExpr.op == AND)) {
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
  switch (type_) {
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
  Expr expr = Expr::createIdentifier(identifier);
  return expr.str() + " IS" + (isNull ? "" : " NOT") + " NULL";
}

string BooleanExpr::str() const {
  switch (type_) {
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
  return "FROM " + protoName;
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
  if ((op == LIKE) && (rhs.isString())) {
    cgr.regexMap.emplace(rhs.getStringValue(), "");
  }
}

void BooleanExpr::extractStatics(CodeGenReqs& cgr) const {
  switch (type_) {
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
  switch (type_) {
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
  if ((op == LIKE) && (rhs.isString())) {
    auto f = cgr.regexMap.find(rhs.getStringValue());
    ASSERT(f != cgr.regexMap.end());
    rhs_code = f->second;
  } else {
    rhs_code = rhs.code(cgr);
  }
  return opFn + "(" + lhs_code + ", " + rhs_code + ")";
}

string NullaryBooleanExpr::code(const CodeGenReqs& cgr) const {
  Expr expr = Expr::createIdentifier(identifier);
  return string(isNull ? "IsNull" : "IsNotNull") + "(" + expr.code(cgr) + ")";
}

string BooleanExpr::code(const CodeGenReqs& cgr) const {
  switch (type_) {
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

void BinaryExpr::removeSelectAliases(const SelectAliases& aliases) {
  lhs->removeSelectAliases(aliases);
  rhs->removeSelectAliases(aliases);
}

void UnaryExpr::removeSelectAliases(const SelectAliases& aliases) {
  expr->removeSelectAliases(aliases);
}

void Fn1CallExpr::removeSelectAliases(const SelectAliases& aliases) {
  expr->removeSelectAliases(aliases);
}

void Fn3CallExpr::removeSelectAliases(const SelectAliases& aliases) {
  expr1->removeSelectAliases(aliases);
  expr2->removeSelectAliases(aliases);
  expr3->removeSelectAliases(aliases);
}

void Expr::removeSelectAliases(const SelectAliases& aliases) {
  switch (type_) {
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

void CompoundBooleanExpr::removeSelectAliases(const SelectAliases& aliases) {
  lhs->removeSelectAliases(aliases);
  rhs->removeSelectAliases(aliases);
}

void SimpleBooleanExpr::removeSelectAliases(const SelectAliases& aliases) {
  lhs.removeSelectAliases(aliases);
  rhs.removeSelectAliases(aliases);
}

void NullaryBooleanExpr::removeSelectAliases(const SelectAliases& aliases) {
  auto f = aliases.find(identifier);
  if (f != aliases.end()) {
    const Expr* expr = f->second;
    ASSERT(expr->isIdentifier(), "Only identifiers can be NULL checked");
    identifier = expr->getIdentifier();
  }
}

void BooleanExpr::removeSelectAliases(const SelectAliases& aliases) {
  switch (type_) {
  case BOOLEAN: compoundBooleanExpr.removeSelectAliases(aliases); break;
  case SIMPLE: simpleBooleanExpr.removeSelectAliases(aliases); break;
  case NULLARY: nullaryBooleanExpr.removeSelectAliases(aliases); break;
  default: THROW("Unhandled BooleanExpr case");
  }
}

void SelectField::removeSelectAliases(const SelectAliases& aliases) {
  expr.removeSelectAliases(aliases);
}

void SelectField::populateAliasIfPresent(SelectAliases& aliases) const {
  if (!alias.empty()) {
    aliases.emplace(alias, &expr);
  }
}

void SelectStmt::removeSelectAliases(SelectAliases& aliases) {
  for (SelectField& selectField : selectFields) {
    if (!aliases.empty()) {
      selectField.removeSelectAliases(aliases);
    }
    selectField.populateAliasIfPresent(aliases);
  }
}

void WhereStmt::removeSelectAliases(const SelectAliases& aliases) {
  if (booleanExpr) {
    booleanExpr->removeSelectAliases(aliases);
  }
}

void OrderByField::removeSelectAliases(const SelectAliases& aliases) {
  expr.removeSelectAliases(aliases);
}

void OrderByStmt::removeSelectAliases(const SelectAliases& aliases) {
  for (OrderByField& orderByField : orderByFields) {
    orderByField.removeSelectAliases(aliases);
  }
}

void SelectQuery::removeSelectAliases() {
  SelectAliases aliases;
  selectStmt.removeSelectAliases(aliases);
  if (!aliases.empty()) {
    whereStmt.removeSelectAliases(aliases);
    orderByStmt.removeSelectAliases(aliases);
  }
}

void SelectQuery::preProcess() {
  removeSelectAliases();
}
