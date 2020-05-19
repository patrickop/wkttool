cmake_minimum_required(VERSION 3.14)
project(wkttool)

add_definitions("-std=c++20")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(wkttool_lib INTERFACE)
target_include_directories(wkttool_lib INTERFACE include)
add_subdirectory(test)

