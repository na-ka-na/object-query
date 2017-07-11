#pragma once

#include <string>
#include "json_generated_common.h"

using namespace std;

string EmployeeStr(const JsonValue& name, const JsonValue& id) {
  return name.toString() + "(id=" + id.toString() + ")";
}
