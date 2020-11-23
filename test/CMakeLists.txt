
# Info
set(Info_Title "Test")
set(Info_Source "test.cpp")
set(Info_App "webui_test")

# --[ WebUI ] -------------------------------------------------------------------

include_directories(${WebUIProject_SOURCE_DIR}/include)
link_directories(${WebUIProject_BIN_DIR}/webui)

# --[ pThread ] -----------------------------------------------------------------

if(UNIX)
	message("[*] Using POSIX threading (pThread).")
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
endif (UNIX)

# --[ Boost ] -------------------------------------------------------------------

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	message("[*] Using local copy of Boost.")
	include_directories(${WebUIProject_SOURCE_DIR}/thirdparty/boost/include)
	link_directories(${WebUIProject_SOURCE_DIR}/thirdparty/boost/lib)
	Set(Boost_INCLUDE_DIR ${WebUIProject_SOURCE_DIR}/thirdparty/boost/include)
	Set(Boost_LIBRARY_DIR_RELEASE ${WebUIProject_SOURCE_DIR}/thirdparty/boost/lib)
endif()

if(UNIX)
	message("[*] Searching for Boost Linux...")
	find_package(Boost 1.70.0 REQUIRED COMPONENTS date_time filesystem regex)
else()
	message("[*] Searching for Boost Windows...")
	find_package(Boost 1.70.0)
endif (UNIX)

if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
else()
	message("[!] Please install Boost libraries (Static MultiThreaded).")
	return()
endif()

set(Boost_DEBUG 				OFF)
set(Boost_USE_STATIC_LIBS       ON)
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    ON)

# --[ Target ] ------------------------------------------------------------------

add_executable(${Info_App} ${Info_Source})

# --[ Compiler flags ] ----------------------------------------------------------

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

	# using Clang
	message("[*] Initializing '${Info_Title}' settings for Clang.")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

	# using GCC
	message("[*] Initializing '${Info_Title}' settings for GCC.")

	# Flags
	set_target_properties(${Info_App} PROPERTIES LINK_FLAGS_RELEASE -s)
	set(CMAKE_CXX_FLAGS_RELEASE " -O3 ")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s ")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")

	# using Intel C++
	message("[*] Initializing '${Info_Title}' settings for Intel.")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

	# Microsoft build tools
	message("[*] Initializing '${Info_Title}' settings for MSVC.")

	# Flags (WINDOWS / CONSOLE)
	add_link_options("/SUBSYSTEM:WINDOWS")
	set_target_properties(${Info_App} PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
	set_property(TARGET ${Info_App} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
	
endif()

# --[ Build ] -------------------------------------------------------------------

if(UNIX)
	target_link_libraries(${Info_App} PRIVATE webui ${Boost_FILESYSTEM_LIBRARIES} Threads::Threads) # PRIVATE | ${Boost_REGEX_LIBRARY} | ${Boost_DATETIME_LIBRARY}
else()
	target_link_libraries(${Info_App} PRIVATE webui ${Boost_FILESYSTEM_LIBRARIES})
endif (UNIX)

# --[ Test ] --------------------------------------------------------------------

add_test(webui_test_run webui_test argv1 argv2 argv3)