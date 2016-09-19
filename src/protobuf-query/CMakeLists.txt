include(FindProtobuf)
find_package(gflags REQUIRED)

add_subdirectory(engine)
add_subdirectory(main)
add_subdirectory(example)
