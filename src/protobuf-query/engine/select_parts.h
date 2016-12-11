/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include "global_include.h"
#include "utils.h"

using namespace std;

class Expr;
class CompoundBooleanExpr;
class SimpleBooleanExpr;
class NullaryBooleanExpr;
class BooleanExpr;
class SelectField;
using SelectAliases = map<string, const Expr*>;

struct CodeGenReqs {
  map<string, string> idVarMap;
  map<string, string> idDefaultMap;
  map<string, string> regexMap;
};

// note: update Expr::str() on mod
enum ExprType {
  BINARY_EXPR,
  UNARY_EXPR,
  FN1_CALL_EXPR,
  FN3_CALL_EXPR,
  IDENTIFIER,
  STRING,
  LONG,
  DOUBLE,
  BOOL,
};

// note: update BinaryExpr::str() on mod
enum BinaryExprOp {
  PLUS, MINUS, MULT, DIVIDE
};

// note: update UnaryExpr::str() on mod
enum UnaryExprOp {
  UMINUS
};

// note: update Fn1CallExpr::str() on mod
enum Fn1 {
  STR, INT, SUM, COUNT
};

// note: update Fn3CallExpr::str() on mod
enum Fn3 {
  SUBSTR
};

class BinaryExpr {
public:
  void removeSelectAliases(const SelectAliases& aliases);
  void getAllIdentifiers(set<string>& identifiers) const;
  string code(const CodeGenReqs& cgr) const;
  string str() const;
private:
  friend Expr;
  BinaryExprOp op_;
  shared_ptr<Expr> lhs_;
  shared_ptr<Expr> rhs_;
};

class UnaryExpr {
public:
  void removeSelectAliases(const SelectAliases& aliases);
  void getAllIdentifiers(set<string>& identifiers) const;
  string code(const CodeGenReqs& cgr) const;
  string str() const;
private:
  friend Expr;
  UnaryExprOp op_;
  shared_ptr<Expr> expr_;
};

class Fn1CallExpr {
public:
  void removeSelectAliases(const SelectAliases& aliases);
  void getAllIdentifiers(set<string>& identifiers) const;
  string code(const CodeGenReqs& cgr) const;
  string str() const;
private:
  friend Expr;
  Fn1 fn1_;
  shared_ptr<Expr> expr_;
};

class Fn3CallExpr {
public:
  void removeSelectAliases(const SelectAliases& aliases);
  void getAllIdentifiers(set<string>& identifiers) const;
  string code(const CodeGenReqs& cgr) const;
  string str() const;
private:
  friend Expr;
  Fn3 fn3_;
  shared_ptr<Expr> expr1_, expr2_, expr3_;
};

class Expr {
public:
  static Expr create(BinaryExprOp op, const Expr& lhs, const Expr& rhs);
  static Expr create(UnaryExprOp op, const Expr& expr);
  static Expr create(Fn1 fn1, const Expr& expr);
  static Expr create(Fn3 fn3, const Expr& expr1, const Expr& expr2,
                     const Expr& expr3);
  static Expr createIdentifier(const string& identifier);
  static Expr createPrimitive(const string& value);
  static Expr createPrimitive(long value);
  static Expr createPrimitive(double value);
  static Expr createPrimitive(bool value);
  void removeSelectAliases(const SelectAliases& aliases);
  void getAllIdentifiers(set<string>& identifiers) const;
  string code(const CodeGenReqs& cgr) const;
  string cppType(const CodeGenReqs& cgr) const;
  string str() const;
  bool isIdentifier() const;
  bool isString() const;
  const string& getIdentifier() const;
  const string& getStringValue() const;
private:
  ExprType type_;
  // TODO(sanchay): figure out how to use variant.
  // union doesn't work nicely, we get complaints about destructor.
  BinaryExpr binary_expr_;
  UnaryExpr unary_expr_;
  Fn1CallExpr fn1_call_expr_;
  Fn3CallExpr fn3_call_expr_;
  string identifier_;
  string string_value_;
  long long_value_;
  double double_value_;
  bool bool_value_;
};

// note: update BooleanExpr::str() on mod
enum BooleanExprType {
  BOOLEAN, SIMPLE, NULLARY
};

// note: update CompoundBooleanExpr::str() on mod
enum CompoundBooleanOp {
  AND, OR
};

// note: update SimpleBooleanExpr::str() on mod
enum SimpleBooleanOp {
  EQ, NE, LT, GT, LE, GE, LIKE
};

class CompoundBooleanExpr {
public:
  void removeSelectAliases(const SelectAliases& aliases);
  void getAllIdentifiers(set<string>& identifiers) const;
  void extractStatics(CodeGenReqs& cgr) const;
  string code(const CodeGenReqs& cgr) const;
  string str() const;
private:
  friend BooleanExpr;
  CompoundBooleanOp op_;
  shared_ptr<BooleanExpr> lhs_;
  shared_ptr<BooleanExpr> rhs_;
};

class SimpleBooleanExpr {
public:
  void removeSelectAliases(const SelectAliases& aliases);
  void getAllIdentifiers(set<string>& identifiers) const;
  void extractStatics(CodeGenReqs& cgr) const;
  string code(const CodeGenReqs& cgr) const;
  string str() const;
private:
  friend BooleanExpr;
  SimpleBooleanOp op_;
  Expr lhs_;
  Expr rhs_;
};

class NullaryBooleanExpr {
public:
  void removeSelectAliases(const SelectAliases& aliases);
  void getAllIdentifiers(set<string>& identifiers) const;
  string code(const CodeGenReqs& cgr) const;
  string str() const;
private:
  friend BooleanExpr;
  bool is_null_;
  string identifier_;
};

class BooleanExpr {
public:
  static BooleanExpr create(
      CompoundBooleanOp op, const BooleanExpr& lhs, const BooleanExpr& rhs);
  static BooleanExpr create(
      SimpleBooleanOp op, const Expr& lhs, const Expr& rhs);
  static BooleanExpr createNullary(bool isNull, const string& identifier);
  void removeSelectAliases(const SelectAliases& aliases);
  void getAllIdentifiers(set<string>& identifiers) const;
  void canoncialize(vector<const BooleanExpr*>& and_clauses) const;
  void extractStatics(CodeGenReqs& cgr) const;
  string code(const CodeGenReqs& cgr) const;
  string str() const;
private:
  BooleanExprType type_;
  // TODO(sanchay): figure out how to use variant.
  CompoundBooleanExpr compound_boolean_expr_;
  SimpleBooleanExpr simple_boolean_expr_;
  NullaryBooleanExpr nullary_boolean_expr_;
};

class SelectField {
public:
  static SelectField create(const Expr& expr, const string& alias);
  void removeSelectAliases(const SelectAliases& aliases);
  void getAllIdentifiers(set<string>& identifiers) const;
  string code(const CodeGenReqs& cgr) const;
  string str() const;
  void populateAliasIfPresent(SelectAliases& aliases) const;
  const Expr& getExpr() const;
  string getHeader() const;
private:
  Expr expr_;
  string alias_;
};

class SelectStmt {
public:
  static SelectStmt create(const vector<SelectField>& select_fields,
                           bool distinct);
  void removeSelectAliases(SelectAliases& aliases);
  string str() const;
  const vector<SelectField>& getSelectFields() const;
private:
  bool distinct_;
  vector<SelectField> select_fields_;
};

class FromStmt {
public:
  static FromStmt create(const string& proto_name);
  string str() const;
  const string& getProtoName() const;
private:
  string proto_name_;
};

class WhereStmt {
public:
  static WhereStmt create();
  static WhereStmt create(BooleanExpr boolean_expr);
  void removeSelectAliases(const SelectAliases& aliases);
  void extractStatics(CodeGenReqs& cgr) const;
  void getAllIdentifiers(set<string>& identifiers) const;
  void canoncialize(vector<const BooleanExpr*>& and_clauses) const;
  string str() const;
private:
  optional<BooleanExpr> boolean_expr_;
};

class OrderByField {
public:
  static OrderByField create(const Expr& expr, bool desc);
  void removeSelectAliases(const SelectAliases& aliases);
  void getAllIdentifiers(set<string>& identifiers) const;
  string code(const CodeGenReqs& cgr) const;
  string str() const;
  const Expr& getExpr() const;
  bool isDesc() const;
private:
  Expr expr_;
  bool desc_;
};

class OrderByStmt {
public:
  static OrderByStmt create();
  static OrderByStmt create(const vector<OrderByField>& order_by_fields);
  void removeSelectAliases(const SelectAliases& aliases);
  string str() const;
  const vector<OrderByField>& getOrderByFields() const;
private:
  vector<OrderByField> order_by_fields_;
};

class SelectQuery;
