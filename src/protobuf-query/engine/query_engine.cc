
#include <fcntl.h>
#include <string>
#include <tuple>
#include <algorithm>
#include <map>
#include <iostream>
#include <functional>
#include <google/protobuf/message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/gzip_stream.h>
#include "example1.pb.h"

using namespace std;
using namespace google::protobuf;

const Message& getDefaultPbInstance(const string& className) {
  if (className == "Example1.Aaa") {
    static Example1::Aaa proto;
    return proto;
  }
  throw runtime_error("No mapping defined for className: " + className);
}

int main(int /*argc*/, char** /*argv*/) {

  string className = "Example1.Aaa";
  const Message& proto = getDefaultPbInstance(className);

}

