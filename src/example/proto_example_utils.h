#pragma once

#include "generated_common.h"
#include "example1.pb.h"

string EmployeeStr(const MyString& name, int id) {
  stringstream ss;
  ss << name << "(id=" << id << ")";
  return ss.str();
}

string STR(const Example1::Employee& t) {
  return EmployeeStr(MyString(&t.name()), t.id());
}
