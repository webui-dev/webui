cmake_minimum_required(VERSION 3.18)

# Project name
project(WebUILibrary 
    VERSION 2.5.0
    DESCRIPTION "Use any web browser or WebView as GUI, with your preferred language in the backend and modern web technologies in the frontend, all in a lightweight portable library."
    HOMEPAGE_URL "https://webui.me/")

# Set C & C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  # using Clang
  add_compile_options(-fPIC)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # using GCC
  add_compile_options(-fPIC)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # using Visual Studio C++
endif()


# Variables for library names, source files, etc.
set(WEBUI_DEFAULT_OUT_LIB_NAME "webui-2")

# Conditional compilation for TLS
option(WEBUI_USE_TLS "Enable TLS support" OFF)
if (WEBUI_USE_TLS)
    find_package(OpenSSL REQUIRED)
    set(WEBUI_DEFAULT_OUT_LIB_NAME "webui-2-secure")
endif()

if (NOT BUILD_SHARED_LIBS)
    set(WEBUI_DEFAULT_OUT_LIB_NAME "${WEBUI_DEFAULT_OUT_LIB_NAME}-static")
endif()

# Output library name
set(WEBUI_OUT_LIB_NAME "${WEBUI_DEFAULT_OUT_LIB_NAME}" CACHE STRING "Name of the output library")

# Source files (already filled)
set(SOURCE_FILES
    src/civetweb/civetweb.c
    src/webui.c
)
if (APPLE)
    # enable macos webview
    enable_language(OBJC)
    set(CMAKE_OBJC_STANDARD 11)
    list(APPEND SOURCE_FILES src/webview/wkwebview.m)
endif()

# Library targets
add_library(webui ${SOURCE_FILES})
target_include_directories(webui PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_compile_definitions(webui PUBLIC NO_CACHING NO_CGI USE_WEBSOCKET 
    "$<$<CONFIG:Debug>:WEBUI_LOG>" "$<$<NOT:$<CONFIG:Debug>>:NDEBUG>")

if (BUILD_SHARED_LIBS AND WIN32)
    target_compile_definitions(webui PRIVATE CIVETWEB_DLL_EXPORTS PUBLIC CIVETWEB_DLL_IMPORTS)
endif()

if (WEBUI_USE_TLS)
    target_compile_definitions(webui PUBLIC WEBUI_TLS NO_SSL_DL OPENSSL_API_1_1)
    target_link_libraries(webui PRIVATE OpenSSL::SSL OpenSSL::Crypto)
else()
    target_compile_definitions(webui PUBLIC NO_SSL)
endif()

if (WIN32)
    target_link_libraries(webui PRIVATE ws2_32 user32 shell32 ole32)
elseif (APPLE)
    # link required frameworks
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(WEBKIT_FRAMEWORK WebKit REQUIRED)

    target_link_libraries(webui PRIVATE ${COCOA_FRAMEWORK} ${WEBKIT_FRAMEWORK})
endif()

set_target_properties(webui PROPERTIES
    OUTPUT_NAME ${WEBUI_OUT_LIB_NAME})

# Install headers
install(FILES include/webui.h include/webui.hpp DESTINATION include)

# Install targets
install(TARGETS webui
    EXPORT webui
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

install(EXPORT webui
    FILE webui-config.cmake
    NAMESPACE webui::
    DESTINATION share/webui
)


#//////////////////////////
# Build examples 
#//////////////////////////

message(STATUS "Source directory is " ${CMAKE_SOURCE_DIR})
message(STATUS "Build directory is " ${CMAKE_BINARY_DIR})

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(minimal ${CMAKE_SOURCE_DIR}/examples/C++/minimal/main.cpp)
add_executable(call_js_from_cpp ${CMAKE_SOURCE_DIR}/examples/C++/call_js_from_cpp/main.cpp)
add_executable(call_js_from_c ${CMAKE_SOURCE_DIR}/examples/C/call_js_from_c/main.c)

target_link_libraries(minimal webui)
target_link_libraries(call_js_from_cpp webui)
target_link_libraries(call_js_from_c webui)

if (MSVC)
  set_target_properties(minimal PROPERTIES LINK_FLAGS "/SubSystem:\"Windows\"" VS_DPI_AWARE "ON")
  set_target_properties(call_js_from_cpp PROPERTIES LINK_FLAGS "/SubSystem:\"Windows\"" VS_DPI_AWARE "ON")
  set_target_properties(call_js_from_c PROPERTIES LINK_FLAGS "/SubSystem:\"Windows\"" VS_DPI_AWARE "ON")
endif()

if (MSVC)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT call_js_from_cpp)
endif()
