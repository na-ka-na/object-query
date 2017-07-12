
include_directories(
    ${CMAKE_SOURCE_DIR}/common
    yy
)

add_library(SqlParser query_tree.h query_tree.hpp select_parts.h select_query.h select_query.cc yy/sql.tab.cc yy/sql.lex.cc)
target_link_libraries(Utils)

add_subdirectory(test)
