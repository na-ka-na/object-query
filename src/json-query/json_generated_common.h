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

template<>
inline string Stringify(const rapidjson::Value* const& value) {
  if (value == nullptr) return "NULL";
  switch (value->GetType()) {
    case rapidjson::kNullType:   return "null";
    case rapidjson::kFalseType:  return Stringify<bool>(false);
    case rapidjson::kTrueType:   return Stringify<bool>(true);
    case rapidjson::kObjectType: /* fallthrough */
    case rapidjson::kArrayType:  {
      rapidjson::StringBuffer strbuf;
      rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
      value->Accept(writer);
      return strbuf.GetString();
    }
    case rapidjson::kStringType: return value->GetString();
    case rapidjson::kNumberType: {
        if (value->IsDouble())      return Stringify<double>(value->GetDouble());
        else if (value->IsInt())    return Stringify<int>(value->GetInt());
        else if (value->IsUint())   return Stringify<uint32_t>(value->GetUint());
        else if (value->IsInt64())  return Stringify<int64_t>(value->GetInt64());
        else if (value->IsUint64()) return Stringify<uint64_t>(value->GetUint64());
        else                        return "UnknownRapidJsonType" + to_string(value->GetType());
    }
  }
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
  static
  MyRangeIterator<rapidjson::Value, rapidjson::Value::ConstValueIterator>
  mk_rjv_iterator(const string& path, const rapidjson::Value* value, const string& key) {
    if (value) {
      if (!value->IsObject()) {
        cerr << "Cannot lookup " << key << " at " << path
             << " since type is not object " << value->GetType() << endl;
        return MyRangeIterator<rapidjson::Value, rapidjson::Value::ConstValueIterator>();
      }
      rapidjson::Value::ConstMemberIterator it = value->FindMember(key.c_str());
      if (it == value->MemberEnd()) {
        return MyRangeIterator<rapidjson::Value, rapidjson::Value::ConstValueIterator>();
      } else if (it->value.IsArray()) {
        return MyRangeIterator<rapidjson::Value, rapidjson::Value::ConstValueIterator>(
            it->value.Begin(), it->value.End());
      } else {
        return MyRangeIterator<rapidjson::Value, rapidjson::Value::ConstValueIterator>(
            &(it->value));
      }
    } else {
      return MyRangeIterator<rapidjson::Value, rapidjson::Value::ConstValueIterator>();
    }
  }
};

