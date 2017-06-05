include_directories(
    SYSTEM ${PROTOBUF_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/protobuf-query/engine
    ${CMAKE_SOURCE_DIR}/common/test
)

add_executable(GeneratedCommonTest generated_common_test.cc)
target_link_libraries(GeneratedCommonTest ${PROTOBUF_LIBRARIES})
add_test(GeneratedCommonTest GeneratedCommonTest)

