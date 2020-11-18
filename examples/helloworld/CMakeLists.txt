
# WebUI paths
include_directories(${WebUIProject_SOURCE_DIR}/include)
link_directories(${WebUIProject_BIN_DIR}/webui)

# Microsoft build tools
if (MSVC)
	message("[*] Initializing 'Examples' settings for MSVC.")
	# Boost
	include_directories(${WebUIProject_SOURCE_DIR}/thirdparty/boost/include)
	link_directories(${WebUIProject_SOURCE_DIR}/thirdparty/boost/lib)
	Set(Boost_INCLUDE_DIR ${WebUIProject_SOURCE_DIR}/thirdparty/boost/include)
	# Flags
    add_link_options("/SUBSYSTEM:WINDOWS")
endif (MSVC)

# Boost settings
find_package(Boost 1.70.0 REQUIRED)
set(Boost_DEBUG 				OFF)
set(Boost_USE_STATIC_LIBS       ON)
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_executable(helloworld helloworld.cpp)
  set_property(TARGET helloworld PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
  target_link_libraries(helloworld webui ${Boost_LIBRARIES})
else()
	message("[!] Please install Boost library.")
	return()
endif()
