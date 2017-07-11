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
#include "utils.h"
#include "query_tree.h"
#include "select_query.h"

using namespace std;

namespace json {

class JsonField : public Field {
public:
  JsonField() = default;
  bool operator<(const JsonField& other) const;
  bool operator==(const JsonField& other) const;
  void addFieldPart(const string&) override;
  bool repeated() const override;
  string code_type() const override;
  string accessor() const override;
  // custom JsonField functions
  const string& lastFieldPart() const;
private:
  vector<string> fieldParts;
};

class JsonQueryTree : public QueryTree<JsonField> {
public:
  string getRootName() const override;
  string getRootType() const override;
  JsonField newField() const override;

  string generatedCommonHeader() const override;
  void printRootForLoop(ostream& out, const string& ind,
                        const Node<JsonField>& node) const override;
  void printNonRootForLoop(ostream& out, const string& ind,
                           const Node<JsonField>& node,
                           const Node<JsonField>& parent) const override;
  void printFieldAssignment(ostream& out, const string& ind,
                            const Node<JsonField>& node,
                            const JsonField& field,
                            bool repeating,
                            const string& fieldType,
                            const string& fieldVar,
                            const string& fieldDefault) const override;
};

class JsonQueryEngine {
public:
  JsonQueryEngine(const CodeGenSpec& spec, const string& rawSql, ostream& out);
  void process();

private:
  CodeGenSpec spec;
  SelectQuery query;
  JsonQueryTree queryTree;
  ostream& out;
};

} // namespace pb
