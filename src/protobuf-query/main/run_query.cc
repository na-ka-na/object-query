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
#include "query_engine.h"

DEFINE_string(codeGenDir, ".", "optional, directory where generated code files "
                               "are emitted, e.g. /tmp, directory must exist");
DEFINE_string(codeCompileDir, ".", "optional, directory where generated code "
                                   "files are compiled, e.g. /tmp, directory "
                                   "must exist");
DEFINE_string(codeGenPrefix, "generated_query", "optional, prefix for "
                                                "generated code files");

DEFINE_string(protoName, "", "\033[1mrequired\033[0m, "
                             "namespace in .proto + message name");
DEFINE_string(cppProtoNamespace, "", "\033[1mrequired\033[0m, "
                                     "c++ namespace of proto");
DEFINE_string(cppProtoHeader, "", "\033[1mrequired\033[0m, "
                                  "path of proto header file");
DEFINE_string(cppProtoLib, "", "\033[1mrequired\033[0m, "
                               "path of built proto library");

DEFINE_string(cppExtraIncludes, "", "optional, comma separated extra headers "
                                    "to include in generated code, e.g. with "
                                    "your custom functions");
DEFINE_string(cppExtraLinkLibs, "", "optional, comma separated extra lib names "
                                    "to link with generated code");
DEFINE_string(cppExtraIncludeDirs, "", "optional, comma separated directories "
                                       "to satisfy cppExtraIncludes");
DEFINE_string(cppExtraLinkLibDirs, "", "optional, comma separated directories "
                                       "to satisfy cppExtraLinkLibs");

using namespace std;

bool validateFlags() {
  return !FLAGS_codeGenDir.empty() && !FLAGS_codeCompileDir.empty() &&
      !FLAGS_codeGenPrefix.empty() && !FLAGS_protoName.empty() &&
      !FLAGS_cppProtoNamespace.empty() && !FLAGS_cppProtoHeader.empty() &&
      !FLAGS_cppProtoLib.empty();
}

static regex notCommaRegex("[^,]+");

ProtoSpec mkProtoSpec() {
  ProtoSpec proto;
  proto.protoName = FLAGS_protoName;
  proto.cppProtoNamespace = FLAGS_cppProtoNamespace;

  auto idx = FLAGS_cppProtoHeader.rfind("/");
  proto.headerIncludes.push_back(
      FLAGS_cppProtoHeader.substr((idx == string::npos) ? 0 : (idx+1)));

  auto extraIncludesBegin = sregex_iterator(
      FLAGS_cppExtraIncludes.begin(), FLAGS_cppExtraIncludes.end(),
      notCommaRegex);
  auto extraIncludesEnd = sregex_iterator();
  for (auto it=extraIncludesBegin; it!=extraIncludesEnd; ++it) {
    proto.headerIncludes.push_back(it->str());
  }
  return proto;
}

// construct -I
set<string> mkIncludeDirs() {
  set<string> includeDirs;
  string thisFile = __FILE__;
  auto idx = thisFile.rfind("/");
  ASSERT((idx != string::npos) && (idx != 0));
  idx = thisFile.rfind("/", idx-1);
  ASSERT(idx != string::npos);
  string codeDir = thisFile.substr(0, idx);
  includeDirs.insert(codeDir);
  includeDirs.insert(codeDir + "/engine");
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
  auto idx = FLAGS_cppProtoLib.rfind("/");
  linkDirs.insert(
      (idx == string::npos) ? "." : FLAGS_cppProtoLib.substr(0, idx));
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
  static regex libRegex("lib(.+)\\.(so|dylib)$");
  smatch libMatch;
  if (!regex_search(FLAGS_cppProtoLib, libMatch, libRegex)) {
    cerr << "Unable to understand lib format " << FLAGS_cppProtoLib << endl;
    exit(1);
  }
  linkLibs.insert(libMatch[1]);
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

  string cmd = "c++ -g --std=c++14 -Wall -Wextra -O3";
  for (const string& includeDir : includeDirs) {
    cmd += " -I" + includeDir;
  }
  cmd += " -o " + FLAGS_codeCompileDir + "/" + FLAGS_codeGenPrefix;
  cmd += " " + FLAGS_codeGenDir + "/" + FLAGS_codeGenPrefix + ".cc";
  for (const string& linkDir : linkDirs) {
    cmd += " -L" + linkDir;
  }
  cmd += " -lprotobuf";
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
    gflags::ShowUsageWithFlagsRestrict(argv[0], __FILE__);
    exit(1);
  }

  cerr << "Generating code..." << endl;

  ProtoSpec proto = mkProtoSpec();
  string rawSql = argv[1];
  string generatedFile = FLAGS_codeGenDir + "/" + FLAGS_codeGenPrefix + ".cc";
  ofstream generated(generatedFile, ios::out | ios::trunc);
  if (!generated.is_open()) {
    cerr << "Unable to create file " << generatedFile << endl;
    exit(1);
  }

  void* libHandle = dlopen(FLAGS_cppProtoLib.c_str(), RTLD_NOW | RTLD_GLOBAL);
  if (libHandle == NULL) {
    cerr << "Unable to load lib " << FLAGS_cppProtoLib << ": " << dlerror();
    exit(1);
  }

  QueryEngine engine(proto, rawSql, generated);
  engine.process();

  int result = dlclose(libHandle);
  if (result != 0) {
    cerr << "Unable to close lib " << FLAGS_cppProtoLib << ": " << dlerror();
    exit(1);
  }

  cerr << "Compiling generated code..." << endl;
  compileGeneratedCode();

  cerr << "Running..." << endl;
  runGeneratedCode(argc-2, argv+2);
}
