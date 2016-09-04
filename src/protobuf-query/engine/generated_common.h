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

void ParsePbFromFile(const string& file, google::protobuf::Message& proto) {
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
inline string Stringify(const T& t) {
  return to_string(t);
}

template<typename T>
inline string Stringify(const optional<T>& t) {
  return t ? Stringify<T>(*t) : "NULL";
}

template<> inline string Stringify(const bool& t) {return t ? "true" : "false";}
template<> inline string Stringify(const string& t) {return t;}

template<typename T>
inline optional<string> ToStr(const optional<T>& t) {
  optional<string> s;
  if (t) {
    s = Stringify(*t);
  }
  return s;
}


template<typename T>
inline bool IsNull(const optional<T>& t) {
  return !static_cast<bool>(t);
}

template<typename T>
inline bool IsNotNull(const optional<T>& t) {
  return !IsNull(t);
}

template<typename L, typename R>
inline bool Eq(const optional<L>& l, const optional<R>& r) {
  return (!l && !r) || (l && r && (*l == *r));
}

template<typename L, typename R>
inline bool Ne(const optional<L>& l, const optional<R>& r) {
  return !Eq(l, r);
}

template<typename L, typename R>
inline bool Lt(const optional<L>& l, const optional<R>& r) {
  return (!l && r) || (l && r && (*l < *r));
}

template<typename L, typename R>
inline bool Le(const optional<L>& l, const optional<R>& r) {
  return !Lt(r, l);
}

template<typename L, typename R>
inline bool Gt(const optional<L>& l, const optional<R>& r) {
  return Lt(r, l);
}

template<typename L, typename R>
inline bool Ge(const optional<L>& l, const optional<R>& r) {
  return !Lt(l, r);
}

template<typename T>
inline auto Uminus(const optional<T>& t) {
  optional<decltype(-(*t))> o;
  if (t) o = -(*t);
  return o;
}

template<typename L, typename R>
inline auto Plus(const optional<L>& l, const optional<R>& r) {
  optional<decltype(*l + *r)> o;
  if (l && r) o = *l + *r;
  return o;
}

template<typename L, typename R>
inline auto Minus(const optional<L>& l, const optional<R>& r) {
  optional<decltype(*l - *r)> o;
  if (l && r) o = *l - *r;
  return o;
}

template<typename L, typename R>
inline auto Mult(const optional<L>& l, const optional<R>& r) {
  optional<decltype((*l) * (*r))> o;
  if (l && r) o = (*l) * (*r);
  return o;
}

template<typename L, typename R>
inline auto Divide(const optional<L>& l, const optional<R>& r) {
  optional<decltype(*l / *r)> o;
  if (l && r) o = *l / *r;
  return o;
}

template<typename T>
inline int ToInt(const optional<T>& t) {
  return (int) t;
}

template<typename T>
inline int Compare(const T& t1, const T& t2) {
  return t1 - t2;
}

template<>
inline int Compare(const string& t1, const string& t2) {
  return t1.compare(t2);
}

template<typename T>
inline int Compare(const optional<T>& t1, const optional<T>& t2) {
  if (!t1) {
    return t2 ? -1 : 0;
  } else if (!t2) {
    return 1;
  } else {
    return Compare(*t1, *t2);
  }
}
