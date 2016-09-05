// Unit tests for select_query_test.cc

#include "test_commons.h"
#include "select_parts.h"

void TestExpr() {
  Expr x = Expr::createIdentifier("x");
  Expr y = Expr::createIdentifier("y");
  Expr e1 = Expr::create(PLUS, x, y);
  Expr e2 = Expr::create(MINUS, x, e1);
  Expr e3 = Expr::create(MULT, y, e2);
  EXPECT_EQ("(y*(x-(x+y)))", e3.str());
  EXPECT_EQ("Mult(y, Minus(x, Plus(x, y)))", e3.code({}));
  CodeGenReqs cgr;
  cgr.idVarMap = {{"x", "s0"}, {"y", "s1"}};
  cgr.idDefaultMap = {{"x", "S0()"}, {"y", "S1()"}};
  EXPECT_EQ("Mult(s1, Minus(s0, Plus(s0, s1)))", e3.code(cgr));
  EXPECT_EQ("decltype(Mult(S1(), Minus(S0(), Plus(S0(), S1()))))",
            e3.cppType(cgr));
}

int main() {
  TestExpr();
}
