
function run_test() {
    proto_spec="$1"
    sql="$2"
    generated_file="$3"
    proto_file="$4"
    golden_out="$5"
    actual_out="$6"
    ./RunQuery "$proto_spec" "$sql" "$generated_file" "$proto_file" > "$actual_out"
    run_query_exit=$?
    if [[ $run_query_exit == 0 ]]
    then
        diff_output=`diff --brief $golden_out $actual_out`
        if [[ "$diff_output" ]]
        then
            echo Diff failed
            echo $diff_output
            return 1
        fi
    else
        echo RunQuery failed
        return $run_query_exit
    fi
    return 0
}
