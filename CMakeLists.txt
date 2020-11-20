cmake_minimum_required(VERSION 3.17.0)

# --[ WebUI version ] --------------------------------------------------------------

project(WebUIProject VERSION 1.0.0)
message("-- [ WebUI version ${CMAKE_PROJECT_VERSION} ]")

# --[ Settings ] -------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_VERBOSE_MAKEFILE OFF)

if(CMAKE_BUILD_TYPE STREQUAL Debug)  
    message("[!] Warning: Build type set to Debug. Make sure you have Debug version of Boost.")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message("[!] Build type changed to ${CMAKE_BUILD_TYPE}.")
endif()

# Comment this line if you realy want a debug version of WebUI.
    set(CMAKE_BUILD_TYPE Release)
    message("[!] Build type changed to ${CMAKE_BUILD_TYPE}.")

if(NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES Release)
    message("[!] Configuration type changed to ${CMAKE_CONFIGURATION_TYPES}.")
endif()

# --[ Directories ] ----------------------------------------------------------------

# Source code
add_subdirectory(${WebUIProject_SOURCE_DIR}/src webui)

# Examples
add_subdirectory(${WebUIProject_SOURCE_DIR}/examples/helloworld)

# Test
include(CTest)
enable_testing()
add_subdirectory(${WebUIProject_SOURCE_DIR}/test)
