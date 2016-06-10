
include_directories(
    SYSTEM ${CMAKE_BINARY_DIR}/protobuf-query/example_protos
    yy
)

add_library(SqlParser yy/sql.tab.cc select_query.cc)

add_executable(QueryEngine query_engine.cc)
target_link_libraries(QueryEngine ExampleProtos SqlParser)
