include_directories(SYSTEM ${PROTOBUF_INCLUDE_DIRS})

file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")
PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders ${ProtoFiles})

add_library(ExampleProtos SHARED ${ProtoSources} ${ProtoHeaders})
target_link_libraries(ExampleProtos ${PROTOBUF_LIBRARIES})

