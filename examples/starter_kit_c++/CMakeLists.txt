# WebUI C++ Starter Kit - CMake build
#
#   cmake -B build
#   cmake --build build --config Release
#
# Produces the same output as the Makefile / GNUmakefile: a single portable
# executable in bin/ (+ .desktop and icon.png on Linux, my_app.app on macOS).
# The first configure must run inside the WebUI repository so the WebUI
# static library can be copied into lib/.

cmake_minimum_required(VERSION 3.16)

# Your application name (the executable name in bin/)
set(APP_NAME my_app)

project(${APP_NAME} LANGUAGES C CXX)

# Default to a Release build (single-config generators)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

# == WebUI static library snapshot (lib/) =====================================

if(MSVC)
    set(WEBUI_LIB webui-2-static.lib)
    # Match the static-runtime WebUI library built with nmake
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
else()
    set(WEBUI_LIB libwebui-2-static.a)
endif()
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(WEBUI_DIST ${CMAKE_CURRENT_SOURCE_DIR}/../../dist)
set(WEBUI_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

# When configuring inside the WebUI repository, refresh the local snapshot
if(EXISTS ${WEBUI_DIST}/${WEBUI_LIB})
    file(COPY ${WEBUI_DIST}/${WEBUI_LIB} DESTINATION ${LIB_DIR})
    file(COPY ${WEBUI_INCLUDE}/webui.h DESTINATION ${LIB_DIR})
    file(COPY ${WEBUI_INCLUDE}/webui.hpp DESTINATION ${LIB_DIR})
endif()
if(NOT EXISTS ${LIB_DIR}/${WEBUI_LIB})
    message(FATAL_ERROR "WebUI static library not found. Build the WebUI "
        "library first (in the WebUI repository root), then re-run CMake.")
endif()

# == UI packer: ui/ -> src/vfs.h ==============================================

add_executable(vfs tools/vfs.c)
file(GLOB_RECURSE UI_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ui/*)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/vfs.h
    COMMAND $<TARGET_FILE:vfs> ${CMAKE_CURRENT_SOURCE_DIR}/ui ${CMAKE_CURRENT_SOURCE_DIR}/src/vfs.h
    DEPENDS vfs ${UI_FILES}
    COMMENT "Pack ui/ into src/vfs.h")

# == The application ==========================================================

set(APP_SOURCES src/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/vfs.h)
if(WIN32)
    # Windows executable icon (ui/favicon.ico at resource ID 32512)
    list(APPEND APP_SOURCES icon.rc)
endif()
if(APPLE)
    # Convert ui/icon.png into the .app bundle icon
    set(APP_ICNS ${CMAKE_CURRENT_BINARY_DIR}/icon.icns)
    add_custom_command(
        OUTPUT ${APP_ICNS}
        COMMAND sips -s format icns ${CMAKE_CURRENT_SOURCE_DIR}/ui/icon.png --out ${APP_ICNS}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ui/icon.png
        COMMENT "Generate icon.icns")
    list(APPEND APP_SOURCES ${APP_ICNS})
    set_source_files_properties(${APP_ICNS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

add_executable(${APP_NAME} ${APP_SOURCES})
target_compile_features(${APP_NAME} PRIVATE cxx_std_17)
target_include_directories(${APP_NAME} PRIVATE ${LIB_DIR})
target_link_libraries(${APP_NAME} PRIVATE ${LIB_DIR}/${WEBUI_LIB})

# Output into bin/ (same as the Makefile builds)
set_target_properties(${APP_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin)

if(WIN32)
    # GUI app in Release (no console), console app in Debug
    set_target_properties(${APP_NAME} PROPERTIES WIN32_EXECUTABLE "$<CONFIG:Release>")
    target_link_libraries(${APP_NAME} PRIVATE user32 Advapi32 Shell32 Ole32 ws2_32)
    if(MINGW)
        target_link_libraries(${APP_NAME} PRIVATE uuid)
        target_link_options(${APP_NAME} PRIVATE -static)
    endif()
elseif(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME ${APP_NAME}
        MACOSX_BUNDLE_GUI_IDENTIFIER com.webui.${APP_NAME}
        MACOSX_BUNDLE_ICON_FILE icon)
    target_link_libraries(${APP_NAME} PRIVATE pthread m dl "-framework Cocoa" "-framework WebKit")
else()
    target_link_libraries(${APP_NAME} PRIVATE pthread m dl)
    # Generate the .desktop launcher + icon next to the executable
    file(GENERATE OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/bin/${APP_NAME}.desktop CONTENT
"[Desktop Entry]
Type=Application
Name=${APP_NAME}
Exec=\"$<TARGET_FILE:${APP_NAME}>\"
Icon=$<TARGET_FILE_DIR:${APP_NAME}>/icon.png
Terminal=false
Categories=Utility;
StartupWMClass=${APP_NAME}
")
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/ui/icon.png $<TARGET_FILE_DIR:${APP_NAME}>/icon.png
        COMMAND chmod +x ${CMAKE_CURRENT_SOURCE_DIR}/bin/${APP_NAME}.desktop)
endif()
