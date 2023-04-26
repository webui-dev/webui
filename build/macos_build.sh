#!/bin/bash

WEBUI_VERSION=2.2.0

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
echo "Platform: macOS x64"
echo "Compiler: Clang"

RootPath="$PWD/../"
cd "$RootPath"

echo "";
echo "Building WebUI using Clang...";
echo "";

# Build WebUI Library using Clang
cd "$RootPath"
cd "build/macOS/Clang"
$CLANG_CMD

echo "";
echo "Copying WebUI libs to the examples folder..."
echo "";

cd "$RootPath"

# Golang
cp -f "include/webui.h" "examples/Go/hello_world/webui/webui.h"
cp -f "build/macOS/Clang/libwebui-2-static-x64.a" "examples/Go/hello_world/webui/libwebui-2-static-x64.a"

# Deno
cp -f "build/macOS/Clang/webui-2-x64.dyn" "examples/TypeScript/Deno/webui-2-x64.dyn"

# Python
cp -f "build/macOS/Clang/webui-2-x64.dyn" "examples/Python/PyPI/Package/src/webui/webui-2-x64.dyn"

# C++ (Minimal)
cp -f "include/webui.hpp" "examples/C++/minimal/webui.hpp"

echo "";
if [ "$ARG1" = "" ]; then

    echo "Copying WebUI libs to the release folder..."
    echo "";

    # Release macOS Include
    cp -f "include/webui.h" "Release/macOS/include/webui.h"
    cp -f "include/webui.hpp" "Release/macOS/include/webui.hpp"

    # Release macOS Clang
    cp -f "build/macOS/Clang/webui-2-x64.dyn" "Release/macOS/Clang/webui-2-x64.dyn"
    cp -f "build/macOS/Clang/libwebui-2-static-x64.a" "Release/macOS/Clang/libwebui-2-static-x64.a"

    echo "";
    echo "Compressing the release folder..."
    echo "";

    TAR_OUT="webui-macos-x64-v$WEBUI_VERSION.zip"
    cd "Release"
    sleep 2
    tar -c -f $TAR_OUT macOS/*
    cd "$RootPath"

    echo "";
    echo "Cleaning..."
    echo "";

    find ./ -type f -name "*.o" -exec rm -f {} \;
fi

cd "build"
