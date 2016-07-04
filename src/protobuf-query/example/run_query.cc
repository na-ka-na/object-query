#include <fstream>
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
  string thisFile = __FILE__;
  auto idx = thisFile.rfind("/");
  string generatedFile = thisFile.substr(0, idx) + "/generated_query.cc";
  ofstream generated(generatedFile, ios::out | ios::trunc);
  QueryEngine engine(argv[1], generated);
  engine.process();
}
