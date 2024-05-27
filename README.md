<div align="center">

![Logo](https://raw.githubusercontent.com/webui-dev/webui-logo/main/webui_240.png)

# WebUI v2.5.0-Beta-1

[build-status]: https://img.shields.io/github/actions/workflow/status/webui-dev/webui/ci.yml?branch=main&style=for-the-badge&logo=githubactions&labelColor=414868&logoColor=C0CAF5
[last-commit]: https://img.shields.io/github/last-commit/webui-dev/webui?style=for-the-badge&logo=github&logoColor=C0CAF5&labelColor=414868
[release-version]: https://img.shields.io/github/v/tag/webui-dev/webui?style=for-the-badge&logo=webtrees&logoColor=C0CAF5&labelColor=414868&color=7664C6
[license]: https://img.shields.io/github/license/webui-dev/webui?style=for-the-badge&logo=opensourcehardware&label=License&logoColor=C0CAF5&labelColor=414868&color=8c73cc

[![][build-status]](https://github.com/webui-dev/webui/actions?query=branch%3Amain)
[![][last-commit]](https://github.com/webui-dev/webui/pulse)
[![][release-version]](https://github.com/webui-dev/webui/releases/latest)
[![][license]](https://github.com/webui-dev/webui/blob/main/LICENSE)

> WebUI is not a web-server solution or a framework, but it allows you to use any web browser as a GUI, with your preferred language in the backend and HTML5 in the frontend. All in a lightweight portable lib.

![Screenshot](webui.png)

</div>

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

- Portable (*Needs only a web browser at runtime*)
- One header file
- Lightweight (*Few Kb library*) & Small memory footprint
- Fast binary communication protocol
- Multi-platform & Multi-Browser
- Using private profile for safety

## Showcase

This [text editor](https://github.com/webui-dev/webui/tree/main/examples/C/text-editor) is a lightweight and portable example written in C using WebUI as the GUI library.

<div align="center">

![Example](https://github.com/webui-dev/webui/assets/34311583/c1ccf29c-806a-4742-bfd7-a3dc30cab70b)

</div>

## UI & The Web Technologies

[Borislav Stanimirov](https://ibob.bg/) discusses using HTML5 in the web browser as GUI at the [C++ Conference 2019 (_YouTube_)](https://www.youtube.com/watch?v=bbbcZd4cuxg).

<!-- <div align="center">
  <a href="https://www.youtube.com/watch?v=bbbcZd4cuxg"><img src="https://img.youtube.com/vi/bbbcZd4cuxg/0.jpg" alt="Embrace Modern Technology: Using HTML 5 for GUI in C++ - Borislav Stanimirov - CppCon 2019"></a>
</div> -->

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

|                                 | WebView           | Qt                         | WebUI               |
| ------------------------------- | ----------------- | -------------------------- | ------------------- |
| Runtime Dependencies on Windows | _WebView2_        | _QtCore, QtGui, QtWidgets_ | **_A Web Browser_** |
| Runtime Dependencies on Linux   | _GTK3, WebKitGTK_ | _QtCore, QtGui, QtWidgets_ | **_A Web Browser_** |
| Runtime Dependencies on macOS   | _Cocoa, WebKit_   | _QtCore, QtGui, QtWidgets_ | **_A Web Browser_** |

## Documentation

> **Note**
> We are currently writing documentation.

- [Online Documentation - C](https://webui.me/docs/#/c_api)
- [Online Documentation - C++](https://webui.me/docs/#/cpp_api)

## Build

- **Windows**

  ```powershell
  # GCC
  mingw32-make

  # MSVC
  nmake
  ```

  **Windows SSL/TLS (_Optional_)**

  Download and install the OpenSSL pre-compiled binaries for Windows:

  - MSVC: [x64 OpenSSL v3.1.3](https://slproweb.com/download/Win64OpenSSL-3_1_3.msi) or [_32Bit_](https://slproweb.com/download/Win32OpenSSL-3_1_3.msi). Please check this [Wiki list](https://wiki.openssl.org/index.php/Binaries) for more info.
  - MinGW: [Curl for Windows win OpenSSL](https://curl.se/windows/)

  ```powershell
  # GCC
  mingw32-make WEBUI_USE_TLS=1 WEBUI_TLS_INCLUDE="C:\curl-xxx-xxx-mingw\curl-xxx-xxx-mingw\include" WEBUI_TLS_LIB="C:\curl-xxx-xxx-mingw\curl-xxx-xxx-mingw\lib"

  # MSVC
  nmake WEBUI_USE_TLS=1 WEBUI_TLS_INCLUDE="C:\Program Files\OpenSSL-xxx\include" WEBUI_TLS_LIB="C:\Program Files\OpenSSL-xxx\lib"
  ```

- **Linux**

  ```sh
  # GCC
  make

  # Clang
  make CC=clang
  ```

  **Linux SSL/TLS (_Optional_)**

  ```sh
  sudo apt update
  sudo apt install libssl-dev

  # GCC
  make WEBUI_USE_TLS=1

  # Clang
  make WEBUI_USE_TLS=1 CC=clang
  ```

- **macOS**

  ```sh
  make
  ```

  **macOS SSL/TLS (_Optional_)**

  ```sh
  brew install openssl
  make WEBUI_USE_TLS=1
  ```

## Examples

- [C](https://github.com/webui-dev/webui/tree/main/examples/C)
- [C++](https://github.com/webui-dev/webui/tree/main/examples/C++)

## Wrappers

| Language                | Status         | Link                                                      |
| ----------------------- | -------------- | --------------------------------------------------------- |
| Go                      | ✔️             | [Go-WebUI](https://github.com/webui-dev/go-webui)         |
| Nim                     | ✔️             | [Nim-WebUI](https://github.com/webui-dev/nim-webui)       |
| Pascal                  | ✔️             | [Pascal-WebUI](https://github.com/webui-dev/pascal-webui) |
| Python                  | ✔️             | [Python-WebUI](https://github.com/webui-dev/python-webui) |
| Rust                    | _not complete_ | [Rust-WebUI](https://github.com/webui-dev/rust-webui)     |
| TypeScript / JavaScript | ✔️             | [Deno-WebUI](https://github.com/webui-dev/deno-webui)     |
| V                       | ✔️             | [V-WebUI](https://github.com/webui-dev/v-webui)           |
| Zig                     | ✔️             | [Zig-WebUI](https://github.com/webui-dev/zig-webui)       |
| Odin                    | _not complete_ | [Odin-WebUI](https://github.com/webui-dev/odin-webui)     |
| Delphi                  | ✔️             | [WebUI4Delphi](https://github.com/salvadordf/WebUI4Delphi)|

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

### License

> Licensed under MIT License.

### Stargazers

[![Stargazers repo roster for @webui-dev/webui](https://reporoster.com/stars/webui-dev/webui)](https://github.com/webui-dev/webui/stargazers)
