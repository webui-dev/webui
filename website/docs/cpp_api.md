# WebUI v2.3.0 C++ APIs

- [Download](/cpp_api?id=download)
- [Build From Source](/cpp_api?id=build-from-source)
- [Examples](/cpp_api?id=examples)
- Window
    - [New Window](/cpp_api?id=new-window)
    - [Show Window](/cpp_api?id=show-window)
    - [Window status](/cpp_api?id=window-status)
- Binding & Events
    - [Bind](/cpp_api?id=Bind)
    - [Events](/cpp_api?id=events)
- Application
    - [Wait](/cpp_api?id=wait)
    - [Exit](/cpp_api?id=exit)
    - [Close](/cpp_api?id=close)
    - [Startup Timeout](/cpp_api?id=startup-timeout)
    - [Multi Access](/cpp_api?id=multi-access)
- JavaScript
    - [Run JavaScript From C++](/cpp_api?id=run-javascript-from-c++)
    - [Run C++ From JavaScript](/cpp_api?id=run-c++-from-javascript)
    - [TypeScript Runtimes](/cpp_api?id=typescript-runtimes)

---
### Download

Download WebUI v2.3.0 prebuilt binaries here: https://webui.me/#download

---
### Build from Source

You can build WebUI from source by cloning the WebUI repo and compile it using any C99 compiler, No need for any external dependencies.

Windows MSVC (_Using x64 Native Tools Command Prompt for VS 20xx_)
```sh
git clone https://github.com/alifcommunity/webui.git
cd webui\build\Windows\MSVC
nmake
nmake debug
```

Windows MinGW
```sh
git clone https://github.com/alifcommunity/webui.git
cd webui\build\Windows\GCC
mingw32-make
mingw32-make debug
```

Windows TCC
```sh
git clone https://github.com/alifcommunity/webui.git
cd webui\build\Windows\TCC
mingw32-make
mingw32-make debug
```

Linux GCC
```sh
git clone https://github.com/alifcommunity/webui.git
cd webui/build/Linux/GCC
make
make debug
```

Linux Clang
```sh
git clone https://github.com/alifcommunity/webui.git
cd webui/build/Linux/Clang
make
make debug
```

macOS Clang
```sh
git clone https://github.com/alifcommunity/webui.git
cd webui/build/macOS/Clang
make
make debug
```

---
You can also use the build script to automatically build WebUI and copy binaries into all examples folder.

Windows
```sh
git clone https://github.com/alifcommunity/webui.git
cd webui\build\
windows_build
windows_build debug
```

Linux
```sh
git clone https://github.com/alifcommunity/webui.git
cd webui/build
sh linux_build.sh
sh linux_build.sh debug
```

macOS
```sh
git clone https://github.com/alifcommunity/webui.git
cd webui/build
sh macos_build.sh
sh macos_build.sh debug
```

For more instructions, please visit [Build WebUI](https://github.com/alifcommunity/webui/tree/main/build) in our GitHub repository.

---
### Examples

A minimal C++ example

```cpp
#include "webui.h"

int main() {

    webui::window my_window;
    my_window.show("<html>Hello!</html>");
	webui::wait();
    return 0;
}
```

Using a local HTML file. Please not that you need to add `<script src="/webui.js"></script>` to all your HTML files

```cpp
#include "webui.h"

int main() {

    webui::window my_window;
    // Please add <script src="/webui.js"></script> to your HTML files
    my_window.show("my_file.html");
	webui::wait();
    return 0;
}
```

Using a specific web browser

```cpp
#include "webui.h"

int main() {

    webui::window my_window;
    my_window.show_browser("<html>Hello!</html>", Chrome);
	webui::wait();
    return 0;
}
```

Please visit [C++ Examples](https://github.com/alifcommunity/webui/tree/main/examples/C%2B%2B) in our GitHub repository for instructions on compiling this example or finding more complete examples.

---
### New Window

To create a new window object, you can use `webui::window()` class, which create an object.

```cpp
webui::window my_window;
```

---
### Show Window

To show a window, you can use `my_window.show()`. If the window is already shown, the UI will get refreshed in the same window.

```cpp
// Show a window using the embedded HTML
std::string my_html = "<html>Hello!</html>";
my_window.show(my_html);

// Show a window using an .html local file
// Please add <script src="/webui.js"></script> to your HTML files
my_window.show("my_file.html");
```

Show a window using a specific web browser

```cpp
std::string my_html = "<html>Hello!</html>";

// Google Chrome
my_window.show_browser(my_html, Chrome);

// Mozilla Firefox
my_window.show_browser(my_html, Firefox);

// Microsoft Edge
my_window.show_browser(my_html, Edge);

// Microsoft Apple Safari (Not Ready)
my_window.show_browser(my_html, Safari);

// The Chromium Project
my_window.show_browser(my_html, Chromium);

// Microsoft Opera Browser (Not Ready)
my_window.show_browser(my_html, Opera);

// The Brave Browser
my_window.show_browser(my_html, Brave);

// The Vivaldi Browser
my_window.show_browser(my_html, Vivaldi);

// The Epic Browser
my_window.show_browser(my_html, Epic);

// The Yandex Browser
my_window.show_browser(my_html, Yandex);

// Default recommended web browser
my_window.show_browser(my_html, AnyBrowser);

// Or simply
my_window.show(my_html);
```

If you need to update the whole UI content, you can also use the same function `my_window.show()()`, which allows you to refresh the window UI with any new HTML content.

```cpp
std::string html = "<html>Hello</html>";
std::string new_html = "<html>New World!</html>";

// Open a window
my_window.show(html);

// Later...

// Refresh the same window with the new content
my_window.show(new_html);
```

---
### Window Status

To know if a specific window is running, you can use `my_window.is_shown()`.

```cpp
if(my_window.is_shown())
    std::cout << "The window is still running" << std::endl;
else
    std::cout << "The window is closed." << std::endl;
```

---
### Bind

Use `my_window.bind()` to receive click events when the user clicks on any HTML element with a specific ID, for example `<button id="MyID">Hello</button>`.

```cpp
void my_function(webui::event* e) {
    // <button id="MyID">Hello</button> gets clicked!
}

my_window.bind("MyID", my_function);
```

Using `webui::bind()` to call a class member method

```cpp
class MyClass {
    public: void my_function(webui::event* e) {
        // <button id="MyID">Hello</button> gets clicked!
    }
};

// Wrapper:
// Because WebUI is written in C, so it can not
// access `MyClass` directly. That's why we should
// create a simple C++ wrapper.
MyClass obj;
void my_function_wrapper(webui::event* e) { obj.my_function(e); }

my_window.bind("MyID", my_function_wrapper);
```

### Events

The *e* corresponds to the word _Event_. `e` is a struct that has these elements:

```cpp
webui::window& window; // The window object
unsigned int event_type; // Event type
std::string element; // HTML element ID
std::string data; // JavaScript data
unsigned int event_number; // Internal WebUI
```

```cpp
void my_function(webui::event* e){

    std::cout << "Hi!, You clicked on " << e.element << std::endl;

    if (e->event_type == webui::CONNECTED)
        std::cout << "Connected." << std::endl;
    else if (e->event_type == webui::DISCONNECTED)
        std::cout << "Disconnected." << std::endl;
    else if (e->event_type == webui::MOUSE_CLICK)
        std::cout << "Click." << std::endl;
    else if (e->event_type == webui::NAVIGATION)
        std::cout << "Starting navigation to: " << e->data << std::endl;

    // Send back a response to JavaScript
    e->window.return_int(e, 123); // As integer
    e->window.return_bool(e, true); // As boolean
    e->window.return_string(e, "My Response"); // As string
}

// Empty ID means all events
my_window.bind("", my_function);
```

---
### Wait

It is essential to call `webui::wait()` at the end of your main function, after you create/shows all your windows. This will make your application run until the user closes all visible windows or when calling *[webui::exit()](/cpp_api?id=exit)*.

```cpp
int main() {

	// Create windows...
	// Bind HTML elements...
    // Show the windows...

    // Wait until all windows get closed
    // or when calling webui::exit()
	webui::wait();

    return 0;
}
```

---
### Exit

At any moment, you can call `webui::exit()`, which tries to close all related opened windows and make *[webui::wait](/cpp_api?id=wait)* break.

```cpp
webui::exit();
```

---
### Close

You can call `my_window.close()` to close a specific window, if there is no running window left *[webui::wait](/cpp_api?id=wait)* will break.

```cpp
my_window.close();
```

---
### Startup Timeout

WebUI waits a couple of seconds (_Default is 30 seconds_) to let the web browser start and connect. You can control this behavior by using `webui::set_timeout()`.

```cpp
// Wait 10 seconds for the browser to start
webui::set_timeout(10);

// Now, After 10 seconds, if the browser did
// not get started, wait() will break
webui::wait();
```

```cpp
// Wait forever.
webui::set_timeout(0);

// webui::wait() will never end
webui::wait();
```

---
### Multi Access

![webui_access_denied](data/webui_access_denied.png)

After the window is loaded, the URL is not valid anymore for safety. WebUI will show an error if someone else tries to access the URL. To allow multi-user access to the same URL, you can use `my_window.set_multi_access()`.

```cpp
my_window.set_multi_access(true);
```

---
### Run JavaScript From C++

You can run JavaScript on any window by using `my_window.script()`. In addition, you can check if the script execution has any errors, as well as receiving the result. Or use `my_window.run()` to run JavaScript quickly with no response (_fire and forget_).

```cpp
void my_function(webui::event* e){

	// Create a buffer to hold the response
    char response[64];

    // This is another way to create a buffer:
    //  std::string buffer;
    //  buffer.reserve(64);
    //  my_window.script(..., ..., &buffer[0], 64);

    // Run JavaScript
    if(!e->window.script(
        "return 2*2;", // JavaScript to be executed
        0, // Maximum waiting time in second
        response, // Local buffer to hold the JavaScript response
        64) // Size of the local buffer
    ) {
        std::cout << "JavaScript Error: " << response << std::endl;
        return;
    }

    // Print the result
    std::cout << "JavaScript Response: " << response << std::endl; // 4

    // Run JavaScript quickly with no waiting for the response
    e->window.run("alert('Fast!');");
}
```

---
### Run C++ From JavaScript

To call a C++ function from JavaScript and get the result back please use `webui_fn('MyID', 'My Data').then((response) => { ... });`. If the function does not have a response then it's safe to remove the `then` method like this `webui_fn('MyID_NoResponse', 'My Data');`.

```c
void my_function(webui::event* e) {

    // Get data from JavaScript
    std::string str = e->data;
    // Or 
    // std::string str = e->window.get_string(e);
    // long long number = e->window.get_int(e);
    // bool status = e->window.get_bool(e);

    // Print the received data
    std::cout << "Data from JavaScript: " << str << std::endl; // Message from JS

    // Return back a response to JavaScript
    e->window.return_string(e, "Message from C++");
    // e->window.return_int(e, number);
    // e->window.return_bool(e, true);
}

my_window.bind("MyID", my_function);
```

JavsScript:

```js
webui_fn('MyID', 'Message from JS').then((response) => {
    console.log(response); // "Message from C++
});
```

---
### TypeScript Runtimes

You may want to interpret JavaScript & TypeScript files and show the output in the UI. You can use `my_window.set_runtime()` and choose between `Deno` or `Nodejs` as your runtimes.

```cpp
// Deno
my_window.set_runtime(Deno);
my_window.show("my_file.ts");

// Nodejs
my_window.set_runtime(Nodejs);
my_window.show("my_file.js");
```
