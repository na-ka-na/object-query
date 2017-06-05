include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/sql
    ${CMAKE_SOURCE_DIR}/protobuf-query
)

add_executable(RunQuery run_query.cc)
target_link_libraries(RunQuery dl gflags QueryEngine)
