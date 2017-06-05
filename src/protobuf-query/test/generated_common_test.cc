/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

// Unit tests for generated_common.h

#include "test_commons.h"
#include "generated_common.h"

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

void TestMyRangeIteratorEmpty() {
  ::google::protobuf::RepeatedField<int> rf;
  auto&& myit = Iterators::mk_iterator(&rf);
  auto&& begin = myit.begin();
  auto&& end = myit.end();
  EXPECT_NE(begin, end);
  EXPECT_EQ((int*) nullptr, *begin);
  ++begin;
  EXPECT_EQ(begin, end);
}

void TestMyRangeIteratorNonEmpty() {
  ::google::protobuf::RepeatedField<int> rf;
  rf.Add(1);
  rf.Add(2);
  auto&& myit = Iterators::mk_iterator(&rf);
  auto&& begin = myit.begin();
  auto&& end = myit.end();
  EXPECT_NE(begin, end);
  EXPECT_EQ(&rf.Get(0), *begin);
  ++begin;
  EXPECT_NE(begin, end);
  EXPECT_EQ(&rf.Get(1), *begin);
  ++begin;
  EXPECT_EQ(begin, end);
}

int main() {
  TestArthimeticFunctions();
  TestMyRangeIteratorEmpty();
  TestMyRangeIteratorNonEmpty();
}