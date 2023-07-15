# WebUI C - Call C from JavaScript Example

Example of how to call C from JavaScript. To build this example, you will need one of those C compilers (_no dependencies are required_).

### Windows

- **Build Tools for Microsoft Visual Studio**

```sh
git clone https://github.com/webui-dev/webui.git
cd webui\examples\C\call_c_from_js\Windows\MSVC
nmake
```

- **MinGW**

```sh
git clone https://github.com/webui-dev/webui.git
cd webui\examples\C\call_c_from_js\Windows\GCC
mingw32-make
```

- **TCC**

```sh
git clone https://github.com/webui-dev/webui.git
cd webui\examples\C\call_c_from_js\Windows\TCC
mingw32-make
```

### Linux

- **GCC**

```sh
git clone https://github.com/webui-dev/webui.git
cd webui/examples/C/call_c_from_js/Linux/GCC
make
```

- **Clang**

```sh
git clone https://github.com/webui-dev/webui.git
cd webui/examples/C/call_c_from_js/Linux/Clang
make
```
