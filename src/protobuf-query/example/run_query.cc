
#include "query_engine.h"
#include "example1.pb.h"

using namespace std;

void Proto::initProto(const string& protoName, Proto& proto) {
  if (protoName == "Example1.Company") {
    static Example1::Company defaultInstance;
    proto.defaultInstance = &defaultInstance;
    proto.protoNamespace = "Example1";
    proto.protoHeaderInclude = "example1.pb.h";
  } else {
    throw runtime_error("No mapping defined for protoName: " + protoName);
  }
}

int main(int argc, char** argv) {
  if (argc < 2) {
    cerr << "Usage: ./QueryEngine <sql-query>" << endl;
    exit(1);
  }
  QueryEngine engine(argv[1], cout);
  engine.process();
}
