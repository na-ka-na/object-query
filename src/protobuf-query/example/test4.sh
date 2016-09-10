#!/bin/bash
source common_test.sh

sql="SELECT financial.quarterly_profits,all_employees.name,all_employees.active FROM ('argv[1]', 'Example1.Company') ORDER BY all_employees.active,financial.quarterly_profits DESC,all_employees.id ASC"
generated_file="generated_query4"
proto_file="../../../src/protobuf-query/example/example1.proto.gz"
golden_out="golden4.out"
actual_out="actual4.out"
run_test "$sql" "$generated_file" "$proto_file" "$golden_out" "$actual_out"
exit $?
