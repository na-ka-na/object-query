include_directories(
    SYSTEM ${PROTOBUF_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}/protobuf-query/example
    ${CMAKE_SOURCE_DIR}/protobuf-query
    ${CMAKE_SOURCE_DIR}/protobuf-query/engine
)

file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")
PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders ${ProtoFiles})

add_library(ExampleProtos SHARED ${ProtoSources} ${ProtoHeaders})
target_link_libraries(ExampleProtos ${PROTOBUF_LIBRARIES})

add_executable(RunQuery run_query.cc)
target_link_libraries(RunQuery ExampleProtos QueryEngine)

add_executable(generated_query1 generated_query1.cc)
target_link_libraries(generated_query1 ExampleProtos)

add_executable(generated_query2 generated_query2.cc)
target_link_libraries(generated_query2 ExampleProtos)

add_executable(generated_query3 generated_query3.cc)
target_link_libraries(generated_query3 ExampleProtos)

add_executable(generated_query4 generated_query4.cc)
target_link_libraries(generated_query4 ExampleProtos)

add_executable(generated_query5 generated_query5.cc)
target_link_libraries(generated_query5 ExampleProtos)

add_executable(generated_query6 generated_query6.cc)
target_link_libraries(generated_query6 ExampleProtos)

add_custom_target(CpTestingScripts ALL COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/*test*sh ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(CpTestingGoldens ALL COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/golden*out ${CMAKE_CURRENT_BINARY_DIR})

add_test(Test1 ${CMAKE_CURRENT_BINARY_DIR}/test1.sh)
add_test(Test2 ${CMAKE_CURRENT_BINARY_DIR}/test2.sh)
add_test(Test3 ${CMAKE_CURRENT_BINARY_DIR}/test3.sh)
add_test(Test4 ${CMAKE_CURRENT_BINARY_DIR}/test4.sh)
add_test(Test5 ${CMAKE_CURRENT_BINARY_DIR}/test5.sh)
add_test(Test6 ${CMAKE_CURRENT_BINARY_DIR}/test6.sh)
