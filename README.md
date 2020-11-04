# WebUI

[![N|Solid](https://cdn2.iconfinder.com/data/icons/whcompare-blue-green-web-hosting-1/397/site-cloning-256.png)](https://nodesource.com/products/nsolid)

[![Build Status](https://img.shields.io/github/issues/alifcommunity/webui.svg?branch=master)](https://github.com/alifcommunity/webui)

WebUI is a free and open source library that can help you using any installed web browser as your user interface. Basically you can transform an console app to a nice GUI application, in a easy way.

## Why WebUI?

First, web technologies is the future., And web browsers today have everything a modern UI need, your application won't need any external library to run.

## How its work?

Basically this library use Boost web server and WebSocket to maintaine the communication in binary mode between the web browser and your application. You receive any click events, and of course you can send data by executing JavaScript code. 

## How I can use it?

- Include one header file:
```sh
#include <webui.hpp>
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

## Complete examples

See examples folder.

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
| Linux | Firefox | Under development |
| Linux | Chrome | Under development |
| macOS | Firefox | Under development |
| macOS | Chrome | Under development |
| macOS | Safari | Under development |

## How to use pre-compiled version ?

- Goto http://webui.me and download latest release WebUI library.

## How to compile from source ?

- Clone source code
```sh
git clonse https://github.com/alifcommunity/webui.git
```

### Compile under Windows
- Boost compiled library version. You can follow instructions on http://boost.org or visit http://webui.me to download one.
- Windows SDK 10x. You can download it from http://microsoft.com 
- Microsoft Visual Studio build tools. basically goto Start -> Visual Studio 20xx -> Native Tools Command Prompt.
- Using your 'Native Tools Command Prompt'
- [!] Please edit build.bat to change your paths as need.
- [!] Make file script comming soon.
```sh
cd webui
build
```

### Compile under Linux
- GCC
- boost boost-dev
- [!] Make file script comming soon.

### Compile under macOS
- [!] Comming soon!

### License

GNU General Public License v3.0
