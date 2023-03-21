/*
    WebUI Library 2.0.7
    C99 Example

    http://webui.me
    https://github.com/alifcommunity/webui

    Licensed under GNU General Public License v3.0.
    Copyright (C)2023 Hassan DRAGA <https://github.com/hassandraga> - Canada.
*/

// Note:
// To see the console logs (printf) you need to build in debug mode
// example 'make debug', or 'nmake debug', or 'mingw32-make debug'

#include "webui.h"

void function_one(webui_event_t* e) {

    // JavaScript: webui_fn('One', 'Hello');

    const char* str = webui_get_string(e);
    printf("function_one: %s\n", str); // Hello
}

void function_two(webui_event_t* e) {

    // JavaScript: webui_fn('Two', 2022);

    int number = webui_get_int(e);
    printf("function_two: %d\n", number); // 2022
}

void function_three(webui_event_t* e) {

    // JavaScript: webui_fn('Three', true);

    bool status = webui_get_bool(e);
    if(status)
        printf("function_three: True\n"); // True
    else
        printf("function_three: False\n"); // False
}

void function_four(webui_event_t* e) {

    // JavaScript: const result = webui_fn('Four', 2);

    int number = webui_get_int(e);
    number = number * 2;
    printf("function_four: %d\n", number); // 4

    // Send back the response to JavaScript
    webui_return_int(e, number);
}

int main() {

    // HTML
    const char* my_html = "<!DOCTYPE html>"
    "<html><head><title>WebUI 2 - C99 Example</title>"
    "<style>body{color: white; background: #0F2027;"
    "background: -webkit-linear-gradient(to right, #2C5364, #203A43, #0F2027);"
    "background: linear-gradient(to right, #2C5364, #203A43, #0F2027);"
    "text-align:center; font-size: 18px; font-family: sans-serif;}</style></head><body>"
    "<h2>WebUI 2 - C99 Example</h2>"
    "<p>Call C function with arguments (See log in the Windows console)</p><br>"
    "<button OnClick=\"webui_fn('One', 'Hello');\">Call C function one</button><br><br>"
    "<button OnClick=\"webui_fn('Two', 2022);\">Call C function two</button><br><br>"
    "<button OnClick=\"webui_fn('Three', true);\">Call C function three</button><br><br>"
    "<p>Call C function four, and wait for the result</p><br>"
    "<button OnClick=\"MyJS();\">Call C function four</button><br><br>"
    "<input type=\"text\" id=\"MyInput\" value=\"2\">"
    "<script>"
    "   function MyJS() {"
    "       const number = document.getElementById('MyInput').value;"
    "       var result = webui_fn('Four', number);"
    "       document.getElementById('MyInput').value = result;"
    "   }"
    "</script>"
    "</body></html>";

    // Create a window
    webui_window_t* my_window = webui_new_window();

    // Bind HTML elements with functions
    webui_bind(my_window, "One", function_one);
    webui_bind(my_window, "Two", function_two);
    webui_bind(my_window, "Three", function_three);
    webui_bind(my_window, "Four", function_four);

    // Show the window
    if(!webui_show(my_window, my_html, webui.browser.chrome))   // Run the window on Chrome
        webui_show(my_window, my_html, webui.browser.any);      // If not, run on any other installed web browser

    // Wait until all windows get closed
    webui_wait();

    return 0;
}

#if defined(_MSC_VER)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
        main();
    }
#endif
