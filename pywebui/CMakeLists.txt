
# Info
set(Info_Title "PyWebUI")
set(Info_Source ${WebUIProject_SOURCE_DIR}/src/webui.cpp "pywebui.cpp")
set(Info_App "pywebui")

# --[ PyWebUI ] -----------------------------------------------------------------

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
	message("[!] Please install Boost libraries.")
	return()
endif()

set(Boost_DEBUG 				OFF)
set(Boost_USE_STATIC_LIBS       ON)
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    ON)

# --[ Python ] ------------------------------------------------------------------

	message("[*] Searching for Python...")
	find_package (Python 3.8.0 COMPONENTS Development)

	if(Python_FOUND)
		include_directories(${Python_INCLUDE_DIRS})
		link_directories(${Python_LIBRARY_DIRS})
	else()
		message("[!] Please install Python.")
		return()
	endif()

# --[ Target ] ------------------------------------------------------------------

add_library(${Info_App} SHARED ${Info_Source})

set_target_properties(${Info_App} PROPERTIES VERSION ${PROJECT_VERSION})

# --[ Compiler flags ] ----------------------------------------------------------

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

	# using Clang
	message("[*] Initializing '${Info_Title}' settings for Clang.")

	target_compile_options(${Info_App} PUBLIC -shared -fPIC -O2)

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

	# using GCC
	message("[*] Initializing '${Info_Title}' settings for GCC.")

	# Flags
	#set_target_properties(${Info_App} PROPERTIES LINK_FLAGS_RELEASE -s)
	#set(CMAKE_CXX_FLAGS_RELEASE " -O3 ")
	#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s ")

	target_compile_options(${Info_App} PUBLIC -shared -fPIC -O2)

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")

	# using Intel C++
	message("[*] Initializing '${Info_Title}' settings for Intel.")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

	# Microsoft build tools
	message("[*] Initializing '${Info_Title}' settings for MSVC.")

	# Flags (WINDOWS / CONSOLE)
	#add_link_options("/SUBSYSTEM:WINDOWS")
	#set_target_properties(${Info_App} PROPERTIES LINK_FLAGS "/WHOLEARCHIVE")
	#set_property(TARGET ${Info_App} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
	#add_compile_options("-shared -fPIC -O2")
	#SET(F_PIC "-shared -fPIC -O2")
	#SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${F_PIC}")

	#target_compile_options(${Info_App} PUBLIC --enable-pic -O2)

endif()

# --[ Build ] -------------------------------------------------------------------

# Add the static lib WebUI to our shared
# lib PyWebUI.
#target_link_libraries(${Info_App} webui)

configure_file(${WebUIProject_SOURCE_DIR}/pywebui/helloworld.py ${CMAKE_CURRENT_BINARY_DIR}/helloworld.py COPYONLY)
