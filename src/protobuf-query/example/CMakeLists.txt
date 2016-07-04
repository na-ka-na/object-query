include_directories(
    SYSTEM ${CMAKE_BINARY_DIR}/protobuf-query/example_protos
    ${CMAKE_SOURCE_DIR}/protobuf-query/engine
)

add_executable(RunQuery run_query.cc)
target_link_libraries(RunQuery ExampleProtos QueryEngine)
