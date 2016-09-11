#include <fstream>
#include <stdio.h>
#include <regex>
#include "query_engine.h"

using namespace std;

vector<ProtoSpec> parseProtoSpecFile(const string& protoSpecFile) {
  vector<ProtoSpec> protos;
  ProtoSpec proto;
  ifstream file(protoSpecFile);
  string line;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }
    if (line.find("----------") == 0) {
      protos.push_back(proto);
      continue;
    }
    static regex kvRegex("(\\w+)\\s+(\\S+)");
    smatch match;
    ASSERT(regex_match(line, match, kvRegex), "Invalid line", line);
    string key = match[1].str();
    string value = match[2].str();
    if (key == "protoName") {
      proto.protoName = value;
    } else if (key == "cppProtoNamespace") {
      proto.cppProtoNamespace = value;
    } else if (key == "cppProtoInclude") {
      proto.cppProtoInclude = value;
    } else if (key == "cppExtraInclude") {
      proto.cppExtraInclude = value;
    } else {
      THROW("Invalid key", key, "in line", line);
    }
  }
  return protos;
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

void runQuery(string generatedFileName, int argc, char** argv) {
  string cmd = "./" + generatedFileName;
  for (int i=0; i<argc; i++) {
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
  if (argc < 4) {
    cerr << "Usage: ./RunQuery <proto-spec-file> <sql-query> "
         << "<generated-query-file-name> [arguments for generated query ...]"
         << endl;
    exit(1);
  }
  vector<ProtoSpec> protos = parseProtoSpecFile(argv[1]);
  string rawSql = argv[2];
  string generatedFileName = argv[3];
  string thisFile = __FILE__;
  auto idx = thisFile.rfind("/");
  string currentDir = thisFile.substr(0, idx);
  string generatedFile = currentDir + "/" + generatedFileName + ".cc";
  ofstream generated(generatedFile, ios::out | ios::trunc);
  QueryEngine engine(protos, rawSql, generated);
  engine.process();
  runMake();
  runQuery(generatedFileName, argc-4, argv+4);
}
