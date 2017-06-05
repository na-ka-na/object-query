include_directories(
    ${CMAKE_SOURCE_DIR}/sql
    ${CMAKE_SOURCE_DIR}/common/test
)

add_executable(SelectQueryTest select_query_test.cc)
target_link_libraries(SelectQueryTest SqlParser)
add_test(SelectQueryTest SelectQueryTest)
