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
  static void Throw(const string& expr, const string& function,
                    const string& file, long line, Args... args) {
    std::stringstream ss;
    ss << "ASSERT " << expr << " at " << file << ":"
       << line << " " << function << " msg:";
    FormatMsg(ss, args...);
    throw runtime_error(ss.str());
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
  static void FormatMsg(std::ostream& oss) {}
};

#ifndef ASSERT
#define ASSERT(expr, ...) \
if (!(expr)) { \
  Assert::Throw(string(#expr), __func__, __FILE__, __LINE__, ##__VA_ARGS__); \
}
#endif
