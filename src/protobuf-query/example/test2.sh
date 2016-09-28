#!/bin/bash
source common_test.sh

sql="SELECT all_employees.id, all_employees.name
     FROM Example1.Company
     WHERE all_employees.id IS NOT NULL AND
           ((all_employees.name='def' AND all_employees.active=true) OR
              all_employees.name='abc') AND
           financial.quarterly_profits>0"
run_test 2 "$sql"
exit $?
