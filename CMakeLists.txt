cmake_minimum_required(VERSION 3.13.0)

# --[ WebUI version ] --------------------------------------------------------------

project(webui VERSION 1.0.6 DESCRIPTION 
"Use your web browser as GUI, and bring web technologies to your desktop app.")
message("-- [ WebUI version ${CMAKE_PROJECT_VERSION} ]")

# --[ Boost ] ----------------------------------------------------------------------

set(Boost_Version_Needed "1.70.0")

if (WIN32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

        # Boost GCC/MinGW
        # Example: C:/boost_1_60_0
        set(Boost_Root "C:/boost_1_75_0_mingw")
        set(Boost_Lib "C:/boost_1_75_0_mingw/stage/lib")

    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

        # Boost Microsoft C++
        # Example: C:/boost_1_60_0
        set(Boost_Root "C:/boost_1_75_0")
        set(Boost_Lib "C:/boost_1_75_0/lib64-msvc-14.2")

    endif()
endif()

# --[ C++ ] ------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --[ Directories ] ----------------------------------------------------------------

# WebUI Library
add_subdirectory(${webui_SOURCE_DIR}/src webui)

# Examples
add_subdirectory(${webui_SOURCE_DIR}/examples/helloworld)

# PyWebUI Library
# if(TARGET pywebui)
    add_subdirectory(${webui_SOURCE_DIR}/pywebui EXCLUDE_FROM_ALL)
# endif()

# Test
# include(CTest)
enable_testing()
add_subdirectory(${webui_SOURCE_DIR}/test)
