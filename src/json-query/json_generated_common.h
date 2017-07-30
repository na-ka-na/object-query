/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

#pragma once

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <tuple>
#include <type_traits>
#include <vector>
#include <regex>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include "global_include.h"
#include "generated_common.h"

using namespace std;

class JsonValue {
  union {
    double d;
    int64_t i;
    uint64_t u;
    MyString str;
    bool b;
    const rapidjson::Value* rjv;
  };
  enum Type { NONE, DOUBLE, INT, INT64, UINT, UINT64, STR, BOOL, OBJ, ARR, NILL};
  Type t;

  void copyFrom(const JsonValue& other) {
    t = other.t;
    switch (t) {
    case DOUBLE   : d = other.d; break;
    case INT      : i = other.i; break;
    case INT64    : i = other.i; break;
    case UINT     : u = other.u; break;
    case UINT64   : u = other.u; break;
    case STR      : str = other.str; break;
    case BOOL     : b = other.b; break;
    case NILL     : break;
    case OBJ      : rjv = other.rjv; break;
    case ARR      : rjv = other.rjv; break;
    default       : THROW("Invalid state", t);
    }
  }

  void moveFrom(JsonValue&& other) {
    t = other.t;
    switch (t) {
    case DOUBLE   : d = other.d; break;
    case INT      : i = other.i; break;
    case INT64    : i = other.i; break;
    case UINT     : u = other.u; break;
    case UINT64   : u = other.u; break;
    case STR      : str = move(other.str); break;
    case BOOL     : b = other.b; break;
    case NILL     : break;
    case OBJ      : rjv = other.rjv; break;
    case ARR      : rjv = other.rjv; break;
    default       : THROW("Invalid state", t);
    }
  }

  void destroy() noexcept {
    if (t == STR) str.~MyString();
  }

#define NUMBEROP(op, ret)\
  /* http://en.cppreference.com/w/cpp/language/operator_arithmetic */\
  switch (t) {\
  case BOOL:\
    switch (other.t) {\
    case BOOL   : return ret(b op other.b);\
    case DOUBLE : return ret(double(b) op other.d);\
    case INT    : return ret(int(b) op other.i);\
    case INT64  : return ret(int64_t(b) op other.i);\
    case UINT   : return ret(unsigned(b) op other.u);\
    case UINT64 : return ret(uint64_t(b) op other.u);\
    default     : break;\
    }\
    break;\
  case DOUBLE:\
    switch (other.t) {\
    case BOOL   : return ret(d op double(other.b));\
    case DOUBLE : return ret(d op other.d);\
    case INT    : return ret(d op double(other.i));\
    case INT64  : return ret(d op double(other.i));\
    case UINT   : return ret(d op double(other.u));\
    case UINT64 : return ret(d op double(other.u));\
    default     : break;\
    }\
    break;\
  case INT:\
    switch (other.t) {\
    case BOOL   : return ret(i op int(other.b));\
    case DOUBLE : return ret(double(i) op other.d);\
    case INT    : return ret(i op other.i);\
    case INT64  : return ret(i op other.i);\
    case UINT   : return ret(unsigned(i) op other.u);\
    case UINT64 : return ret(uint64_t(i) op other.u);\
    default     : break;\
    }\
    break;\
  case INT64:\
    switch (other.t) {\
    case BOOL   : return ret(i op int64_t(other.b));\
    case DOUBLE : return ret(double(i) op other.d);\
    case INT    : return ret(i op other.i);\
    case INT64  : return ret(i op other.i);\
    case UINT   : return ret(i op int64_t(other.u));\
    case UINT64 : return ret(uint64_t(i) op other.u);\
    default     : break;\
    }\
    break;\
  case UINT:\
    switch (other.t) {\
    case BOOL   : return ret(u op unsigned(other.b));\
    case DOUBLE : return ret(double(u) op other.d);\
    case INT    : return ret(u op unsigned(other.i));\
    case INT64  : return ret(int64_t(u) op other.i);\
    case UINT   : return ret(u op other.u);\
    case UINT64 : return ret(u op other.u);\
    default     : break;\
    }\
    break;\
  case UINT64:\
    switch (other.t) {\
    case BOOL   : return ret(u op uint64_t(other.b));\
    case DOUBLE : return ret(double(u) op other.d);\
    case INT    : return ret(u op uint64_t(other.i));\
    case INT64  : return ret(u op uint64_t(other.i));\
    case UINT   : return ret(u op other.u);\
    case UINT64 : return ret(u op other.u);\
    default     : break;\
    }\
    break;\
  default: break;\
  }\

#define STROP(op, ret)\
  if ((t == STR) && (other.t == STR)) return ret(str op other.str);

#define NULLOP(ret1, ret2, ret3)\
  if (t == NILL) {\
    if (other.t == NILL) return ret1;\
    else return ret2;\
  } else if (other.t == NILL) {\
    return ret3;\
  }\

#define THROWINVALID(op) THROW("Invalid op:" #op, t, other.t);

#define COMPAREOP(op, nullret1, nullret2, nullret3)\
  bool operator op(const JsonValue& other) const {\
    NULLOP(nullret1, nullret2, nullret3)\
    STROP(op, bool)\
    NUMBEROP(op, bool)\
    THROWINVALID(op)\
  }\

#define COMPAREOPS()\
  COMPAREOP(==, true, false, false)\
  COMPAREOP(<, false, true, false)\
  COMPAREOP(<=, true, true, false)\
  COMPAREOP(>, false, false, true)\
  COMPAREOP(>=, true, false, true)\

#define PLUSOP()\
  JsonValue operator +(const JsonValue& other) const {\
    NULLOP(JsonValue(), JsonValue(), JsonValue())\
    STROP(+, JsonValue)\
    NUMBEROP(+, JsonValue)\
    THROWINVALID(op)\
  }\

#define ARITHOP(op)\
  JsonValue operator op(const JsonValue& other) const {\
    NULLOP(JsonValue(), JsonValue(), JsonValue())\
    NUMBEROP(op, JsonValue)\
    THROWINVALID(op)\
  }\

#define ARITHMETICOPS()\
  PLUSOP()\
  ARITHOP(-)\
  ARITHOP(*)\
  ARITHOP(/)\

public:
  explicit JsonValue(double d)   : t(DOUBLE)   {this->d = d;}
  explicit JsonValue(int i)      : t(INT)      {this->i = i;}
  explicit JsonValue(int64_t i)  : t(INT64)    {this->i = i;}
  explicit JsonValue(unsigned u) : t(UINT)     {this->u = u;}
  explicit JsonValue(uint64_t u) : t(UINT64)   {this->u = u;}
  explicit JsonValue(const string& s)   : t(STR) {this->str = MyString(s);}
  explicit JsonValue(string&& s)        : t(STR) {this->str = MyString(move(s));}
  explicit JsonValue(const MyString& s) : t(STR) {this->str = s;}
  explicit JsonValue(MyString&& s)      : t(STR) {this->str = move(s);}
  explicit JsonValue(bool b)     : t(BOOL)     {this->b = b;}
  explicit JsonValue()           : t(NILL)     {}
  explicit JsonValue(const rapidjson::Value& v) {
    switch (v.GetType()) {
      case rapidjson::kNullType:   t = NILL; break;
      case rapidjson::kFalseType:  t = BOOL; b = false; break;
      case rapidjson::kTrueType:   t = BOOL; b = true; break;
      case rapidjson::kObjectType: t = OBJ;  rjv = &v; break;
      case rapidjson::kArrayType:  t = ARR;  rjv = &v; break;
      case rapidjson::kStringType: t = STR;  str = MyString(v.GetString(), v.GetStringLength()); break;
      case rapidjson::kNumberType: {
             if (v.IsDouble()) {t = DOUBLE; d = v.GetDouble();}
        else if (v.IsInt())    {t = INT;    i = v.GetInt();   }
        else if (v.IsUint())   {t = UINT;   u = v.GetUint();  }
        else if (v.IsInt64())  {t = INT64;  i = v.GetInt64(); }
        else if (v.IsUint64()) {t = UINT64; u = v.GetUint64();}
      } break;
      default: THROW("Unable to understand json type", v.GetType());
    }
  }
  JsonValue(const JsonValue& other)            {copyFrom(other);}
  JsonValue(JsonValue&& other)                 {moveFrom(move(other));}
  JsonValue& operator=(const JsonValue& other) {destroy(); copyFrom(other); return *this;}
  JsonValue& operator=(JsonValue&& other)      {destroy(); moveFrom(move(other)); return *this;}
  ~JsonValue()                                 {destroy();}
  COMPAREOPS()
  ARITHMETICOPS()
  inline void Print(std::ostream& stream) const {
    switch (t) {
    case DOUBLE : stream << to_string(d); break;
    case INT    : stream << to_string(i); break;
    case INT64  : stream << to_string(i); break;
    case UINT   : stream << to_string(u); break;
    case UINT64 : stream << to_string(u); break;
    case STR    : str.Print(stream); break;
    case BOOL   : stream << (b ? "true" : "false"); break;
    case NILL   : stream << "null"; break;
    case OBJ    : /* fallthrough */
    case ARR    : {
      rapidjson::StringBuffer strbuf;
      rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
      rjv->Accept(writer);
      stream << strbuf.GetString();
      break;
    }
    default: THROW("Invalid state");
    }
  }
  inline size_t PrintSize() const {
    if (t == STR) return str.PrintSize();
    stringstream ss;
    Print(ss);
    return ss.str().size();
  }
  friend ostream& operator<<(ostream& stream, const JsonValue& v) {
    v.Print(stream);
    return stream;
  }
  int  getType()  const { return t; }
  bool isObject() const { return t == OBJ; }
  bool isArray()  const { return t == ARR; }
  const rapidjson::Value* getObject() const { ASSERT(isObject()); return rjv; }
  const rapidjson::Value* getArray() const  { ASSERT(isArray()); return rjv; }
  bool regexMatch(const std::regex& r) const {
    if (t == STR) return str.regexMatch(r);
    else          return false;
  }
};

#define JSONVALUENUMBEROP(type, op, ret)\
  inline ret operator op(const JsonValue& value, type n) {\
    return value op JsonValue(n);\
  }\
  inline ret operator op(type n, const JsonValue& value) {\
    return JsonValue(n) op value;\
  }\

#define JSONVALUEOTHEROP(type, op, ret)\
  inline ret operator op(const JsonValue& value, const type& n) {\
    return value op JsonValue(n);\
  }\
  inline ret operator op(const type& n, const JsonValue& value) {\
    return JsonValue(n) op value;\
  }\

#define JSONVALUENUMBEROPS(type)\
  JSONVALUENUMBEROP(type, ==, bool)\
  JSONVALUENUMBEROP(type, <, bool)\
  JSONVALUENUMBEROP(type, >, bool)\
  JSONVALUENUMBEROP(type, >=, bool)\
  JSONVALUENUMBEROP(type, <=, bool)\
  JSONVALUENUMBEROP(type, +, JsonValue)\
  JSONVALUENUMBEROP(type, -, JsonValue)\
  JSONVALUENUMBEROP(type, *, JsonValue)\
  JSONVALUENUMBEROP(type, /, JsonValue)\

#define JSONVALUESTRINGOPS()\
  JSONVALUEOTHEROP(string, ==, bool)\
  JSONVALUEOTHEROP(string, <, bool)\
  JSONVALUEOTHEROP(string, >, bool)\
  JSONVALUEOTHEROP(string, <=, bool)\
  JSONVALUEOTHEROP(string, >=, bool)\
  JSONVALUEOTHEROP(string, +, JsonValue)\

JSONVALUENUMBEROPS(double)
JSONVALUENUMBEROPS(int)
JSONVALUENUMBEROPS(unsigned)
JSONVALUENUMBEROPS(int64_t)
JSONVALUENUMBEROPS(uint64_t)
JSONVALUESTRINGOPS()

template<>
void Print(std::ostream& stream, const JsonValue& t) {
  t.Print(stream);
}

template<>
inline size_t PrintSize(const JsonValue& value) {
  return value.PrintSize();
}

template<>
inline bool Like(const optional<JsonValue>& l, const std::regex& r) {
  return (l && l->regexMatch(r));
}

void GetJsonObjectFromFile(const string& file, rapidjson::Document& doc) {
  FILE* fp = fopen(file.c_str(), "r");
  ASSERT(static_cast<bool>(fp), "Unable to open file", file);
  size_t bufSize = 256 * 1024;
  char buf[bufSize];
  rapidjson::FileReadStream stream(fp, buf, bufSize);
  doc.ParseStream(stream);
  fclose(fp);
  ASSERT(!doc.HasParseError(), "Malformed json at offset:",
         doc.GetErrorOffset(), "error:", doc.GetParseError());
}

#ifndef FROM
#define FROM(argc, argv, objects) \
ASSERT(argc > 1, "Json file(s) not provided");\
for (int i=1; i<argc; i++) {\
  objects.emplace_back();\
  GetJsonObjectFromFile(argv[i], objects.back());\
}
#endif

class Iterators : public BaseIterators {
public:
  using JsonRangeIterator = MyRangeIterator<JsonValue, rapidjson::Value, rapidjson::Value::ConstValueIterator>;

  static JsonRangeIterator mk_json_iterator(const string& path, const JsonValue* value, const string& key) {
    auto convert_fn = [](const rapidjson::Value* t, JsonValue& value) -> const JsonValue* {
      if (t == nullptr) return nullptr;
      value = JsonValue(*t);
      return &value;
    };
    if (value) {
      if (!value->isObject()) {
        cerr << "Cannot lookup " << key << " at " << path
             << " since type is not object " << value->getType() << endl;
        return JsonRangeIterator::mk_singular(convert_fn);
      }
      const rapidjson::Value* obj = value->getObject(); ASSERT(obj);
      rapidjson::Value::ConstMemberIterator it = obj->FindMember(key.c_str());
      if (it == obj->MemberEnd()) {
        return JsonRangeIterator::mk_singular(convert_fn);
      } else if (it->value.IsArray()) {
        return JsonRangeIterator::mk_normal(convert_fn, it->value.Begin(), it->value.End());
      } else {
        return JsonRangeIterator::mk_singular(convert_fn, &(it->value));
      }
    } else {
      return JsonRangeIterator::mk_singular(convert_fn);
    }
  }

  using JsonDocumentIterator = MyRangeIterator<JsonValue, rapidjson::Document, typename vector<rapidjson::Document>::const_iterator>;

  static JsonDocumentIterator mk_json_iterator(const vector<rapidjson::Document>& documents) {
    auto convert_fn = [](const rapidjson::Document* t, JsonValue& value) -> const JsonValue* {
      if (t == nullptr) return nullptr;
      value = JsonValue(*t);
      return &value;
    };
    return JsonDocumentIterator::mk_normal(convert_fn, documents.cbegin(), documents.cend());
  }
};

