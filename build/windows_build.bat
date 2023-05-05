@echo off

set WEBUI_VERSION=2.2.0

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

Set RootPath=%CD%\..\
cd "%RootPath%"

echo.
echo Building WebUI using MSVC...

REM Build WebUI Library using MSVC
cd "%RootPath%"
cd "build\Windows\MSVC"
%MSVC_CMD%

echo.
echo Building WebUI using GCC...
echo.

REM Build WebUI Library using GCC
cd "%RootPath%"
cd "build\Windows\GCC"
%GCC_CMD%

echo.
echo Building WebUI using TCC...
echo.

REM Build WebUI Library using TCC
cd "%RootPath%"
cd "build\Windows\TCC"
%GCC_CMD%

echo.
echo Copying WebUI libs to the examples folder...
echo.

cd "%RootPath%"

REM Golang
copy /Y "include\webui.h" "examples\Go\hello_world\webui\webui.h"
copy /Y "build\Windows\GCC\libwebui-2-static-x64.a" "examples\Go\hello_world\webui\libwebui-2-static-x64.a"

REM Deno
copy /Y "build\Windows\MSVC\webui-2-x64.dll" "examples\TypeScript\Deno\webui-2-x64.dll"

REM Python
copy /Y "build\Windows\MSVC\webui-2-x64.dll" "examples\Python\PyPI\Package\src\webui\webui-2-x64.dll"

REM C++ (Minimal)
copy /Y "include\webui.hpp" "examples\C++\minimal\webui.hpp"

REM C++ (Visual Studio 2022)
copy /Y "include\webui.h" "examples\C++\VS2022\serve_a_folder\my_webui_app\webui.h"
copy /Y "include\webui.hpp" "examples\C++\VS2022\serve_a_folder\my_webui_app\webui.hpp"
copy /Y "build\Windows\MSVC\webui-2-static-x64.lib" "examples\C++\VS2022\serve_a_folder\my_webui_app\webui-2-static-x64.lib"

REM C++ (Visual Studio 2019)
copy /Y "include\webui.h" "examples\C++\VS2019\serve_a_folder\my_webui_app\webui.h"
copy /Y "include\webui.hpp" "examples\C++\VS2019\serve_a_folder\my_webui_app\webui.hpp"
copy /Y "build\Windows\MSVC\webui-2-static-x64.lib" "examples\C++\VS2019\serve_a_folder\my_webui_app\webui-2-static-x64.lib"

echo.
IF "%ARG1%"=="" (

    echo Copying WebUI libs to the release folder...
    echo.

    REM Release Windows Include
    copy /Y "include\webui.h" "Release\Windows\include\webui.h"    

    REM Release Windows MSVC
    copy /Y "build\Windows\MSVC\webui-2-x64.dll" "Release\Windows\MSVC\webui-2-x64.dll"
    copy /Y "build\Windows\MSVC\webui-2-x64.lib" "Release\Windows\MSVC\webui-2-x64.lib"
    copy /Y "build\Windows\MSVC\webui-2-static-x64.lib" "Release\Windows\MSVC\webui-2-static-x64.lib"

    REM Release Windows GCC
    copy /Y "build\Windows\GCC\webui-2-x64.dll" "Release\Windows\GCC\webui-2-x64.dll"
    copy /Y "build\Windows\GCC\libwebui-2-static-x64.a" "Release\Windows\GCC\libwebui-2-static-x64.a"

    REM Release Windows TCC
    copy /Y "build\Windows\TCC\webui-2-x64.dll" "Release\Windows\TCC\webui-2-x64.dll"
    copy /Y "build\Windows\TCC\libwebui-2-static-x64.a" "Release\Windows\TCC\libwebui-2-static-x64.a"
    copy /Y "build\Windows\TCC\webui-2-x64.def" "Release\Windows\TCC\webui-2-x64.def"

    echo.
    echo Compressing the release folder...

    set TAR_OUT=webui-windows-x64-v%WEBUI_VERSION%.zip
    cd "Release"
    timeout 2 > NUL
    tar.exe -c -f %TAR_OUT% Windows\*
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
