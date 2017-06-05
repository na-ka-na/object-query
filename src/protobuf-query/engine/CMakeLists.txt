
include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/protobuf-query
    yy
)

add_library(SqlParser select_query.cc yy/sql.tab.cc yy/sql.lex.cc)
target_link_libraries(Utils)

add_library(QueryEngine query_engine.cc)
target_link_libraries(QueryEngine ${PROTOBUF_LIBRARIES} Utils SqlParser)

add_subdirectory(test)
