
# Microsoft build tools
if (MSVC)
	message("[*] Initializing 'WebUI' settings for MSVC.")
    add_link_options("/SUBSYSTEM:WINDOWS")
	include_directories(${WebUIProject_SOURCE_DIR}/thirdparty/boost/include)    
endif (MSVC)

# Boost settings
find_package(Boost 1.70.0 REQUIRED)
set(Boost_DEBUG 				OFF)
set(Boost_USE_STATIC_LIBS       ON)
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  #add_executable(webui_test test.cpp)
  #set_property(TARGET webui_test PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
  #target_link_libraries(webui_test webui ${Boost_LIBRARIES})
else()
	message("[!] Please install Boost library.")
	return()
endif()

# Build WebUI Library
add_library(webui STATIC webui.cpp)
set_property(TARGET webui PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
set_target_properties(webui PROPERTIES OUTPUT_NAME webui${bit}) # Trying to change lib name to webuix32/webuix64 but not working!
