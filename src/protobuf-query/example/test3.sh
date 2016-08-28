#!/bin/bash
source common_test.sh

sql="SELECT all_employees.id, financial.quarterly_revenues/all_employees.active_direct_reports FROM ('argv[1]', 'Example1.Company') WHERE all_employees.active_direct_reports > 0 AND financial.quarterly_revenues*2 > 1"
generated_file="generated_query3"
proto_file="../../../src/protobuf-query/example_protos/example1.proto.gz"
golden_out="golden3.out"
actual_out="actual3.out"
run_test "$sql" "$generated_file" "$proto_file" "$golden_out" "$actual_out"
exit $?