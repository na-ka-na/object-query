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

class FieldPart {
public:
  static string full_name_to_cpp_type(const string& full_name);
  static FieldPart parseFrom(const Descriptor& parentDescriptor,
                             const string& partName);
  enum Type { NORMAL, SIZE, HAS };
  FieldPart(const FieldDescriptor* descriptor, Type type=NORMAL);
  bool operator<(const FieldPart& other) const;
  bool operator==(const FieldPart& other) const;
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

struct Field {
  vector<FieldPart> fieldParts;

  bool operator<(const Field& other) const;
  bool operator==(const Field& other) const;
  string code_type() const;
  bool is_enum() const;
  string wrap_enum_with_name_accessor(const string& accessor) const;
  string accessor(const string& objName, bool useNameForEnum) const;
  string has_check(const string& objName) const;
};

struct QueryGraph {
  const Descriptor* protoDescriptor;
  Node<Field> root;
  map<string, Field> idFieldMap;
  void initProto(const SelectQuery& query);
  void initGraph(const SelectQuery& query);
  void resolveStarIdentifier(const string& star_identifier,
                             vector<string>& resolved_identifiers);

  string getProtoCppType() const;
  void addReadIdentifier(const string& identifier);
  void processSelect(const SelectStmt& selectStmt);
  void processWhere(const WhereStmt& whereStmt);
  void processOrderBy(const OrderByStmt& orderByStmt);
  void processExpr(const set<string>& identifiers,
                   const function<void(Node<Field>& node)>& callback);
  static void addExpr(vector<const Expr*>& exprs, const Expr* expr);
};

class QueryEngine {
public:
  QueryEngine(const CodeGenSpec& spec, const string& rawSql, ostream& out);
  void process();

private:
  CodeGenSpec spec;
  SelectQuery query;
  QueryGraph queryGraph;
  ostream& out;
  void printPlan();
  void printCode();
};

} // namespace pb
