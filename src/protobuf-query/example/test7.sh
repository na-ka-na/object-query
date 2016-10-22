#!/bin/bash
source common_test.sh

sql="SELECT all_employees
     FROM Example1.Company"
run_test 7 "$sql"
exit $?
