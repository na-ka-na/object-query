/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

// Unit tests for protobuf_generated_common.h

#include "test_commons.h"
#include "protobuf_generated_common.h"

void TestMyRangeIteratorEmpty() {
  ::google::protobuf::RepeatedField<int> rf;
  auto&& myit = Iterators::mk_pb_iterator(&rf);
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
  auto&& myit = Iterators::mk_pb_iterator(&rf);
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
  TestMyRangeIteratorEmpty();
  TestMyRangeIteratorNonEmpty();
}
