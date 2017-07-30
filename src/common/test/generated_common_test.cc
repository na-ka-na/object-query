/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

// Unit tests for generated_common.h

#include <cstdlib>
#include <ctime>
#include "test_commons.h"
#include "generated_common.h"

void TestArthimeticFunctions() {
  auto a = Uminus(optional<double>(1.1));
  EXPECT_EQ(-1.1, *a);

  auto b = optional<int>(2);
  auto c = Plus(a, b);
  EXPECT_EQ(9, int((*c) * 10));

  auto d = Minus(a, b);
  EXPECT_EQ(-31, int((*d) * 10));

  auto e = Mult(a, b);
  EXPECT_EQ(-22, int((*e) * 10));

  auto f = Divide(a, b);
  EXPECT_EQ(-55, int((*f) * 100));
}

string randomString() {
  srand (time(NULL));
  size_t size = rand() % 140;
  if (size % 10) size = 0;
  return string(size, 'a');
}

void checkEq(const string& expected, const MyString& actual) {
  EXPECT_EQ(expected.size(), actual.size());
  stringstream ss;
  ss << actual;
  EXPECT_EQ(expected, ss.str());
}

void TestMyString_constructor() {
  {
    MyString mystr;
    checkEq("", mystr);
  }
  {
    string str = randomString();
    MyString mystr(str.c_str(), str.size());
    checkEq(str, mystr);
  }
  {
    string str = randomString();
    MyString mystr(&str);
    checkEq(str, mystr);
  }
  {
    string str = randomString();
    MyString mystr(str);
    checkEq(str, mystr);
  }
  {
    string str = randomString();
    string copy = str;
    MyString mystr(move(copy));
    checkEq(str, mystr);
  }
}

void TestMyString_copyConstructor() {
  {
    string str = randomString();
    MyString mystr1(str.c_str(), str.size());
    MyString mystr(mystr1);
    checkEq(str, mystr);
  }
  {
    string str = randomString();
    MyString mystr1(&str);
    MyString mystr(mystr1);
    checkEq(str, mystr);
  }
  {
    string str = randomString();
    MyString mystr1(str);
    MyString mystr(mystr1);
    checkEq(str, mystr);
  }
  {
    string str = randomString();
    string copy = str;
    MyString mystr1(move(copy));
    MyString mystr(mystr1);
    checkEq(str, mystr);
  }
}

void TestMyString_moveConstructor() {
  {
    string str = randomString();
    MyString mystr1(str.c_str(), str.size());
    MyString mystr(move(mystr1));
    checkEq(str, mystr);
  }
  {
    string str = randomString();
    MyString mystr1(&str);
    MyString mystr(move(mystr1));
    checkEq(str, mystr);
  }
  {
    string str = randomString();
    MyString mystr1(str);
    MyString mystr(move(mystr1));
    checkEq(str, mystr);
  }
  {
    string str = randomString();
    string copy = str;
    MyString mystr1(move(copy));
    MyString mystr(move(mystr1));
    checkEq(str, mystr);
  }
}

int main() {
  TestArthimeticFunctions();
  TestMyString_constructor();
  TestMyString_copyConstructor();
  TestMyString_moveConstructor();
}
