
# Name
set(ExampleName "Hello World")

# WebUI paths
include_directories(${WebUIProject_SOURCE_DIR}/include)
link_directories(${WebUIProject_BIN_DIR}/webui)



# Boost settings
find_package(Boost 1.70.0 REQUIRED COMPONENTS date_time filesystem regex)
set(Boost_DEBUG 				OFF)
set(Boost_USE_STATIC_LIBS       ON)
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
if(Boost_FOUND)

	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
	add_executable(helloworld helloworld.cpp)

	#message("[*] Found boost include: ${Boost_INCLUDE_DIRS}")
	#message("[*] Found boost lib: ${Boost_LIBRARY_DIRS}")

	# Compilers
	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

		# using Clang
		message("[*] Initializing '${ExampleName}' settings for Clang.")

	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

		# using GCC
		message("[*] Initializing '${ExampleName}' settings for GCC.")

		# Flags
		set_target_properties(helloworld PROPERTIES LINK_FLAGS_RELEASE -s)
		set(CMAKE_CXX_FLAGS_RELEASE " -O3 ")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s ")

	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")

		# using Intel C++
		message("[*] Initializing '${ExampleName}' settings for Intel.")

	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

		# Microsoft build tools
		message("[*] Initializing '${ExampleName}' settings for MSVC.")

		# Boost
		include_directories(${WebUIProject_SOURCE_DIR}/thirdparty/boost/include)
		link_directories(${WebUIProject_SOURCE_DIR}/thirdparty/boost/lib)
		Set(Boost_INCLUDE_DIR ${WebUIProject_SOURCE_DIR}/thirdparty/boost/include)

		# Flags
		add_link_options("/SUBSYSTEM:WINDOWS")
		set_property(TARGET helloworld PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
		
	endif()

	#pThread
	if(UNIX)
		message("[*] Using POSIX threading (pThread).")
		set(THREADS_PREFER_PTHREAD_FLAG ON)
		find_package(Threads REQUIRED)
	endif (UNIX)

	target_link_libraries(helloworld PRIVATE webui ${Boost_FILESYSTEM_LIBRARIES} Threads::Threads) # PRIVATE | ${Boost_REGEX_LIBRARY} | ${Boost_DATETIME_LIBRARY}
else()
	message("[!] Please install Boost library.")
	return()
endif()
