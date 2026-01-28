@echo off

echo.
echo * Build React project...

cd webui-react-example
call npm install
call npm run build
cd ..

echo.
echo * Embedding React's build files into 'vfs.h'

python vfs.py "./webui-react-example/build" "vfs.h"

echo.
echo * Compiling 'main.c' into 'main.exe' using Microsoft Visual Studio...

nmake
