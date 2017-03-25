include_directories(
    SYSTEM ${PROTOBUF_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}/protobuf-query/example
    ${CMAKE_SOURCE_DIR}/protobuf-query
    ${CMAKE_SOURCE_DIR}/protobuf-query/engine
)

file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")
PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders ${ProtoFiles})

add_library(ExampleProtos SHARED ${ProtoSources} ${ProtoHeaders})
target_link_libraries(ExampleProtos ${PROTOBUF_LIBRARIES})

set(common_test_py "common_test.py")
add_custom_command(OUTPUT ${common_test_py}
                   COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${common_test_py} ${CMAKE_CURRENT_BINARY_DIR}
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${common_test_py})

string(REPLACE ";" "," CMAKE_PREFIX_PATHS "${CMAKE_PREFIX_PATH}")

foreach(testnum RANGE 1 8)
  set(golden_out "golden${testnum}.out")
  add_custom_command(OUTPUT ${golden_out}
                     COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${golden_out} ${CMAKE_CURRENT_BINARY_DIR}
                     DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${golden_out})
  add_custom_target(Test${testnum} ALL DEPENDS ${common_test_py} ${golden_out} RunQuery)
  add_test(NAME Test${testnum} COMMAND python ${common_test_py} ${testnum} ${CMAKE_PREFIX_PATHS})
  set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "actual${testnum}.out")
  set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "generated_query${testnum}")
  set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "generated_query${testnum}.dSYM")
endforeach(testnum)
