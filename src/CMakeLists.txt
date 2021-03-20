
# Info
set(Info_Title	"WebUI")
set(Info_Source	"webui.cpp")
set(Info_App	"webui")

# --[ WebUI ] -------------------------------------------------------------------

include_directories(${webui_SOURCE_DIR}/include)
link_directories(${webui_BIN_DIR}/webui)

# --[ Boost ] -----------------------------------------------------------------

message("[*] Initializing Boost for ${Info_Title}...")

if (WIN32)
	set(BOOST_ROOT ${Boost_Root})
	set(Boost_LIBRARY_DIR ${Boost_Lib})
	message("[*] Using Boost root: ${BOOST_ROOT}")
	message("[*] Using Boost lib: ${Boost_LIBRARY_DIR}")

	set(Boost_ARCHITECTURE "-x64")
	# set(Boost_DEBUG					OFF)
	# set(Boost_USE_STATIC_LIBS			ON)
	# set(Boost_USE_MULTITHREADED		ON)
	# set(Boost_USE_STATIC_RUNTIME		ON)
	# set(Boost_DETAILED_FAILURE_MSG	OFF)
	# set(Boost_THREADAPI             	"win32")
	# set(Boost_COMPILER				"-vc142")
	add_definitions( -DBOOST_ALL_NO_LIB )
else()
    # set(Boost_DEBUG 					OFF)
    set(Boost_USE_STATIC_LIBS       	ON)
    # set(Boost_USE_MULTITHREADED		ON)
    # set(Boost_USE_STATIC_RUNTIME		ON)
    # set(Boost_ARCHITECTURE			"-x64")
    # set(Boost_THREADAPI				"pthread")
    # set(Boost_DETAILED_FAILURE_MSG	ON)
    # set(Boost_COMPILER				"-gcc")
	add_definitions( -DBOOST_ALL_NO_LIB )
endif()

message("[*] Searching for Boost libs...")

find_package(Boost ${Boost_Version_Needed} REQUIRED COMPONENTS date_time filesystem regex)

if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
else()
	message("[!] Please install Boost lib.")
	return()
endif()

# --[ pThread ] ---------------------------------------------------------------

if(UNIX)
	message("[*] Using POSIX threading (pThread).")
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
endif (UNIX)

# --[ Target ] ----------------------------------------------------------------

	# Windows
	# WebUI --> static lib
	# depend --> static boost lib

	# Linux
	# WebUI --> static lib
	# depend --> shared boost lib

add_library(${Info_App} STATIC ${Info_Source})

set_target_properties(${Info_App} PROPERTIES PUBLIC_HEADER ${webui_SOURCE_DIR}/include/webui/webui.hpp)

# --[ Compiler flags ] --------------------------------------------------------

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

	# using Clang
	message("[*] Initializing '${Info_Title}' settings for Clang.")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

	if (WIN32)

		# using MinGW/GCC Windows
		message("[*] Initializing '${Info_Title}' settings for MinGW/GCC (Windows).")

		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj -fvisibility=hidden -Wall -O3 -std=gnu++17" )

	else()

		# using GCC Linux
		message("[*] Initializing '${Info_Title}' settings for GCC (Linux).")

		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -Wall -O3 -std=gnu++17" )
		
	endif()

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")

	# using Intel C++
	message("[*] Initializing '${Info_Title}' settings for Intel.")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

	# Microsoft build tools
	message("[*] Initializing '${Info_Title}' settings for MSVC.")

	# Flags (WINDOWS / CONSOLE)
	# add_link_options("/SUBSYSTEM:WINDOWS")
	# set_target_properties(${Info_App} PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
	# set_property(TARGET ${Info_App} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
	
endif()

# --[ Build ] -----------------------------------------------------------------

# ...

# --[ Install ] ---------------------------------------------------------------

if(UNIX)
	message("[*] Initializing installation...")
	install (TARGETS ${Info_App}
			PUBLIC_HEADER DESTINATION include
			ARCHIVE DESTINATION lib
			LIBRARY DESTINATION lib
			RUNTIME DESTINATION lib)
endif (UNIX)

# --[ Package ] ---------------------------------------------------------------

if(UNIX)
	message("[*] Initializing debian package...")
	SET(CPACK_GENERATOR "DEB")
	SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "KK") #required
	INCLUDE(CPack)
endif (UNIX)

