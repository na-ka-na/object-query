#!/bin/bash
source common_test.sh

sql="SELECT STR(all_employees.id) + ': ' + all_employees.name + ' (' + STR(all_employees.active) + ')' AS employee, STR(all_employees.id) + ' ' + employee + ' ' + STR(founded) AS employee2 FROM ('argv[1]', 'Example1.Company') WHERE employee2 LIKE '.*true.*'"
run_test 5 "$sql"
exit $?
