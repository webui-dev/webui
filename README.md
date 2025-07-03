<div align="center">

![Logo](https://raw.githubusercontent.com/webui-dev/webui-logo/14fd595844f57ce751dfc751297b1468b10de77a/webui_120.svg)

# WebUI v2.5.0-beta.4

[build-status]: https://img.shields.io/github/actions/workflow/status/webui-dev/webui/ci.yml?branch=main&style=for-the-badge&logo=githubactions&labelColor=414868&logoColor=C0CAF5
[last-commit]: https://img.shields.io/github/last-commit/webui-dev/webui?style=for-the-badge&logo=github&logoColor=C0CAF5&labelColor=414868
[release-version]: https://img.shields.io/github/v/tag/webui-dev/webui?style=for-the-badge&logo=webtrees&logoColor=C0CAF5&labelColor=414868&color=7664C6
[license]: https://img.shields.io/github/license/webui-dev/webui?style=for-the-badge&logo=opensourcehardware&label=License&logoColor=C0CAF5&labelColor=414868&color=8c73cc

[![][build-status]](https://github.com/webui-dev/webui/actions?query=branch%3Amain)
[![][last-commit]](https://github.com/webui-dev/webui/pulse)
[![][release-version]](https://github.com/webui-dev/webui/releases/latest)
[![][license]](https://github.com/webui-dev/webui/blob/main/LICENSE)

> Use any web browser or WebView as GUI, with your preferred language in the backend and modern web technologies in the frontend, all in a lightweight portable library.

![Screenshot](https://raw.githubusercontent.com/webui-dev/webui-logo/main/screenshot.png)

</div>

## Ask Any Question About the WebUI – Powered by ChatGPT

<div align="center">

![GPT](https://github.com/user-attachments/assets/70455739-94c9-410e-9519-2b0318129b4a)

</div>

- [WebUI GPT - C](https://chatgpt.com/g/g-685b01d42208819185a5a7c8ed74fa23-webui-c)

## Download

- [Latest Stable Release](https://github.com/webui-dev/webui/releases)
- [Nightly Build](https://github.com/webui-dev/webui/releases/tag/nightly)

## Contents

- [Features](#features)
- [Showcase](#showcase)
- [UI & The Web Technologies](#ui--the-web-technologies)
- [Documentation](#documentation)
- [Build](#build)
- [Examples](#examples)
- [Wrappers](#wrappers)
- [Supported Web Browsers](#supported-web-browsers)
- [License](#license)

## Features

- Portable (*Needs only a web browser or a WebView at runtime*)
- One header file
- Lightweight (*Few Kb library*) & Small memory footprint
- Fast binary communication protocol
- Multi-platform & Multi-Browser
- Using private profile for safety
- Cross-platform WebView

## UI & The Web Technologies

[Borislav Stanimirov](https://ibob.bg/) discusses using HTML5 in the web browser as GUI at the [C++ Conference 2019 (_YouTube_)](https://www.youtube.com/watch?v=bbbcZd4cuxg).

<div align="center">

![CPPCon](https://github.com/webui-dev/webui/assets/34311583/4e830caa-4ca0-44ff-825f-7cd6d94083c8)

</div>

Web application UI design is not just about how a product looks but how it works. Using web technologies in your UI makes your product modern and professional, And a well-designed web application will help you make a solid first impression on potential customers. Great web application design also assists you in nurturing leads and increasing conversions. In addition, it makes navigating and using your web app easier for your users.

### Why Use Web Browsers?

Today's web browsers have everything a modern UI needs. Web browsers are very sophisticated and optimized. Therefore, using it as a GUI will be an excellent choice. While old legacy GUI lib is complex and outdated, a WebView-based app is still an option. However, a WebView needs a huge SDK to build and many dependencies to run, and it can only provide some features like a real web browser. That is why WebUI uses real web browsers to give you full features of comprehensive web technologies while keeping your software lightweight and portable.

### How Does it Work?

<div align="center">

![Diagram](https://github.com/ttytm/webui/assets/34311583/dbde3573-3161-421e-925c-392a39f45ab3)

</div>

Think of WebUI like a WebView controller, but instead of embedding the WebView controller in your program, which makes the final program big in size, and non-portable as it needs the WebView runtimes. Instead, by using WebUI, you use a tiny static/dynamic library to run any installed web browser and use it as GUI, which makes your program small, fast, and portable. **All it needs is a web browser**.

### Runtime Dependencies Comparison

|                                 | Tauri / WebView   | Qt                         | WebUI               |
| ------------------------------- | ----------------- | -------------------------- | ------------------- |
| Runtime Dependencies on Windows | _WebView2_        | _QtCore, QtGui, QtWidgets_ | **_A Web Browser_** |
| Runtime Dependencies on Linux   | _GTK3, WebKitGTK_ | _QtCore, QtGui, QtWidgets_ | **_A Web Browser_** |
| Runtime Dependencies on macOS   | _Cocoa, WebKit_   | _QtCore, QtGui, QtWidgets_ | **_A Web Browser_** |

## Documentation

> **Note**
> We are currently writing documentation.

- [Online Documentation](https://webui.me/docs/)

## Build WebUI Library

### Windows

| Compiler | Command |
|----------|---------|
| GCC      | `mingw32-make` |
| MSVC     | `nmake` |

<details>
  <summary><strong>Windows SSL/TLS (Optional)</strong></summary>
  
  Download and install the OpenSSL pre-compiled binaries for Windows:

  - **MSVC**: [x64 OpenSSL v3.3.1](https://slproweb.com/download/Win64OpenSSL-3_3_1.msi) or [_32Bit_](https://slproweb.com/download/Win32OpenSSL-3_3_1.msi). See the [Wiki list](https://wiki.openssl.org/index.php/Binaries) for more info.
  - **MinGW**: [Curl for Windows with OpenSSL](https://curl.se/windows/)

  ```powershell
  # GCC
  mingw32-make WEBUI_USE_TLS=1 WEBUI_TLS_INCLUDE="C:\curl-xxx-xxx-mingw\curl-xxx-xxx-mingw\include" WEBUI_TLS_LIB="C:\curl-xxx-xxx-mingw\curl-xxx-xxx-mingw\lib"

  # MSVC
  nmake WEBUI_USE_TLS=1 WEBUI_TLS_INCLUDE="C:\Program Files\OpenSSL-xxx\include" WEBUI_TLS_LIB="C:\Program Files\OpenSSL-xxx\lib"
  ```
</details>

### Linux

| Compiler | Command |
|----------|---------|
| GCC      | `make` |
| Clang    | `make CC=clang` |

<details>
  <summary><strong>Linux SSL/TLS (Optional)</strong></summary>
  
  ```sh
  sudo apt update
  sudo apt install libssl-dev

  # GCC
  make WEBUI_USE_TLS=1

  # Clang
  make WEBUI_USE_TLS=1 CC=clang
  ```
</details>

### macOS

| Compiler | Command |
|----------|---------|
| Default  | `make` |

<details>
  <summary><strong>macOS SSL/TLS (Optional)</strong></summary>
  
  ```sh
  brew install openssl
  make WEBUI_USE_TLS=1
  ```
</details>

## Minimal WebUI Application

- **C**

  ```c
  #include "webui.h"

  int main() {
    size_t my_window = webui_new_window();
    webui_show(my_window, "<html><head><script src=\"webui.js\"></script></head> Hello World ! </html>");
    webui_wait();
    return 0;
  }
  ```

- **C++**

  ```cpp
  #include "webui.hpp"
  #include <iostream>

  int main() {
    webui::window my_window;
    my_window.show("<html><head><script src=\"webui.js\"></script></head> C++ Hello World ! </html>");
    webui::wait();
    return 0;
  }
  ```

- **More C/C++ Examples**

  - [C](https://github.com/webui-dev/webui/tree/main/examples/C)
  - [C++](https://github.com/webui-dev/webui/tree/main/examples/C++)

- **Other Languages**

  - [Wrappers List](#Wrappers)

## Build WebUI Application

### Windows

| Compiler | Type    | Command |
|----------|--------|---------|
| GCC      | Static  | `gcc -Os -Wl,-subsystem=windows my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebui-2-static -lws2_32 -Wall -luser32 -static -lole32 -o my_application.exe` |
| GCC      | Dynamic | `gcc -Wl,-subsystem=windows my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" "webui-2.dll" -lws2_32 -Wall -luser32 -lole32 -o my_application.exe` |
| MSVC     | Static  | `cl my_application.c /I"_PATH_TO_WEBUI_INCLUDE_" /link /LIBPATH:"_PATH_TO_WEBUI_LIB_" /SUBSYSTEM:WINDOWS webui-2-static.lib user32.lib Advapi32.lib Shell32.lib Ole32.lib /OUT:my_application.exe` |
| MSVC     | Dynamic | `cl my_application.c /I"_PATH_TO_WEBUI_INCLUDE_" /link /LIBPATH:"_PATH_TO_WEBUI_LIB_" /SUBSYSTEM:WINDOWS webui-2.lib user32.lib Advapi32.lib Shell32.lib Ole32.lib /OUT:my_application.exe` |

<details>
  <summary><strong>Windows With SSL/TLS (Optional)</strong></summary>
  
  | Compiler | Type    | Command |
  |----------|--------|---------|
  | GCC      | Static  | `gcc -Os -Wl,-subsystem=windows my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebui-2-secure-static -lws2_32 -Wall -luser32 -static -lole32 -o my_application.exe` |
  | GCC      | Dynamic | `gcc -Wl,-subsystem=windows my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" "webui-2-secure.dll" -lws2_32 -Wall -luser32 -lole32 -o my_application.exe` |
  | MSVC     | Static  | `cl my_application.c /I"_PATH_TO_WEBUI_INCLUDE_" /link /LIBPATH:"_PATH_TO_WEBUI_LIB_" /SUBSYSTEM:WINDOWS webui-2-secure-static.lib user32.lib Advapi32.lib Shell32.lib Ole32.lib /OUT:my_application.exe` |
  | MSVC     | Dynamic | `cl my_application.c /I"_PATH_TO_WEBUI_INCLUDE_" /link /LIBPATH:"_PATH_TO_WEBUI_LIB_" /SUBSYSTEM:WINDOWS webui-2-secure.lib user32.lib Advapi32.lib Shell32.lib Ole32.lib /OUT:my_application.exe` |
</details>

### Linux

| Compiler | Type    | Command |
|----------|--------|---------|
| GCC      | Static  | `gcc -Os my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebui-2-static -lpthread -lm -ldl -o my_application` |
| GCC      | Dynamic | `gcc my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebui-2 -lpthread -lm -ldl -o my_application` |
| Clang    | Static  | `clang -Os my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebui-2-static -lpthread -lm -ldl -o my_application` |
| Clang    | Dynamic | `clang my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebui-2 -lpthread -lm -ldl -o my_application` |

<details>
  <summary><strong>Linux With SSL/TLS (Optional)</strong></summary>
  
  | Compiler | Type    | Command |
  |----------|--------|---------|
  | GCC      | Static  | `gcc -Os my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebui-2-secure-static -lpthread -lm -ldl -o my_application` |
  | GCC      | Dynamic | `gcc my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebui-2-secure -lpthread -lm -ldl -o my_application` |
  | Clang    | Static  | `clang -Os my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebui-2-secure-static -lpthread -lm -ldl -o my_application` |
  | Clang    | Dynamic | `clang my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebui-2-secure -lpthread -lm -ldl -o my_application` |
</details>

### macOS

| Compiler | Type    | Command |
|----------|--------|---------|
| Clang    | Static  | `clang -Os my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebui-2-static -lpthread -lm -framework Cocoa -framework WebKit -o my_application` |
| Clang    | Dynamic | `clang my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebui-2 -lpthread -lm -framework Cocoa -framework WebKit -o my_application` |

<details>
  <summary><strong>macOS With SSL/TLS (Optional)</strong></summary>
  
  | Compiler | Type    | Command |
  |----------|--------|---------|
  | Clang    | Static  | `clang -Os my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebui-2-secure-static -lpthread -lm -framework Cocoa -framework WebKit -o my_application` |
  | Clang    | Dynamic | `clang my_application.c -I"_PATH_TO_WEBUI_INCLUDE_" -L"_PATH_TO_WEBUI_LIB_" -lwebui-2-secure -lpthread -lm -framework Cocoa -framework WebKit -o my_application` |
</details>

## Wrappers

| Language        | v2.4.0 API | v2.5.0 API | Link                                                    |
| --------------- | --- | -------------- | ---------------------------------------------------------  |
| Python          | ✔️ | _not complete_ | [Python-WebUI](https://github.com/webui-dev/python-webui)  |
| Go              | ✔️ | _not complete_ | [Go-WebUI](https://github.com/webui-dev/go-webui)          |
| Zig             | ✔️ |  _not complete_ | [Zig-WebUI](https://github.com/webui-dev/zig-webui)        |
| Nim             | ✔️ |  _not complete_ | [Nim-WebUI](https://github.com/webui-dev/nim-webui)        |
| V               | ✔️ |  _not complete_ | [V-WebUI](https://github.com/webui-dev/v-webui)            |
| Rust            | _not complete_ |  _not complete_ | [Rust-WebUI](https://github.com/webui-dev/rust-webui)      |
| TS / JS (Deno)  | ✔️ |  _not complete_ | [Deno-WebUI](https://github.com/webui-dev/deno-webui)      |
| TS / JS (Bun)   | _not complete_ |  _not complete_ | [Bun-WebUI](https://github.com/webui-dev/bun-webui)        |
| Swift           | _not complete_ |  _not complete_ | [Swift-WebUI](https://github.com/webui-dev/swift-webui)    |
| Odin            | _not complete_ |  _not complete_ | [Odin-WebUI](https://github.com/webui-dev/odin-webui)      |
| Pascal          | _not complete_ |  _not complete_ | [Pascal-WebUI](https://github.com/webui-dev/pascal-webui)  |
| Purebasic       | _not complete_ |  _not complete_ | [Purebasic-WebUI](https://github.com/webui-dev/purebasic-webui)|
| - |  |  |
| Common Lisp     | _not complete_ |  _not complete_ | [cl-webui](https://github.com/garlic0x1/cl-webui)          |
| Delphi          | _not complete_ |  _not complete_ | [WebUI4Delphi](https://github.com/salvadordf/WebUI4Delphi) |
| C#              | _not complete_ |  _not complete_ | [WebUI4CSharp](https://github.com/salvadordf/WebUI4CSharp) |
| WebUI.NET       | _not complete_ |  _not complete_ | [WebUI.NET](https://github.com/Juff-Ma/WebUI.NET)          |
| QuickJS         | _not complete_ |  _not complete_ | [QuickUI](https://github.com/xland/QuickUI)                |
| PHP             | _not complete_ |  _not complete_ | [PHPWebUiComposer](https://github.com/KingBes/php-webui-composer) |

## Supported Web Browsers

| Browser         | Windows         | macOS         | Linux           |
| --------------- | --------------- | ------------- | --------------- |
| Mozilla Firefox | ✔️              | ✔️            | ✔️              |
| Google Chrome   | ✔️              | ✔️            | ✔️              |
| Microsoft Edge  | ✔️              | ✔️            | ✔️              |
| Chromium        | ✔️              | ✔️            | ✔️              |
| Yandex          | ✔️              | ✔️            | ✔️              |
| Brave           | ✔️              | ✔️            | ✔️              |
| Vivaldi         | ✔️              | ✔️            | ✔️              |
| Epic            | ✔️              | ✔️            | _not available_ |
| Apple Safari    | _not available_ | _coming soon_ | _not available_ |
| Opera           | _coming soon_   | _coming soon_ | _coming soon_   |

## Supported WebView

| WebView         | Status         |
| --------------- | --------------- |
| Windows WebView2 | ✔️ |
| Linux GTK WebView   | ✔️ |
| macOS WKWebView  | ✔️ |

### License

> Licensed under MIT License.

### Stargazers

[![Stargazers repo roster for @webui-dev/webui](https://reporoster.com/stars/webui-dev/webui)](https://github.com/webui-dev/webui/stargazers)
