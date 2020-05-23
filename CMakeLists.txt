cmake_minimum_required(VERSION 3.14)
project(wkttool)

add_definitions("-std=c++20 -fconcepts")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(wkttool_lib INTERFACE)
target_include_directories(wkttool_lib INTERFACE include)
add_subdirectory(test)

add_executable(main src/main.cpp)
target_include_directories(main PRIVATE include)
target_link_libraries(main  ${CONAN_LIBS})

