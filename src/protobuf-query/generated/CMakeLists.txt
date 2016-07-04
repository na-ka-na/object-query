
include_directories(
    SYSTEM ${CMAKE_BINARY_DIR}/protobuf-query/example_protos
    ${CMAKE_SOURCE_DIR}/protobuf-query/engine
)

add_executable(sql1 sql1.cc)
target_link_libraries(sql1 ExampleProtos)

