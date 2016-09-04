#!/bin/bash
source common_test.sh

sql="SELECT STR(all_employees.id) + ': ' + all_employees.name + ' (' + STR(all_employees.active) + ')' AS employee FROM ('argv[1]', 'Example1.Company')"
generated_file="generated_query5"
proto_file="../../../src/protobuf-query/example_protos/example1.proto.gz"
golden_out="golden5.out"
actual_out="actual5.out"
run_test "$sql" "$generated_file" "$proto_file" "$golden_out" "$actual_out"
exit $?
