# V-WebUI v2.3.0 APIs

- [Download](/v_api?id=download)
- [Build From Source](/v_api?id=build-from-source)
- [Examples](/v_api?id=examples)
- Window
    - [New Window](/v_api?id=new-window)
    - [Show Window](/v_api?id=show-window)
    - [Window status](/v_api?id=window-status)
- Binding & Events
    - [Bind](/v_api?id=Bind)
    - [Events](/v_api?id=events)
- Application
    - [Wait](/v_api?id=wait)
    - [Exit](/v_api?id=exit)
    - [Close](/v_api?id=close)
    - [Startup Timeout](/v_api?id=startup-timeout)
    - [Multi Access](/v_api?id=multi-access)
- JavaScript
    - [Run JavaScript From V](/v_api?id=run-javascript-from-c)
    - [Run V From JavaScript](/v_api?id=run-c-from-javascript)
    - [TypeScript Runtimes](/v_api?id=typescript-runtimes)

---
### Download

Download V-WebUI v2.3.0 prebuilt binaries here: https://github.com/malisipi/vwebui/releases

`v install https://github.com/malisipi/vwebui`

---
### Build from Source

You can build V-WebUI from source by cloning the V-WebUI repo and compile it using the V compiler, No need for any external dependencies.

```sh
git clone https://github.com/malisipi/vwebui.git
cd vwebui
v build
```

For more instructions, please visit [Build V-WebUI](https://github.com/malisipi/vwebui) in our GitHub repository.

---
### Examples

A minimal V example

```v
import malisipi.vwebui as webui

mut my_window := webui.new_window()
my_window.show("<html>Hello</html>")
webui.wait()
```

Using a local HTML file. Please not that you need to add `<script src="/webui.js"></script>` to all your HTML files

```v
import malisipi.vwebui as webui

// Please add <script src="/webui.js"></script> to your HTML files
mut my_window := webui.new_window()
my_window.show("my_file.html")
webui.wait()
```

Using a specific web browser

```v
import malisipi.vwebui as webui

mut my_window := webui.new_window()
my_window.showBrowser("<html>Hello</html>", Chrome)
webui.wait()
```

Please visit [V Examples](https://github.com/alifcommunity/webui/tree/main/examples/V) in our GitHub repository for instructions on compiling this example or finding more complete examples.

---
### New Window

To create a new window object, you can use `webui_new_window()`, which returns a void pointer. Please note that this pointer does *NOT* need to be freed.

```v
import malisipi.vwebui as webui
mut my_window := webui.new_window()
```

---
### Show Window

To show a window, you can use `webui_show()`. If the window is already shown, the UI will get refreshed in the same window.

```v
import malisipi.vwebui as webui
// Show a window using the embedded HTML
my_window.show("<html>Hello!</html>")

// Show a window using an .html local file
// Please add <script src="/webui.js"></script> to your HTML files
my_window.show("my_file.html")
```

Show a window using a specific web browser

```v
import malisipi.vwebui as webui

my_html := "<html>Hello!</html>";

// Google Chrome
my_window.showBrowser(my_html, Chrome);

// Mozilla Firefox
my_window.showBrowser(my_html, Firefox);

// Microsoft Edge
my_window.showBrowser(my_html, Edge);

// Microsoft Apple Safari (Not Ready)
my_window.showBrowser(my_html, Safari);

// The Chromium Project
my_window.showBrowser(my_html, Chromium);

// Opera Browser (Not Ready)
my_window.showBrowser(my_html, Opera);

// The Brave Browser
my_window.showBrowser(my_html, Brave);

// The Vivaldi Browser
my_window.showBrowser(my_html, Vivaldi);

// The Epic Browser
my_window.showBrowser(my_html, Epic);

// The Yandex Browser
my_window.showBrowser(my_html, Yandex);

// Default recommended web browser
my_window.showBrowser(my_html, AnyBrowser);

// Or simply
webui_show(my_window, my_html);
```

If you need to update the whole UI content, you can also use the same function `webui_show()`, which allows you to refresh the window UI with any new HTML content.

```v
import malisipi.vwebui as webui

html := "<html>Hello</html>";
new_html := "<html>New World!</html>";

// Open a window
my_window.show(html)

// Later...

// Refresh the same window with the new content
my_window.show(new_html)
```

---
### Window Status

To know if a specific window is running, you can use `webui_is_shown()`.

```v
import malisipi.vwebui as webui

if(webui_is_shown(my_window))
    printf("The window is still running");
else
    printf("The window is closed.");
```

---
### Bind

Use `webui_bind()` to receive click events when the user clicks on any HTML element with a specific ID, for example `<button id="MyID">Hello</button>`.

```v
import malisipi.vwebui as webui
void my_function(webui_event_t* e) {
    // <button id="MyID">Hello</button> gets clicked!
}

webui_bind(my_window, "MyID", my_function);
```

### Events

The *e* corresponds to the word _Event_. `e` is a struct that has these elements:

```v
import malisipi.vwebui as webui
size_t window; // Pointer to the window struct.
unsigned int event_type; // Event type (WEBUI_EVENT_MOUSE_CLICK, WEBUI_EVENT_NAVIGATION...).
char* element; // HTML element ID.
char* data; // The data are coming from JavaScript, if any.
char* response; // Internally used by webui_return_xxx().
```

```v
import malisipi.vwebui as webui
void my_function(webui_event_t* e){

    printf("Hi!, You clicked on %s element\n", e.element);

    if(e->event_type == WEBUI_EVENT_CONNECTED)
        printf("Connected. \n");
    else if(e->event_type == WEBUI_EVENT_DISCONNECTED)
        printf("Disconnected. \n");
    else if(e->event_type == WEBUI_EVENT_MOUSE_CLICK)
        printf("Click. \n");
    else if(e->event_type == WEBUI_EVENT_NAVIGATION)
        printf("Starting navigation to: %s \n", e->data);    

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

It is essential to call `webui_wait()` at the end of your main function, after you create/shows all your windows. This will make your application run until the user closes all visible windows or when calling *[webui_exit()](/v_api?id=exit)*.

```v
import malisipi.vwebui as webui
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

At any moment, you can call `webui_exit()`, which tries to close all related opened windows and make *[webui_wait](/v_api?id=wait)* break.

```v
import malisipi.vwebui as webui
webui_exit();
```

---
### Close

You can call `webui_close()` to close a specific window, if there is no running window left *[webui_wait](/v_api?id=wait)* will break.

```v
import malisipi.vwebui as webui
webui_close(my_window);
```

---
### Startup Timeout

V-WebUI waits a couple of seconds (_Default is 30 seconds_) to let the web browser start and connect. You can control this behavior by using `webui_set_timeout()`.

```v
import malisipi.vwebui as webui
// Wait 10 seconds for the browser to start
webui_set_timeout(10);

// Now, After 10 seconds, if the browser did
// not get started, wait() will break
webui_wait();
```

```v
import malisipi.vwebui as webui
// Wait forever.
webui_set_timeout(0);

// webui_wait() will never end
webui_wait();
```

---
### Multi Access

![webui_access_denied](data/webui_access_denied.png)

After the window is loaded, the URL is not valid anymore for safety. V-WebUI will show an error if someone else tries to access the URL. To allow multi-user access to the same URL, you can use `webui_set_multi_access()`.

```v
import malisipi.vwebui as webui
webui_set_multi_access(my_window, true);
```

---
### Run JavaScript From V

You can run JavaScript on any window to read values, update the view, or anything else. In addition, you can check if the script execution has errors, as well as receive data.

```v
import malisipi.vwebui as webui
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
### Run V From JavaScript

To call a V function from JavaScript and get the result back please use `webui_fn('MyID', 'My Data').then((response) => { ... });`. If the function does not have a response then it's safe to remove the `then` method like this `webui_fn('MyID_NoResponse', 'My Data');`.

```v
import malisipi.vwebui as webui
void my_function(webui_event_t* e) {

    // Get data from JavaScript
    str = webui_get_string(e);
    // long long number = webui_get_int(e);
    // bool status = webui_get_bool(e);

    // Print the received data
    printf("Data from JavaScript: %s\n", str); // Message from JS

    // Return back a response to JavaScript
    webui_return_string(e, "Message from V");
    // webui_return_int(e, number);
    // webui_return_bool(e, true);
}

webui_bind(my_window, "MyID", my_function);
```

JavsScript:

```js
webui_fn('MyID', 'Message from JS').then((response) => {
    console.log(response); // "Message from V
});
```

---
### TypeScript Runtimes

You may want to interpret JavaScript & TypeScript files and show the output in the UI. You can use `webui_set_runtime()` and choose between `Deno` or `Nodejs` as your runtimes.

```v
import malisipi.vwebui as webui
// Deno
webui_set_runtime(my_window, Deno);
webui_show(my_window, "my_file.ts");

// Nodejs
webui_set_runtime(my_window, Nodejs);
webui_show(my_window, "my_file.js");
```
