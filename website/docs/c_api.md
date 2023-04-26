# WebUI v2.2.0 C APIs

- [Download](/c_api?id=download)
- [Build From Source](/c_api?id=build-from-source)
- [Examples](/c_api?id=examples)
- Window
    - [New Window](/c_api?id=new-window)
    - [Show Window](/c_api?id=show-window)
    - [Window status](/c_api?id=window-status)
- Binding & Events
    - [Bind](/c_api?id=Bind)
    - [Events](/c_api?id=events)
- Application
    - [Wait](/c_api?id=wait)
    - [Exit](/c_api?id=exit)
    - [Close](/c_api?id=close)
    - [Startup Timeout](/c_api?id=startup-timeout)
    - [Multi Access](/c_api?id=multi-access)
- JavaScript
    - [Run JavaScript](/c_api?id=run-javascript)
    - [TypeScript Runtimes](/c_api?id=typescript-runtimes)

---
### Download

Download WebUI v2.2.0 prebuilt binaries here: https://webui.me/#download

---
### Build from Source

You can build WebUI from source by cloning the WebUI repo and compile it using any C compiler, No need for any external dependencies.

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

A minimal C example

```c
#include "webui.h"

int main() {

    void* win = webui_new_window();
    webui_show(win, "<html>Hello!</html>");
	webui_wait();
    return 0;
}
```

Using a local HTML file. Please not that you need to add `<script src="/webui.js"></script>` to all your HTML files

```c
#include "webui.h"

int main() {

    void* win = webui_new_window();
    // Please add <script src="/webui.js"></script> to your HTML files
    webui_show(win, "my_file.html");
	webui_wait();
    return 0;
}
```

Using a specific web browser

```c
#include "webui.h"

int main() {

    void* win = webui_new_window();
    webui_show_browser(win, "<html>Hello!</html>", Chrome);
	webui_wait();
    return 0;
}
```

Please visit [C Examples](https://github.com/alifcommunity/webui/tree/main/examples/C) in our GitHub repository for instructions on compiling this example or finding more complete examples.

---
### New Window

To create a new window object, you can use `webui_new_window()`, which returns a void pointer. Please note that this pointer does *NOT* need to be freed.

```c
void* my_window = webui_new_window();
```

---
### Show Window

To show a window, you can use `webui_show()`. If the window is already shown, the UI will get refreshed in the same window.

```c
// Show a window using the embedded HTML
const char* my_html = "<html>Hello!</html>";
webui_show(my_window, my_html);

// Show a window using an .html local file
// Please add <script src="/webui.js"></script> to your HTML files
webui_show(my_window, "my_file.html");
```

Show a window using a specific web browser

```c
const char* my_html = "<html>Hello!</html>";

// Google Chrome
webui_show_browser(my_window, my_html, Chrome);

// Mozilla Firefox
webui_show_browser(my_window, my_html, Firefox);

// Microsoft Edge
webui_show_browser(my_window, my_html, Edge);

// Microsoft Apple Safari (Not Ready)
webui_show_browser(my_window, my_html, Safari);

// The Chromium Project
webui_show_browser(my_window, my_html, Chromium);

// Microsoft Opera Browser (Not Ready)
webui_show_browser(my_window, my_html, Opera);

// The Brave Browser
webui_show_browser(my_window, my_html, Brave);

// The Vivaldi Browser
webui_show_browser(my_window, my_html, Vivaldi);

// The Epic Browser
webui_show_browser(my_window, my_html, Epic);

// The Yandex Browser
webui_show_browser(my_window, my_html, Yandex);

// Default recommended web browser
webui_show_browser(my_window, my_html, AnyBrowser);

// Or simply
webui_show(my_window, my_html);
```

If you need to update the whole UI content, you can also use the same function `webui_show()`, which allows you to refresh the window UI with any new HTML content.

```c
const char* html = "<html>Hello</html>";
const char* new_html = "<html>New World!</html>";

// Open a window
webui_show(my_window, html);

// Later...

// Refresh the same window with the new content
webui_show(my_window, new_html);
```

---
### Window Status

To know if a specific window is running, you can use `webui_is_shown()`.

```c
if(webui_is_shown(my_window))
    printf("The window is still running");
else
    printf("The window is closed.");
```

---
### Bind

Use `webui_bind()` to receive click events when the user clicks on any HTML element with a specific ID, for example `<button id="MyID">Hello</button>`.

```c
void my_function(webui_event_t* e) {
    // <button id="MyID">Hello</button> gets clicked!
}

webui_bind(my_window, "MyID", my_function);
```

### Events

The *e* corresponds to the word _Event_. `e` is a struct that has these elements:

```c
void* window; // Pointer to the window struct.
unsigned int type; // Event type (WEBUI_EVENT_MOUSE_CLICK, WEBUI_EVENT_NAVIGATION...).
char* element; // HTML element ID.
char* data; // The data are coming from JavaScript, if any.
char* response; // Internally used by webui_return_xxx().
```

```c
void my_function(webui_event_t* e){

    printf("Hi!, You clicked on %s element\n", e.element);

    if(e->type == WEBUI_EVENT_CONNECTED)
        printf("Connected. \n");
    else if(e->type == WEBUI_EVENT_DISCONNECTED)
        printf("Disconnected. \n");
    else if(e->type == WEBUI_EVENT_MOUSE_CLICK)
        printf("Click. \n");
    else if(e->type == WEBUI_EVENT_NAVIGATION)
        printf("Starting navigation to: %s \n", (char *)e->data);    

    // Send back a response to JavaScript
    webui_return_int(e, 123); // As integer
    webui_return_bool(e, true); // As boolean
    webui_return_string(e, "My Response"); // As string
}

// Empty ID means all events on all elements
webui_bind(my_window, "", my_function);
```

---
### Wait

It is essential to call `webui_wait()` at the end of your main function, after you create/shows all your windows. This will make your application run until the user closes all visible windows or when calling *[webui_exit()](/c_api?id=exit)*.

```c
int main() {

	// Create windows...
	// Bind HTML elements...
    // Show the windows...

    // Wait until all windows get closed
    // or when calling webui_exit()
	webui_wait();

    return 0;
}
```

---
### Exit

At any moment, you can call `webui_exit()`, which tries to close all related opened windows and make *[webui_wait](/c_api?id=wait)* break.

```c
webui_exit();
```

---
### Close

You can call `webui_close()` to close a specific window, if there is no running window left *[webui_wait](/c_api?id=wait)* will break.

```c
webui_close(my_window);
```

---
### Startup Timeout

WebUI waits a couple of seconds (_Default is 30 seconds_) to let the web browser start and connect. You can control this behavior by using `webui_set_timeout()`.

```c
// Wait 10 seconds for the browser to start
webui_set_timeout(10);

// Now, After 10 seconds, if the browser did
// not get started, wait() will break
webui_wait();
```

```c
// Wait forever.
webui_set_timeout(0);

// webui_wait() will never end
webui_wait();
```

---
### Multi Access

![webui_access_denied](data/webui_access_denied.png)

After the window is loaded, the URL is not valid anymore for safety. WebUI will show an error if someone else tries to access the URL. To allow multi-user access to the same URL, you can use `webui_set_multi_access()`.

```c
webui_set_multi_access(my_window, true);
```

---
### Run JavaScript

You can run JavaScript on any window to read values, update the view, or anything else. In addition, you can check if the script execution has errors, as well as receive data.

```c
void my_function(webui_event_t* e){

	// Create a buffer to hold the response
    char response[64];

    // Run JavaScript
    if(!webui_script(
        e->window, // Window
        "return 2*2;", // JavaScript to be executed
        0, // Maximum waiting time in second
        response, // Local buffer to hold the JavaScript response
        64) // Size of the local buffer
    ) {
        printf("JavaScript Error: %s\n", response);
        return;
    }

    // Print the result
    printf("JavaScript Response: %s\n", response); // 4

    // Run JavaScript quickly with no waiting for the response
    webui_run(e->window, "alert('Fast!');");
}
```

---
### TypeScript Runtimes

You may want to interpret JavaScript & TypeScript files and show the output in the UI. You can use `webui_set_runtime()` and choose between `Deno` or `Nodejs` as your runtimes.

```c
// Deno
webui_set_runtime(my_window, Deno);
webui_show(my_window, "my_file.ts");

// Nodejs
webui_set_runtime(my_window, Nodejs);
webui_show(my_window, "my_file.js");
```
