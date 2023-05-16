# WebUI v2.3.0 Nim APIs

- [Download And Install](/nim_api?id=download-and-install)
- [Examples](/nim_api?id=examples)
- Window
    - [New Window](/nim_api?id=new-window)
    - [Show Window](/nim_api?id=show-window)
    - [Window status](/nim_api?id=window-status)
- Binding & Events
    - [Bind](/nim_api?id=Bind)
    - [Events](/nim_api?id=events)
- Application
    - [Wait](/nim_api?id=wait)
    - [Exit](/nim_api?id=exit)
    - [Close](/nim_api?id=close)
    - [Startup Timeout](/nim_api?id=startup-timeout)
    - [Multi Access](/nim_api?id=multi-access)
- JavaScript
    - [Run JavaScript From Python](/nim_api?id=run-javascript-from-nim)
    - [Run Python From JavaScript](/nim_api?id=run-nim-from-javascript)
    - [TypeScript Runtimes](/nim_api?id=typescript-runtimes)

---
### Download And Install

Install the WebUI package from Nimble.

```sh
nimble install webui
```

Or from GitHub:

```sh
nimble install https://github.com/webui-dev/nim-webui
```

To see the WebUI Nim wrapper source code, please visit [nim-webui](https://github.com/webui-dev/nim-webui/), our GitHub repository.

---
### Examples

A very *minimal* Nim example

```nim
import webui

let window = newWindow() # Create a new Window
window.show("<html>Hello</html>") # Show the window with html content

wait() # Wait until the window gets closed
```

Using a local HTML file. Please note that you need to add `<script src="/webui.js"></script>` to all your HTML files.

```nim
import webui

let window = newWindow() 

# Please add <script src="/webui.js"></script> to your HTML files
window.show("index.html")
wait()
```

Using a specific web browser

```nim
import webui

let window = newWindow()
window.show("<html>Hello</html>", BrowserChrome)

wait()
```

Please visit [Nim Examples](https://github.com/webui-dev/nim-webui/tree/main/examples) in our GitHub repository for more complete and complex examples.

---
### New Window

To create a new window object, you can use `newWindow()`, which returns a `Window` object.

```nim
let window = newWindow()
```

---
### Show Window

To show a window, you can use `show()`. If the window is already shown, the UI will get refreshed in the same window.

```nim
# Shows html in any installed web browser, using the embedded html
window.show("<html>Hello!</html>")
```

```nim
# Show a window using a local html file
# Please add <script src="/webui.js"></script> to your HTML files
window.show("my_file.html")
```

Show a window using a specific web browser

```python
const html = "<html>Hello!</html>"

# Chrome
window.show(html, Browsers.Chrome)

# Firefox
window.show(html, Browsers.Firefox)

# Microsoft Edge
window.show(html, Browsers.Edge)

# Pure enums, so Chrome, Firefox, etc. work aswell

# Other browsers...

# Any available web browser
window.show(html, Browsers.Any)
```

If you need to update the whole UI content, you can also use `show()`, which allows you to refresh the window UI with any new HTML content.

```nim
import std/os

let html = "<html>Hello</html>"
let newHtml = "<html>New World!</html>"

# Open a window
window.show(html)

# Later...
sleep(5000)

# Refresh the same window with the new content
window.show(newHtml)
```

---
### Window Status

To know if a specific window is running, you can use `shown()`.

```nim
if window.shown():
  echo "A window is running..."
else:
  echo "No window is running."
```

---
### Bind

Use `bind()` to receive click events when the user clicks on any HTML element with a specific ID, for example `<button id="MyID">Hello</button>`.

```nim
window.bind("MyID") do (e: Event):
  # <button id="MyID">Hello</button> gets clicked!
  echo "Binding element ", e.element, "!"
```

Instead of using `do` notation, you can also define a proc separately and
bind it manually:

```nim
proc exitApp(e: Event) =
  exit()

window.bind(exitApp)
```

You can also have a return value on your function, it must be either a `string`, `int`, or `bool`. The return value will be automatically passed back to the Javascript code for you.

```nim
window.bind("MyID") do (e: Event) -> int:
  return 1 + 2  # 3
```

### Events

```nim
# Empty ID means bind all events on all elements
window.bind("") do (e: Event):
  echo "Hi!, You clicked on ", e.element, " element"
```

The *e* corresponds to the *Event* object. `e` is a object that has these elements:

```nim
e.window       # Window: The window the event occurred on
e.eventType    # Events enum: Event type
e.element      # string: HTML element ID
e.data         # string: JavaScript data/response
```

You can access other attributes like `eventNumber`, but those are used by WebUI, and are only meant for internal use by the library.

---
### Wait

It is essential to call `wait()` at the end of your application, after you create/shows all your windows. This will make your application run until the user closes all visible windows or when calling *[exit()](/nim_api?id=exit)*.

```nim
# Create windows...
# Bind HTML elements...
# Show the windows...

# Wait until all windows get closed
# or when calling exit()
wait()
```

---
### Exit

At any moment, you may call `exit()`, which tries to close all related opened windows and make *[wait()](/nim_api?id=wait)* break.

```nim
exit()
```

---
### Close

You can call `close()` to close a specific window, if there is no running window left *[wait](/nim_api?id=wait)* will break.

```python
window.close()
```

---
### Startup Timeout

WebUI waits a couple of seconds (*Default is 30 seconds*) to let the web browser start and connect. You can control this behavior by using `set_timeout()`.

```nim
# Wait 10 seconds for the web browser to start
setTimeout(10)

# Now, after 10 seconds, if the web browser
# did not start yet, wait() will break
wait()
```

```nim
# Wait forever.
setTimeout(0)

# wait() will never end
wait()
```

---
### Multi Access

![WebUI "Access Denied" Image](data/webui_access_denied.png)

After the window is loaded, the URL is not valid anymore for safety. WebUI will show an error if someone else tries to access the URL. To allow multi-user access to the same URL, you can use `multiAccess=`.

```nim
window.multiAccess = true
```

---
### Run JavaScript From Nim

You can run JavaScript on any window to read values, update the view, or anything else. In addition, you can check if the script execution has errors, as well as receive data.

```python
window.bind("ExampleElement") do (e: Event):
  # Run JavaScript to get the result
  let res = e.window.script("return 2*2;")

  # Check for any error
  if res.error:
    echo "JavaScript Error: ", res.data
  else:
    echo "JavaScript Response: ", res.data # 4

  # Run JavaScript quickly with no waiting for the response
  e.window.run("alert('Fast!')")
```

---
### Run Nim From JavaScript

To call a Nim function from JavaScript and get the result back please use `webui_fn('MyID', 'My Data').then((response) => { ... });`. If the function does not have a response then it's safe to remove the `then` method like this `webui_fn('MyID_NoResponse', 'My Data');`.

```python
window.bind("MyID") do (e: Event) -> string:
  echo "Data from JavaScript: ", e.data # Message from JS

  return "Message from Nim"
```

JavaScript:

```js
webui_fn('MyID', 'Message from JS').then((response) => {
    console.log(response); // "Message from Python
});
```

---
### TypeScript Runtimes

You may want to interpret JavaScript & TypeScript files and show the output in the UI. You can use `runtime=` and choose between Deno or Nodejs as your runtime.

```python
# Deno
window.runtime = Deno
window.show("my_file.ts")

# Nodejs
window.runtime = NodeJS
window.show("my_file.js")

# Disable
window.runtime = None
```
