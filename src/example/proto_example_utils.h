#pragma once

#include "generated_common.h"
#include "example1.pb.h"

string EmployeeStr(const MyString& name, int id) {
  return name.toString() + "(id=" + to_string(id) + ")";
}

string STR(const Example1::Employee& t) {
  return EmployeeStr(MyString(&t.name()), t.id());
}
