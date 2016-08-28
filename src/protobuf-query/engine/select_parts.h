
#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include "global_include.h"
#include "utils.h"

using namespace std;

struct Expr;

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

struct BinaryExpr {
  BinaryExprOp op;
  shared_ptr<Expr> lhs;
  shared_ptr<Expr> rhs;
  void getAllIdentifiers(set<string>& identifiers) const;
  string code(const map<string, string>& idMap) const;
  string str() const;
};

struct UnaryExpr {
  UnaryExprOp op;
  shared_ptr<Expr> expr;
  void getAllIdentifiers(set<string>& identifiers) const;
  string code(const map<string, string>& idMap) const;
  string str() const;
};

struct Fn1CallExpr {
  Fn1 fn1;
  shared_ptr<Expr> expr;
  void getAllIdentifiers(set<string>& identifiers) const;
  string code(const map<string, string>& idMap) const;
  string str() const;
};

struct Fn3CallExpr {
  Fn3 fn3;
  shared_ptr<Expr> expr1, expr2, expr3;
  void getAllIdentifiers(set<string>& identifiers) const;
  string code(const map<string, string>& idMap) const;
  string str() const;
};

struct Expr {
  ExprType type;
  // TODO(sanchay): figure out how to use variant.
  // union doesn't work nicely, we get complaints about destructor.
  BinaryExpr binaryExpr;
  UnaryExpr unaryExpr;
  Fn1CallExpr fn1CallExpr;
  Fn3CallExpr fn3CallExpr;
  string identifier;
  string stringValue;
  long longValue;
  double doubleValue;
  bool boolValue;
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
  void getAllIdentifiers(set<string>& identifiers) const;
  string code(const map<string, string>& idMap) const;
  string cppType(const map<string, string>& idDefaultsMap) const;
  string str() const;
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

struct BooleanExpr;

struct CompoundBooleanExpr {
  CompoundBooleanOp op;
  shared_ptr<BooleanExpr> lhs;
  shared_ptr<BooleanExpr> rhs;
  void getAllIdentifiers(set<string>& identifiers) const;
  string code(const map<string, string>& idMap) const;
  string str() const;
};

struct SimpleBooleanExpr {
  SimpleBooleanOp op;
  Expr lhs;
  Expr rhs;
  void getAllIdentifiers(set<string>& identifiers) const;
  string code(const map<string, string>& idMap) const;
  string str() const;
};

struct NullaryBooleanExpr {
  bool isNull;
  string identifier;
  void getAllIdentifiers(set<string>& identifiers) const;
  string code(const map<string, string>& idMap) const;
  string str() const;
};

struct BooleanExpr {
  BooleanExprType type;
  // TODO(sanchay): figure out how to use variant.
  CompoundBooleanExpr compoundBooleanExpr;
  SimpleBooleanExpr simpleBooleanExpr;
  NullaryBooleanExpr nullaryBooleanExpr;
  static BooleanExpr create(
      CompoundBooleanOp op, const BooleanExpr& lhs, const BooleanExpr& rhs);
  static BooleanExpr create(
      SimpleBooleanOp op, const Expr& lhs, const Expr& rhs);
  static BooleanExpr createNullary(bool isNull, const string& identifier);
  void getAllIdentifiers(set<string>& identifiers) const;
  void canoncialize(vector<const BooleanExpr*>& andClauses) const;
  string code(const map<string, string>& idMap) const;
  string str() const;
};

struct SelectField {
  Expr expr;
  void getAllIdentifiers(set<string>& identifiers) const;
  string code(const map<string, string>& idMap) const;
  string str() const;
};

struct SelectStmt {
  bool distinct = false;
  vector<SelectField> selectFields;
  string str() const;
};

struct OrderByField {
  Expr expr;
  bool desc = false;
  void getAllIdentifiers(set<string>& identifiers) const;
  string code(const map<string, string>& idMap) const;
  string str() const;
};

struct OrderByStmt {
  vector<OrderByField> orderByFields;
  string str() const;
};

struct FromStmt {
  string fromFile;
  string fromRootProto;
  string str() const;
};

struct WhereStmt {
  optional<BooleanExpr> booleanExpr;
  void getAllIdentifiers(set<string>& identifiers) const;
  void canoncialize(vector<const BooleanExpr*>& andClauses) const;
  string str() const;
};

class SelectQuery;
