#pragma once

#include "generated_common.h"
#include "example1.pb.h"

string EmployeeStr(const MyString& name, int id) {
  stringstream ss;
  Print(ss, name);
  Print(ss, (const char*) "(id=");
  Print(ss, id);
  Print(ss, (const char*) ")");
  return ss.str();
}

string STR(const Example1::Employee& t) {
  return EmployeeStr(MyString(&t.name()), t.id());
}
