# WebUI

[![N|Solid](https://raw.githubusercontent.com/alifcommunity/webui/main/screenshot.png)](https://github.com/alifcommunity/webui)

[![Build Status](https://img.shields.io/github/issues/alifcommunity/webui.svg?branch=master)](https://github.com/alifcommunity/webui)

WebUI is a free and open source GUI library that use any installed web browser as your user interface. Basically you can transform an basic console app or any Python script to a nice GUI application, in a easy way.

## Why WebUI?

First, web technologies is everywhere now, and web browsers today have everything a modern UI need, your application will look nicer and multi-platform.

## How its work?

Basically this library use latest web server and WebSocket standards to maintaine the communication in binary mode between the web browser (UI) and your application. You receive any click events, and of course you can send data or executing JavaScript code. 

## How I can use it?

- Include one header file:
```sh
#include <webui/webui.hpp>
```

- Set your HTML code
```sh
const std::string html = R"V0G0N(
<!DOCTYPE html>
<html>
	<head>
		<title>My first WebUI app</title>
	</head>
	<body>
		<h1>Welcome to WebUI!</h1>
		<button id="MyButtonID">Click on me!</button>
	</body>
</html>
)V0G0N";
```

- Create a window object
```sh
webui::window my_window(&html);
```

- Create your handler function
```sh
void my_handler(){

    std::cout << "You clicked on a button!" << std::endl;
}
```

- Bind your HTML element with your handler function
```sh
my_window.bind("MyButtonID", my_handler);
```

- Show the window!
```sh
my_window.show();
```

- Make infinit loop while window shown
```sh
std::thread ui(webui::loop);
ui.join();
```

You can also show the window using a specific web browser

```sh
if(!my_window.show(webui::browser::firefox))    // If Firefox not installed
    my_window.show();                           // try other web browsers.
```

## Python

```sh
def my_function():
    print('You clicked on the first button!')

def my_function_two():
    print('You clicked on the second button!')

# Create a window object
MyWindow = WebUI()

# Bind am HTML element ID with a python function
MyWindow.bind('MyButtonID1', my_function)
MyWindow.bind('MyButtonID2', my_function_two)

# Show the window
MyWindow.show(my_html)

# Wait unitil all windows are closed
MyWindow.loop()

print('Good! All windows are closed now.')
sys.exit(0)
```

## Complete examples

Please see examples folder.

## Features

- C++ 17 
- Lightweight and fast binary mode communication 
- One header file 
- Multiplatform & Multi Browser 
- Private user browser profiles 
- Customized app mode look & feel

## Status

| OS | Browser  | Status |
| ------ | ------ | ------ |
| Windows | Firefox | Supported |
| Windows | Chrome | Supported |
| Windows | Edge | Supported |
| Linux | Firefox | Supported |
| Linux | Chrome | Supported |
| macOS | Firefox | Supported |
| macOS | Chrome | Supported |
| macOS | Safari | Under development |

## How to use pre-compiled version ?

- Goto http://webui.me and download latest release WebUI library.

## Build from source - Windows
- [ ! ] Boost for Windows is already embedded with this repository, no action needed.
- [ ! ] Microsoft Visual Studio 2017 is not supported. 
- Windows SDK 10x. You can download it from http://microsoft.com 
- Microsoft Visual Studio 2019.
- CMake +3.15.0. You can download it from https://cmake.org/download
- Python 3.8 (only if you want PyWebUI).

### Using MSVC
```sh
git clone https://github.com/alifcommunity/webui.git
cd webui
mkdir build
cd build
```

Generate Visual Studio 2019 solution
```sh
cmake .. -G "Visual Studio 16 2019"
```

Generate Makefile for Microsoft Visual Studio build tools 2019.
```sh
cmake .. -G "NMake Makefiles"
nmake
```

### Using MinGW
```sh
git clone https://github.com/alifcommunity/webui.git
cd webui
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
```

Build PyWebUI
```sh
nmake pywebui
```

## Build from source - Linux
- C++17 compiler (GCC/Clang): ```sudo apt install build-essential```
- Boost lib +1.70.0: ```sudo apt install libboost-all-dev```
- CMake +3.15.0: ```sudo apt install cmake```
- Python 3.8 (only if you want PyWebUI).

```sh
git clone https://github.com/alifcommunity/webui.git
cd webui
mkdir build
cd build
cmake ..
make
sudo make install
```

Build PyWebUI
```sh
make pywebui
```

## Build from source - macOS
- Clang
- [!] Comming soon!

### License

GNU General Public License v3.0
