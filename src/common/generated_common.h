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

class MyString {
  struct strview {
    const char* cs = nullptr;
    size_t sz = 0;
  };
  union {
    strview strv;
    const string* ptr;
    string own;
  };
  enum Type {VIEW, PTR, OWN};
  Type type = VIEW;
  inline void setStrView(const char* cs, size_t sz) noexcept {
    this->type = VIEW;
    this->strv.cs = cs;
    this->strv.sz = sz;
  }
  inline void setStrPtr(const string* ptr) noexcept {
    this->type = PTR;
    this->ptr = ptr;
  }
  inline void setStrOwn(const string& str) {
    this->type = OWN;
    new (&this->own) string(str);
  }
  inline void setStrOwn(string&& str) {
    this->type = OWN;
    new (&this->own) string(move(str));
  }
  inline void destroy() noexcept {
    if (type == OWN) own.~string();
  }
  inline string copyView() const {
    return string(strv.cs, strv.sz);
  }
  inline void copyFromOther(const MyString& other) {
    switch (other.type) {
    case VIEW: setStrView(other.strv.cs, other.strv.sz); break;
    case PTR:  setStrPtr(other.ptr); break;
    case OWN:  setStrOwn(other.own); break;
    }
  }
  inline void moveFromOther(MyString&& other) {
    switch (other.type) {
    case VIEW: setStrView(other.strv.cs, other.strv.sz); break;
    case PTR:  setStrPtr(other.ptr); break;
    case OWN:  setStrOwn(move(other.own)); break;
    }
  }
public:
  MyString()                            noexcept { }
  MyString(const char* cs, size_t sz)   noexcept { setStrView(cs, sz); }
  explicit MyString(const string* ptr)  noexcept { setStrPtr(ptr); }
  explicit MyString(const string& str)           { setStrOwn(str); }
  explicit MyString(string&& str)                { setStrOwn(move(str)); }
  MyString(const MyString& other)                { copyFromOther(other); }
  MyString(MyString&& other)                     { moveFromOther(move(other)); }
  MyString& operator=(const MyString& other)     { destroy(); copyFromOther(other); return *this; }
  MyString& operator=(MyString&& other) noexcept { destroy(); moveFromOther(move(other)); return *this; }
  ~MyString()                           noexcept { destroy(); }
  inline string toString() const {
    switch (type) {
    case VIEW: return copyView();
    case PTR:  return *ptr;
    case OWN:  return own;
    default:   THROW();
    }
  }
  inline size_t size() const {
    switch (type) {
    case VIEW: return strv.sz;
    case PTR:  return ptr->size();
    case OWN:  return own.size();
    default:   THROW();
    }
  }
  friend std::ostream& operator<< (std::ostream& stream, const MyString& v) {
    switch (v.type) {
    case VIEW: stream << v.copyView(); break;
    case PTR:  stream << (*(v.ptr)); break;
    case OWN:  stream << v.own; break;
    }
    return stream;
  }
  bool regexMatch(const std::regex& r) const {
    switch (type) {
    case VIEW: return std::regex_match(copyView(), r);
    case PTR:  return std::regex_match(*ptr, r);
    case OWN:  return std::regex_match(own, r);
    default:   THROW();
    }
  }
#define MYSTRINGOP(op, ret)\
  ret operator op(const MyString& other) const {\
  switch (type) {\
  case VIEW: {\
    switch (other.type) {\
    case VIEW: return copyView() op other.copyView();\
    case PTR:  return copyView() op (*(other.ptr));\
    case OWN:  return copyView() op other.own;\
    default:   THROW();\
    }\
  }\
  case PTR:  {\
    switch (other.type) {\
    case VIEW: return (*ptr) op other.copyView();\
    case PTR:  return (*ptr) op (*(other.ptr));\
    case OWN:  return (*ptr) op other.own;\
    default:   THROW();\
    }\
  }\
  case OWN:  {\
    switch (other.type) {\
    case VIEW: return own op other.copyView();\
    case PTR:  return own op (*(other.ptr));\
    case OWN:  return own op other.own;\
    default:   THROW();\
    }\
  }\
  default:   THROW();\
  }\
  }
  MYSTRINGOP(==, bool)\
  MYSTRINGOP(<, bool)\
  MYSTRINGOP(<=, bool)\
  MYSTRINGOP(>, bool)\
  MYSTRINGOP(>=, bool)\
  MYSTRINGOP(+, string)\
};

#define MYSTRINGSTRINGOP(op, ret)\
  inline ret operator op(const MyString& mys, const string& s) {\
    return mys op MyString(&s);\
  }\
  inline ret operator op(const string& s, const MyString& mys) {\
    return MyString(&s) op mys;\
  }\

MYSTRINGSTRINGOP(==, bool)\
MYSTRINGSTRINGOP(<, bool)\
MYSTRINGSTRINGOP(<=, bool)\
MYSTRINGSTRINGOP(>, bool)\
MYSTRINGSTRINGOP(>=, bool)\
MYSTRINGSTRINGOP(+, string)\

template<typename T>
inline string Stringify(const T& t) {
  return to_string(t);
}

template<typename T>
inline string Stringify(const T* t) {
  return t ? Stringify<T>(*t) : "NULL";
}

template<typename T>
inline string Stringify(const optional<T>& t) {
  return t ? Stringify<T>(*t) : "NULL";
}

template<> inline string Stringify(const bool& t) {return t ? "true" : "false";}
template<> inline string Stringify(const string& t) {return t;}
template<> inline string Stringify(const MyString& t) {return t.toString();}

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
inline bool IsNull(const T& t) {
  return !static_cast<bool>(t);
}

template<typename T>
inline bool IsNotNull(const T& t) {
  return !IsNull(t);
}

template<typename L, typename R>
inline bool Eq(const L& l, const R& r) {
  return (!l && !r) || (l && r && (*l == *r));
}

template<typename L, typename R>
inline bool Ne(const L& l, const R& r) {
  return !Eq(l, r);
}

template<typename L, typename R>
inline bool Lt(const L& l, const R& r) {
  return (!l && r) || (l && r && (*l < *r));
}

template<typename L, typename R>
inline bool Le(const L& l, const R& r) {
  return !Lt(r, l);
}

template<typename L, typename R>
inline bool Gt(const L& l, const R& r) {
  return Lt(r, l);
}

template<typename L, typename R>
inline bool Ge(const L& l, const R& r) {
  return !Lt(l, r);
}

template<typename L, typename R>
inline bool Like(const L& l, const R& r) {
  return (!l && !r) || (l && r && std::regex_match(*l, std::regex(*r)));
}

template<typename L>
inline bool Like(const L& l, const std::regex& r) {
  return (l && std::regex_match(*l, r));
}

template<typename T>
inline auto Uminus(const T& t) {
  optional<decltype(-(*t))> o;
  if (t) o = -(*t);
  return o;
}

template<typename L, typename R>
inline auto Plus(const L& l, const R& r) {
  optional<decltype(*l + *r)> o;
  if (l && r) o = *l + *r;
  return o;
}

template<typename L, typename R>
inline auto Minus(const L& l, const R& r) {
  optional<decltype(*l - *r)> o;
  if (l && r) o = *l - *r;
  return o;
}

template<typename L, typename R>
inline auto Mult(const L& l, const R& r) {
  optional<decltype((*l) * (*r))> o;
  if (l && r) o = (*l) * (*r);
  return o;
}

template<typename L, typename R>
inline auto Divide(const L& l, const R& r) {
  optional<decltype(*l / *r)> o;
  if (l && r) o = *l / *r;
  return o;
}

template<typename T>
inline int compare(const T& t1, const T& t2) {
  if (t1 < t2) return -1;
  else if (t1 > t2) return 1;
  return 0;
}

template<>
inline int compare(const string& t1, const string& t2) {
  return t1.compare(t2);
}

template<typename T>
inline int Compare(const T& t1, const T& t2) {
  if (!t1) {
    return t2 ? -1 : 0;
  } else if (!t2) {
    return 1;
  } else {
    return compare(*t1, *t2);
  }
}

// T is the output, E is the element type of It
template<typename T, typename E, typename It>
class MyRangeIterator {
public:
  // Convert given E (may be nullptr) to T. Return pointer to converted T.
  // May modify second argument and return its address.
  using ConvertFnType = function<const T*(const E*, T&)>;

  class ConstIterator {
  public:
    static ConstIterator mk_normal(const ConvertFnType* fn, It* curr) {
      ConstIterator it;
      it.fn = fn;
      it.singular = 0;
      it.curr = curr;
      it.setElemPtr();
      return it;
    }

    static ConstIterator mk_singular(
        const ConvertFnType* fn, bool begin, const E* singular_elem=nullptr) {
      ConstIterator it;
      it.fn = fn;
      it.singular = begin ? 1 : 2;
      it.singular_elem = singular_elem;
      it.setElemPtr();
      return it;
    }

    inline void setElemPtr() {
      switch (singular) {
      case 0: elemptr = (*fn)(&**curr, elem); break;
      case 1: elemptr = (*fn)(singular_elem, elem); break;
      case 2: elemptr = nullptr;
      }
    }

    inline ConstIterator& operator++() {
      if (singular == 0) {
        ++(*curr);
      } else {
        singular = 2;
        singular_elem = nullptr;
      }
      setElemPtr();
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
      return elemptr;
    }

    friend std::ostream& operator<< (std::ostream& stream, const ConstIterator& it) {
      stream << *it;
      return stream;
    }

  private:
    const ConvertFnType* fn;
    It* curr = nullptr;
    const E* singular_elem = nullptr;
    int singular = 0; // 0 for normal case, 1 for begin, 2 for end
    T elem;
    const T* elemptr;
  };

  static MyRangeIterator mk_singular(
      const ConvertFnType& fn, const E* singular_elem = nullptr) {
    MyRangeIterator it;
    it.fn = fn;
    it.singular = true;
    it.singular_elem = singular_elem;
    return it;
  }

  static MyRangeIterator mk_normal(
      const ConvertFnType& fn, It&& orig_begin, It&& orig_end) {
    MyRangeIterator it;
    it.fn = fn;
    it.singular = false;
    it.singular_elem = nullptr;
    it.orig_begin = orig_begin;
    it.orig_end = orig_end;
    return it;
  }

  ConstIterator begin() const {
    return (!singular && (orig_begin != orig_end))
        ? ConstIterator::mk_normal(&fn, (It*) &orig_begin)
        : ConstIterator::mk_singular(&fn, true, singular_elem);
  }

  ConstIterator end() const {
    return (!singular && (orig_begin != orig_end))
        ? ConstIterator::mk_normal(&fn, (It*) &orig_end)
        : ConstIterator::mk_singular(&fn, false);
  }

private:
  ConvertFnType fn;
  bool singular;
  const E* singular_elem;
  It orig_begin;
  It orig_end;
};

class BaseIterators {
public:
  template<typename T>
  static
  MyRangeIterator<T, T, typename vector<T>::const_iterator>
  mk_vec_iterator(const vector<T>* v) {
    auto convert_fn = [](const T* t, T&) {return t;};
    if (v) {
      return MyRangeIterator<T, T, typename vector<T>::const_iterator>::mk_normal(
          convert_fn, v->begin(), v->end());
    } else {
      return MyRangeIterator<T, T, typename vector<T>::const_iterator>::mk_singular(
          convert_fn);
    }
  }
};

