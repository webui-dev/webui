cmake_minimum_required(VERSION 3.13.0)

# --[ WebUI version ] --------------------------------------------------------------

project(webui VERSION 1.0.20 DESCRIPTION 
"Use your web browser as UI, and bring the web technologies to your app.")
message("-- [ WebUI version ${CMAKE_PROJECT_VERSION} ]")

# --[ Boost ] ----------------------------------------------------------------------

set(Boost_Version_Needed "1.76.0")
set(Boost_NO_WARN_NEW_VERSIONS ON)

if (WIN32)
    if(EXISTS "C:/Alif3/")

        # Windows - Boost
        message("[*] Using Boost embeded with Alif v3 'C:/Alif3/boost'")
        set(Boost_Root "C:/Alif3/boost/include")
        set(Boost_Lib "C:/Alif3/boost/lib")
        
    endif()
endif()

# --[ C++ ] ------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --[ Targets ] --------------------------------------------------------------------

if (WIN32)
	set(webui_lib "webui_win_x86_64")
	set(cwebui_lib "cwebui_win_x86_64")
elseif(UNIX)
	set(webui_lib "webui_linux_x86_64")
	set(cwebui_lib "cwebui_linux_x86_64")
else()
	set(webui_lib "webui_macos_x86_64")
	set(cwebui_lib "cwebui_macos_x86_64")
endif()

# --[ Directories ] ----------------------------------------------------------------

# WebUI Library
add_subdirectory(${webui_SOURCE_DIR}/src webui)

# Examples
add_subdirectory(${webui_SOURCE_DIR}/examples)

# cWebUI Library
# if(TARGET cwebui)
    add_subdirectory(${webui_SOURCE_DIR}/cwebui EXCLUDE_FROM_ALL)
# endif()

# Test
# include(CTest)
enable_testing()
add_subdirectory(${webui_SOURCE_DIR}/test)
