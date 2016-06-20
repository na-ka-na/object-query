
#pragma once

#include <string>
#include <vector>
#include "utils.h"

using namespace std;

struct SelectField {
  string identifier;
  string str() const;
};

struct SelectStmt {
  bool distinct = false;
  vector<SelectField> selectFields;
  string str() const;
};

struct FromStmt {
  string fromFile;
  string fromRootProto;
  string str() const;
};

class SelectQuery;
