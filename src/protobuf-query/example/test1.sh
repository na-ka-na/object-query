#!/bin/bash
source common_test.sh

proto_spec="../../../src/protobuf-query/example/proto_spec.txt"
sql="SELECT financial.quarterly_profits,financial.quarterly_revenues,all_employees.id,all_employees.name,all_employees.active,all_employees.active_direct_reports,founded,board_of_directors FROM ('argv[1]', 'Example1.Company')"
generated_file="generated_query1"
proto_file="../../../src/protobuf-query/example/example1.proto.gz"
golden_out="golden1.out"
actual_out="actual1.out"
run_test "$proto_spec" "$sql" "$generated_file" "$proto_file" "$golden_out" "$actual_out"
exit $?
