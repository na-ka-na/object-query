include_directories(
    SYSTEM ${CMAKE_BINARY_DIR}/protobuf-query/example_protos
    ${CMAKE_SOURCE_DIR}/protobuf-query
    ${CMAKE_SOURCE_DIR}/protobuf-query/engine
)

add_executable(RunQuery run_query.cc)
target_link_libraries(RunQuery ExampleProtos QueryEngine)

add_executable(generated_query1 generated_query1.cc)
target_link_libraries(generated_query1 ExampleProtos)

add_executable(generated_query2 generated_query2.cc)
target_link_libraries(generated_query2 ExampleProtos)

add_custom_target(CpTestingScripts ALL COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/*test*sh ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(CpTestingGoldens ALL COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/golden*out ${CMAKE_CURRENT_BINARY_DIR})

add_test(Test1 ${CMAKE_CURRENT_BINARY_DIR}/test1.sh)
add_test(Test2 ${CMAKE_CURRENT_BINARY_DIR}/test2.sh)
