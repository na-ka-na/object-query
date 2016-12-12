#!/bin/bash
source common_test.sh

sql="SELECT *, all_employees.* AS emp
     FROM Example1.Company"
run_test 8 "$sql"
exit $?
