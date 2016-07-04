
#include <fcntl.h>
#include <tuple>
#include <algorithm>
#include <iostream>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/gzip_stream.h>
#include "example1.pb.h"
#include "utils.h"
#include "query_engine.h"

using namespace std;
using namespace google::protobuf;

void Proto::initProto(const string& protoName, Proto& proto) {
  if (protoName == "Example1.Company") {
    static Example1::Company defaultInstance;
    proto.defaultInstance = &defaultInstance;
    proto.protoNamespace = "Example1";
    proto.protoHeaderInclude = "example1.pb.h";
  } else {
    throw runtime_error("No mapping defined for protoName: " + protoName);
  }
}

bool Field::operator<(const Field& other) const {
  return fieldParts < other.fieldParts;
}

bool Field::operator==(const Field& other) const {
  return fieldParts == other.fieldParts;
}

string Field::type() const {
  if (fieldParts.empty()) {
    throw runtime_error("Can't determine type of empty field");
  }
  const FieldDescriptor* lastPart = fieldParts.back();
  if (lastPart->type() == FieldDescriptor::Type::TYPE_MESSAGE) {
    const Descriptor* msgType = lastPart->message_type();
    return msgType->name();
  } else {
    return lastPart->cpp_type_name();
  }
}

string Field::accessor(const string& objName) const {
  string str = objName;
  for (const FieldDescriptor* part : fieldParts) {
    str += "." + part->name() + "()";
  }
  return str;
}

string Field::hasAccessor(const string& objName, uint32_t end) const {
  string str = objName;
  for (uint32_t i=0; i<end; i++) {
    str += string(".") + ((i==(end-1)) ? "has_" : "") +
           fieldParts[i]->name() + "()";
  }
  return str;
}

string Field::sizeAccessor(const string& objName) const {
  string str = objName;
  for (uint32_t i=0; i<fieldParts.size(); i++) {
    str += string(".") + fieldParts[i]->name() +
           ((i==(fieldParts.size()-1)) ? "_size" : "") + "()";
  }
  return str;
}

QueryEngine::QueryEngine(const string& rawSql, ostream& out) :
    query(SelectQuery(rawSql)), out(out) {}

string QueryEngine::constructObjNameForRepeated(const FieldDescriptor* field) {
  string fieldName = field->name();
  return (fieldName[fieldName.size()-1] == 's') ?
      fieldName.substr(0, fieldName.size()-1) : fieldName;
}

void QueryEngine::calculateQueryGraph() {
  Proto::initProto(query.fromStmt.fromRootProto, queryGraph.proto);
  const Descriptor* rootDescriptor =
      queryGraph.proto.defaultInstance->GetDescriptor();
  queryGraph.root.type = ROOT;
  queryGraph.root.objName = rootDescriptor->name();
  std::transform(queryGraph.root.objName.begin(), queryGraph.root.objName.end(),
                 queryGraph.root.objName.begin(), ::tolower);
  for (const SelectField& selectField : query.selectStmt.selectFields) {
    const Descriptor* parentDescriptor = rootDescriptor;
    Node* parent = &(queryGraph.root);
    Field field;
    vector<string> selectFieldParts = splitString(selectField.identifier, '.');
    for (size_t j=0; j<selectFieldParts.size(); j++) {
      const FieldDescriptor* fieldPart =
          parentDescriptor->FindFieldByName(selectFieldParts[j]);
      if (fieldPart == nullptr) {
        throw runtime_error("No fieldPart by name " + selectFieldParts[j]);
      }
      field.fieldParts.push_back(fieldPart);
      if (j != (selectFieldParts.size()-1)) {
        if (fieldPart->type() != FieldDescriptor::Type::TYPE_MESSAGE) {
          throw runtime_error(
              "FieldPart " + selectFieldParts[j] +
              " expected to be message but found" + fieldPart->type_name());
        }
        if (fieldPart->label() == FieldDescriptor::LABEL_REPEATED) {
          Node& child = parent->children[field];
          child.type = REPEATED_MESSAGE;
          child.objName = constructObjNameForRepeated(fieldPart);
          child.repeatedField = field;
          parent = &child;
          field.fieldParts.clear();
        }
        parentDescriptor = fieldPart->message_type();
      } else {
        if (fieldPart->type() == FieldDescriptor::Type::TYPE_MESSAGE) {
          throw runtime_error("FieldPart " + selectFieldParts[j] +
                              " not expected to be message ");
        }
        if (fieldPart->label() == FieldDescriptor::LABEL_REPEATED) {
          Node& child = parent->children[field];
          child.type = REPEATED_PRIMITIVE;
          child.objName = constructObjNameForRepeated(fieldPart);
          child.repeatedField = field;
          queryGraph.selectFields.push_back(field);
        } else {
          parent->selectFields.push_back(field);
          queryGraph.selectFields.push_back(field);
        }
      }
    }
  }
}

void QueryEngine::walkNode(const Node& node,
                           int& indent,
                           const SelectFieldsFn& selectFieldsFn,
                           const StartForAllFn& startForAllFn,
                           const uint32_t indentInc,
                           map<int, const Node*>& endFieldsMap) {
  selectFieldsFn(indent, node);
  for (const auto& e : node.children) {
    const Node& child = e.second;
    startForAllFn(indent, child, node);
    endFieldsMap.emplace(-indent, &child);
    indent += indentInc;
    walkNode(child, indent, selectFieldsFn, startForAllFn,
             indentInc, endFieldsMap);
  }
}

void QueryEngine::walkNode(const Node& root,
                           const SelectFieldsFn& selectFieldsFn,
                           const StartForAllFn& startForAllFn,
                           const EndForAllFn& endForAllFn,
                           const uint32_t indentInc) {
  int indent = 0;
  map<int, const Node*> endNodeMap;
  walkNode(root, indent, selectFieldsFn, startForAllFn,
           indentInc, endNodeMap);
  for (const auto& e : endNodeMap) {
    int indent = -e.first;
    const Node* node = e.second;
    endForAllFn(indent, *node);
  }
}

void QueryEngine::printPlan() {
  SelectFieldsFn selectFieldsFn = [this](int indent, const Node& node) {
    if (node.type == REPEATED_PRIMITIVE) {
      out << string(indent, ' ') << "print " << node.objName << endl;
    } else {
      for (const Field& field : node.selectFields) {
        out << string(indent, ' ') << "print " << field.accessor(node.objName) << endl;
      }
    }
  };
  StartForAllFn startForAllFn = [this](int indent, const Node& node, const Node& parent) {
    out << string(indent, ' ') << "for each " << node.objName << " in "
        << node.repeatedField.accessor(parent.objName) << " {" << endl;
  };
  EndForAllFn endForAllFn = [this](int indent, const Node& node) {
    out << string(indent, ' ') << "} //" << node.objName << endl;
  };
  walkNode(queryGraph.root, selectFieldsFn, startForAllFn, endForAllFn);
}

void QueryEngine::printCode() {
  out << "#include \"" << queryGraph.proto.protoHeaderInclude << "\"" << endl;
  out << "#include \"generated_common.h\"" << endl << endl;
  out << "using namespace std;" << endl;
  out << "using namespace " << queryGraph.proto.protoNamespace << ";" << endl << endl;
  SelectFieldsFn selectFieldsFn = [](int indent, const Node& node) {};
  StartForAllFn startForAllFn = [](int indent, const Node& node, const Node& parent) {};
  EndForAllFn endForAllFn = [](int indent, const Node& node) {};

  vector<const Field*> allSelectFields;
  selectFieldsFn = [&](int indent, const Node& node) {
    if (node.type == REPEATED_PRIMITIVE) {
      allSelectFields.push_back(&node.repeatedField);
    } else {
      for (const Field& field : node.selectFields) {
        allSelectFields.push_back(&field);
      }
    }
  };
  walkNode(queryGraph.root, selectFieldsFn, startForAllFn, endForAllFn);

  // select fields header
  if (queryGraph.selectFields.size() != query.selectStmt.selectFields.size()) {
    throw runtime_error(
        "queryGraph.querySelects.size()=" + to_string(queryGraph.selectFields.size()) +
        "query.selects.size()=" + to_string(query.selectStmt.selectFields.size()));
  }
  string header = "vector<string> header = {\n";
  map<uint32_t, uint32_t> querySelectsIdxToTupleIdxMap;
  for (size_t i=0; i<queryGraph.selectFields.size(); i++) {
    int idx = -1;
    for (size_t j=0; j<allSelectFields.size(); j++) {
      if (*allSelectFields[j] == queryGraph.selectFields[i]) {
        idx = j;
        break;
      }
    }
    if (idx == -1) {
      throw runtime_error(
          "Unable to find " + queryGraph.selectFields[i].accessor("") +
          " in tuples list");
    }
    querySelectsIdxToTupleIdxMap[i] = idx;
    header += "  \"" + query.selectStmt.selectFields[i].identifier + "\",\n";
  }
  header += "};";
  out << header << endl;

  map<const Field*, string> selectFieldVarMap;
  map<const Field*, string> selectFieldTypeMap;
  map<const Field*, string> selectFieldDefaultMap;
  for (uint32_t i=0; i<allSelectFields.size(); i++) {
    const Field* field = allSelectFields[i];
    selectFieldVarMap[field] = "s" + to_string(i);
    selectFieldTypeMap[field] = "S" + to_string(i);
    selectFieldDefaultMap[field] = selectFieldTypeMap[field] + "()";
    string type = "optional<" + field->type() + ">";
    string spaces(((type.size() < 16) ? (16-type.size()) : 0), ' ');
    out << "using " << selectFieldTypeMap[field] << " = " << type << ";"
        << spaces << " /*" << field->accessor("") << "*/" << endl;
  }
  string tupleType = "using TupleType = tuple<";
  tupleType += joinVec<const Field*>(
      ", ", allSelectFields,
      [&](const Field* field) {return selectFieldTypeMap[field];});
  tupleType += ">;";
  out << tupleType << endl;
  out << endl;

  out << "void runSelect(const "
      << queryGraph.proto.defaultInstance->GetDescriptor()->name() << "& "
      << queryGraph.root.objName << ", vector<TupleType>& tuples) {" << endl;
  vector<const Field*> selectFieldsProcessed;
  selectFieldsFn = [&](int indent, const Node& node) {
    string ind = string(indent+2, ' ');
    if (node.type == REPEATED_PRIMITIVE) {
      const Field* field = &node.repeatedField;
      out << ind << selectFieldTypeMap[field] << " "
          << selectFieldVarMap[field] << " = " << node.objName << ";" << endl;
      selectFieldsProcessed.push_back(field);
    } else {
      for (const Field& field : node.selectFields) {
        out << ind << selectFieldTypeMap[&field] << " "
            << selectFieldVarMap[&field] << " = "
            << selectFieldDefaultMap[&field] << ";" << endl;
        vector<string> checks;
        for (uint32_t i=0; i<field.fieldParts.size(); i++) {
          checks.push_back(field.hasAccessor(node.objName, i+1));
        }
        out << ind << "if(" << joinVec<string>(" && ", checks, string2str)
            << ") {" << endl;
        out << ind << "  " << selectFieldVarMap[&field] << " = "
            << field.accessor(node.objName) << ";" << endl;
        out << ind << "}" << endl;
        selectFieldsProcessed.push_back(&field);
      }
    }
    if (selectFieldsProcessed.size() == allSelectFields.size()) {
      string selectList = joinVec<const Field*>(", ", allSelectFields,
          [&](const Field* field) {return selectFieldVarMap[field];});
      out << ind << "tuples.emplace_back(" + selectList + ");" << endl;
    }
  };
  startForAllFn = [this](int indent, const Node& node, const Node& parent) {
    string ind = string(indent+2, ' ');
    out << ind << "if (" << node.repeatedField.sizeAccessor(parent.objName)
        << " > 0) {" << endl;
    out << ind << "  for (const " << node.repeatedField.type() << "& "
        << node.objName << " : " << node.repeatedField.accessor(parent.objName)
        << ") {" << endl;
  };
  set<const Field*> endedFieldSet;
  endForAllFn = [&](int indent, const Node& node) {
    string ind = string(indent+2, ' ');
    out << ind << "  }" << endl;
    out << ind << "} else { // no " << node.objName << endl;
    if (node.type == REPEATED_PRIMITIVE) {
      endedFieldSet.insert(&node.repeatedField);
    } else {
      for (const Field& field : node.selectFields) {
        endedFieldSet.insert(&field);
      }
    }
    string selectList = joinVec<const Field*>(
        ", ", selectFieldsProcessed,
        [&](const Field* field) {
            return endedFieldSet.find(field) == endedFieldSet.end() ?
                selectFieldVarMap[field] : selectFieldDefaultMap[field];
        });
    out << ind << "  " << "tuples.emplace_back(" + selectList + ");" << endl;
    out << ind << "}" << endl;
  };
  walkNode(queryGraph.root, selectFieldsFn, startForAllFn, endForAllFn, 4);
  out << "}" << endl;

  // print tuples
  out << R"fff(
void printTuples(const vector<TupleType>& tuples) {
  vector<size_t> sizes;
  for (size_t i=0; i<header.size(); i++) {
    sizes.push_back(header[i].size());
  }
)fff";
  out << "  for (const TupleType& t : tuples) {" << endl;
  for (size_t i=0; i<query.selectStmt.selectFields.size(); i++) {
    out << "    sizes[" << i << "] = max(sizes[" << i << "], stringify(get<"
        << querySelectsIdxToTupleIdxMap[i] << ">(t)).size());" << endl;
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
  for (size_t i=0; i<query.selectStmt.selectFields.size(); i++) {
    out << "    cout << " << ((i==0) ? "         " : "\" | \" << ")
        << "setw(sizes[" << i << "]) << "
        << "stringify(get<" << querySelectsIdxToTupleIdxMap[i]
        << ">(t));" << endl;
  }
  out << "    cout << endl;" << endl;
  out << "  }" << endl;
  out << "}" << endl << endl;

  // main
  out << "int main(int argc, char** argv) {" << endl;
  out << "  " << queryGraph.proto.defaultInstance->GetDescriptor()->name()
      << " " << queryGraph.root.objName << ";" << endl;
  string fromFile = (query.fromStmt.fromFile.find("argv") == 0) ?
      query.fromStmt.fromFile : ("\"" + query.fromStmt.fromFile + "\"");
  out << "  parsePbFromFile(" << fromFile << ", " << queryGraph.root.objName
      << ");" << endl;
  out << "  vector<TupleType> tuples;" << endl;
  out << "  runSelect(company, tuples);" << endl;
  out << "  printTuples(tuples);" << endl;
  out << "}" << endl;
}

void QueryEngine::process() {
  if (!query.parse()) {
    throw runtime_error("Parsing select query failed");
  }
  out << "/*" << endl;
  out << query.str() << endl << endl;
  calculateQueryGraph();
  printPlan();
  out << "*/" << endl;
  printCode();
}

int main(int argc, char** argv) {
  if (argc < 2) {
    cerr << "Usage: ./QueryEngine <sql-query>" << endl;
    exit(1);
  }
  QueryEngine engine(argv[1], cout);
  engine.process();
}

