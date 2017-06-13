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
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/status.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/gzip_stream.h>
#include <google/protobuf/util/json_util.h>
#include "global_include.h"
#include "generated_common.h"

using namespace std;

using int8   = ::google::protobuf::int8;
using int16  = ::google::protobuf::int16;
using int32  = ::google::protobuf::int32;
using int64  = ::google::protobuf::int64;
using uint8  = ::google::protobuf::uint8;
using uint16 = ::google::protobuf::uint16;
using uint32 = ::google::protobuf::uint32;
using uint64 = ::google::protobuf::uint64;

bool GetProtoFromString(const string& data, google::protobuf::Message& proto) {
  if (data.size() && ((data[0] == '{') || (data[0] == '['))) {
    // First try json format, then try binary format
    return google::protobuf::util::JsonStringToMessage(data, &proto).ok() ||
           proto.ParseFromString(data);
  } else {
    // First try binary format, else try json format
    return proto.ParseFromString(data) ||
           google::protobuf::util::JsonStringToMessage(data, &proto).ok();
  }
}

void ReadFromZeroCopyInputStream(
    google::protobuf::io::ZeroCopyInputStream& steam, string& data) {
  const void* buffer;
  int size;
  while (steam.Next(&buffer, &size)) {
    data.append((const char*) buffer, size);
  }
}

void GetProtoFromFile(const string& file, google::protobuf::Message& proto) {
  int fd = open(file.c_str(), O_RDONLY);
  ASSERT(fd >= 0, "Unable to open file", file);
  string data;
  if ((file.size() > 3) && (file.substr(file.size()-3) == ".gz")) {
    google::protobuf::io::FileInputStream fis(fd);
    google::protobuf::io::GzipInputStream gzfis(&fis);
    ReadFromZeroCopyInputStream(gzfis, data);
  } else {
    google::protobuf::io::FileInputStream fis(fd);
    ReadFromZeroCopyInputStream(fis, data);
  }
  ASSERT(close(fd) == 0, "Unable to close file", file);
  ASSERT(GetProtoFromString(data, proto), "Unable to parse", file);
}

#ifndef FROM
#define FROM(argc, argv, protos) \
ASSERT(argc > 1, "Proto file(s) not provided");\
for (int i=1; i<argc; i++) {\
  protos.emplace_back();\
  GetProtoFromFile(argv[i], protos.back());\
}
#endif

class Iterators : public BaseIterators {
public:
  template<typename T>
  static
  MyRangeIterator<T, typename ::google::protobuf::RepeatedField<T>::const_iterator>
  mk_pb_iterator(const ::google::protobuf::RepeatedField<T>* rf) {
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
  mk_pb_iterator(const ::google::protobuf::RepeatedPtrField<T>* rpf) {
    if (rpf) {
      return MyRangeIterator<T, typename ::google::protobuf::RepeatedPtrField<T>::const_iterator>(
          rpf->begin(), rpf->end());
    } else {
      return MyRangeIterator<T, typename ::google::protobuf::RepeatedPtrField<T>::const_iterator>();
    }
  }
};

