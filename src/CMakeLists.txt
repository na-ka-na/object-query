cmake_minimum_required(VERSION 3.3)
project("object-query")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g --std=c++14 -Wall -Wextra")

enable_testing()
enable_language(CXX)

add_subdirectory(protobuf-query)

