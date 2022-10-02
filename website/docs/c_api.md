# WebUI C API's

- [New Window](/c_api?id=New-Window)
- [Bind](/c_api?id=Bind)
- [Bind All](/c_api?id=Bind-All)
- [Show Window](/c_api?id=Show-Window)
- [Loop](/c_api?id=Loop)
- [Event](/c_api?id=Event)

### New Window

To create a new window object we use `webui_new_window()` which return a pointer to a struct `webui_window_t`. This pointer should never be freed.

```c
webui_window_t* my_window = webui_new_window();
```

### Bind

To receive a click event (*function call*) when the user click on an HTML element (*all kind of elements*) thats have an HTML id `MyID` for example `<button id="MyID">Hello</button>`, we use `webui_bind()`.

```c
void my_function(const webui_event_t e) {
    ...
}

webui_bind(my_window, "MyID", my_function);
```

### Bind All

You can also automatically bind all HTML element with one function call.

```c
void all_clicks(const webui_event_t e) {
    ...
}

webui_bind_all(my_window, all_clicks);
```

### Show Window

To show a window we use `webui_show()`. If the window is already shown the UI will get refreshed in the same window.

```c
const char* my_html = "<!DOCTYPE html><html>...</html>";

// Chrome
webui_show(my_window, my_html, webui.browser.chrome);

// Firefox
webui_show(my_window, my_html, webui.browser.firefox);

// Microsoft Edge
webui_show(my_window, my_html, webui.browser.edge);

// Any available browser
webui_show(my_window, my_html, webui.browser.any);
```

### Loop

It's very important to call `webui_loop()` at the end of your main function after you created/shows all your windows.

```c
int main() {

	// Create windows...
	// Bind HTML elements...
    // Show the windows...

    // Wait until all windows get closed
	webui_loop();

    return 0;
}
```

### Event

When you use [webui_bind()](/c_api?id=Bind) your application receive an event every time the user click on the HTML element you selected. The event come with the `element_name` which is The HTML ID of the clicked element, example `MyButton`, `MyInput`.., The event come also with the WebUI unique element ID and the unique window ID, those two IDs are not needed in general except if you are written a wrapper for WebUI in other language than C.

```c
void my_function(const webui_event_t e){

	printf("Hi!, You clicked on %s element\n", e.element_name);
}
```

