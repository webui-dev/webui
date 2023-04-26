# WebUI v2.2.0

[![Website](https://img.shields.io/circleci/project/github/badges/shields/master?style=for-the-badge)](https://github.com/alifcommunity/webui) [![Website](https://img.shields.io/github/issues/alifcommunity/webui.svg?branch=master&style=for-the-badge&url=https://google.com)](https://github.com/alifcommunity/webui/issues) [![Website](https://img.shields.io/website?label=webui.me&style=for-the-badge&url=https://google.com)](https://webui.me/)

> Use any web browser as GUI, with your preferred language in the backend and HTML5 in the frontend, all in a lightweight portable lib.

![ScreenShot](screenshot.png)

> :warning: **Notice**:
> 
> * WebUI it's not a web-server solution or a framework, but it's an lightweight portable lib to use any installed web browser as a user interface.
> 
> * We are currently writing documentation.

## Features

- Written in Pure C
- Fully Independent (*No need for any third-party runtimes*)
- Lightweight (*~160 Kb using TCC*) & Small memory footprint
- Fast binary communication protocol between WebUI and the browser (*Instead of JSON*)
- One header file
- Multi-platform & Multi-Browser
- Using private profile for safety

## CppCon 2019 Presentation

[Borislav Stanimirov](https://ibob.bg/) discusses using HTML5 in the web browser as GUI at the [C++ Conference 2019 (*YouTube*)](https://www.youtube.com/watch?v=bbbcZd4cuxg).

<!-- <div align="center">
  <a href="https://www.youtube.com/watch?v=bbbcZd4cuxg"><img src="https://img.youtube.com/vi/bbbcZd4cuxg/0.jpg" alt="Embrace Modern Technology: Using HTML 5 for GUI in C++ - Borislav Stanimirov - CppCon 2019"></a>
</div> -->

![ScreenShot](cppcon_2019.png)

## UI & The Web Technologies

Web application UI design is not just about how a product looks but how it works. Using web technologies in your UI makes your product modern and professional, And a well-designed web application will help you make a solid first impression on potential customers. Great web application design also assists you in nurturing leads and increasing conversions. In addition, it makes navigating and using your web app easier for your users.

## Why Use Web Browser?

Today's web browsers have everything a modern UI needs. Web browsers are very sophisticated and optimized. Therefore, using it as a GUI will be an excellent choice. While old legacy GUI lib is complex and outdated, a WebView-based app is still an option. However, a WebView needs a huge SDK to build and many dependencies to run, and it can only provide some features like a real web browser. That is why WebUI uses real web browsers to give you full features of comprehensive web technologies while keeping your software lightweight and portable.

## How does it work?

![ScreenShot](webui_diagram.png)

Think of WebUI like a WebView controller, but instead of embedding the WebView controller in your program, which makes the final program big in size, and non-portable as it needs the WebView runtimes. Instead, by using WebUI, you use a tiny static/dynamic library to run any installed web browser and use it as GUI, which makes your program small, fast, and portable. **All it needs is a web browser**.

## Runtime Dependencies Comparison

|  | WebView | Qt | WebUI |
| ------ | ------ | ------ | ------ |
| Runtime Dependencies on Windows | *WebView2* | *QtCore, QtGui, QtWidgets* | ***A Web Browser*** |
| Runtime Dependencies on Linux | *GTK3, WebKitGTK* | *QtCore, QtGui, QtWidgets* | ***A Web Browser*** |
| Runtime Dependencies on macOS | *Cocoa, WebKit* | *QtCore, QtGui, QtWidgets* | ***A Web Browser*** |

## Documentation

 - [Online Documentation - C](https://webui.me/docs/#/c_api)
 - [Online Documentation - C++](https://webui.me/docs/#/cpp_api)
 - [Online Documentation - Python](https://webui.me/docs/#/python_api)
 - Online Documentation - TypeScript / JavaScript (*Not Complete*)
 - [Online Documentation - Go](https://webui.me/docs/#/golang_api)
 - Online Documentation - Rust (*Not Complete*)
 - Online Documentation - V (*Not Complete*)
 - [Online Documentation - Nim](https://neroist.github.io/webui-docs/) (*Not Complete*)
 - Online Documentation - Zig (*Not Complete*)

## Build

 - [Windows](https://github.com/alifcommunity/webui/tree/main/build/Windows)
 - [Linux](https://github.com/alifcommunity/webui/tree/main/build/Linux)
 - [macOS](https://github.com/alifcommunity/webui/tree/main/build/macOS)

## Examples

 - [C](https://github.com/alifcommunity/webui/tree/main/examples/C)
 - [C++](https://github.com/alifcommunity/webui/tree/main/examples/C++)
 - [Python](https://github.com/alifcommunity/webui/tree/main/examples/Python)
 - [TypeScript / JavaScript](https://github.com/alifcommunity/webui/tree/main/examples/TypeScript)
 - [Go](https://github.com/alifcommunity/webui/tree/main/examples/Go/hello_world)
 - [Rust](https://github.com/alifcommunity/webui/tree/main/examples/Rust/hello_world) (*Not Complete*)
 - [V - malisipi/vwebui](https://github.com/malisipi/vwebui/tree/main/examples) (*Not Complete*)
 - [Nim - neroist/webui](https://github.com/neroist/webui/tree/main/examples) (*Not Complete*)
 - [Zig - desttinghim/webui](https://github.com/desttinghim/webui/tree/main/examples) (*Not Complete*)

## Supported Web Browsers

| OS | Browser | Status |
| ------ | ------ | ------ |
| Windows | Mozilla Firefox | ✔️ |
| Windows | Google Chrome | ✔️ |
| Windows | Microsoft Edge | ✔️ |
| Windows | Chromium | ✔️ |
| Windows | Yandex | ✔️ |
| Windows | Brave | ✔️ |
| Windows | Vivaldi | ✔️ |
| Windows | Epic | ✔️ |
| Windows | Opera | *coming soon* |
| - | - | - |
| Linux | Mozilla Firefox | ✔️ |
| Linux | Google Chrome | ✔️ |
| Linux | Microsoft Edge | ✔️ |
| Linux | Chromium | ✔️ |
| Linux | Yandex | ✔️ |
| Linux | Brave | ✔️ |
| Linux | Vivaldi | ✔️ |
| Linux | Epic | *Does Not Exist* |
| Linux | Opera | *coming soon* |
| - | - | - |
| macOS | Mozilla Firefox | *coming soon* |
| macOS | Google Chrome | ✔️ |
| macOS | Microsoft Edge | *coming soon* |
| macOS | Chromium | *coming soon* |
| macOS | Yandex | *coming soon* |
| macOS | Brave | *coming soon* |
| macOS | Vivaldi | *coming soon* |
| macOS | Epic | *coming soon* |
| macOS | Apple Safari | *coming soon* |
| macOS | Opera | *coming soon* |

## Supported Languages

| Language | Status | Link |
| ------ | ------ | ------ |
| C | ✔️ | [examples/C](https://github.com/alifcommunity/webui/tree/main/examples/C) |
| C++ |  ✔️ | [examples/C++](https://github.com/alifcommunity/webui/tree/main/examples/C%2B%2B) |
| Python | ✔️ | [examples/Python](https://github.com/alifcommunity/webui/tree/main/examples/Python) |
| JavaScript | ✔️ | [examples/TypeScript/Nodejs](https://github.com/alifcommunity/webui/tree/main/examples/TypeScript/Nodejs) |
| TypeScript | ✔️ | [examples/TypeScript/Deno](https://github.com/alifcommunity/webui/tree/main/examples/TypeScript/Deno) |
| Go | ✔️ | [examples/Go](https://github.com/alifcommunity/webui/tree/main/examples/Go) |
| Rust | *Not Complete* | [examples/Rust](https://github.com/alifcommunity/webui/tree/main/examples/Rust) |
| V | *Not Complete* | [malisipi/vwebui](https://github.com/malisipi/vwebui) |
| Nim | *Not Complete* | [neroist/webui](https://github.com/neroist/webui) |
| Zig | *Not Complete* | [desttinghim/webui](https://github.com/desttinghim/webui) |

### License

> Licensed under GNU General Public License v2.0.

### Stargazers

[![Stargazers repo roster for @alifcommunity/webui](https://reporoster.com/stars/alifcommunity/webui)](https://github.com/alifcommunity/webui/stargazers)
