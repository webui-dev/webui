# WebUI v2.2.0 C++ APIs

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
    - [Run JavaScript](/cpp_api?id=run-javascript)
    - [TypeScript Runtimes](/cpp_api?id=typescript-runtimes)

---
### Download

Download WebUI v2.2.0 prebuilt binaries here: https://webui.me/#download
Download WebUI C++ header file here: https://github.com/alifcommunity/webui/tree/main/examples/C%2B%2B/minimal

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

    void* win = webui::new_window();
    webui::show(win, "<html>Hello!</html>");
	webui::wait();
    return 0;
}
```

Using a local HTML file. Please not that you need to add `<script src="/webui.js"></script>` to all your HTML files

```cpp
#include "webui.h"

int main() {

    void* win = webui::new_window();
    // Please add <script src="/webui.js"></script> to your HTML files
    webui::show(win, "my_file.html");
	webui::wait();
    return 0;
}
```

Using a specific web browser

```cpp
#include "webui.h"

int main() {

    void* win = webui::new_window();
    webui::show_browser(win, "<html>Hello!</html>", Chrome);
	webui::wait();
    return 0;
}
```

Please visit [C++ Examples](https://github.com/alifcommunity/webui/tree/main/examples/C%2B%2B) in our GitHub repository for instructions on compiling this example or finding more complete examples.

---
### New Window

To create a new window object, you can use `webui::new_window()`, which returns a void pointer. Please note that this pointer does *NOT* need to be freed.

```cpp
void* my_window = webui::new_window();
```

---
### Show Window

To show a window, you can use `webui::show()`. If the window is already shown, the UI will get refreshed in the same window.

```cpp
// Show a window using the embedded HTML
const char* my_html = "<html>Hello!</html>";
webui::show(my_window, my_html);

// Show a window using an .html local file
// Please add <script src="/webui.js"></script> to your HTML files
webui::show(my_window, "my_file.html");
```

Show a window using a specific web browser

```cpp
const char* my_html = "<html>Hello!</html>";

// Google Chrome
webui::show_browser(my_window, my_html, Chrome);

// Mozilla Firefox
webui::show_browser(my_window, my_html, Firefox);

// Microsoft Edge
webui::show_browser(my_window, my_html, Edge);

// Microsoft Apple Safari (Not Ready)
webui::show_browser(my_window, my_html, Safari);

// The Chromium Project
webui::show_browser(my_window, my_html, Chromium);

// Microsoft Opera Browser (Not Ready)
webui::show_browser(my_window, my_html, Opera);

// The Brave Browser
webui::show_browser(my_window, my_html, Brave);

// The Vivaldi Browser
webui::show_browser(my_window, my_html, Vivaldi);

// The Epic Browser
webui::show_browser(my_window, my_html, Epic);

// The Yandex Browser
webui::show_browser(my_window, my_html, Yandex);

// Default recommended web browser
webui::show_browser(my_window, my_html, AnyBrowser);

// Or simply
webui::show(my_window, my_html);
```

If you need to update the whole UI content, you can also use the same function `webui::show()`, which allows you to refresh the window UI with any new HTML content.

```cpp
const char* html = "<html>Hello</html>";
const char* new_html = "<html>New World!</html>";

// Open a window
webui::show(my_window, html);

// Later...

// Refresh the same window with the new content
webui::show(my_window, new_html);
```

---
### Window Status

To know if a specific window is running, you can use `webui::is_shown()`.

```cpp
if(webui::is_shown(my_window))
    std::cout << "The window is still running" << std::endl;
else
    std::cout << "The window is closed." << std::endl;
```

---
### Bind

Use `webui::bind()` to receive click events when the user clicks on any HTML element with a specific ID, for example `<button id="MyID">Hello</button>`.

```cpp
void my_function(webui_event_t* e) {
    // <button id="MyID">Hello</button> gets clicked!
}

webui::bind(my_window, "MyID", my_function);
```

Using `webui::bind()` to call a class member method

```cpp
class MyClass {
    public: void my_function(webui_event_t* e) {
        // <button id="MyID">Hello</button> gets clicked!
    }
};

// Wrapper:
// Because WebUI is written in C, so it can not
// access `MyClass` directly. That's why we should
// create a simple C++ wrapper.
MyClass obj;
void my_function_wrapper(webui_event_t* e) { obj.my_function(e); }

webui::bind(my_window, "MyID", my_function_wrapper);
```

### Events

The *e* corresponds to the word _Event_. `e` is a struct that has these elements:

```cpp
void* window; // Pointer to the window struct.
unsigned int event_type; // Event type (WEBUI_EVENT_MOUSE_CLICK, WEBUI_EVENT_NAVIGATION...).
char* element; // HTML element ID.
char* data; // The data are coming from JavaScript, if any.
char* response; // Internally used by webui::return_xxx().
```

```cpp
void my_function(webui_event_t* e){

    std::cout << "Hi!, You clicked on " << e.element << std::endl;

    if (e->event_type == WEBUI_EVENT_CONNECTED)
        std::cout << "Connected." << std::endl;
    else if (e->event_type == WEBUI_EVENT_DISCONNECTED)
        std::cout << "Disconnected." << std::endl;
    else if (e->event_type == WEBUI_EVENT_MOUSE_CLICK)
        std::cout << "Click." << std::endl;
    else if (e->event_type == WEBUI_EVENT_NAVIGATION)
        std::cout << "Starting navigation to: " << e->data << std::endl;

    // Send back a response to JavaScript
    webui::return_int(e, 123); // As integer
    webui::return_bool(e, true); // As boolean
    webui::return_string(e, "My Response"); // As string
}

// Empty ID means all events on all elements
webui::bind(my_window, "", my_function);
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

You can call `webui::close()` to close a specific window, if there is no running window left *[webui::wait](/cpp_api?id=wait)* will break.

```cpp
webui::close(my_window);
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

![webui::access_denied](data/webui::access_denied.png)

After the window is loaded, the URL is not valid anymore for safety. WebUI will show an error if someone else tries to access the URL. To allow multi-user access to the same URL, you can use `webui::set_multi_access()`.

```cpp
webui::set_multi_access(my_window, true);
```

---
### Run JavaScript

You can run JavaScript on any window to read values, update the view, or anything else. In addition, you can check if the script execution has errors, as well as receive data.

```cpp
void my_function(webui_event_t* e){

	// Create a buffer to hold the response
    char response[64];

    // Run JavaScript
    if(!webui::script(
        e->window, // Window
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
    webui::run(e->window, "alert('Fast!');");
}
```

---
### TypeScript Runtimes

You may want to interpret JavaScript & TypeScript files and show the output in the UI. You can use `webui::set_runtime()` and choose between `Deno` or `Nodejs` as your runtimes.

```cpp
// Deno
webui::set_runtime(my_window, Deno);
webui::show(my_window, "my_file.ts");

// Nodejs
webui::set_runtime(my_window, Nodejs);
webui::show(my_window, "my_file.js");
```
