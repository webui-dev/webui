
# WebUI Examples - C

The makefile build the WebUI library for you, then build the C example. You will need one of those C compiler, no dependencies is needed.

## Windows

- **Build Tools for Microsoft Visual Studio - Release**
```sh
cd examples\C\Windows\MSVC
nmake
```

- **Build Tools for Microsoft Visual Studio - Debug**
```sh
cd examples\C\Windows\MSVC
nmake debug
```

- **MinGW - Release**
```sh
cd examples\C\Windows\gcc
mingw32-make
```

- **MinGW - Debug**
```sh
cd examples\C\Windows\gcc
mingw32-make debug
```

- **TCC - Release**
```sh
cd examples\C\Windows\tcc
mingw32-make debug
```

- **TCC - Debug**
```sh
cd examples\C\Windows\tcc
mingw32-make debug
```
