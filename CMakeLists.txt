cmake_minimum_required(VERSION 3.17.0)

# WebUI Version
project(WebUIProject VERSION 1.0.0)

# Settings
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_BUILD_TYPE STRING Release)
message("[!] Build type set to ${CMAKE_BUILD_TYPE}.")

# Source code
add_subdirectory(${WebUIProject_SOURCE_DIR}/src webui)
# Examples
add_subdirectory(${WebUIProject_SOURCE_DIR}/examples/helloworld)
# Test
include(CTest)
enable_testing()
add_subdirectory(${WebUIProject_SOURCE_DIR}/tests)
