include_directories(
    SYSTEM ${CMAKE_BINARY_DIR}/protobuf-query/example_protos
    ${CMAKE_SOURCE_DIR}/protobuf-query
    ${CMAKE_SOURCE_DIR}/protobuf-query/engine
)

add_executable(RunQuery run_query.cc)
target_link_libraries(RunQuery ExampleProtos QueryEngine)

add_executable(GeneratedQuery generated_query.cc)
target_link_libraries(GeneratedQuery ExampleProtos)

add_custom_target(CpTestingScripts ALL COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/test*sh ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(CpTestingGoldens ALL COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/golden*out ${CMAKE_CURRENT_BINARY_DIR})

add_test(Test1 ${CMAKE_CURRENT_BINARY_DIR}/test1.sh)
