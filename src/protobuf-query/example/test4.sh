#!/bin/bash
source common_test.sh

sql="SELECT financial.quarterly_profits,all_employees.name,all_employees.active
     FROM Example1.Company
     ORDER BY all_employees.active,financial.quarterly_profits DESC,all_employees.id ASC"
run_test 4 "$sql"
exit $?
