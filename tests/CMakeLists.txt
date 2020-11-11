

# WebUI paths
include_directories(${WebUIProject_SOURCE_DIR}/include)
link_directories(${WebUIProject_BIN_DIR}/webui)

# Boost paths
include_directories(${WebUIProject_SOURCE_DIR}/thirdparty/boost/include)
link_directories(${WebUIProject_SOURCE_DIR}/thirdparty/boost/lib)

# Flags
if (MSVC)
    add_link_options("/SUBSYSTEM:WINDOWS")
endif (MSVC)

# Boost settings
Set(Boost_INCLUDE_DIR ${WebUIProject_SOURCE_DIR}/thirdparty/boost/include)
set(Boost_DEBUG OFF)
set(Boost_USE_STATIC_LIBS       ON)
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.74.0)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_executable(webui_test test.cpp)
  target_link_libraries(webui_test webui ${Boost_LIBRARIES})
endif()

# Test
add_test(webui_test_run webui_test a b c)
