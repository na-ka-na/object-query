#pragma once

#include <string>
#include "json_generated_common.h"

using namespace std;

string EmployeeStr(const JsonValue& name, const JsonValue& id) {
  stringstream ss;
  Print(ss, name);
  Print(ss, (const char*) "(id=");
  Print(ss, id);
  Print(ss, (const char*) ")");
  return ss.str();
}
