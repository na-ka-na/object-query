
include_directories(
    ${CMAKE_SOURCE_DIR}/common
    yy
)

add_library(SqlParser select_query.cc yy/sql.tab.cc yy/sql.lex.cc)
target_link_libraries(Utils)

add_subdirectory(test)
