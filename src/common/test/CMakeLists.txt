include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/common/test
)

add_executable(GeneratedCommonTest generated_common_test.cc)
target_link_libraries(GeneratedCommonTest)
add_test(GeneratedCommonTest GeneratedCommonTest)
