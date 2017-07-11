/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

#include <fstream>
#include <regex>
#include <stdio.h>
#include <dlfcn.h>
#include <gflags/gflags.h>
#include <cstdlib>
#include "utils.h"
#include "global_include.h"
#include "protobuf_query_engine.h"
#include "json_query_engine.h"

DEFINE_string(queryType, "", "\033[1mrequired\033[0m, either proto or json");

DEFINE_string(codeGenDir, ".",
              "optional, directory where generated code files are emitted, "
              "e.g. /tmp, directory must exist");
DEFINE_string(codeCompileDir, ".",
              "optional, directory where compiled binary is emitted, "
              "e.g. /tmp, directory must exist");
DEFINE_string(codeGenPrefix, "generated_query",
              "optional, prefix for generated code files and binary");

DEFINE_string(cppProtoHeader, "",
              "\033[1mrequired\033[0m, path/to/compiled/proto/header.pb.h. See "
              "src/protobuf-query/example/CMakeLists.txt for an example of how "
              "proto is compiled, you may add your proto there to build it.");
DEFINE_string(cppProtoLib, "",
              "\033[1mrequired\033[0m, path/to/built/proto/library.so which "
              "contains the proto defintion, either .so or .dylib");

DEFINE_string(cppExtraIncludes, "",
              "optional, comma separated extra headers to include in generated "
              "code, e.g. with FROM macro overriden, or custom function "
              "definitions.");
DEFINE_string(cppExtraLinkLibs, "",
              "optional, comma separated extra lib names to link with generated "
              "code, used for the -l argument.");
DEFINE_string(cppExtraIncludeDirs, "",
              "optional, comma separated directories to satisfy "
              "cppExtraIncludes, used for the -I argument.");
DEFINE_string(cppExtraLinkLibDirs, "",
              "optional, comma separated directories to satisfy "
              "cppExtraLinkLibs, used for the -L argument.");

using namespace std;

bool validateFlags() {
  if ((FLAGS_queryType != "proto") && (FLAGS_queryType != "json")) {
    return false;
  }
  if (FLAGS_codeGenDir.empty() || FLAGS_codeCompileDir.empty() ||
      FLAGS_codeGenPrefix.empty()) {
    return false;
  }
  if (FLAGS_queryType == "proto") {
    if (FLAGS_cppProtoHeader.empty() || FLAGS_cppProtoLib.empty()) {
      return false;
    }
  }
  return true;
}

static regex notCommaRegex("[^,]+");

CodeGenSpec mkCodeGenSpec() {
  CodeGenSpec spec;
  if (FLAGS_queryType == "proto") {
    auto idx = FLAGS_cppProtoHeader.rfind("/");
    spec.headerIncludes.push_back(
        FLAGS_cppProtoHeader.substr((idx == string::npos) ? 0 : (idx+1)));
  }

  auto extraIncludesBegin = sregex_iterator(
      FLAGS_cppExtraIncludes.begin(), FLAGS_cppExtraIncludes.end(),
      notCommaRegex);
  auto extraIncludesEnd = sregex_iterator();
  for (auto it=extraIncludesBegin; it!=extraIncludesEnd; ++it) {
    spec.headerIncludes.push_back(it->str());
  }
  return spec;
}

// construct -I
set<string> mkIncludeDirs() {
  set<string> includeDirs;
  string thisFile = __FILE__;
  auto idx = thisFile.rfind("/");
  ASSERT(idx != string::npos);
  string codeDir = thisFile.substr(0, idx);
  idx = codeDir.rfind("/");
  ASSERT(idx != string::npos);
  string parentDir = codeDir.substr(0, idx);
  includeDirs.insert(parentDir + "/common");
  if (FLAGS_queryType == "proto") {
    includeDirs.insert(parentDir + "/protobuf-query");
  } else if (FLAGS_queryType == "json") {
    includeDirs.insert(parentDir + "/third-party/rapidjson/include");
    includeDirs.insert(parentDir + "/json-query");
  }
  idx = FLAGS_cppProtoHeader.rfind("/");
  includeDirs.insert(
      (idx == string::npos) ? "." : FLAGS_cppProtoHeader.substr(0, idx));
  auto extraIncludesBegin = sregex_iterator(
      FLAGS_cppExtraIncludeDirs.begin(), FLAGS_cppExtraIncludeDirs.end(),
      notCommaRegex);
  auto extraIncludesEnd = sregex_iterator();
  for (auto it=extraIncludesBegin; it!=extraIncludesEnd; ++it) {
    includeDirs.insert(it->str());
  }
  return includeDirs;
}

// construct -L
set<string> mkLinkDirs() {
  set<string> linkDirs;
  if (FLAGS_queryType == "proto") {
    auto idx = FLAGS_cppProtoLib.rfind("/");
    linkDirs.insert(
        (idx == string::npos) ? "." : FLAGS_cppProtoLib.substr(0, idx));
  }
  auto extraLinksBegin = sregex_iterator(
      FLAGS_cppExtraLinkLibDirs.begin(), FLAGS_cppExtraLinkLibDirs.end(),
      notCommaRegex);
  auto extraLinksEnd = sregex_iterator();
  for (auto it=extraLinksBegin; it!=extraLinksEnd; ++it) {
    linkDirs.insert(it->str());
  }
  return linkDirs;
}

// consturct -l
set<string> mkLinkLibs() {
  set<string> linkLibs;
  if (FLAGS_queryType == "proto") {
    static regex libRegex("lib(.+)\\.(so|dylib)$");
    smatch libMatch;
    if (!regex_search(FLAGS_cppProtoLib, libMatch, libRegex)) {
      cerr << "Unable to understand lib format " << FLAGS_cppProtoLib << endl;
      exit(1);
    }
    linkLibs.insert(libMatch[1]);
  }
  auto extraLinksBegin = sregex_iterator(
      FLAGS_cppExtraLinkLibs.begin(), FLAGS_cppExtraLinkLibs.end(),
      notCommaRegex);
  auto extraLinksEnd = sregex_iterator();
  for (auto it=extraLinksBegin; it!=extraLinksEnd; ++it) {
    linkLibs.insert(it->str());
  }
  return linkLibs;
}

void compileGeneratedCode() {
  set<string> includeDirs = mkIncludeDirs();
  set<string> linkDirs = mkLinkDirs();
  set<string> linkLibs = mkLinkLibs();

  string cpp_compiler = getenv("CXX") ? getenv("CXX") : "c++";
  string cmd = cpp_compiler + " -g --std=c++14 -Wall -Wextra -O3";
  for (const string& includeDir : includeDirs) {
    cmd += " -I" + includeDir;
  }
  cmd += " -o " + FLAGS_codeCompileDir + "/" + FLAGS_codeGenPrefix;
  cmd += " " + FLAGS_codeGenDir + "/" + FLAGS_codeGenPrefix + ".cc";
  for (const string& linkDir : linkDirs) {
    cmd += " -L" + linkDir;
  }
  if (FLAGS_queryType == "proto") {
    cmd += " -lprotobuf";
  }
  for (const string& linkLib : linkLibs) {
    cmd += " -l" + linkLib;
  }
  for (const string& linkDir : linkDirs) {
    cmd += " -Wl,-rpath," + linkDir;
  }
  cerr << cmd << endl;
  FILE* fp = popen(cmd.c_str(), "r");
  ASSERT(fp != NULL, "Unable to popen");
  string output;
  char buffer[1024];
  while (fgets(buffer, 1024, fp) != NULL) {
    output += buffer;
  }
  ASSERT(pclose(fp)==0, "Make failed:\n", output);
}

void runGeneratedCode(int argc, char** argv) {
  string cmd = FLAGS_codeCompileDir + "/" + FLAGS_codeGenPrefix;
  for (int i=0; i<argc; i++) {
    cmd += string(" \"") + argv[i] + "\"";
  }
  cerr << cmd << endl;
  FILE* fp = popen(cmd.c_str(), "r");
  ASSERT(fp != NULL, "Unable to popen", cmd);
  char buffer[1024];
  while (fgets(buffer, 1024, fp) != NULL) {
    cout << buffer;
  }
  ASSERT(pclose(fp)==0, "Running query failed");
}

int main(int argc, char** argv) {
  gflags::SetUsageMessage("<required-flags> [optional-flags] "
                          "<sql-query> [sql-args...]");
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  if (!validateFlags() || (argc < 2)) {
    cerr << "Invalid arguments ..." << endl;
    gflags::ShowUsageWithFlagsRestrict(argv[0], __FILE__);
    exit(1);
  }

  cerr << "Generating code..." << endl;

  CodeGenSpec spec = mkCodeGenSpec();
  string rawSql = argv[1];
  string generatedFile = FLAGS_codeGenDir + "/" + FLAGS_codeGenPrefix + ".cc";
  ofstream generated(generatedFile, ios::out | ios::trunc);
  if (!generated.is_open()) {
    cerr << "Unable to create file " << generatedFile << endl;
    exit(1);
  }

  if (FLAGS_queryType == "proto") {
    void* libHandle = dlopen(FLAGS_cppProtoLib.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (libHandle == NULL) {
      cerr << "Unable to load lib " << FLAGS_cppProtoLib << ": " << dlerror();
      exit(1);
    }
    pb::ProtobufQueryEngine engine(spec, rawSql, generated);
    engine.process();
    int result = dlclose(libHandle);
    if (result != 0) {
      cerr << "Unable to close lib " << FLAGS_cppProtoLib << ": " << dlerror();
      exit(1);
    }

  } else if (FLAGS_queryType == "json") {
    json::JsonQueryEngine engine(spec, rawSql, generated);
    engine.process();
  }

  cerr << "Compiling generated code..." << endl;
  compileGeneratedCode();

  cerr << "Running..." << endl;
  runGeneratedCode(argc-2, argv+2);
}
