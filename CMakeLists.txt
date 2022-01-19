cmake_minimum_required(VERSION 3.18)

project(qhttp VERSION 3.1 LANGUAGES CXX C)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include(3rdparty.cmake)

add_subdirectory(src)
add_subdirectory(example)
