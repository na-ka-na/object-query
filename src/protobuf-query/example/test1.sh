#!/bin/bash
source common_test.sh

sql="SELECT financial.quarterly_profits,financial.quarterly_revenues,all_employees.id,all_employees.name,all_employees.active,all_employees.active_direct_reports,founded,board_of_directors FROM ('argv[1]', 'Example1.Company')"
run_test 1 "$sql"
exit $?
