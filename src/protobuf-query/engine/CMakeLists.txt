
include_directories(
    SYSTEM ${CMAKE_BINARY_DIR}/protobuf-query/example_protos
)

add_executable(QueryEngine query_engine.cc)
target_link_libraries(QueryEngine ExampleProtos)

