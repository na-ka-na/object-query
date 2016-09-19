/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

#pragma once

#include <string>
#include <ostream>
#include <sstream>
#include <experimental/optional>

template<typename T>
using optional = std::experimental::optional<T>;

using namespace std;

template<typename T>
ostream& operator<<(ostream& os, const optional<T>& obj) {
  obj ? (os << *obj) : (os << "NULL");
  return os;
}

class Assert {
public:
  template<typename... Args>
  static runtime_error CreateException(
      const string& expr, const string& function,
      const string& file, long line, Args... args) {
    std::stringstream ss;
    ss << "ASSERT " << expr << " at " << file << ":"
       << line << " " << function << " msg:";
    FormatMsg(ss, args...);
    return runtime_error(ss.str());
  }
private:
  template<typename Type, typename... Args>
  static void FormatMsg(std::ostream& oss, Type inst, Args... args) {
    FormatMsg(oss, inst);
    FormatMsg(oss, args...);
  }
  template<typename Type>
  static void FormatMsg(std::ostream& oss, Type inst) {
    oss << " " << inst;
  }
  static void FormatMsg(std::ostream&) {}
};

#ifndef ASSERT
#define ASSERT(expr, ...) \
if (!(expr)) { \
  throw Assert::CreateException(string(#expr), __func__, __FILE__, __LINE__, ##__VA_ARGS__); \
}
#endif

#ifndef THROW
#define THROW(...) \
  throw Assert::CreateException("thrown", __func__, __FILE__, __LINE__, ##__VA_ARGS__);
#endif
