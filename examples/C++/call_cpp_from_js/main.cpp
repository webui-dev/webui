// Call C++ from JavaScript Example

// Include the WebUI header
#include "webui.hpp"

// Include C++ STD
#include <iostream>

void my_function_string(webui_event_t* e) {

    // JavaScript:
    // webui_fn('MyID_One', 'Hello');

    std::string str = webui::get_string(e);
    std::cout << "my_function_string: " << str << std::endl; // Hello

    // Need Multiple Arguments?
    //
    // WebUI support only one argument. To get multiple arguments
    // you can send a JSON string from JavaScript then decode it.
    // Example:
    //
    // my_json = my_json_decoder(str);
    // foo = my_json[0];
    // bar = my_json[1];
}

void my_function_integer(webui_event_t* e) {

    // JavaScript:
    // webui_fn('MyID_Two', 123456789);

    long long number = webui::get_int(e);
    std::cout << "my_function_integer: " << number << std::endl; // 123456789
}

void my_function_boolean(webui_event_t* e) {

    // JavaScript:
    // webui_fn('MyID_Three', true);

    bool status = webui::get_bool(e); // True
    if(status)
        std::cout << "my_function_boolean: True" << std::endl;
    else
        std::cout << "my_function_boolean: False" << std::endl;
}

void my_function_with_response(webui_event_t* e) {

    // JavaScript:
    // const result = webui_fn('MyID_Four', number);

    long long number = webui::get_int(e);
    number = number * 2;
    std::cout << "my_function_with_response: " << number << std::endl;

    // Send back the response to JavaScript
    webui::return_int(e, number);
}

int main() {

    // HTML
    const std::string my_html = R"V0G0N(
    <html>
      <head>
        <title>Call C++ from JavaScript Example</title>
        <style>
          body {
            color: white;
            background: #0F2027;
            text-align: center;
            font-size: 16px;
            font-family: sans-serif;
          }
        </style>
      </head>
      <body>
        <h2>WebUI - Call C++ from JavaScript Example</h2>
        <p>Call C++ function with argument (<em>See the logs in your terminal</em>)</p>
        <br>
        <button onclick="webui_fn('MyID_One', 'Hello');">Call my_function_string()</button>
        <br>
        <br>
        <button onclick="webui_fn('MyID_Two', 123456789);">Call my_function_integer()</button>
        <br>
        <br>
        <button onclick="webui_fn('MyID_Three', true);">Call my_function_boolean()</button>
        <br>
        <br>
        <p>Call C++ function and wait for the response</p>
        <br>
        <button onclick="MyJS();">Call my_function_with_response()</button>
        <br>
        <br>
        <input type="text" id="MyInputID" value="2">
        <script>
          function MyJS() {
            const MyInput = document.getElementById('MyInputID');
            const number = MyInput.value;
            const result = webui_fn('MyID_Four', number);
            MyInput.value = result;
          }
        </script>
      </body>
    </html>
    )V0G0N";

    // Create a window
    void* my_window = webui::new_window();

    // Bind HTML elements with C++ functions
    webui::bind(my_window, "MyID_One", my_function_string);
    webui::bind(my_window, "MyID_Two", my_function_integer);
    webui::bind(my_window, "MyID_Three", my_function_boolean);
    webui::bind(my_window, "MyID_Four", my_function_with_response);

    // Show the window
    webui::show(my_window, my_html); // webui_show_browser(my_window, my_html, Chrome);

    // Wait until all windows get closed
    webui::wait();

    return 0;
}

#if defined(_MSC_VER)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
        main();
    }
#endif
