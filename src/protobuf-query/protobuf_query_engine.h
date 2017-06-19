/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <iostream>
#include <google/protobuf/descriptor.h>
#include "utils.h"
#include "query_tree.h"
#include "select_query.h"

using namespace std;
using namespace google::protobuf;

namespace pb {

class PbFieldPart {
public:
  static string full_name_to_cpp_type(const string& full_name);
  static PbFieldPart parseFrom(const Descriptor& parentDescriptor,
                               const string& partName);
  enum Type { NORMAL, SIZE, HAS };
  PbFieldPart(const FieldDescriptor* descriptor, Type type=NORMAL);
  bool operator<(const PbFieldPart& other) const;
  bool operator==(const PbFieldPart& other) const;
  string name() const;
  FieldDescriptor::Type type() const;
  string type_name() const;
  FieldDescriptor::CppType cpp_type() const;
  string cpp_type_name() const;
  bool is_message() const;
  const Descriptor* message_descriptor() const;
  bool is_repeated() const;
  bool is_enum() const;
  const EnumDescriptor* enum_descriptor() const;
  string code_type() const;
  string accessor() const;
  bool needs_has_check() const;
  string has_accessor() const;
private:
  const FieldDescriptor* descriptor;
  Type part_type;
};

class PbField : public Field {
public:
  PbField() = default;
  PbField(const Descriptor* rootDescriptor);
  bool operator<(const PbField& other) const;
  bool operator==(const PbField& other) const;
  string code_type() const;
  bool is_enum() const;
  string wrap_enum_with_name_accessor(const string& accessor) const;
  string has_check(const string& objName) const;
  void addFieldPart(const string&) override;
  bool repeated() const override;
  string accessor() const override;
private:
  vector<PbFieldPart> fieldParts;
  const Descriptor* descriptor = nullptr;
};

struct PbQueryTree : public QueryTree<PbField> {
  const Descriptor* protoDescriptor;
  void initProto(const string& protoName);
  void resolveStarIdentifier(const string& star_identifier,
                             vector<string>& resolved_identifiers);
  string getProtoCppType() const;
  string getRootName() override;
  PbField newField() override;
};

class ProtobufQueryEngine {
public:
  ProtobufQueryEngine(const CodeGenSpec& spec, const string& rawSql, ostream& out);
  void process();

private:
  CodeGenSpec spec;
  SelectQuery query;
  PbQueryTree queryTree;
  ostream& out;
  void printPlan();
  void printCode();
};

} // namespace pb
