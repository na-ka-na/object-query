#include <fstream>
#include <stdio.h>
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
    ASSERT(false, "No mapping defined for protoName:", protoName);
  }
}

void runMake() {
  FILE* fp = popen("make", "r");
  ASSERT(fp != NULL, "Unable to popen make");
  string output;
  char buffer[1024];
  while (fgets(buffer, 1024, fp) != NULL) {
    output += buffer;
  }
  ASSERT(pclose(fp)==0, "Make failed:\n", output);
}

void runQuery(int argc, char** argv) {
  string cmd = string("./") + argv[2];
  for (int i=3; i<argc; i++) {
    cmd += string(" ") + argv[i];
  }
  FILE* fp = popen(cmd.c_str(), "r");
  ASSERT(fp != NULL, "Unable to popen", cmd);
  char buffer[1024];
  while (fgets(buffer, 1024, fp) != NULL) {
    cout << buffer;
  }
  ASSERT(pclose(fp)==0, "Running query failed");
}

int main(int argc, char** argv) {
  if (argc < 3) {
    cerr << "Usage: ./QueryEngine <sql-query> <generated-query-file>"
         << " [arguments for generated query ...]" << endl;
    exit(1);
  }
  string thisFile = __FILE__;
  auto idx = thisFile.rfind("/");
  string generatedFile = thisFile.substr(0, idx) + "/" + argv[2] + ".cc";
  ofstream generated(generatedFile, ios::out | ios::trunc);
  QueryEngine engine(argv[1], generated);
  engine.process();
  runMake();
  runQuery(argc, argv);
}
