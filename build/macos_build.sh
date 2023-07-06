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
echo "Converting JS source to C-String using xxd"
echo "";

#Converting JS source to C-String using xxd
cd "$RootPath"
cd "src/client"
xxd -i ./webui.js ./webui.c

echo "";
echo "Copying WebUI libs to the examples folder..."
echo "";

cd "$RootPath"

# C - Text Editor
cp -f "include/webui.h" "examples/C/text-editor/webui.h"
cp -f "build/macOS/Clang/webui-2-x64.dyn" "examples/C/text-editor/webui-2-x64.dyn"

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

    TAR_OUT="webui-macos-x64-v$WEBUI_VERSION.tar.gz"
    cd "Release"
    sleep 2
    tar -czf $TAR_OUT macOS/*
    cd "$RootPath"

    echo "";
    echo "Cleaning..."
    echo "";

    find ./ -type f -name "*.o" -exec rm -f {} \;
fi

cd "build"
