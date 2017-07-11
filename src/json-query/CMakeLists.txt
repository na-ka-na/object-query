
include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/sql
    ${CMAKE_SOURCE_DIR}/json-query
)

add_library(JsonQueryEngine json_query_engine.cc)
target_link_libraries(JsonQueryEngine Utils SqlParser)

add_subdirectory(test)
