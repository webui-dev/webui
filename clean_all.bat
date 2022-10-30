@echo off

Set CurrentPath=%CD%

echo.
echo Clean all...

cd build\Windows\MSVC
nmake clean
cd "%CurrentPath%"

cd build\Windows\GCC
mingw32-make clean
cd "%CurrentPath%"

cd build\Windows\TCC
mingw32-make clean
cd "%CurrentPath%"

cd examples\C\hello_world\Windows\MSVC
nmake clean
cd "%CurrentPath%"

cd examples\C\hello_world\Windows\GCC
mingw32-make clean
cd "%CurrentPath%"

cd examples\C\hello_world\Windows\TCC
mingw32-make clean
cd "%CurrentPath%"

cd examples\C++\hello_world\Windows\MSVC
nmake clean
cd "%CurrentPath%"

cd examples\C++\hello_world\Windows\GCC
mingw32-make clean
cd "%CurrentPath%"

cd examples\Go\hello_world\
go clean -modcache
go clean --cache
go clean
cd "%CurrentPath%"

cd examples\Rust\hello_world
cargo clean
cd "%CurrentPath%"
