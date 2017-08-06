#pragma once

#include "generated_common.h"
#include "example1.pb.h"

string EmployeeStr(const oq::MyString& name, int id) {
  stringstream ss;
  ss << name << "(id=" << id << ")";
  return ss.str();
}

string STR(const Example1::Employee& t) {
  return EmployeeStr(oq::MyString(&t.name()), t.id());
}
