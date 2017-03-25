cmake_minimum_required(VERSION 2.8.12)
project("object-query")

if (APPLE)
  cmake_policy(SET CMP0042 NEW)
  SET(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g --std=c++14 -Wall -Wextra")

enable_testing()
enable_language(CXX)

add_subdirectory(protobuf-query)

