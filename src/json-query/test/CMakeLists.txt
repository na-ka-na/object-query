include_directories(
    SYSTEM ${CMAKE_SOURCE_DIR}/third-party/rapidjson/include
    ${CMAKE_SOURCE_DIR}/json-query
    ${CMAKE_SOURCE_DIR}/common/test
)

add_executable(JsonGeneratedCommonTest json_generated_common_test.cc)
target_link_libraries(JsonGeneratedCommonTest)
add_test(JsonGeneratedCommonTest JsonGeneratedCommonTest)
