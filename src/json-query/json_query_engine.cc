/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

#include <regex>
#include <algorithm>
#include "utils.h"
#include "json_query_engine.h"

using namespace std;
using namespace json;

bool JsonField::operator<(const JsonField& other) const {
  return fieldParts < other.fieldParts;
}

bool JsonField::operator==(const JsonField& other) const {
  return fieldParts == other.fieldParts;
}

void JsonField::addFieldPart(const string& fieldPart) {
  fieldParts.push_back(fieldPart);
}

bool JsonField::repeated() const {
  return true;
}

string JsonField::code_type() const {
  return "JsonValue";
}

string JsonField::accessor() const {
  return Utils::joinVec(".", fieldParts, Utils::string2str);
}

const string& JsonField::lastFieldPart() const {
  ASSERT(!fieldParts.empty(), "Fields expected to be not empty");
  return fieldParts.back();
}

string JsonQueryTree::getRootName() const {
  return "document";
}

string JsonQueryTree::getRootType() const {
  return "rapidjson::Document";
}

JsonField JsonQueryTree::newField() const {
  return JsonField();
}

string JsonQueryTree::generatedCommonHeader() const {
  return "json_generated_common.h";
}

void JsonQueryTree::printRootForLoop(
    ostream& out, const string& ind, const Node<JsonField>& node) const {
  out << ind << "for (const auto* " << node.objName
      << " : Iterators::mk_json_iterator("
      << Utils::makePlural(root.objName) << ")) {" << endl;
}

void JsonQueryTree::printNonRootForLoop(
    ostream& out, const string& ind, const Node<JsonField>& node,
    const Node<JsonField>& parent) const {
  out << ind << "for (const auto* " << node.objName
      << " : Iterators::mk_json_iterator("
      << "\"" << node.repeatedField.accessor() << "\"" << ", "
      << parent.objName << ", \"" << node.repeatedField.lastFieldPart()
      << "\")) {" << endl;
}

void JsonQueryTree::printFieldAssignment(
    ostream& out, const string& ind, const Node<JsonField>& node,
    const JsonField& /*field*/, bool /*repeating*/, const string& fieldType,
    const string& fieldVar, const string& fieldDefault) const {
  out << ind << fieldType << " " << fieldVar << " = " << node.objName
      << " ? *" << node.objName << " : " << fieldDefault << ";" << endl;
}

JsonQueryEngine::JsonQueryEngine(const CodeGenSpec& spec, const string& rawSql, ostream& out) :
    spec(spec), query(SelectQuery(rawSql)), out(out) {}

void JsonQueryEngine::process() {
  ASSERT(query.parse(), "Parsing select query failed");
  out << "/*" << endl;
  out << query.str() << endl << endl;
  auto resolver = [] (const string& identifier, vector<string>&) {
    THROW("Star identifiers not allowed for json queries", identifier);
  };
  query.resolveSelectStars(resolver);
  query.removeSelectAliases();
  queryTree.process(query);
  queryTree.printPlan(query, out);
  out << "*/" << endl;
  queryTree.printCode(query, out, spec);
}
