
include_directories(
    yy
)

add_library(SqlParser select_query.cc yy/sql.tab.cc yy/sql.lex.cc)

add_library(QueryEngine query_engine.cc)
target_link_libraries(QueryEngine SqlParser)
