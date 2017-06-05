
include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/sql
    ${CMAKE_SOURCE_DIR}/protobuf-query
)

add_library(QueryEngine query_engine.cc)
target_link_libraries(QueryEngine ${PROTOBUF_LIBRARIES} Utils SqlParser)

add_subdirectory(test)
