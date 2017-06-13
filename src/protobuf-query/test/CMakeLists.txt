include_directories(
    SYSTEM ${PROTOBUF_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/protobuf-query
    ${CMAKE_SOURCE_DIR}/common/test
)

add_executable(ProtobufGeneratedCommonTest protobuf_generated_common_test.cc)
target_link_libraries(ProtobufGeneratedCommonTest ${PROTOBUF_LIBRARIES})
add_test(ProtobufGeneratedCommonTest ProtobufGeneratedCommonTest)
