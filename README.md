# WebUI

[![Website](https://img.shields.io/circleci/project/github/badges/shields/master?style=for-the-badge)](https://github.com/alifcommunity/webui) [![Website](https://img.shields.io/github/issues/alifcommunity/webui.svg?branch=master&style=for-the-badge&url=https://google.com)](https://github.com/alifcommunity/webui/issues) [![Website](https://img.shields.io/website?label=webui.me&style=for-the-badge&url=https://google.com)](https://webui.me/)

> Use any web browser as GUI, with your preferred language in the backend, and HTML/JS/TS/CSS in the frontend.

![ScreenShot](screenshot.png)

> :warning: **Notice**:
> 
> * WebUI it's not a web-server solution or a framework, but it's an easy-embeddable tool to use any installed web browser as a user interface.
> 
> * We are currently writing documentation.
> 
> * WebUI is not ready yet for production release.

## Features

- Written in Pure C
- Fully Independent (*No need for any third-party library*)
- Lightweight (*~160 Kb using TCC*) & Small memory footprint
- Fast WS binary communication (*App--WebUI--Browser*)
- One header file
- Multi-platform & Multi-Browser
- Using private profile for safety

## Comparison

![ScreenShot](webui_diagram.png)

|  | WebView | Qt | WebUI |
| ------ | ------ | ------ | ------ |
| Dependencies on Windows | *Windows APIs, WebView2* | *QtCore, QtGui, QtWidgets* | *No need* |
| Dependencies on Linux | *GTK3, WebKitGTK* | *QtCore, QtGui, QtWidgets* | *No need* |
| Dependencies on macOS | *Cocoa, WebKit* | *QtCore, QtGui, QtWidgets* | *No need* |

## Build

 - [Windows](https://github.com/alifcommunity/webui/tree/main/build/Windows)
 - [Linux](https://github.com/alifcommunity/webui/tree/main/build/Linux)

## Examples

 - [C](https://github.com/alifcommunity/webui/tree/main/examples/C)
 - [C++](https://github.com/alifcommunity/webui/tree/main/examples/C++)
 - [Python](https://github.com/alifcommunity/webui/tree/main/examples/Python)
 - [TypeScript / JavaScript](https://github.com/alifcommunity/webui/tree/main/examples/TypeScript)
 - [Go](https://github.com/alifcommunity/webui/tree/main/examples/Go/hello_world)
 - [Rust](https://github.com/alifcommunity/webui/tree/main/examples/Rust/hello_world)

## Supported Browser

| OS | Browser | Status |
| ------ | ------ | ------ |
| Windows | Firefox | ✔️ |
| Windows | Chrome | ✔️ |
| Windows | Edge | ✔️ |
| Linux | Firefox | ✔️ |
| Linux | Chrome | ✔️ |
| macOS | Firefox | *coming soon* |
| macOS | Chrome | *coming soon* |
| macOS | Safari | *coming soon* |

## Supported Language

| Language | Status |
| ------ | ------ |
| C | ✔️ |
| C++ | ✔️ |
| Python | ✔️ |
| JavaScript | ✔️ |
| TypeScript | ✔️ |
| Go | ✔️ |
| Rust | ✔️ |

### License

> GNU General Public License v3.0

### Stargazers

[![Stargazers repo roster for @alifcommunity/webui](https://reporoster.com/stars/alifcommunity/webui)](https://github.com/alifcommunity/webui/stargazers)
