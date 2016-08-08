
#include <algorithm>
#include "utils.h"
#include "query_engine.h"

using namespace std;
using namespace google::protobuf;

bool Field::operator<(const Field& other) const {
  return fieldParts < other.fieldParts;
}

bool Field::operator==(const Field& other) const {
  return fieldParts == other.fieldParts;
}

string Field::type() const {
  ASSERT(!fieldParts.empty(), "Can't determine type of empty field");
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

void Node::walkNode(Node* parent,
                    Node& node,
                    int& indent,
                    StartNodeFn& startNodeFn,
                    uint32_t indentInc,
                    map<int, Node*>& endNodesMap) {
  startNodeFn(indent, node, parent);
  endNodesMap.emplace(-indent, &node);
  for (auto& e : node.children) {
    Node& child = e.second;
    indent += indentInc;
    walkNode(&node, child, indent, startNodeFn, indentInc, endNodesMap);
  }
}

void Node::walkNode(Node& root,
                    StartNodeFn& startNodeFn,
                    EndNodeFn& endNodeFn,
                    uint32_t indentInc) {
  int indent = 0;
  map<int, Node*> endNodesMap;
  walkNode(nullptr, root, indent, startNodeFn, indentInc, endNodesMap);
  for (auto& e : endNodesMap) {
    int indent = -e.first;
    Node* node = e.second;
    endNodeFn(indent, *node);
  }
}

string QueryGraph::constructObjNameForRepeated(const FieldDescriptor* field) {
  string fieldName = field->name();
  return (fieldName[fieldName.size()-1] == 's') ?
      fieldName.substr(0, fieldName.size()-1) : fieldName;
}

Field QueryGraph::addReadIdentifier(const string& identifier, bool partOfSelect) {
  const Descriptor* parentDescriptor = proto.defaultInstance->GetDescriptor();
  Node* parent = &root;
  Field field;
  vector<string> selectFieldParts = splitString(identifier, '.');
  for (size_t j=0; j<selectFieldParts.size(); j++) {
    const FieldDescriptor* fieldPart =
        parentDescriptor->FindFieldByName(selectFieldParts[j]);
    ASSERT(fieldPart != nullptr, "No fieldPart by name", selectFieldParts[j]);
    field.fieldParts.push_back(fieldPart);
    if (j != (selectFieldParts.size()-1)) {
      ASSERT(fieldPart->type() == FieldDescriptor::Type::TYPE_MESSAGE,
             "FieldPart", selectFieldParts[j], "expected to be message but is",
             fieldPart->type_name());
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
      ASSERT(fieldPart->type() != FieldDescriptor::Type::TYPE_MESSAGE,
             "FieldPart", selectFieldParts[j], "expected not to be message");
      if (fieldPart->label() == FieldDescriptor::LABEL_REPEATED) {
        Node& child = parent->children[field];
        child.type = REPEATED_PRIMITIVE;
        child.objName = constructObjNameForRepeated(fieldPart);
        child.repeatedField = field;
        parent = &child;
      }
      if (partOfSelect) {
        parent->selectFields.insert(field);
      }
      parent->allFields.insert(field);
    }
  }
  return field;
}

void QueryGraph::readFieldsFromSelect(const SelectStmt& selectStmt) {
  for (size_t i=0; i<selectStmt.selectFields.size(); i++) {
    selectFieldIdxReadFieldMap[i] =
        addReadIdentifier(selectStmt.selectFields[i].identifier, true);
  }
}

void QueryGraph::processWhere(const WhereStmt& whereStmt) {
  vector<const BooleanExpr*> andClauses;
  whereStmt.canoncialize(andClauses);
  for (const BooleanExpr* clause : andClauses) {
    set<string> identifiers;
    clause->getAllIdentifiers(identifiers);
    set<Field> fields;
    map<string, Field> whereClauseIdMap;
    for (const string& identifier : identifiers) {
      Field f = addReadIdentifier(identifier, false);
      fields.insert(f);
      whereClauseIdMap.emplace(identifier, f);
    }
    StartNodeFn startNodeFn = [&](int indent, Node& node, Node* parent){
      if (fields.empty()) {
        return;
      }
      for(auto it = fields.begin(); it != fields.end();) {
        if (node.allFields.find(*it) != node.allFields.end()) {
          it = fields.erase(it);
        } else {
          ++it;
        }
      }
      if (fields.empty()) {
        node.whereClauses.push_back(clause);
        node.whereClauseIdMaps.push_back(whereClauseIdMap);
      }
    };
    EndNodeFn endNodeFn = [](int indent, Node& node) {};
    Node::walkNode(root, startNodeFn, endNodeFn);
  }
}

void QueryGraph::calculateGraph(const SelectQuery& query) {
  Proto::initProto(query.fromStmt.fromRootProto, proto);
  const Descriptor* rootDescriptor = proto.defaultInstance->GetDescriptor();
  root.type = ROOT;
  root.objName = rootDescriptor->name();
  std::transform(root.objName.begin(), root.objName.end(),
                 root.objName.begin(), ::tolower);
  readFieldsFromSelect(query.selectStmt);
  processWhere(query.whereStmt);
}

QueryEngine::QueryEngine(const string& rawSql, ostream& out) :
    query(SelectQuery(rawSql)), out(out) {}

void QueryEngine::printPlan() {
  StartNodeFn startNodeFn = [this](int indent, const Node& node, const Node* parent) {
    if (!parent) { //root
      out << string(indent, ' ') << "for (1..1) {" << endl;
    } else {
      out << string(indent, ' ') << "for each " << node.objName << " in "
          << node.repeatedField.accessor(parent->objName) << " {" << endl;
    }
    indent+=2;
    for (const BooleanExpr* expr : node.whereClauses) {
      out << string(indent, ' ') << "if (!" << expr->str()
          << ") { continue; }" << endl;
    }
    for (const Field& field : node.selectFields) {
      out << string(indent, ' ') << "print "
          << ((node.type == REPEATED_PRIMITIVE) ? node.objName :
              field.accessor(node.objName)) << endl;
    }
  };
  EndNodeFn endNodeFn = [this](int indent, const Node& node) {
    out << string(indent, ' ') << "} //" << node.objName << endl;
  };
  Node::walkNode(queryGraph.root, startNodeFn, endNodeFn, 2);
}

void QueryEngine::printCode() {
  out << "#include \"" << queryGraph.proto.protoHeaderInclude << "\"" << endl;
  out << "#include \"generated_common.h\"" << endl << endl;
  out << "using namespace std;" << endl;
  out << "using namespace " << queryGraph.proto.protoNamespace << ";" << endl << endl;
  StartNodeFn startNodeFn = [](int indent, const Node& node, const Node* parent) {};
  EndNodeFn endNodeFn = [](int indent, const Node& node) {};

  vector<Field> allSelectFields, allFields;
  startNodeFn = [&](int indent, const Node& node, const Node* parent) {
    for (const Field& field : node.selectFields) {
      allSelectFields.push_back(field);
    }
    for (const Field& field : node.allFields) {
      allFields.push_back(field);
    }
  };
  Node::walkNode(queryGraph.root, startNodeFn, endNodeFn);

  // select fields header
  string header = "vector<string> header = {\n";
  for (const SelectField& selectField : query.selectStmt.selectFields) {
    header += "  \"" + selectField.identifier + "\",\n";
  }
  header += "};";
  out << header << endl;

  map<Field, string> fieldVarMap;
  map<Field, string> fieldTypeMap;
  map<Field, string> fieldDefaultMap;
  for (uint32_t i=0; i<allFields.size(); i++) {
    const Field& field = allFields[i];
    fieldVarMap[field] = "s" + to_string(i);
    fieldTypeMap[field] = "S" + to_string(i);
    fieldDefaultMap[field] = fieldTypeMap[field] + "()";
    string type = "optional<" + field.type() + ">";
    string spaces(((type.size() < 16) ? (16-type.size()) : 0), ' ');
    out << "using " << fieldTypeMap[field] << " = " << type << ";"
        << spaces << " /*" << field.accessor("") << "*/" << endl;
  }
  string tupleType = "using TupleType = tuple<";
  tupleType += joinVec<Field>(
      ", ", allSelectFields,
      [&](const Field& field) {return fieldTypeMap[field];});
  tupleType += ">;";
  out << tupleType << endl;
  out << endl;

  out << "void runSelect(const "
      << queryGraph.proto.defaultInstance->GetDescriptor()->name() << "& "
      << queryGraph.root.objName << ", vector<TupleType>& tuples) {" << endl;
  vector<Field> selectFieldsProcessed;
  startNodeFn = [&](int indent, const Node& node, const Node* parent) {
    string ind = string(indent+2, ' ');
    if (!parent) { //root
      out << ind << "if (" << node.objName << ".ByteSize()) {" << endl;
      out << ind << "  for (int _=0; _<1; _++) {" << endl;
    } else {
      out << ind << "if (" << node.repeatedField.sizeAccessor(parent->objName)
          << " > 0) {" << endl;
      out << ind << "  for (const " << node.repeatedField.type() << "& "
          << node.objName << " : " << node.repeatedField.accessor(parent->objName)
          << ") {" << endl;
    }
    indent+=4;
    ind = string(indent+2, ' ');
    for (const Field& field : node.allFields) {
      bool isSelect = node.selectFields.find(field) != node.selectFields.end();
      if (isSelect) {
        selectFieldsProcessed.push_back(field);
      }
      out << ind << fieldTypeMap[field] << " "
          << fieldVarMap[field] << " = "
          << ((node.type == REPEATED_PRIMITIVE) ? node.objName :
              fieldDefaultMap[field]) << ";" << endl;
      if (node.type != REPEATED_PRIMITIVE) {
        vector<string> checks;
        for (uint32_t i=0; i<field.fieldParts.size(); i++) {
          checks.push_back(field.hasAccessor(node.objName, i+1));
        }
        out << ind << "if(" << joinVec<string>(" && ", checks, string2str)
            << ") {" << endl;
        out << ind << "  " << fieldVarMap[field] << " = "
            << field.accessor(node.objName) << ";" << endl;
        out << ind << "}" << endl;
      }
    }
    // TODO(sanchay): print variable assignment and where clauses in optimal order
    for (size_t i=0; i<node.whereClauses.size(); i++) {
      const BooleanExpr* whereClause = node.whereClauses[i];
      const map<string, Field>& whereClauseIdMap = node.whereClauseIdMaps[i];
      map<string, string> idVarMap;
      for (const auto& e : whereClauseIdMap) {
        idVarMap.emplace(e.first, fieldVarMap[e.second]);
      }
      out << ind << "if (!" << whereClause->code(idVarMap)
          << ") { continue; }" << endl;
    }
    if (selectFieldsProcessed.size() == allSelectFields.size()) {
      string selectList = joinVec<Field>(", ", allSelectFields,
          [&](const Field& field) {return fieldVarMap[field];});
      out << ind << "tuples.emplace_back(" + selectList + ");" << endl;
    }
  };
  set<Field> endedFieldSet;
  endNodeFn = [&](int indent, const Node& node) {
    string ind = string(indent+2, ' ');
    out << ind << "  }" << endl;
    out << ind << "} else { // no " << node.objName << endl;
    for (const Field& field : node.selectFields) {
      endedFieldSet.insert(field);
    }
    string selectList = joinVec<Field>(
        ", ", selectFieldsProcessed,
        [&](const Field& field) {
            return endedFieldSet.find(field) == endedFieldSet.end() ?
                fieldVarMap[field] : fieldDefaultMap[field];
        });
    out << ind << "  " << "tuples.emplace_back(" + selectList + ");" << endl;
    out << ind << "}" << endl;
  };
  Node::walkNode(queryGraph.root, startNodeFn, endNodeFn, 4);
  out << "}" << endl;

  // print tuples
  map<uint32_t, uint32_t> querySelectsIdxToTupleIdxMap;
  for (size_t i=0; i<query.selectStmt.selectFields.size(); i++) {
    const Field& readField = queryGraph.selectFieldIdxReadFieldMap[i];
    int idx = -1;
    for (size_t j=0; j<allSelectFields.size(); j++) {
      if (allSelectFields[j] == readField) {
        idx = j;
        break;
      }
    }
    ASSERT(idx >= 0, "Unable to find", readField.accessor(""), "in tuples");
    querySelectsIdxToTupleIdxMap[i] = idx;
  }
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
  ASSERT(query.parse(), "Parsing select query failed");
  out << "/*" << endl;
  out << query.str() << endl << endl;
  queryGraph.calculateGraph(query);
  printPlan();
  out << "*/" << endl;
  printCode();
}
