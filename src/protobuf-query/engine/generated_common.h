#pragma once

#include <fcntl.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <tuple>
#include <type_traits>
#include <vector>
#include <experimental/optional>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/gzip_stream.h>

using namespace std;

template<typename T>
using optional = std::experimental::optional<T>;

using int32  = ::google::protobuf::int32;
using int64  = ::google::protobuf::int64;
using uint32 = ::google::protobuf::uint32;
using uint64 = ::google::protobuf::uint64;

void parsePbFromFile(const string& file, google::protobuf::Message& proto) {
  int fd = open(file.c_str(), O_RDONLY);
  if (fd < 0) {
    throw runtime_error("Unable to open file " + file);
  }
  bool ret = false;
  if (file.rfind(".gz") != string::npos) {
    google::protobuf::io::FileInputStream fis(fd);
    google::protobuf::io::GzipInputStream gzfis(&fis);
    ret = proto.ParseFromZeroCopyStream(&gzfis);
  } else {
    ret = proto.ParseFromFileDescriptor(fd);
  }
  close(fd);
  if (!ret) {
    throw runtime_error("Unable to parse " + file);
  }
}

template<typename T> inline string stringify(T t) {
  throw runtime_error(string("stringify not implemented ") +
                      typeid(t).name());
}
template<> inline string stringify(int32 t) { return to_string(t); }
template<> inline string stringify(int64 t) { return to_string(t); }
template<> inline string stringify(uint32 t) { return to_string(t); }
template<> inline string stringify(uint64 t) { return to_string(t); }
template<> inline string stringify(double t) { return to_string(t); }
template<> inline string stringify(float t) { return to_string(t); }
template<> inline string stringify(bool t) { return t ? "true" : "false"; }
template<> inline string stringify(string t) { return t; }

template<typename T> inline string stringify(const optional<T>& t) {
  return t ? stringify<T>(*t) : "NULL";
}
