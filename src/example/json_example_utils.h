#pragma once

#include <string>
#include "json_generated_common.h"

using namespace std;

string EmployeeStr(const JsonValue& name, const JsonValue& id) {
  stringstream ss;
  ss << name << "(id=" << id << ")";
  return ss.str();
}
