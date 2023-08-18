@echo off

set WEBUI_VERSION=2.3.0

set ARG1=%1
IF "%ARG1%"=="debug" (
    set MSVC_CMD=nmake debug
    set GCC_CMD=mingw32-make debug
) else (
    set MSVC_CMD=nmake
    set GCC_CMD=mingw32-make
)

echo.
echo WebUI v%WEBUI_VERSION% Build Script
echo Platform: Microsoft Windows x64
echo Compiler: MSVC, GCC and TCC

Set RootPath=%CD%\..
Set BuildPath=%RootPath%\build\Windows
Set DistPath=%RootPath%\dist\Windows
cd "%RootPath%"

echo.
echo Building WebUI using MSVC...

REM Build WebUI Library using MSVC
cd "%BuildPath%\MSVC"
%MSVC_CMD%

echo.
echo Building WebUI using GCC...
echo.

REM Build WebUI Library using GCC
cd "%BuildPath%\GCC"
%GCC_CMD%

echo.
echo Building WebUI using TCC...
echo.

REM Build WebUI Library using TCC
cd "%BuildPath%\TCC"
%GCC_CMD%

echo.
echo Copying WebUI libs to the examples folder...
echo.

cd "%RootPath%"

REM C++ (Visual Studio 2022)
copy /Y "include\webui.h" "examples\C++\VS2022\serve_a_folder\my_webui_app\webui.h"
copy /Y "include\webui.hpp" "examples\C++\VS2022\serve_a_folder\my_webui_app\webui.hpp"
copy /Y "%BuildPath%\MSVC\webui-2-static-x64.lib" "examples\C++\VS2022\serve_a_folder\my_webui_app\webui-2-static-x64.lib"

REM C++ (Visual Studio 2019)
copy /Y "include\webui.h" "examples\C++\VS2019\serve_a_folder\my_webui_app\webui.h"
copy /Y "include\webui.hpp" "examples\C++\VS2019\serve_a_folder\my_webui_app\webui.hpp"
copy /Y "%BuildPath%\MSVC\webui-2-static-x64.lib" "examples\C++\VS2019\serve_a_folder\my_webui_app\webui-2-static-x64.lib"

REM C - Text Editor
copy /Y "include\webui.h" "examples\C\text-editor\webui.h"
copy /Y "%BuildPath%\MSVC\webui-2-x64.dll" "examples\C\text-editor\webui-2-x64.dll"

echo.
IF "%ARG1%"=="" (

    echo Copying WebUI libs to %DistPath%...
    echo.

    REM Remove Windows distributable files directory if it exits
    if exist "%DistPath%" rmdir /s /q "%DistPath%"

    REM Create Windows output directories
    mkdir "%DistPath%\include" 2>nul
    mkdir "%DistPath%\MSVC" 2>nul
    mkdir "%DistPath%\GCC" 2>nul

    REM Copy include files
    copy /Y "include\webui.h" "%DistPath%\include\webui.h"

    REM Copy Windows MSVC
    copy /Y "%BuildPath%\MSVC\webui-2-x64.dll" "%DistPath%\MSVC\webui-2-x64.dll"
    copy /Y "%BuildPath%\MSVC\webui-2-x64.lib" "%DistPath%\MSVC\webui-2-x64.lib"
    copy /Y "%BuildPath%\MSVC\webui-2-static-x64.lib" "%DistPath%\MSVC\webui-2-static-x64.lib"

    REM Copy Windows GCC
    copy /Y "%BuildPath%\GCC\webui-2-x64.dll" "%DistPath%\GCC\webui-2-x64.dll"
    copy /Y "%BuildPath%\GCC\libwebui-2-static-x64.a" "%DistPath%\GCC\libwebui-2-static-x64.a"

    REM Copy Windows TCC
    REM copy /Y "%BuildPath%\TCC\webui-2-x64.dll" "%DistPath%\TCC\webui-2-x64.dll"
    REM copy /Y "%BuildPath%\TCC\webui-2-x64.def" "%DistPath%\TCC\webui-2-x64.def"
    copy /Y "%BuildPath%\TCC\libwebui-2-static-x64.a" "%DistPath%\TCC\libwebui-2-static-x64.a"

    echo.
    echo Compressing the release folder...

    set TAR_OUT=webui-windows-x64-v%WEBUI_VERSION%.tar.gz
    cd "dist"
    timeout 2 > NUL
    tar.exe -czf %TAR_OUT% Windows\*
    cd "%RootPath%"

    echo.
    echo Cleaning...

    DEL /Q /F /S "*.exe" >nul 2>&1
    DEL /Q /F /S "*.o" >nul 2>&1
    DEL /Q /F /S "*.exp" >nul 2>&1
    DEL /Q /F /S "*.pdb" >nul 2>&1
    DEL /Q /F /S "*.ilk" >nul 2>&1
    DEL /Q /F /S "*.obj" >nul 2>&1
    DEL /Q /F /S "*.iobj" >nul 2>&1
    DEL /Q /F /S "*.res" >nul 2>&1
    DEL /Q /F /S "*.bak" >nul 2>&1
    DEL /Q /F /S "*.DS_Store" >nul 2>&1
)

cd "build"
