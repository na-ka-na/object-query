
include_directories(
    SYSTEM ${CMAKE_BINARY_DIR}/protobuf-query/example_protos
    yy
)

add_library(SqlParser select_query.cc yy/sql.tab.cc yy/sql.lex.cc)

add_executable(QueryEngine query_engine.cc)
target_link_libraries(QueryEngine ExampleProtos SqlParser)
