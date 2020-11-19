cmake_minimum_required(VERSION 3.1.0)

# WebUI Version
project(WebUIProject VERSION 1.0.0)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Settings
set(CMAKE_VERBOSE_MAKEFILE OFF)
if(NOT CMAKE_BUILD_TYPE)  
    set(CMAKE_BUILD_TYPE STRING Release)
    message("[*] Build type changed to ${CMAKE_BUILD_TYPE}.")
endif()

# Source code
add_subdirectory(${WebUIProject_SOURCE_DIR}/src webui)

# Examples
add_subdirectory(${WebUIProject_SOURCE_DIR}/examples/helloworld)

# Test
include(CTest)
enable_testing()
add_subdirectory(${WebUIProject_SOURCE_DIR}/test)
