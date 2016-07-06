
function run_test() {
    sql="$1"
    generated_file="$2"
    proto_file="$3"
    golden_out="$4"
    actual_out="$5"
    ./RunQuery "$sql" "$generated_file" "$proto_file" > "$actual_out"
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
