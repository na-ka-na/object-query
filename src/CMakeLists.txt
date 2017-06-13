cmake_minimum_required(VERSION 2.8.12)
project("object-query")

if (APPLE)
  cmake_policy(SET CMP0042 NEW)
  SET(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif()

if (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")
  add_definitions (-D__cplusplus=201402L)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g --std=c++14 -Wall -Wextra")

enable_testing()
enable_language(CXX)

include(FindProtobuf)
find_package(gflags REQUIRED)

add_subdirectory(common)
add_subdirectory(sql)
add_subdirectory(protobuf-query)
add_subdirectory(json-query)
add_subdirectory(example)
add_subdirectory(main)
