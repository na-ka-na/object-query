#!/bin/bash
source common_test.sh

sql="SELECT STR(all_employees.id) + ': ' + all_employees.name + ' (' + STR(all_employees.active) + ')' AS employee, STR(all_employees.id) + ' ' + employee + ' ' + STR(founded) AS employee2 FROM ('argv[1]', 'Example1.Company') WHERE employee2 LIKE '.*true.*'"
generated_file="generated_query5"
proto_file="../../../src/protobuf-query/example/example1.proto.gz"
golden_out="golden5.out"
actual_out="actual5.out"
run_test "$sql" "$generated_file" "$proto_file" "$golden_out" "$actual_out"
exit $?
