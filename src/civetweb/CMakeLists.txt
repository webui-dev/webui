cmake_minimum_required(VERSION 3.10)
project(civetweb)

# Source files
set(SOURCE_FILES
    civetweb.c
    civetweb.h
    handle_form.inl
    match.inl
    md5.inl
    response.inl
    sha1.inl
    sort.inl
)

# The C API library
add_library(civetweb ${SOURCE_FILES})

# Include directories
target_include_directories(civetweb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link libraries
find_package(Threads REQUIRED)
target_link_libraries(civetweb Threads::Threads)

# If on Windows, link with WinSock
if(WIN32)
  target_link_libraries(civetweb ws2_32)
endif()
