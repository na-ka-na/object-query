#!/bin/bash
source common_test.sh

sql="SELECT all_employees.name, all_employees.active_direct_reports FROM ('argv[1]', 'Example1.Company') WHERE all_employees.active_direct_reports IS NOT NULL"
generated_file="generated_query6"
proto_file="../../../src/protobuf-query/example/example1.proto.gz"
golden_out="golden6.out"
actual_out="actual6.out"
run_test "$sql" "$generated_file" "$proto_file" "$golden_out" "$actual_out"
exit $?
