
# Info
set(Info_Title "Examples")

# --[ WebUI Lib ] ------------------------------------------------------------------

include_directories(${webui_SOURCE_DIR}/include)
link_directories(${webui_BIN_DIR}/webui)

# --[ Boost ] ----------------------------------------------------------------------

message("[*] Initializing Boost for ${Info_Title}... ")

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

# --[ Compiler flags ] ----------------------------------------------------------

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

	# using Clang
	message("[*] Initializing '${Info_Title}' settings for Clang.")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

	if (WIN32)

		# using MinGW/GCC Windows
		message("[*] Initializing '${Info_Title}' settings for MinGW/GCC (Windows).")

		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj -fvisibility=hidden -Wall -O3 -std=gnu++17" )

	else()

		# using GCC lINUX
		message("[*] Initializing '${Info_Title}' settings for GCC (Linux).")

		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -Wall -O3 -std=gnu++17" )
		
	endif()

	# Flags
	# set_target_properties(${Info_Appzzzzzzzzzzzz} PROPERTIES LINK_FLAGS_RELEASE -s)
	# set(CMAKE_CXX_FLAGS_RELEASE " -O3 ")
	# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s ")
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mbig-obj")
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -Wall -O3 -Wa,-mbig-obj -std=gnu++17" )

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")

	# using Intel C++
	message("[*] Initializing '${Info_Title}' settings for Intel.")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

	# Microsoft build tools
	message("[*] Initializing '${Info_Title}' settings for MSVC.")

	# Flags (WINDOWS / CONSOLE)
	# add_link_options("/SUBSYSTEM:WINDOWS")
	# set_target_properties(${Info_Appzzzzzzzzzzzz} PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
	# set_property(TARGET ${Info_Appzzzzzzzzzzzz} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
	set(CMAKE_CXX_STANDARD 20)
	#add_compile_options("/std:c++latest")
	
endif()

# --[ Target / Build ] ----------------------------------------------------------

file(GLOB files ${webui_SOURCE_DIR}/examples/*.cpp)
foreach(filePath ${files})

	# Remove path
	get_filename_component(file ${filePath} NAME)

	# Remove extension
	string(REPLACE ".cpp" "" example_app ${file})

	message("[*] Found WebUI example: ${file} ")

	add_executable(${example_app} ${file})

	if(UNIX)
		#Linux
		target_link_libraries(${example_app} PRIVATE ${webui_lib} ${Boost_LIBRARIES} Threads::Threads)
	else()
		#Windows
		target_link_libraries(${example_app} PUBLIC ${webui_lib} ws2_32 wsock32 ${Boost_LIBRARIES})
	endif (UNIX)

endforeach()

# Copy files for serve_folder example
configure_file(${webui_SOURCE_DIR}/examples/index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html COPYONLY)
configure_file(${webui_SOURCE_DIR}/examples/style.css ${CMAKE_CURRENT_BINARY_DIR}/style.css COPYONLY)
