/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

#include <regex>
#include <algorithm>
#include "utils.h"
#include "protobuf_query_engine.h"

using namespace std;
using namespace google::protobuf;
using namespace pb;

PbFieldPart PbFieldPart::parseFrom(const Descriptor& parentDescriptor,
                                   const string& partName) {
  const FieldDescriptor* descriptor = parentDescriptor.FindFieldByName(partName);
  if (descriptor != nullptr) {
    return PbFieldPart(descriptor);
  }
  static std::regex size_regex("^(.+)_size$");
  std::smatch size_match;
  if (std::regex_match(partName, size_match, size_regex)) {
    std::ssub_match base_match = size_match[1];
    string base_part = base_match.str();
    descriptor = parentDescriptor.FindFieldByName(base_part);
    if (descriptor != nullptr) {
      return PbFieldPart(descriptor, SIZE);
    }
  }
  static std::regex has_regex("^has_(.+)$");
  std::smatch has_match;
  if (std::regex_match(partName, has_match, has_regex)) {
    std::ssub_match base_match = has_match[1];
    string base_part = base_match.str();
    descriptor = parentDescriptor.FindFieldByName(base_part);
    if (descriptor != nullptr) {
      return PbFieldPart(descriptor, HAS);
    }
  }
  THROW("No fieldPart by name", partName);
}

PbFieldPart::PbFieldPart(const FieldDescriptor* descriptor, Type type) :
    descriptor(descriptor), part_type(type) {}

bool PbFieldPart::operator<(const PbFieldPart& other) const {
  if (part_type < other.part_type) {
    return true;
  } else if (part_type == other.part_type) {
    return descriptor < other.descriptor;
  } else {
    return false;
  }
}

bool PbFieldPart::operator==(const PbFieldPart& other) const {
  return (part_type == other.part_type) && (descriptor == other.descriptor);
}

string PbFieldPart::full_name_to_cpp_type(const string& full_name) {
  static regex dot("\\.");
  return regex_replace(full_name, dot, "::");
}

string PbFieldPart::name() const {
  switch (part_type) {
  case NORMAL : return descriptor->name();
  case SIZE : return descriptor->name() + "_size";
  case HAS : return "has_" + descriptor->name();
  default : THROW("Unhandled type");
  }
}

FieldDescriptor::Type PbFieldPart::type() const {
  switch (part_type) {
  case NORMAL : return descriptor->type();
  case SIZE : return FieldDescriptor::Type::TYPE_INT32;
  case HAS : return FieldDescriptor::Type::TYPE_BOOL;
  default : THROW("Unhandled type");
  }
}

string PbFieldPart::type_name() const {
  return FieldDescriptor::TypeName(type());
}

FieldDescriptor::CppType PbFieldPart::cpp_type() const {
  return FieldDescriptor::TypeToCppType(type());
}

string PbFieldPart::cpp_type_name() const {
  return FieldDescriptor::CppTypeName(cpp_type());
}

bool PbFieldPart::is_message() const {
  return type() == FieldDescriptor::Type::TYPE_MESSAGE;
}

const Descriptor* PbFieldPart::message_descriptor() const {
  ASSERT(is_message());
  return descriptor->message_type();
}

bool PbFieldPart::is_repeated() const {
  return (part_type == NORMAL) &&
      (descriptor->label() == FieldDescriptor::LABEL_REPEATED);
}

bool PbFieldPart::is_enum() const {
  return type() == FieldDescriptor::Type::TYPE_ENUM;
}

const EnumDescriptor* PbFieldPart::enum_descriptor() const {
  ASSERT(is_enum());
  return descriptor->enum_type();
}


string PbFieldPart::code_type() const {
  if (is_message()) {
    return full_name_to_cpp_type(message_descriptor()->full_name());
  } else if (is_enum()) {
    return "string";
  } else {
    return cpp_type_name();
  }
}

string PbFieldPart::accessor() const {
  switch (part_type) {
  case NORMAL : return descriptor->name() + "()";
  case SIZE : return descriptor->name() + "_size()";
  case HAS : return "has_" + descriptor->name() + "()";
  default : THROW("Unhandled type");
  }
}

bool PbFieldPart::needs_has_check() const {
  return part_type == NORMAL;
}

string PbFieldPart::has_accessor() const {
  ASSERT(needs_has_check());
  return "has_" + descriptor->name() + "()";
}

PbField::PbField(const Descriptor* rootDescriptor) :
    descriptor(rootDescriptor) {}

bool PbField::operator<(const PbField& other) const {
  if (fieldParts < other.fieldParts) return true;
  else if (other.fieldParts < fieldParts) return false;
  return descriptor < other.descriptor;
}

bool PbField::operator==(const PbField& other) const {
  return (fieldParts == other.fieldParts) &&
         (descriptor == other.descriptor);
}

string PbField::code_type() const {
  ASSERT(!fieldParts.empty(), "Can't determine type of empty field");
  return fieldParts.back().code_type();
}

bool PbField::is_enum() const {
  return !fieldParts.empty() && fieldParts.back().is_enum();
}

string PbField::wrap_enum_with_name_accessor(const string& accessor) const {
  ASSERT(is_enum());
  string type = PbFieldPart::full_name_to_cpp_type(
      fieldParts.back().enum_descriptor()->full_name());
  return type + "_Name(static_cast<" + type + ">(" + accessor + "))";
}

string PbField::has_check(const string& objName) const {
  vector<string> checks;
  for (uint32_t i=0; i<fieldParts.size(); i++) {
    if (!fieldParts[i].needs_has_check()) {
      continue;
    }
    string check = objName;
    for (uint32_t j=0; j<i; j++) {
      if (j != 0) check += ".";
      check += fieldParts[j].accessor();
    }
    if (i != 0) check += ".";
    check += fieldParts[i].has_accessor();
    checks.push_back(check);
  }
  return Utils::joinVec(" && ", checks, Utils::string2str);
}

void PbField::addFieldPart(const string& fieldPartStr) {
  if (repeated()) fieldParts.clear();
  ASSERT(descriptor != nullptr,
         "FieldPart", fieldParts.back().name(), "expected to be message but is",
         fieldParts.back().type_name());
  PbFieldPart fieldPart = PbFieldPart::parseFrom(*descriptor, fieldPartStr);
  fieldParts.push_back(fieldPart);
  descriptor = fieldPart.is_message() ? fieldPart.message_descriptor() : nullptr;
}

bool PbField::repeated() const {
  return !fieldParts.empty() && fieldParts.back().is_repeated();
}

string PbField::accessor() const {
  return Utils::joinVec<PbFieldPart>(".", fieldParts,
      [] (const PbFieldPart& part) {return part.accessor();});
}

string PbQueryTree::getRootType() const {
  return PbFieldPart::full_name_to_cpp_type(protoDescriptor->full_name());
}

void PbQueryTree::resolveStarIdentifier(const string& star_identifier,
                                        vector<string>& resolved_identifiers) {
  const Descriptor* parentDescriptor = protoDescriptor;
  string parentPath;
  vector<string> selectFieldParts = Utils::splitDotIdentifier(star_identifier);
  for (size_t j=0; j<selectFieldParts.size(); j++) {
    if (j != (selectFieldParts.size()-1)) {
      PbFieldPart fieldPart = PbFieldPart::parseFrom(
          *parentDescriptor, selectFieldParts[j]);
      parentDescriptor = fieldPart.message_descriptor();
      parentPath += (fieldPart.name() + ".");
    } else {
      ASSERT(selectFieldParts[j] == "*");
      for (int i=0; i<parentDescriptor->field_count(); i++) {
        PbFieldPart field_part(parentDescriptor->field(i));
        if (!field_part.is_message() && !field_part.is_repeated()) {
          resolved_identifiers.push_back(parentPath + field_part.name());
        }
      }
    }
  }
}

void PbQueryTree::initProto(const string& protoName) {
  const DescriptorPool* pool = google::protobuf::DescriptorPool::generated_pool();
  protoDescriptor = pool->FindMessageTypeByName(protoName);
  ASSERT(protoDescriptor != nullptr,
         "Unable to find proto descriptor for", protoName);
}

string PbQueryTree::getRootName() const {
  string rootName = protoDescriptor->name();
  std::transform(rootName.begin(), rootName.end(),
                 rootName.begin(), ::tolower);
  return rootName;
}

PbField PbQueryTree::newField() const {
  return PbField(protoDescriptor);
}

string PbQueryTree::generatedCommonHeader() const {
  return "protobuf_generated_common.h";
}

void PbQueryTree::printRootForLoop(
    ostream& out, const string& ind, const Node<PbField>& node) const {
  out << ind << "for (const auto* " << node.objName
      << " : Iterators::mk_vec_iterator(&"
      << Utils::makePlural(root.objName) << ")) {" << endl;
}

void PbQueryTree::printNonRootForLoop(
    ostream& out, const string& ind, const Node<PbField>& node,
    const Node<PbField>& parent) const {
  out << ind << "for (const auto* "
      << node.objName << " : Iterators::mk_pb_iterator(" << parent.objName
      << " ? &" << parent.objName << "->" << node.repeatedField.accessor()
      << " : nullptr)) {" << endl;
}

void PbQueryTree::printFieldAssignment(
    ostream& out, const string& ind, const Node<PbField>& node,
    const PbField& field, bool repeating, const string& fieldType,
    const string& fieldVar, const string& fieldDefault) const {
  out << ind << fieldType << " " << fieldVar << " = " << fieldDefault
      << ";" << endl;
  if (repeating) {
    out << ind << "if (" << node.objName << ") {" << endl;
    out << ind << "  " << fieldVar << " = "
        << (field.is_enum()
            ? field.wrap_enum_with_name_accessor("*" + node.objName)
            : "*" + node.objName)
        << ";" << endl;
    out << ind << "}" << endl;
  } else {
    string checks = field.has_check(node.objName + "->");
    out << ind << "if (" << node.objName
        << (checks.empty() ? "" : " && " + checks) << ") {" << endl;
    out << ind << "  " << fieldVar << " = "
        << (field.is_enum()
            ? field.wrap_enum_with_name_accessor(
                node.objName + "->" + field.accessor())
            : node.objName + "->" + field.accessor())
        << ";" << endl;
    out << ind << "}" << endl;
  }
}

ProtobufQueryEngine::ProtobufQueryEngine(const CodeGenSpec& spec, const string& rawSql, ostream& out) :
    spec(spec), query(SelectQuery(rawSql)), out(out) {}

void ProtobufQueryEngine::process() {
  ASSERT(query.parse(), "Parsing select query failed");
  out << "/*" << endl;
  out << query.str() << endl << endl;
  queryTree.initProto(query.fromStmt.getProtoName());
  auto resolver = std::bind(&PbQueryTree::resolveStarIdentifier, &queryTree,
                            std::placeholders::_1, std::placeholders::_2);
  query.resolveSelectStars(resolver);
  query.removeSelectAliases();
  queryTree.process(query);
  queryTree.printPlan(query, out);
  out << "*/" << endl;
  queryTree.printCode(query, out, spec);
}
