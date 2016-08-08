#!/bin/bash
source common_test.sh

sql="SELECT all_employees.id, all_employees.name FROM ('argv[1]', 'Example1.Company') WHERE all_employees.id IS NOT NULL AND ((all_employees.name='def' AND all_employees.active=true) OR all_employees.name='abc') AND financial.quarterly_profits>0"
generated_file="generated_query2"
proto_file="../../../src/protobuf-query/example_protos/example1.proto.gz"
golden_out="golden2.out"
actual_out="actual2.out"
run_test "$sql" "$generated_file" "$proto_file" "$golden_out" "$actual_out"
exit $?
