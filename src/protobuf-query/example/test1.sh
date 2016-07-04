#!/usr/bin/env bash
./RunQuery "SELECT financial.quarterly_profits,financial.quarterly_revenues,all_employees.id,all_employees.name,all_employees.active,all_employees.active_direct_reports,founded,board_of_directors FROM ('argv[1]', 'Example1.Company')" ./GeneratedQuery ../../../src/protobuf-query/example_protos/example1.proto.gz > actual1.out
if [[ $? == 0 ]]
then
    diff_output="$(diff --brief golden1.out actual1.out)"
    if [[ "$diff_output" ]]
    then
        echo Diff failed
        echo $diff_output
        exit 1
    fi
else
    echo RunQuery failed
    exit $?
fi
