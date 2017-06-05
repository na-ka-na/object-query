/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

#include <regex>
#include <algorithm>
#include "utils.h"
#include "query_engine.h"

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
  string str = Utils::joinVec<PbFieldPart>(".", fieldParts,
      [] (const PbFieldPart& part) {return part.accessor();});
  if (is_enum()) {
    return wrap_enum_with_name_accessor(str);
  }
  return str;
}

string PbQueryTree::getProtoCppType() const {
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

string PbQueryTree::getRootName() {
  string rootName = protoDescriptor->name();
  std::transform(rootName.begin(), rootName.end(),
                 rootName.begin(), ::tolower);
  return rootName;
}

PbField PbQueryTree::newField() {
  return PbField(protoDescriptor);
}

QueryEngine::QueryEngine(const CodeGenSpec& spec, const string& rawSql, ostream& out) :
    spec(spec), query(SelectQuery(rawSql)), out(out) {}

void QueryEngine::printCode() {
  for (const string& headerInclude : spec.headerIncludes) {
    out << "#include \"" << headerInclude << "\"" << endl;
  }
  out << "#include \"generated_common.h\"" << endl << endl;
  out << "using namespace std;" << endl;
  out << endl;

  // select fields header
  string header = "vector<string> header = {\n";
  header += Utils::joinVec<SelectField>(
      "\n", query.selectStmt.getSelectFields(),
      [](const SelectField& sf) {
        return "  \"" + sf.getHeader() + "\",";
      });
  header += "\n};";
  out << header << endl;

  CodeGenReqs cgr;
  query.extractStatics(cgr);

  for (const auto& f : cgr.fnMap) {
    out << "\n";
    const string& fnname = f.first;
    unsigned num_params = f.second;
    out << "template<";
    for (unsigned i=0; i<num_params; i++) {
      out << "typename Arg" << i << ", ";
    }
    out << "typename Ret=decltype(" << fnname << "(";
    for (unsigned i=0; i<num_params; i++) {
      out << "Arg" << i << "()";
      if (i != (num_params-1)) out << ", ";
    }
    out << "))>\n";
    out << "optional<Ret> $" << fnname << "(";
    for (unsigned i=0; i<num_params; i++) {
      out << "const optional<Arg" << i << ">& arg" << i;
      if (i != (num_params-1)) out << ", ";
    }
    out << ") {\n";
    out << "  if (";
    for (unsigned i=0; i<num_params; i++) {
      out << "arg" << i;
      if (i != (num_params-1)) out << " && ";
    }
    out << ") {\n";
    out << "    return optional<Ret>(" << fnname << "(";
    for (unsigned i=0; i<num_params; i++) {
      out << "*arg" << i;
      if (i != (num_params-1)) out << ", ";
    }
    out << "));\n";
    out << "  } else {\n";
    out << "    return optional<Ret>();\n";
    out << "  }\n";
    out << "}\n";
  }

  map<PbField, string> fieldVarMap;
  map<PbField, string> fieldTypeMap;
  map<PbField, string> fieldDefaultMap;
  uint32_t varIdx = 0;
  for (const auto& e : queryTree.idFieldMap) {
    const string& id = e.first;
    const PbField& field = e.second;
    fieldVarMap[field] = "s" + to_string(varIdx);
    fieldTypeMap[field] = "S" + to_string(varIdx);
    fieldDefaultMap[field] = "S" + to_string(varIdx) + "()";
    varIdx++;
    string type = "optional<" + field.code_type() + ">";
    string spaces(((type.size() < 16) ? (16-type.size()) : 0), ' ');
    out << "using " << fieldTypeMap[field] << " = " << type << ";"
        << spaces << " /* " << field.accessor() << " */" << endl;
    cgr.idVarMap[id] = fieldVarMap[field];
    cgr.idDefaultMap[id] = fieldDefaultMap[field];
  }

  vector<const Expr*> selectAndOrderByExprs;
  for (const SelectField& selectField : query.selectStmt.getSelectFields()) {
    PbQueryTree::addExpr(selectAndOrderByExprs, &(selectField.getExpr()));
  }
  for (const OrderByField& orderByField : query.orderByStmt.getOrderByFields()) {
    PbQueryTree::addExpr(selectAndOrderByExprs, &(orderByField.getExpr()));
  }

  map<string, string> exprVarMap;
  map<string, string> exprTypeMap;
  map<string, string> exprDefaultMap;
  for (const Expr* expr : selectAndOrderByExprs) {
    string exprStr = expr->str();
    if (expr->isIdentifier()) {
      PbField f = queryTree.idFieldMap[expr->getIdentifier()];
      exprVarMap[exprStr] = fieldVarMap[f];
      exprTypeMap[exprStr] = fieldTypeMap[f];
      exprDefaultMap[exprStr] = fieldDefaultMap[f];
    } else {
      exprVarMap[exprStr] = "s" + to_string(varIdx);
      exprTypeMap[exprStr] = "S" + to_string(varIdx);
      exprDefaultMap[exprStr] = "S" + to_string(varIdx) + "()";
      varIdx++;
      string type = expr->cppType(cgr);
      string spaces(((type.size() < 16) ? (16-type.size()) : 0), ' ');
      out << "using " << exprTypeMap[exprStr] << " = " << type << ";"
          << spaces << " /* " << exprStr << " */" << endl;
    }
  }

  string tupleType = "using TupleType = tuple<";
  tupleType += Utils::joinVec<const Expr*>(
      ", ", selectAndOrderByExprs,
      [&](const Expr* expr) {return exprTypeMap[expr->str()];});
  tupleType += ">;";
  out << tupleType << endl;
  out << endl;

  if (!cgr.regexMap.empty()) {
    for (auto& e : cgr.regexMap) {
      string regexVar = "r" + to_string(varIdx);
      varIdx++;
      out << "std::regex " << regexVar << "(\"" << e.first << "\", "
          << "std::regex::optimize);" << endl;
      e.second = regexVar;
    }
    out << endl;
  }

  out << "void runSelect(const vector<" << queryTree.getProtoCppType() << ">& "
      << Utils::makePlural(queryTree.root.objName)
      << ", vector<TupleType>& tuples) {" << endl;
  unsigned numSelectAndOrderByFieldsProcessed = 0;
  bool allSelectAndOrderByFieldsProcessed = false;
  StartNodeFn<PbField> startNodeFn =
      [&](int indent, const Node<PbField>& node, const Node<PbField>* parent) {
        string ind = string(indent+2, ' ');
        if (!parent) { //root
          out << ind << "for (const auto* " << node.objName
              << " : Iterators::mk_iterator(&"
              << Utils::makePlural(queryTree.root.objName) << ")) {" << endl;
        } else {
          out << ind << "for (const auto* "
              << node.objName << " : Iterators::mk_iterator(" << parent->objName
              << " ? &" << parent->objName << "->" << node.repeatedField.accessor()
              << " : nullptr)) {" << endl;
        }
        ind += "  ";
        for (const auto& f : node.allFields) {
          const PbField& field = f.first;
          bool repeating = f.second;
          out << ind << fieldTypeMap[field] << " " << fieldVarMap[field] << " = "
              << fieldDefaultMap[field] << ";" << endl;
          if (repeating) {
            out << ind << "if (" << node.objName << ") {" << endl;
            out << ind << "  " << fieldVarMap[field] << " = "
                << (field.is_enum()
                    ? field.wrap_enum_with_name_accessor("*" + node.objName)
                    : "*" + node.objName)
                << ";" << endl;
            out << ind << "}" << endl;
          } else {
            string checks = field.has_check(node.objName + "->");
            out << ind << "if (" << node.objName
                << (checks.empty() ? "" : " && " + checks) << ") {" << endl;
            out << ind << "  " << fieldVarMap[field] << " = "
                << node.objName << "->" << field.accessor() << ";" << endl;
            out << ind << "}" << endl;
          }
        }
        // TODO(sanchay): print variable assignment and where clauses in optimal order
        for (const BooleanExpr* whereClause : node.whereClauses) {
          out << ind << "if (!" << whereClause->code(cgr)
              << ") { continue; }" << endl;
        }
        for (const Expr* expr : node.selectAndOrderByExprs) {
          if (!expr->isIdentifier()) {
            out << ind << exprTypeMap[expr->str()] << " "
                << exprVarMap[expr->str()] << " = "
                << expr->code(cgr) << ";" << endl;
          }
        }
        numSelectAndOrderByFieldsProcessed += node.selectAndOrderByExprs.size();
        if (!allSelectAndOrderByFieldsProcessed &&
            (numSelectAndOrderByFieldsProcessed == selectAndOrderByExprs.size())) {
          string tuplesList = Utils::joinVec<const Expr*>(
              ", ", selectAndOrderByExprs,
              [&](const Expr* expr) {return exprVarMap[expr->str()];});
          out << ind << "tuples.emplace_back(" + tuplesList + ");" << endl;
          allSelectAndOrderByFieldsProcessed = true;
        }
      };
  EndNodeFn<PbField> endNodeFn =
      [&](int indent, const Node<PbField>&) {
        string ind = string(indent+2, ' ');
        out << ind << "}" << endl;
      };
  Node<PbField>::walkNode(queryTree.root, startNodeFn, endNodeFn);
  out << "}" << endl;

  if (!query.orderByStmt.getOrderByFields().empty()) {
    out << endl;
    out << "bool compareTuples(const TupleType& t1, const TupleType& t2) {" << endl;
    out << "  int c;" << endl;
    for (const OrderByField& orderByField : query.orderByStmt.getOrderByFields()) {
      int idx = -1;
      string exprStr = orderByField.getExpr().str();
      for (size_t j=0; j<selectAndOrderByExprs.size(); j++) {
        if (exprStr == selectAndOrderByExprs[j]->str()) {
          idx = j;
          break;
        }
      }
      ASSERT(idx != -1);
      out << "  c = " << (orderByField.isDesc() ? "-" : "")
          << "Compare(get<" << idx << ">(t1), get<" << idx << ">(t2));" << endl;
      out << "  if (c < 0) {return true;} else if (c > 0) {return false;}" << endl;
    }
    out << "  return false;" << endl;
    out << "}" << endl;
  }

  out << R"fff(
void printTuples(const vector<TupleType>& tuples) {
  vector<size_t> sizes;
  for (size_t i=0; i<header.size(); i++) {
    sizes.push_back(header[i].size());
  }
)fff";
  out << "  for (const TupleType& t : tuples) {" << endl;
  for (size_t i=0; i<query.selectStmt.getSelectFields().size(); i++) {
    out << "    sizes[" << i << "] = max(sizes[" << i << "], Stringify(get<"
        << i << ">(t)).size());" << endl;
  }
  out << "  }" << endl;
  out << "  cout << left;";
  out << R"fff(
  for (size_t i=0; i<header.size(); i++) {
    cout << ((i==0) ? "" : " | ") << setw(sizes[i]) << header[i];
  }
  cout << endl;
  for (size_t i=0; i<header.size(); i++) {
    cout << ((i==0) ? "" : " | ") << string(sizes[i], '-');
  }
  cout << endl;
)fff";
  out << "  for(const TupleType& t : tuples) {" << endl;
  for (size_t i=0; i<query.selectStmt.getSelectFields().size(); i++) {
    out << "    cout << " << ((i==0) ? "         " : "\" | \" << ")
        << "setw(sizes[" << i << "]) << "
        << "Stringify(get<" << i << ">(t));" << endl;
  }
  out << "    cout << endl;" << endl;
  out << "  }" << endl;
  out << "}" << endl << endl;

  // main
  out << "int main(int argc, char** argv) {" << endl;
  string protosVecIden = Utils::makePlural(queryTree.root.objName);
  out << "  vector<" << queryTree.getProtoCppType() << "> "
      << protosVecIden << ";" << endl;
  out << "  FROM(argc, argv, " << protosVecIden << ");" << endl;
  out << "  vector<TupleType> tuples;" << endl;
  out << "  runSelect(" << protosVecIden << ", tuples);" << endl;
  if (!query.orderByStmt.getOrderByFields().empty()) {
    out << "  std::sort(tuples.begin(), tuples.end(), compareTuples);" << endl;
  }
  out << "  printTuples(tuples);" << endl;
  out << "}" << endl;
}

void QueryEngine::process() {
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
  printCode();
}
