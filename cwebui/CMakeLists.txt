
# Info
set(Info_Title "cWebUI")
set(Info_Source ${webui_SOURCE_DIR}/src/webui.cpp "cwebui.cpp")
set(Info_App ${cwebui_lib})

# --[ cWebUI ] -----------------------------------------------------------------

include_directories(${webui_SOURCE_DIR}/include)
link_directories(${webui_BIN_DIR}/webui)

# --[ Boost ] ----------------------------------------------------------------------

message("[*] Initializing Boost for ${Info_Title}...")

set(Boost_NO_WARN_NEW_VERSIONS ON)

if (WIN32)
	set(Boost_ARCHITECTURE 				"-x64")
	set(Boost_USE_STATIC_LIBS			ON)
	set(Boost_USE_MULTITHREADED			ON)
	if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		set(Boost_USE_STATIC_RUNTIME		OFF)
	else()
		set(Boost_USE_STATIC_RUNTIME		ON)
	endif()
	# set(Boost_COMPILER				"-mgw8")

	# set(Boost_DETAILED_FAILURE_MSG	OFF)
	# set(Boost_DEBUG					OFF)	
	# set(Boost_THREADAPI             	"win32")
	# set(Boost_COMPILER				"-vc142")
	# add_definitions( -DBOOST_ALL_NO_LIB )
else()
    # set(Boost_DEBUG 					OFF)
    set(Boost_USE_STATIC_LIBS       	OFF)
    set(Boost_USE_MULTITHREADED			ON)
    set(Boost_USE_STATIC_RUNTIME		OFF)
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

# --[ pThread ] -----------------------------------------------------------------

if(UNIX)
	message("[*] Using POSIX threading (pThread).")
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
endif (UNIX)

# --[ Python ] ------------------------------------------------------------------

	message("[*] Searching for Python...")
	find_package (Python 3.8.0 COMPONENTS Development)

	if(Python_FOUND)
		include_directories(${Python_INCLUDE_DIRS})
		link_directories(${Python_LIBRARY_DIRS})
	else()
		message("[!] Please install Python.")
		#return()
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

	if (WIN32)

		# using MinGW/GCC Windows
		message("[*] Initializing '${Info_Title}' settings for MinGW/GCC (Windows).")

		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj -fvisibility=hidden -Wall -O3 -std=gnu++17" )
		target_compile_options(${Info_App} PUBLIC -fPIC -Wl,-Bstatic -lboost_filesystem -Wl,-Bshared)

	else()

		# using GCC lINUX
		message("[*] Initializing '${Info_Title}' settings for GCC (Linux).")

		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -Wall -O3 -std=gnu++17" )
		target_compile_options(${Info_App} PUBLIC -fPIC -Wl,-Bstatic -lboost_filesystem -Wl,-Bshared)

	endif()
	
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")

	# using Intel C++
	message("[*] Initializing '${Info_Title}' settings for Intel.")
	
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

	# Microsoft build tools
	message("[*] Initializing '${Info_Title}' settings for MSVC.")

	# Flags (WINDOWS / CONSOLE)
	#add_link_options("/SUBSYSTEM:WINDOWS")

	#set_target_properties(${Info_App} PROPERTIES LINK_FLAGS "/WHOLEARCHIVE")
	set_property(TARGET ${Info_App} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
	set(CMAKE_CXX_STANDARD 20)

	#add_compile_options("-shared -fPIC -O2")
	#SET(F_PIC "-shared -fPIC -O2")
	#SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${F_PIC}")
	#target_compile_options(${Info_App} PUBLIC --enable-pic -O2)
	
endif()

# --[ Build ] -------------------------------------------------------------------

# Add the static lib WebUI to our shared
# lib cWebUI.
#target_link_libraries(${Info_App} webui)

if(UNIX)
	target_link_libraries(${Info_App} PRIVATE ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} Threads::Threads)
else()
	target_link_libraries(${Info_App} PRIVATE ws2_32 wsock32 ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
endif (UNIX)

configure_file(${webui_SOURCE_DIR}/cwebui/helloworld.py ${CMAKE_CURRENT_BINARY_DIR}/helloworld.py COPYONLY)
