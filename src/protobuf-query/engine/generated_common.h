#pragma once

#include <fcntl.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <tuple>
#include <type_traits>
#include <vector>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/gzip_stream.h>
#include "global_include.h"

using namespace std;

using int8   = ::google::protobuf::int8;
using int16  = ::google::protobuf::int16;
using int32  = ::google::protobuf::int32;
using int64  = ::google::protobuf::int64;
using uint8  = ::google::protobuf::uint8;
using uint16 = ::google::protobuf::uint16;
using uint32 = ::google::protobuf::uint32;
using uint64 = ::google::protobuf::uint64;

void parsePbFromFile(const string& file, google::protobuf::Message& proto) {
  int fd = open(file.c_str(), O_RDONLY);
  ASSERT(fd >= 0, "Unable to open file", file);
  bool ret = false;
  if (file.rfind(".gz") != string::npos) {
    google::protobuf::io::FileInputStream fis(fd);
    google::protobuf::io::GzipInputStream gzfis(&fis);
    ret = proto.ParseFromZeroCopyStream(&gzfis);
  } else {
    ret = proto.ParseFromFileDescriptor(fd);
  }
  close(fd);
  ASSERT(ret, "Unable to parse", file);
}

template<typename T>
inline string stringify(const T& t) {
  return to_string(t);
}

template<typename T>
inline string stringify(const optional<T>& t) {
  return t ? stringify<T>(*t) : "NULL";
}

template<> inline string stringify(const bool& t) {return t ? "true" : "false";}
template<> inline string stringify(const string& t) {return t;}

template<typename T>
inline bool isNull(const optional<T>& t) {
  return !static_cast<bool>(t);
}

template<typename T>
inline bool isNotNull(const optional<T>& t) {
  return !isNull(t);
}

template<typename L, typename R>
inline bool eq(const optional<L>& l, const optional<R>& r) {
  return (!l && !r) || (l && r && (*l == *r));
}

template<typename L, typename R>
inline bool ne(const optional<L>& l, const optional<R>& r) {
  return !eq(l, r);
}

template<typename L, typename R>
inline bool lt(const optional<L>& l, const optional<R>& r) {
  return (!l && r) || (l && r && (*l < *r));
}

template<typename L, typename R>
inline bool le(const optional<L>& l, const optional<R>& r) {
  return !lt(r, l);
}

template<typename L, typename R>
inline bool gt(const optional<L>& l, const optional<R>& r) {
  return lt(r, l);
}

template<typename L, typename R>
inline bool ge(const optional<L>& l, const optional<R>& r) {
  return !lt(l, r);
}

template<typename T>
inline optional<T> uminus(const optional<T>& t) {
  return t ? optional<T>(-(*t)) : optional<T>();
}

template<typename T>
inline optional<T> plus(const optional<T>& l, const optional<T>& r) {
  return (l && r) ? optional<T>(*l + *r) : optional<T>();
}

template<typename T>
inline optional<T> minus(const optional<T>& l, const optional<T>& r) {
  return (l && r) ? optional<T>(*l - *r) : optional<T>();
}

template<typename T>
inline optional<T> mult(const optional<T>& l, const optional<T>& r) {
  return (l && r) ? optional<T>((*l) * (*r)) : optional<T>();
}

template<typename T>
inline optional<T> divide(const optional<T>& l, const optional<T>& r) {
  return (l && r) ? optional<T>(*l / *r) : optional<T>();
}

template<typename T>
inline int toInt(const optional<T>& t) {
  return (int) t;
}

////////////////////////////////////////////////////////////////////////////////
#define BINARY_OPS(T, L, R)\
  inline optional<T> plus(const optional<L>& l, const optional<R>& r) {\
    return (l && r) ? optional<T>(*l + *r) : optional<T>();\
  }\
  inline optional<T> minus(const optional<L>& l, const optional<R>& r) {\
    return (l && r) ? optional<T>(*l - *r) : optional<T>();\
  }\
  inline optional<T> mult(const optional<L>& l, const optional<R>& r) {\
    return (l && r) ? optional<T>((*l) * (*r)) : optional<T>();\
  }\
  inline optional<T> divide(const optional<L>& l, const optional<R>& r) {\
    return (l && r) ? optional<T>(*l / *r) : optional<T>();\
  }

BINARY_OPS(double, double, double)
BINARY_OPS(double, double, float)
BINARY_OPS(double, double, uint8)
BINARY_OPS(double, double, uint16)
BINARY_OPS(double, double, uint32)
BINARY_OPS(double, double, uint64)
BINARY_OPS(double, double, int8)
BINARY_OPS(double, double, int16)
BINARY_OPS(double, double, int32)
BINARY_OPS(double, double, int64)

BINARY_OPS(double, float, double)
BINARY_OPS(float, float, float)
BINARY_OPS(float, float, uint8)
BINARY_OPS(float, float, uint16)
BINARY_OPS(float, float, uint32)
BINARY_OPS(float, float, uint64)
BINARY_OPS(float, float, int8)
BINARY_OPS(float, float, int16)
BINARY_OPS(float, float, int32)
BINARY_OPS(float, float, int64)

BINARY_OPS(double, uint8, double)
BINARY_OPS(float, uint8, float)
BINARY_OPS(uint8, uint8, uint8)
BINARY_OPS(uint16, uint8, uint16)
BINARY_OPS(uint32, uint8, uint32)
BINARY_OPS(uint64, uint8, uint64)
BINARY_OPS(uint8, uint8, int8)
BINARY_OPS(int16, uint8, int16)
BINARY_OPS(int32, uint8, int32)
BINARY_OPS(int64, uint8, int64)

BINARY_OPS(double, uint16, double)
BINARY_OPS(float, uint16, float)
BINARY_OPS(uint16, uint16, uint8)
BINARY_OPS(uint16, uint16, uint16)
BINARY_OPS(uint32, uint16, uint32)
BINARY_OPS(uint64, uint16, uint64)
BINARY_OPS(uint16, uint16, int8)
BINARY_OPS(uint16, uint16, int16)
BINARY_OPS(int32, uint16, int32)
BINARY_OPS(int64, uint16, int64)

BINARY_OPS(double, uint32, double)
BINARY_OPS(float, uint32, float)
BINARY_OPS(uint32, uint32, uint8)
BINARY_OPS(uint32, uint32, uint16)
BINARY_OPS(uint32, uint32, uint32)
BINARY_OPS(uint64, uint32, uint64)
BINARY_OPS(uint32, uint32, int8)
BINARY_OPS(uint32, uint32, int16)
BINARY_OPS(uint32, uint32, int32)
BINARY_OPS(int64, uint32, int64)

BINARY_OPS(double, uint64, double)
BINARY_OPS(float, uint64, float)
BINARY_OPS(uint64, uint64, uint8)
BINARY_OPS(uint64, uint64, uint16)
BINARY_OPS(uint64, uint64, uint32)
BINARY_OPS(uint64, uint64, uint64)
BINARY_OPS(uint64, uint64, int8)
BINARY_OPS(uint64, uint64, int16)
BINARY_OPS(uint64, uint64, int32)
BINARY_OPS(uint64, uint64, int64)

BINARY_OPS(double, int8, double)
BINARY_OPS(float, int8, float)
BINARY_OPS(uint8, int8, uint8)
BINARY_OPS(uint16, int8, uint16)
BINARY_OPS(uint32, int8, uint32)
BINARY_OPS(uint64, int8, uint64)
BINARY_OPS(int8, int8, int8)
BINARY_OPS(int16, int8, int16)
BINARY_OPS(int32, int8, int32)
BINARY_OPS(int64, int8, int64)

BINARY_OPS(double, int16, double)
BINARY_OPS(float, int16, float)
BINARY_OPS(int16, int16, uint8)
BINARY_OPS(uint16, int16, uint16)
BINARY_OPS(uint32, int16, uint32)
BINARY_OPS(uint64, int16, uint64)
BINARY_OPS(int16, int16, int8)
BINARY_OPS(int16, int16, int16)
BINARY_OPS(int32, int16, int32)
BINARY_OPS(int64, int16, int64)

BINARY_OPS(double, int32, double)
BINARY_OPS(float, int32, float)
BINARY_OPS(int32, int32, uint8)
BINARY_OPS(int32, int32, uint16)
BINARY_OPS(uint32, int32, uint32)
BINARY_OPS(uint64, int32, uint64)
BINARY_OPS(int32, int32, int8)
BINARY_OPS(int32, int32, int16)
BINARY_OPS(int32, int32, int32)
BINARY_OPS(int64, int32, int64)

BINARY_OPS(double, int64, double)
BINARY_OPS(float, int64, float)
BINARY_OPS(int64, int64, uint8)
BINARY_OPS(int64, int64, uint16)
BINARY_OPS(int64, int64, uint32)
BINARY_OPS(uint64, int64, uint64)
BINARY_OPS(int64, int64, int8)
BINARY_OPS(int64, int64, int16)
BINARY_OPS(int64, int64, int32)
BINARY_OPS(int64, int64, int64)
