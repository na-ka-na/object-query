
function run_test() {
    testnum="$1"
    sql="$2"
    proto_file="../../../src/protobuf-query/example/example1.proto.gz"
    golden_out="golden${testnum}.out"
    actual_out="actual${testnum}.out"
    ../main/RunQuery --codeGenDir=../../../src/protobuf-query/example \
               --codeGenPrefix="generated_query${testnum}" --codeCompileDir=. \
               --protoName=Example1.Company --cppProtoNamespace=Example1 \
               --cppProtoHeader=example1.pb.h --cppProtoLib=libExampleProtos.dylib \
               "$sql" "$proto_file"  > "$actual_out"
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
