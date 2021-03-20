
# Info
set(Info_Title "PyWebUI")
set(Info_Source ${webui_SOURCE_DIR}/src/webui.cpp "pywebui.cpp")
set(Info_App "pywebui")

# --[ PyWebUI ] -----------------------------------------------------------------

include_directories(${webui_SOURCE_DIR}/include)
link_directories(${webui_BIN_DIR}/webui)

# --[ Boost ] ----------------------------------------------------------------------

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
else()
    # set(Boost_DEBUG 					OFF)
    set(Boost_USE_STATIC_LIBS       	ON)
    # set(Boost_USE_MULTITHREADED		ON)
    # set(Boost_USE_STATIC_RUNTIME		ON)
    # set(Boost_ARCHITECTURE			"-x64")
    # set(Boost_THREADAPI				"pthread")
    # set(Boost_DETAILED_FAILURE_MSG	ON)
    # set(Boost_COMPILER				"-gcc")
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

	#add_compile_options("-shared -fPIC -O2")
	#SET(F_PIC "-shared -fPIC -O2")
	#SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${F_PIC}")
	#target_compile_options(${Info_App} PUBLIC --enable-pic -O2)
	
endif()

# --[ Build ] -------------------------------------------------------------------

# Add the static lib WebUI to our shared
# lib PyWebUI.
#target_link_libraries(${Info_App} webui)

if(UNIX)
	target_link_libraries(${Info_App} PRIVATE ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} Threads::Threads)
else()
	target_link_libraries(${Info_App} PRIVATE ws2_32 wsock32 ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
endif (UNIX)

configure_file(${webui_SOURCE_DIR}/pywebui/helloworld.py ${CMAKE_CURRENT_BINARY_DIR}/helloworld.py COPYONLY)
