<div align="center">

![Logo](https://github.com/webui-dev/webui/assets/34311583/c92e712f-0698-486a-a460-d4acea28a4f8)

# WebUI v2.4.0

[![BuildStatus](https://img.shields.io/circleci/project/github/badges/shields/master?style=for-the-badge)](https://github.com/webui-dev/webui/actions?query=branch%3Amain) [![Issues](https://img.shields.io/github/issues/webui-dev/webui.svg?branch=master&style=for-the-badge&url=https://google.com)](https://github.com/webui-dev/webui/issues) [![Website](https://img.shields.io/website?label=webui.me&style=for-the-badge&url=https://google.com)](https://webui.me/)

</div>

> Use any web browser as GUI, with your preferred language in the backend and HTML5 in the frontend, all in a lightweight portable lib.

![Screenshot](https://github.com/webui-dev/webui/assets/34311583/57992ef1-4f7f-4d60-8045-7b07df4088c6)

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

## Screenshot

This [text editor example](https://github.com/webui-dev/webui/tree/main/examples/C/text-editor) is written in C using WebUI as the GUI library. The final executable is portable and has less than _1 MB_ in size (_+html and css files_).

<div align="center">

![Example](https://github.com/webui-dev/webui/assets/34311583/c1ccf29c-806a-4742-bfd7-a3dc30cab70b)

</div>

## CppCon 2019 Presentation

[Borislav Stanimirov](https://ibob.bg/) discusses using HTML5 in the web browser as GUI at the [C++ Conference 2019 (*YouTube*)](https://www.youtube.com/watch?v=bbbcZd4cuxg).

<!-- <div align="center">
  <a href="https://www.youtube.com/watch?v=bbbcZd4cuxg"><img src="https://img.youtube.com/vi/bbbcZd4cuxg/0.jpg" alt="Embrace Modern Technology: Using HTML 5 for GUI in C++ - Borislav Stanimirov - CppCon 2019"></a>
</div> -->

<div align="center">

![CPPCon](https://github.com/webui-dev/webui/assets/34311583/cf796ead-66d3-4298-ac80-b551c25f3e41)

</div>

## UI & The Web Technologies

Web application UI design is not just about how a product looks but how it works. Using web technologies in your UI makes your product modern and professional, And a well-designed web application will help you make a solid first impression on potential customers. Great web application design also assists you in nurturing leads and increasing conversions. In addition, it makes navigating and using your web app easier for your users.

## Why Use Web Browser?

Today's web browsers have everything a modern UI needs. Web browsers are very sophisticated and optimized. Therefore, using it as a GUI will be an excellent choice. While old legacy GUI lib is complex and outdated, a WebView-based app is still an option. However, a WebView needs a huge SDK to build and many dependencies to run, and it can only provide some features like a real web browser. That is why WebUI uses real web browsers to give you full features of comprehensive web technologies while keeping your software lightweight and portable.

## How does it work?

<div align="center">

![Diagram](https://github.com/webui-dev/webui/assets/34311583/ef56944a-d92c-44cb-935a-affc8a442eb4)

</div>

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

## Build

 - [Windows](https://github.com/webui-dev/webui/tree/main/build/Windows)
 - [Linux](https://github.com/webui-dev/webui/tree/main/build/Linux)
 - [macOS](https://github.com/webui-dev/webui/tree/main/build/macOS)

## Examples

 - [C](https://github.com/webui-dev/webui/tree/main/examples/C)
 - [C++](https://github.com/webui-dev/webui/tree/main/examples/C++)

 ## Wrappers

 - [Python](https://github.com/webui-dev/python-webui)
 - [TypeScript / JavaScript](https://github.com/webui-dev/deno-webui)
 - [Go](https://github.com/webui-dev/go-webui)
 - [Rust](https://github.com/webui-dev/rust-webui) (*Not Complete*)
 - [V](https://github.com/webui-dev/v-webui)
 - [Nim](https://github.com/webui-dev/nim-webui)
 - [Zig](https://github.com/webui-dev/zig-webui) (*Not Complete*)

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
| macOS | Mozilla Firefox | ✔️ |
| macOS | Google Chrome | ✔️ |
| macOS | Microsoft Edge | ✔️ |
| macOS | Chromium | ✔️ |
| macOS | Yandex | ✔️ |
| macOS | Brave | ✔️ |
| macOS | Vivaldi | ✔️ |
| macOS | Epic | ✔️ |
| macOS | Apple Safari | *coming soon* |
| macOS | Opera | *coming soon* |

## Supported Languages

| Language | Status | Link |
| ------ | ------ | ------ |
| C/C++ | ✔️ | [WebUI](https://github.com/webui-dev/webui) |
| Python | ✔️ | [Python-WebUI](https://github.com/webui-dev/python-webui) |
| TypeScript / JavaScript | ✔️ | [Deno-WebUI](https://github.com/webui-dev/deno-webui) |
| Go | ✔️ | [Go-WebUI](https://github.com/webui-dev/go-webui) |
| Rust | *Not Complete* | [Rust-WebUI](https://github.com/webui-dev/rust-webui) |
| V | ✔️ | [V-WebUI](https://github.com/webui-dev/v-webui) |
| Nim | ✔️ | [Nim-WebUI](https://github.com/webui-dev/nim-webui) |
| Zig | *Not Complete* | [Zig-WebUI](https://github.com/webui-dev/zig-webui) |

### License

> Licensed under MIT License.

### Stargazers

[![Stargazers repo roster for @webui-dev/webui](https://reporoster.com/stars/webui-dev/webui)](https://github.com/webui-dev/webui/stargazers)
