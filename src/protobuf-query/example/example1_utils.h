#include "generated_common.h"
#include "example1.pb.h"

string EmployeeStr(const string& name, int id) {
  return name + "(id=" + to_string(id) + ")";
}

string STR(const Example1::Employee& t) {
  return EmployeeStr(t.name(), t.id());
}
