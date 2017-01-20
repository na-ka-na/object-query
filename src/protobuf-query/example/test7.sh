#!/bin/bash
source common_test.sh

sql="SELECT EmployeeStr(all_employees.name, all_employees.id), STR(all_employees.id), STR(all_employees)
     FROM Example1.Company"
run_test 7 "$sql"
exit $?
