// Unit tests for generated_common.h

#include <iostream>
#include "generated_common.h"

using namespace std;

#ifndef EXPECT_EQ
#define EXPECT_EQ(expected, actual, ...) \
  ASSERT(expected == actual, "expected:", expected, "actual", actual, ##__VA_ARGS__)
#endif

void TestArthimeticFunctions() {
  auto a = Uminus(optional<double>(1.1));
  EXPECT_EQ("-1.100000", Stringify(a));

  auto b = optional<int>(2);
  auto c = Plus(a, b);
  EXPECT_EQ("0.900000", Stringify(c));

  auto d = Minus(a, b);
  EXPECT_EQ("-3.100000", Stringify(d));

  auto e = Mult(a, b);
  EXPECT_EQ("-2.200000", Stringify(e));

  auto f = Divide(a, b);
  EXPECT_EQ("-0.550000", Stringify(f));
}

int main() {
  using S0 = optional<int>;
  using S1 = optional<double>;
  decltype(Mult(S1(), Minus(S0(), Plus(S0(), S1())))) x;
  x = 2.3;
  cout << x << endl;

  TestArthimeticFunctions();
}
