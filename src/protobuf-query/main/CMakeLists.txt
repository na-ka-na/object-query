include_directories(
    ${CMAKE_SOURCE_DIR}/protobuf-query
    ${CMAKE_SOURCE_DIR}/protobuf-query/engine
)

add_executable(RunQuery run_query.cc)
target_link_libraries(RunQuery dl gflags QueryEngine)
