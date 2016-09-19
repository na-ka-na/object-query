#!/bin/bash
source common_test.sh

sql="SELECT all_employees.id, financial.quarterly_revenues/all_employees.active_direct_reports FROM ('argv[1]', 'Example1.Company') WHERE all_employees.active_direct_reports > 0 AND financial.quarterly_revenues*2 > 1"
run_test 3 "$sql"
exit $?
