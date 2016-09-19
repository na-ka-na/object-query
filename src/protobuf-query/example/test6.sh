#!/bin/bash
source common_test.sh

sql="SELECT all_employees.name, all_employees.active_direct_reports
     FROM ('argv[1]', 'Example1.Company')
     WHERE all_employees.active_direct_reports IS NOT NULL"
run_test 6 "$sql"
exit $?
