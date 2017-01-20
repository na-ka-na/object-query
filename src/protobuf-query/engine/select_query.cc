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
  expr.binary_expr_.op_ = op;
  expr.binary_expr_.lhs_.reset(new Expr());
  expr.binary_expr_.rhs_.reset(new Expr());
  *(expr.binary_expr_.lhs_) = lhs;
  *(expr.binary_expr_.rhs_) = rhs;
  return expr;
}

Expr Expr::create(UnaryExprOp op, const Expr& uexpr) {
  Expr expr;
  expr.type_ = UNARY_EXPR;
  expr.unary_expr_.op_ = op;
  expr.unary_expr_.expr_.reset(new Expr());
  *(expr.unary_expr_.expr_) = uexpr;
  return expr;
}

Expr Expr::createFnCall(const string& fn, const vector<Expr>& params) {
  Expr expr;
  expr.type_ = FN_CALL_EXPR;
  expr.fn_call_expr_.fn_ = fn;
  for (const Expr& param : params) {
    auto param_ptr = make_shared<Expr>();
    *param_ptr = param;
    expr.fn_call_expr_.params_.push_back(param_ptr);
  }
  return expr;
}

Expr Expr::createIdentifier(const string& identifier) {
  Expr expr;
  expr.type_ = IDENTIFIER;
  expr.identifier_ = identifier;
  return expr;
}

Expr Expr::createPrimitive(const string& value) {
  Expr expr;
  expr.type_ = STRING;
  expr.string_value_ = value;
  return expr;
}

Expr Expr::createPrimitive(long value) {
  Expr expr;
  expr.type_ = LONG;
  expr.long_value_ = value;
  return expr;
}

Expr Expr::createPrimitive(double value) {
  Expr expr;
  expr.type_ = DOUBLE;
  expr.double_value_ = value;
  return expr;
}

Expr Expr::createPrimitive(bool value) {
  Expr expr;
  expr.type_ = BOOL;
  expr.bool_value_ = value;
  return expr;
}

BooleanExpr BooleanExpr::create(
    CompoundBooleanOp op, const BooleanExpr& lhs, const BooleanExpr& rhs) {
  BooleanExpr bexpr;
  bexpr.type_ = BOOLEAN;
  bexpr.compound_boolean_expr_.op_ = op;
  bexpr.compound_boolean_expr_.lhs_.reset(new BooleanExpr());
  bexpr.compound_boolean_expr_.rhs_.reset(new BooleanExpr());
  *(bexpr.compound_boolean_expr_.lhs_) = lhs;
  *(bexpr.compound_boolean_expr_.rhs_) = rhs;
  return bexpr;
}

BooleanExpr BooleanExpr::create(
    SimpleBooleanOp op, const Expr& lhs, const Expr& rhs) {
  BooleanExpr bexpr;
  bexpr.type_ = SIMPLE;
  bexpr.simple_boolean_expr_.op_ = op;
  bexpr.simple_boolean_expr_.lhs_ = lhs;
  bexpr.simple_boolean_expr_.rhs_ = rhs;
  return bexpr;
}

BooleanExpr BooleanExpr::createNullary(bool isNull, const string& identifier) {
  BooleanExpr bexpr;
  bexpr.type_ = NULLARY;
  bexpr.nullary_boolean_expr_.is_null_ = isNull;
  bexpr.nullary_boolean_expr_.identifier_ = identifier;
  return bexpr;
}

RawSelectField RawSelectField::create(const Expr& expr, const string& alias) {
  RawSelectField raw_select_field;
  raw_select_field.expr_ = expr;
  raw_select_field.alias_ = alias;
  return raw_select_field;
}

RawSelectField RawSelectField::create(const string& star_identifier,
                                      const string& alias) {
  RawSelectField raw_select_field;
  raw_select_field.star_identifier_ = star_identifier;
  raw_select_field.alias_ = alias;
  return raw_select_field;
}

void RawSelectField::resolveSelectStar(
    const StarFieldResolver& resolver, vector<SelectField>& resolved) const {
  if (!star_identifier_.empty()) {
    vector<string> resolved_identifiers;
    resolver(star_identifier_, resolved_identifiers);
    for (const string& resolved_identifier : resolved_identifiers) {
      Expr expr = Expr::createIdentifier(resolved_identifier);
      string alias;
      if (!alias_.empty()) {
        alias = alias_ + ".";
        auto idx = resolved_identifier.rfind(".");
        if (idx == string::npos) {
          alias += resolved_identifier;
        } else {
          alias += resolved_identifier.substr(idx+1);
        }
      }
      resolved.push_back(SelectField::create(expr, alias));
    }
  } else {
    resolved.push_back(SelectField::create(expr_, alias_));
  }
}

SelectField SelectField::create(const Expr& expr, const string& alias) {
  SelectField select_field;
  select_field.expr_ = expr;
  select_field.alias_ = alias;
  return select_field;
}

SelectStmt SelectStmt::create(const vector<RawSelectField>& raw_select_fields,
                              bool distinct) {
  SelectStmt select_stmt;
  select_stmt.distinct_ = distinct;
  select_stmt.raw_select_fields_ = raw_select_fields;
  return select_stmt;
}

void SelectStmt::resolveSelectStars(const StarFieldResolver& resolver) {
  for (const RawSelectField& raw_select_field : raw_select_fields_) {
    raw_select_field.resolveSelectStar(resolver, select_fields_);
  }
}

FromStmt FromStmt::create(const string& proto_name) {
  FromStmt from_stmt;
  from_stmt.proto_name_ = proto_name;
  return from_stmt;
}

WhereStmt WhereStmt::create() {
  return WhereStmt();
}

WhereStmt WhereStmt::create(BooleanExpr boolean_expr) {
  WhereStmt where_stmt;
  where_stmt.boolean_expr_ = boolean_expr;
  return where_stmt;
}

OrderByField OrderByField::create(const Expr& expr, bool desc) {
  OrderByField order_by_field;
  order_by_field.expr_ = expr;
  order_by_field.desc_ = desc;
  return order_by_field;
}

OrderByStmt OrderByStmt::create() {
  return OrderByStmt();
}

OrderByStmt OrderByStmt::create(const vector<OrderByField>& order_by_fields) {
  OrderByStmt order_by_stmt;
  order_by_stmt.order_by_fields_ = order_by_fields;
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
  return identifier_;
}

const string& Expr::getStringValue() const {
  ASSERT(isString());
  return string_value_;
}

const Expr& SelectField::getExpr() const {
  return expr_;
}

string SelectField::getHeader() const {
  return alias_.empty() ? str() : alias_;
}

const Expr& OrderByField::getExpr() const {
  return expr_;
}

bool OrderByField::isDesc() const {
  return desc_;
}

const vector<SelectField>& SelectStmt::getSelectFields() const {
  return select_fields_;
}

const string& FromStmt::getProtoName() const {
  return proto_name_;
}

const vector<OrderByField>& OrderByStmt::getOrderByFields() const {
  return order_by_fields_;
}

void BinaryExpr::getAllIdentifiers(set<string>& identifiers) const {
  lhs_->getAllIdentifiers(identifiers);
  rhs_->getAllIdentifiers(identifiers);
}

void UnaryExpr::getAllIdentifiers(set<string>& identifiers) const {
  expr_->getAllIdentifiers(identifiers);
}

void FnCallExpr::getAllIdentifiers(set<string>& identifiers) const {
  for (auto& param : params_) {
    param->getAllIdentifiers(identifiers);
  }
}

void Expr::getAllIdentifiers(set<string>& identifiers) const {
  switch (type_) {
  case BINARY_EXPR: binary_expr_.getAllIdentifiers(identifiers); break;
  case UNARY_EXPR: unary_expr_.getAllIdentifiers(identifiers); break;
  case FN_CALL_EXPR: fn_call_expr_.getAllIdentifiers(identifiers); break;
  case IDENTIFIER: identifiers.insert(identifier_); break;
  case STRING: break;
  case LONG: break;
  case DOUBLE: break;
  case BOOL: break;
  default: THROW("Unhandled Expr case");
  }
}

void CompoundBooleanExpr::getAllIdentifiers(set<string>& identifiers) const {
  lhs_->getAllIdentifiers(identifiers);
  rhs_->getAllIdentifiers(identifiers);
}

void SimpleBooleanExpr::getAllIdentifiers(set<string>& identifiers) const {
  lhs_.getAllIdentifiers(identifiers);
  rhs_.getAllIdentifiers(identifiers);
}

void BooleanExpr::getAllIdentifiers(set<string>& identifiers) const {
  switch (type_) {
  case BOOLEAN: compound_boolean_expr_.getAllIdentifiers(identifiers); break;
  case SIMPLE: simple_boolean_expr_.getAllIdentifiers(identifiers); break;
  case NULLARY: nullary_boolean_expr_.getAllIdentifiers(identifiers); break;
  default: THROW("Unhandled BooleanExpr case");
  }
}

void NullaryBooleanExpr::getAllIdentifiers(set<string>& identifiers) const {
  identifiers.insert(identifier_);
}

void SelectField::getAllIdentifiers(set<string>& identifiers) const {
  expr_.getAllIdentifiers(identifiers);
}

void OrderByField::getAllIdentifiers(set<string>& identifiers) const {
  expr_.getAllIdentifiers(identifiers);
}

void WhereStmt::getAllIdentifiers(set<string>& identifiers) const {
  if (boolean_expr_) {
    boolean_expr_->getAllIdentifiers(identifiers);
  }
}

void BooleanExpr::canoncialize(vector<const BooleanExpr*>& andClauses) const {
  if ((type_ == BOOLEAN) && (compound_boolean_expr_.op_ == AND)) {
    compound_boolean_expr_.lhs_->canoncialize(andClauses);
    compound_boolean_expr_.rhs_->canoncialize(andClauses);
  } else {
    andClauses.push_back(this);
  }
}

void WhereStmt::canoncialize(vector<const BooleanExpr*>& andClauses) const {
  if (boolean_expr_) {
    boolean_expr_->canoncialize(andClauses);
  }
}

string BinaryExpr::str() const {
  string opStr;
  switch (op_) {
  case PLUS:   opStr = "+"; break;
  case MINUS:  opStr = "-"; break;
  case MULT:   opStr = "*"; break;
  case DIVIDE: opStr = "/"; break;
  default:     opStr = "<BinaryExpOp>";
  }
  return "(" + lhs_->str() + opStr + rhs_->str() + ")";
}

string UnaryExpr::str() const {
  string prefix;
  switch (op_) {
  case UMINUS: prefix = "-"; break;
  default:     prefix = "<UnaryExpOp>";
  }
  return prefix + expr_->str();
}

string FnCallExpr::str() const {
  string str = fn_ + "(";
  if (params_.size() > 0) {
    str += params_[0]->str();
    for (size_t i=1; i<params_.size(); i++) {
      str += "," + params_[i]->str();
    }
  }
  str += ")";
  return str;
}

string Expr::str() const {
  switch (type_) {
  case BINARY_EXPR:   return binary_expr_.str();
  case UNARY_EXPR:    return unary_expr_.str();
  case FN_CALL_EXPR:  return fn_call_expr_.str();
  case IDENTIFIER:    return identifier_;
  case STRING:        return "'"+string_value_+"'";
  case LONG:          return to_string(long_value_);
  case DOUBLE:        return to_string(double_value_);
  case BOOL:          return bool_value_ ? "TRUE" : "FALSE";
  default:            return "<Expr>";
  }
}

string CompoundBooleanExpr::str() const {
  string opStr;
  switch (op_) {
  case AND: opStr = "AND"; break;
  case OR:  opStr = "OR"; break;
  default:  opStr = "<CompoundBooleanExprOp>";
  }
  return "(" + lhs_->str() + " " + opStr + " " + rhs_->str() + ")";
}

string SimpleBooleanExpr::str() const {
  string opStr;
  switch (op_) {
  case EQ:   opStr = "="; break;
  case NE:   opStr = "!="; break;
  case LT:   opStr = "<"; break;
  case GT:   opStr = ">"; break;
  case LE:   opStr = "<="; break;
  case GE:   opStr = ">="; break;
  case LIKE: opStr = "LIKE"; break;
  default:   opStr = "<SimpleBooleanExprOp>";
  }
  return "(" + lhs_.str() + " " + opStr + " " + rhs_.str() + ")";
}

string NullaryBooleanExpr::str() const {
  Expr expr = Expr::createIdentifier(identifier_);
  return expr.str() + " IS" + (is_null_ ? "" : " NOT") + " NULL";
}

string BooleanExpr::str() const {
  switch (type_) {
  case BOOLEAN: return compound_boolean_expr_.str();
  case SIMPLE:  return simple_boolean_expr_.str();
  case NULLARY: return nullary_boolean_expr_.str();
  default:      return "<BooleanExpr>";
  }
}

string RawSelectField::str() const {
  return (star_identifier_.empty() ? expr_.str() : star_identifier_) +
         (alias_.empty() ? "" : (" AS " + alias_));
}

string SelectField::str() const {
  return expr_.str() + (alias_.empty() ? "" : (" AS " + alias_));
}

string SelectStmt::str() const {
  return "SELECT " + string(distinct_ ? "DISTINCT " : "") +
         joinVec(", ", raw_select_fields_, strfn<RawSelectField>());
}

string OrderByField::str() const {
  return expr_.str() + (desc_ ? " DESC" : "");
}

string OrderByStmt::str() const {
  return order_by_fields_.empty() ? "" :
      ("ORDER BY " + joinVec(", ", order_by_fields_, strfn<OrderByField>()));
}

string FromStmt::str() const {
  return "FROM " + proto_name_;
}

string WhereStmt::str() const {
  return boolean_expr_ ? ("WHERE " + boolean_expr_->str()) : "";
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

void BinaryExpr::extractStatics(CodeGenReqs& cgr) const {
  lhs_->extractStatics(cgr);
  rhs_->extractStatics(cgr);
}

void UnaryExpr::extractStatics(CodeGenReqs& cgr) const {
  expr_->extractStatics(cgr);
}

void FnCallExpr::extractStatics(CodeGenReqs& cgr) const {
  auto f = cgr.fnMap.find(fn_);
  if (f == cgr.fnMap.end()) {
    cgr.fnMap.emplace(fn_, params_.size());
  } else {
    ASSERT(params_.size() == f->second, "Num params mismatch for fn_",
           fn_, f->second, params_.size());
  }
}

void Expr::extractStatics(CodeGenReqs& cgr) const {
  switch (type_) {
  case BINARY_EXPR:   binary_expr_.extractStatics(cgr); break;
  case UNARY_EXPR:    unary_expr_.extractStatics(cgr); break;
  case FN_CALL_EXPR:  fn_call_expr_.extractStatics(cgr); break;
  default:            break;
  }
}

void CompoundBooleanExpr::extractStatics(CodeGenReqs& cgr) const {
  lhs_->extractStatics(cgr);
  rhs_->extractStatics(cgr);
}

void SimpleBooleanExpr::extractStatics(CodeGenReqs& cgr) const {
  if ((op_ == LIKE) && (rhs_.isString())) {
    cgr.regexMap.emplace(rhs_.getStringValue(), "");
  }
  lhs_.extractStatics(cgr);
  rhs_.extractStatics(cgr);
}

void NullaryBooleanExpr::extractStatics(CodeGenReqs&) const {
  /* nothing to do */
}

void BooleanExpr::extractStatics(CodeGenReqs& cgr) const {
  switch (type_) {
  case BOOLEAN: compound_boolean_expr_.extractStatics(cgr); break;
  case SIMPLE: simple_boolean_expr_.extractStatics(cgr); break;
  case NULLARY: nullary_boolean_expr_.extractStatics(cgr); break;
  default: THROW("Unhandled BooleanExpr case");
  }
}

void SelectField::extractStatics(CodeGenReqs& cgr) const {
  expr_.extractStatics(cgr);
}

void OrderByField::extractStatics(CodeGenReqs& cgr) const {
  expr_.extractStatics(cgr);
}

void SelectStmt::extractStatics(CodeGenReqs& cgr) const {
  for (const SelectField& selectField : select_fields_) {
    selectField.extractStatics(cgr);
  }
}

void WhereStmt::extractStatics(CodeGenReqs& cgr) const {
  if (boolean_expr_) {
    boolean_expr_->extractStatics(cgr);
  }
}

void OrderByStmt::extractStatics(CodeGenReqs& cgr) const {
  for (const OrderByField& orderByField : order_by_fields_) {
    orderByField.extractStatics(cgr);
  }
}

void SelectQuery::extractStatics(CodeGenReqs& cgr) const {
  selectStmt.extractStatics(cgr);
  whereStmt.extractStatics(cgr);
  orderByStmt.extractStatics(cgr);
}

string BinaryExpr::code(const CodeGenReqs& cgr) const {
  string opFn;
  switch (op_) {
  case PLUS:   opFn = "Plus"; break;
  case MINUS:  opFn = "Minus"; break;
  case MULT:   opFn = "Mult"; break;
  case DIVIDE: opFn = "Divide"; break;
  default:     opFn = "<BinaryExpOp>";
  }
  return opFn + "(" + lhs_->code(cgr) + ", " + rhs_->code(cgr) + ")";
}

string UnaryExpr::code(const CodeGenReqs& cgr) const {
  string unaryFn;
  switch (op_) {
  case UMINUS: unaryFn = "Uminus"; break;
  default:     unaryFn = "<UnaryExpOp>";
  }
  return unaryFn + "(" + expr_->code(cgr) + ")";
}

string FnCallExpr::code(const CodeGenReqs& cgr) const {
  string code = "$" + fn_ + "(";
  if (params_.size() > 0) {
    code += params_[0]->code(cgr);
    for (size_t i=1; i<params_.size(); i++) {
      code += ", " + params_[i]->code(cgr);
    }
  }
  code += ")";
  return code;
}

string Expr::code(const CodeGenReqs& cgr) const {
  switch (type_) {
  case BINARY_EXPR:   return binary_expr_.code(cgr);
  case UNARY_EXPR:    return unary_expr_.code(cgr);
  case FN_CALL_EXPR:  return fn_call_expr_.code(cgr);
  case IDENTIFIER:    {auto f = cgr.idVarMap.find(identifier_);
                       return f==cgr.idVarMap.end() ? identifier_ : f->second;}
  case STRING:        return string("optional<string>(") + "\"" + string_value_ + "\")";
  case LONG:          return string("optional<int64>(") + to_string(long_value_) + ")";
  case DOUBLE:        return string("optional<double>(") + to_string(double_value_) + ")";
  case BOOL:          return string("optional<bool>(") + (bool_value_ ? "true" : "false") + ")";
  default:            return "<Expr>";
  }
}

string CompoundBooleanExpr::code(const CodeGenReqs& cgr) const {
  string opStr;
  switch (op_) {
  case AND: opStr = "&&"; break;
  case OR:  opStr = "||"; break;
  default:  opStr = "<CompoundBooleanExprOp>";
  }
  return "(" + lhs_->code(cgr) + " " + opStr + " " + rhs_->code(cgr) + ")";
}

string SimpleBooleanExpr::code(const CodeGenReqs& cgr) const {
  string opFn;
  switch (op_) {
  case EQ:   opFn = "Eq"; break;
  case NE:   opFn = "Ne"; break;
  case LT:   opFn = "Lt"; break;
  case GT:   opFn = "Gt"; break;
  case LE:   opFn = "Le"; break;
  case GE:   opFn = "Ge"; break;
  case LIKE: opFn = "Like"; break;
  default:   opFn = "<SimpleBooleanExprOp>";
  }
  string lhs_code = lhs_.code(cgr);
  string rhs_code;
  if ((op_ == LIKE) && (rhs_.isString())) {
    auto f = cgr.regexMap.find(rhs_.getStringValue());
    ASSERT(f != cgr.regexMap.end());
    rhs_code = f->second;
  } else {
    rhs_code = rhs_.code(cgr);
  }
  return opFn + "(" + lhs_code + ", " + rhs_code + ")";
}

string NullaryBooleanExpr::code(const CodeGenReqs& cgr) const {
  Expr expr = Expr::createIdentifier(identifier_);
  return string(is_null_ ? "IsNull" : "IsNotNull") + "(" + expr.code(cgr) + ")";
}

string BooleanExpr::code(const CodeGenReqs& cgr) const {
  switch (type_) {
  case BOOLEAN: return compound_boolean_expr_.code(cgr);
  case SIMPLE:  return simple_boolean_expr_.code(cgr);
  case NULLARY: return nullary_boolean_expr_.code(cgr);
  default:      return "<BooleanExpr>";
  }
}

string OrderByField::code(const CodeGenReqs& cgr) const {
  return expr_.code(cgr);
}

string SelectField::code(const CodeGenReqs& cgr) const {
  return expr_.code(cgr);
}

string Expr::cppType(const CodeGenReqs& cgr) const {
  CodeGenReqs copy = cgr;
  copy.idVarMap = copy.idDefaultMap;
  return "decltype(" + code(copy) + ")";
}

void BinaryExpr::removeSelectAliases(const SelectAliases& aliases) {
  lhs_->removeSelectAliases(aliases);
  rhs_->removeSelectAliases(aliases);
}

void UnaryExpr::removeSelectAliases(const SelectAliases& aliases) {
  expr_->removeSelectAliases(aliases);
}

void FnCallExpr::removeSelectAliases(const SelectAliases& aliases) {
  for (auto& param : params_) {
    param->removeSelectAliases(aliases);
  }
}

void Expr::removeSelectAliases(const SelectAliases& aliases) {
  switch (type_) {
  case BINARY_EXPR:   binary_expr_.removeSelectAliases(aliases); break;
  case UNARY_EXPR:    unary_expr_.removeSelectAliases(aliases); break;
  case FN_CALL_EXPR:  fn_call_expr_.removeSelectAliases(aliases); break;
  case IDENTIFIER:    {
                       auto f = aliases.find(identifier_);
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
  lhs_->removeSelectAliases(aliases);
  rhs_->removeSelectAliases(aliases);
}

void SimpleBooleanExpr::removeSelectAliases(const SelectAliases& aliases) {
  lhs_.removeSelectAliases(aliases);
  rhs_.removeSelectAliases(aliases);
}

void NullaryBooleanExpr::removeSelectAliases(const SelectAliases& aliases) {
  auto f = aliases.find(identifier_);
  if (f != aliases.end()) {
    const Expr* expr = f->second;
    ASSERT(expr->isIdentifier(), "Only identifiers can be NULL checked");
    identifier_ = expr->getIdentifier();
  }
}

void BooleanExpr::removeSelectAliases(const SelectAliases& aliases) {
  switch (type_) {
  case BOOLEAN: compound_boolean_expr_.removeSelectAliases(aliases); break;
  case SIMPLE: simple_boolean_expr_.removeSelectAliases(aliases); break;
  case NULLARY: nullary_boolean_expr_.removeSelectAliases(aliases); break;
  default: THROW("Unhandled BooleanExpr case");
  }
}

void SelectField::removeSelectAliases(const SelectAliases& aliases) {
  expr_.removeSelectAliases(aliases);
}

void SelectField::populateAliasIfPresent(SelectAliases& aliases) const {
  if (!alias_.empty()) {
    aliases.emplace(alias_, &expr_);
  }
}

void SelectStmt::removeSelectAliases(SelectAliases& aliases) {
  for (SelectField& selectField : select_fields_) {
    if (!aliases.empty()) {
      selectField.removeSelectAliases(aliases);
    }
    selectField.populateAliasIfPresent(aliases);
  }
}

void WhereStmt::removeSelectAliases(const SelectAliases& aliases) {
  if (boolean_expr_) {
    boolean_expr_->removeSelectAliases(aliases);
  }
}

void OrderByField::removeSelectAliases(const SelectAliases& aliases) {
  expr_.removeSelectAliases(aliases);
}

void OrderByStmt::removeSelectAliases(const SelectAliases& aliases) {
  for (OrderByField& orderByField : order_by_fields_) {
    orderByField.removeSelectAliases(aliases);
  }
}

void SelectQuery::resolveSelectStars(const StarFieldResolver& resolver) {
  selectStmt.resolveSelectStars(resolver);
}

void SelectQuery::removeSelectAliases() {
  SelectAliases aliases;
  selectStmt.removeSelectAliases(aliases);
  if (!aliases.empty()) {
    whereStmt.removeSelectAliases(aliases);
    orderByStmt.removeSelectAliases(aliases);
  }
}
