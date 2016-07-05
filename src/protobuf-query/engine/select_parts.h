
#pragma once

#include <string>
#include <vector>
#include <memory>
#include "global_include.h"
#include "utils.h"

using namespace std;

struct SelectField {
  string identifier;
  string str() const;
};

struct SelectStmt {
  bool distinct = false;
  vector<SelectField> selectFields;
  string str() const;
};

struct FromStmt {
  string fromFile;
  string fromRootProto;
  string str() const;
};

enum ExprType {
  BINARY_EXPR,
  UNINARY_EXPR,
  FN1_CALL_EXPR,
  FN3_CALL_EXPR,
  IDENTIFIER,
  STRING,
  LONG,
  DOUBLE,
};

enum BinaryExprOp {
  PLUS, MINUS, MULT, DIVIDE
};

enum UnaryExprOp {
  UMINUS
};

enum Fn1 {
  STR, INT, SUM, COUNT
};

enum Fn3 {
  SUBSTR
};

struct Expr;

struct BinaryExpr {
  BinaryExprOp op;
  shared_ptr<Expr> lhs;
  shared_ptr<Expr> rhs;
};

struct UnaryExpr {
  UnaryExprOp op;
  shared_ptr<Expr> expr;
};

struct Fn1CallExpr {
  Fn1 fn1;
  shared_ptr<Expr> expr;
};

struct Fn3CallExpr {
  Fn3 fn3;
  shared_ptr<Expr> expr1, expr2, expr3;
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
  static Expr create(BinaryExprOp op, const Expr& lhs, const Expr& rhs);
  static Expr create(UnaryExprOp op, const Expr& expr);
  static Expr create(Fn1 fn1, const Expr& expr);
  static Expr create(Fn3 fn3, const Expr& expr1, const Expr& expr2,
                     const Expr& expr3);
  static Expr createIdentifier(const string& identifier);
  static Expr createPrimitive(const string& value);
  static Expr createPrimitive(long value);
  static Expr createPrimitive(double value);
};

enum BooleanExprType {
  BOOLEAN, SIMPLE
};

enum CompoundBooleanOp {
  AND, OR
};

enum SimpleBooleanOp {
  EQ, NE, LT, GT, LE, GE, LIKE
};

struct BooleanExpr;

struct CompoundBooleanExpr {
  CompoundBooleanOp op;
  shared_ptr<BooleanExpr> lhs;
  shared_ptr<BooleanExpr> rhs;
};

struct SimpleBooleanExpr {
  SimpleBooleanOp op;
  Expr lhs;
  Expr rhs;
};


struct BooleanExpr {
  BooleanExprType type;
  // TODO(sanchay): figure out how to use variant.
  CompoundBooleanExpr compoundBooleanExpr;
  SimpleBooleanExpr simpleBooleanExpr;
  static BooleanExpr create(
      CompoundBooleanOp op, const BooleanExpr& lhs, const BooleanExpr& rhs);
  static BooleanExpr create(
      SimpleBooleanOp op, const Expr& lhs, const Expr& rhs);
};

struct WhereStmt {
  optional<BooleanExpr> booleanExpr;
};

class SelectQuery;
