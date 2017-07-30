/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

// Unit tests for json_generated_common.h

#include "test_commons.h"
#include "json_generated_common.h"

void TestMyRangeIteratorEmpty() {
  rapidjson::Document object;
  object.Parse("{\"abc\": 123}");
  EXPECT_FALSE(object.HasParseError());
  JsonValue value(object);
  auto&& myit = Iterators::mk_json_iterator("<root>", &value, "xyz");
  auto&& begin = myit.begin();
  auto&& end = myit.end();
  EXPECT_NE(begin, end);
  EXPECT_EQ((const JsonValue*) nullptr, *begin);
  ++begin;
  EXPECT_EQ(begin, end);
}

void TestMyRangeIteratorEmptyArray() {
  rapidjson::Document object;
  object.Parse("{\"abc\": []}");
  EXPECT_FALSE(object.HasParseError());
  JsonValue value(object);
  auto&& myit = Iterators::mk_json_iterator("<root>", &value, "abc");
  auto&& begin = myit.begin();
  auto&& end = myit.end();
  EXPECT_NE(begin, end);
  EXPECT_EQ((const JsonValue*) nullptr, *begin);
  ++begin;
  EXPECT_EQ(begin, end);
}

void TestMyRangeIteratorNonEmpty() {
  rapidjson::Document object;
  object.Parse("{\"abc\": \"def\"}");
  EXPECT_FALSE(object.HasParseError());
  JsonValue value(object);
  auto&& myit = Iterators::mk_json_iterator("<roo>", &value, "abc");
  auto&& begin = myit.begin();
  auto&& end = myit.end();
  EXPECT_NE(begin, end);
  EXPECT_EQ(JsonValue(object["abc"]), **begin);
  stringstream ss;
  ss << **begin;
  EXPECT_EQ("def", ss.str());
  ++begin;
  EXPECT_EQ(begin, end);
}

void TestMyRangeIteratorNonEmptyArray() {
  rapidjson::Document object;
  object.Parse("{\"abc\": [1.322423, 456]}");
  EXPECT_FALSE(object.HasParseError());
  JsonValue value(object);
  auto&& myit = Iterators::mk_json_iterator("<roo>", &value, "abc");
  auto&& begin = myit.begin();
  auto&& end = myit.end();
  EXPECT_NE(begin, end);
  EXPECT_EQ(JsonValue(object["abc"][0]), **begin);
  stringstream ss;
  ss << **begin;
  EXPECT_EQ("1.322423", ss.str());
  ++begin;
  EXPECT_NE(begin, end);
  EXPECT_EQ(JsonValue(object["abc"][1]), **begin);
  stringstream ss1;
  ss1 << **begin;
  EXPECT_EQ("456", ss1.str());
  ++begin;
  EXPECT_EQ(begin, end);
}

int main() {
  TestMyRangeIteratorEmpty();
  TestMyRangeIteratorEmptyArray();
  TestMyRangeIteratorNonEmpty();
  TestMyRangeIteratorNonEmptyArray();
}
