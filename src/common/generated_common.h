/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

#pragma once

#include <stdio.h>
#include <fcntl.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <tuple>
#include <type_traits>
#include <vector>
#include <regex>
#include "global_include.h"

using namespace std;

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
inline string STR(const T& t) {
  return Stringify(t);
}

template<typename T>
inline int Intify(const T& t) {
  return (int) t;
}

template<> inline int Intify(const string& t) {
  return stoi(t);
}

template<typename T>
inline int INT(const T& t) {
  return Intify(t);
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

    static ConstIterator mk_singular(bool begin, const T* singular_elem=nullptr) {
      ConstIterator it;
      it.singular = begin ? 1 : 2;
      it.singular_elem = singular_elem;
      return it;
    }

    inline ConstIterator& operator++() {
      if (singular == 0) {
        ++(*curr);
      } else {
        singular = 2;
        singular_elem = nullptr;
      }
      return *this;
    }

    inline bool operator==(const ConstIterator& other) const {
      if ((singular == 0) && (other.singular == 0)) {
        return (*curr) == (*other.curr);
      } else if ((singular == 1) && (other.singular == 1)) {
        return singular_elem == other.singular_elem;
      } else if ((singular == 2) && (other.singular == 2)) {
        return true;
      }
      return false;
    }

    inline bool operator!=(const ConstIterator& other) const {
      return !(*this == other);
    }

    inline const T* operator*() const {
      switch (singular) {
      case 0: return &**curr;
      case 1: return singular_elem;
      default: return nullptr;
      }
    }

    friend std::ostream& operator<< (std::ostream& stream, const ConstIterator& it) {
      stream << *it;
      return stream;
    }

  private:
    It* curr = nullptr;
    const T* singular_elem = nullptr;
    int singular = 0; // 0 for normal case, 1 for begin, 2 for end
  };

  MyRangeIterator(const T* singular_elem = nullptr) :
      singular(true), singular_elem(singular_elem) {}

  MyRangeIterator(It&& orig_begin, It&& orig_end) :
      singular(false), singular_elem(nullptr),
      orig_begin(orig_begin), orig_end(orig_end) {}

  ConstIterator begin() const {
    return (!singular && (orig_begin != orig_end))
        ? ConstIterator::mk_normal((It*) &orig_begin)
        : ConstIterator::mk_singular(true, singular_elem);
  }

  ConstIterator end() const {
    return (!singular && (orig_begin != orig_end))
        ? ConstIterator::mk_normal((It*) &orig_end)
        : ConstIterator::mk_singular(false);
  }

private:
  bool singular;
  const T* singular_elem;
  It orig_begin;
  It orig_end;
};

class BaseIterators {
public:
  template<typename T>
  static
  MyRangeIterator<T, typename vector<T>::const_iterator>
  mk_vec_iterator(const vector<T>* v) {
    if (v) {
      return MyRangeIterator<T, typename vector<T>::const_iterator>(
          v->begin(), v->end());
    } else {
      return MyRangeIterator<T, typename vector<T>::const_iterator>();
    }
  }
};

