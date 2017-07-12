include_directories(
    SYSTEM ${PROTOBUF_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}/example
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/protobuf-query
    ${CMAKE_SOURCE_DIR}/json-query
    ${CMAKE_SOURCE_DIR}/third-party/rapidjson/include
)

file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")
PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders ${ProtoFiles})

# protobuf 3.3 generates some code with -Wunused-parameter warnings.
# I guess we can disable all warning checks for generated files, but only disabling one for now.
set_source_files_properties(${ProtoSources} PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)

add_library(ExampleProtos SHARED ${ProtoSources} ${ProtoHeaders})
target_link_libraries(ExampleProtos ${PROTOBUF_LIBRARIES})

set(common_test_py "common_test.py")
add_custom_command(OUTPUT ${common_test_py}
                   COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${common_test_py} ${CMAKE_CURRENT_BINARY_DIR}
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${common_test_py})

string(REPLACE ";" "," CMAKE_PREFIX_PATHS "${CMAKE_PREFIX_PATH}")

foreach(qtype IN ITEMS proto json)
  foreach(testnum RANGE 1 8)
    if((${qtype} STREQUAL "json") AND (${testnum} STREQUAL "8"))
      continue()
    endif()
  	set(golden_out "${qtype}_golden${testnum}.out")
  	add_custom_command(OUTPUT ${golden_out}
                       COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${golden_out} ${CMAKE_CURRENT_BINARY_DIR}
                       DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${golden_out})
  	add_custom_target(${qtype}Test${testnum} ALL DEPENDS ${common_test_py} ${golden_out} RunQuery)
  	add_test(NAME ${qtype}Test${testnum} COMMAND python ${common_test_py} ${qtype} ${testnum} ${CMAKE_PREFIX_PATHS})
  	set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${qtype}"_actual${testnum}.out")
  	set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${qtype}"_generated_query${testnum}")
  	set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${qtype}"_generated_query${testnum}.dSYM")
  endforeach(testnum)
endforeach(qtype)

# This is just for IDEs
file(GLOB GenFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")
add_library(GenFilesLib ${GenFiles})
