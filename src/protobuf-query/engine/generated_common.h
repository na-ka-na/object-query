/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

#pragma once

#include <fcntl.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <tuple>
#include <type_traits>
#include <vector>
#include <regex>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
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

#ifndef FROM
#define FROM(argc, argv, protos) \
ASSERT(argc > 1, "Proto binary file not provided");\
protos.emplace_back();\
ParsePbFromFile(argv[1], protos.back());
#endif

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
inline int Intify(const T& t) {
  return (int) t;
}

template<> inline int Intify(const string& t) {
  return stoi(t);
}

template<typename T>
inline optional<int> ToInt(const optional<T>& t) {
  optional<int> i;
  if (t) {
    i = Intify(*t);
  }
  return i;
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

inline bool Like(const optional<string>& l, const optional<string>& r) {
  return (!l && !r) || (l && r && std::regex_match(*l, std::regex(*r)));
}

inline bool Like(const optional<string>& l, const std::regex& r) {
  return (l && std::regex_match(*l, r));
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

template<typename T, typename It>
class MyRangeIterator {
public:
  class ConstIterator {
  public:
    static ConstIterator mk_normal(It* curr) {
      ConstIterator it;
      it.curr = curr;
      return it;
    }

    static ConstIterator mk_singular(bool begin) {
      ConstIterator it;
      it.singular = begin ? 1 : 2;
      return it;
    }

    inline ConstIterator& operator++() {
      if (singular == 0) {
        ++(*curr);
      } else {
        singular = 2;
      }
      return *this;
    }

    inline bool operator==(const ConstIterator& other) const {
      if (singular == 0) {
        return (other.singular == 0) && (*curr == *(other.curr));
      } else {
        return singular == other.singular;
      }
    }

    inline bool operator!=(const ConstIterator& other) const {
      return !(*this == other);
    }

    inline const T* operator*() const {
      if (singular == 0) {
        return &**curr;
      } else {
        return nullptr;
      }
    }

    friend std::ostream& operator<< (std::ostream& stream, const ConstIterator& it) {
      if (it.singular == 0) {
        stream << *it;
      } else {
        stream << ((it.singular == 1) ? "singular_begin" : "singular_end");
      }
      return stream;
    }

  private:
    It* curr = nullptr;
    int singular = 0; // 0 for normal case, 1 for begin, 2 for end
  };

  MyRangeIterator() : singular(true) {}

  MyRangeIterator(It&& orig_begin, It&& orig_end) :
      singular(false), orig_begin(orig_begin), orig_end(orig_end) {}

  ConstIterator begin() const {
    return (!singular && (orig_begin != orig_end))
        ? ConstIterator::mk_normal((It*) &orig_begin)
        : ConstIterator::mk_singular(true);
  }

  ConstIterator end() const {
    return (!singular && (orig_begin != orig_end))
        ? ConstIterator::mk_normal((It*) &orig_end)
        : ConstIterator::mk_singular(false);
  }

private:
  bool singular;
  It orig_begin;
  It orig_end;
};

class Iterators {
public:
  template<typename T>
  static
  MyRangeIterator<T, typename ::google::protobuf::RepeatedField<T>::const_iterator>
  mk_iterator(const ::google::protobuf::RepeatedField<T>* rf) {
    if (rf) {
      return MyRangeIterator<T, typename ::google::protobuf::RepeatedField<T>::const_iterator>(
          rf->begin(), rf->end());
    } else {
      return MyRangeIterator<T, typename ::google::protobuf::RepeatedField<T>::const_iterator>();
    }
  }

  template<typename T>
  static
  MyRangeIterator<T, typename ::google::protobuf::RepeatedPtrField<T>::const_iterator>
  mk_iterator(const ::google::protobuf::RepeatedPtrField<T>* rpf) {
    if (rpf) {
      return MyRangeIterator<T, typename ::google::protobuf::RepeatedPtrField<T>::const_iterator>(
          rpf->begin(), rpf->end());
    } else {
      return MyRangeIterator<T, typename ::google::protobuf::RepeatedPtrField<T>::const_iterator>();
    }
  }

  template<typename T>
  static
  MyRangeIterator<T, typename vector<T>::const_iterator>
  mk_iterator(const vector<T>* v) {
    if (v) {
      return MyRangeIterator<T, typename vector<T>::const_iterator>(
          v->begin(), v->end());
    } else {
      return MyRangeIterator<T, typename vector<T>::const_iterator>();
    }
  }
};

