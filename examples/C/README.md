
# WebUI Examples - C

To build a C WebUI application, you will need to [build](https://github.com/alifcommunity/webui/tree/main/build) the WebUI library first, then build your application using the `static` version of WebUI, or the `dynamic` one. Static version make your final executable totally portable. While the dynamic version make your application need the WebUI shared library (*.dll/.so*) to run properly.

## Windows

- **Build Tools for Microsoft Visual Studio - Static**
```sh
copy ..\..\build\Windows\MSVC\webui-2-static-x64.lib webui-2-static-x64.lib
rc win.rc
cl "main.c" /I "../../include" /link /MACHINE:X64 /SUBSYSTEM:WINDOWS win.res webui-2-static-x64.lib /OUT:main.exe
```

- **Build Tools for Microsoft Visual Studio - Dynamic**
```sh
copy ..\..\build\Windows\MSVC\webui-2-x64.dll webui-2-x64.dll
copy ..\..\build\Windows\MSVC\webui-2-x64.lib webui-2-x64.lib
rc win.rc
cl "main.c" /I "../../include" /link /MACHINE:X64 /SUBSYSTEM:WINDOWS win.res webui-2-x64.lib /OUT:main.exe
```

- **MinGW - Static**
```sh
copy ..\..\build\Windows\GCC\libwebui-2-static-x64.a libwebui-2-static-x64.a
windres win.rc -O coff -o win.res
gcc -static -Os -m64 -o main.exe "main.c" -I "../../include" -L. win.res -lwebui-2-static-x64 -lws2_32 -Wall -Wl,-subsystem=windows -luser32
strip --strip-all main.exe
```

- **MinGW - Dynamic**
```sh
copy ..\..\build\Windows\GCC\webui-2-x64.dll webui-2-x64.dll
windres win.rc -O coff -o win.res
gcc -m64 -o main.exe "main.c" -I "../../include" -L. win.res webui-2-x64.dll -lws2_32 -Wall -Wl,-subsystem=windows -luser32
strip --strip-all example_dynamic.exe
```

- **TCC - Static**
```sh
copy ..\..\build\Windows\TCC\libwebui-2-static-x64.a libwebui-2-static-x64.a
tcc -m64 -o main.exe "main.c" -I "../../include" -L. -lwebui-2-static-x64 -lws2_32 -Wall -Wl,-subsystem=windows -luser32
```

- **TCC - Dynamic**
```sh
copy ..\..\build\Windows\TCC\webui-2-x64.dll webui-2-x64.dll
tcc -impdef webui-2-x64.dll -o webui-2-x64.def
tcc -m64 -o example_dynamic.exe "main.c" -I "../../include" -L. webui-2-x64.def -lws2_32 -Wall -Wl,-subsystem=windows -luser32
```
