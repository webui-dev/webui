cmake_minimum_required(VERSION 3.13.4)

# --[ WebUI version ] --------------------------------------------------------------

project(WebUIProject VERSION 1.0.3 DESCRIPTION "Use web browser as GUI.")
message("-- [ WebUI version ${CMAKE_PROJECT_VERSION} ]")

# --[ Settings ] -------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_VERBOSE_MAKEFILE OFF)

# Default build
set(default_build_type "Release")
if(CMAKE_BUILD_TYPE_INITIALIZED_TO_DEFAULT)
    set(CMAKE_BUILD_TYPE Release)
endif()
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (default Release)" FORCE)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message("[!] Build type changed to ${CMAKE_BUILD_TYPE}.")
    if(NOT CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_CONFIGURATION_TYPES Release)
        message("[!] Configuration type changed to ${CMAKE_CONFIGURATION_TYPES}.")
    endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)  
    message("[!] Warning: Build type set to Debug. Make sure you have Debug version of Boost.")
endif()

# --[ Directories ] ----------------------------------------------------------------

# WebUI Library
add_subdirectory(${WebUIProject_SOURCE_DIR}/src webui)

# Examples
add_subdirectory(${WebUIProject_SOURCE_DIR}/examples/helloworld)

# PyWebUI Library
#if(TARGET pywebui)
    add_subdirectory(${WebUIProject_SOURCE_DIR}/pywebui EXCLUDE_FROM_ALL)
#endif()

# Test
#include(CTest)
enable_testing()
add_subdirectory(${WebUIProject_SOURCE_DIR}/test)
