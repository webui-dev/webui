#!/bin/bash

WEBUI_VERSION=2.3.0

ARG1=$1
if [ "$ARG1" = "debug" ]; then
    CLANG_CMD="make debug"
    GCC_CMD="make debug"
else
    CLANG_CMD="make"
    GCC_CMD="make"
fi

echo "";
echo "WebUI v$WEBUI_VERSION Build Script"
echo "Platform: Linux x64"
echo "Compiler: GCC and Clang"

RootPath="$PWD/../"
cd "$RootPath"

echo "";
echo "Building WebUI using GCC...";
echo "";

# Build WebUI Library using GCC
cd "$RootPath"
cd "build/Linux/GCC"
$GCC_CMD

echo "";
echo "Building WebUI using Clang...";
echo "";

# Build WebUI Library using Clang
cd "$RootPath"
cd "build/Linux/Clang"
$CLANG_CMD

echo "";
echo "Copying WebUI libs to the examples folder..."
echo "";

cd "$RootPath"

# Golang
cp -f "include/webui.h" "examples/Go/hello_world/webui/webui.h"
cp -f "build/Linux/GCC/libwebui-2-static-x64.a" "examples/Go/hello_world/webui/libwebui-2-static-x64.a"

# Deno
cp -f "build/Linux/GCC/webui-2-x64.so" "examples/TypeScript/Deno/webui-2-x64.so"

# Python
cp -f "build/Linux/GCC/webui-2-x64.so" "examples/Python/PyPI/Package/src/webui/webui-2-x64.so"

# C++ (Minimal)
cp -f "include/webui.hpp" "examples/C++/minimal/webui.hpp"

echo "";
if [ "$ARG1" = "" ]; then

    echo "Copying WebUI libs to the release folder..."
    echo "";

    # Release Linux Include
    cp -f "include/webui.h" "Release/Linux/include/webui.h"
    cp -f "include/webui.hpp" "Release/Linux/include/webui.hpp"

    # Release Linux GCC
    cp -f "build/Linux/GCC/webui-2-x64.so" "Release/Linux/GCC/webui-2-x64.so"
    cp -f "build/Linux/GCC/libwebui-2-static-x64.a" "Release/Linux/GCC/libwebui-2-static-x64.a"

    # Release Linux Clang
    cp -f "build/Linux/Clang/webui-2-x64.so" "Release/Linux/Clang/webui-2-x64.so"
    cp -f "build/Linux/Clang/libwebui-2-static-x64.a" "Release/Linux/Clang/libwebui-2-static-x64.a"

    echo "";
    echo "Compressing the release folder..."
    echo "";

    TAR_OUT="webui-linux-x64-v$WEBUI_VERSION.zip"
    cd "Release"
    sleep 2
    tar -c -f $TAR_OUT Linux/*
    cd "$RootPath"

    echo "";
    echo "Cleaning..."
    echo "";

    find ./ -type f -name "*.o" -exec rm -f {} \;
fi

cd "build"
