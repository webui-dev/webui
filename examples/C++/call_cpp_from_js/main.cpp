// Call C++ from JavaScript Example

// Include the WebUI header
#include "webui.hpp"

// Include C++ STD
#include <iostream>

void my_function_string(webui::window::event* e) {

    // JavaScript:
    // webui.call('MyID_One', 'Hello');

    std::string str = e->get_string();
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

void my_function_integer(webui::window::event* e) {

    // JavaScript:
    // webui.call('MyID_Two', 123456789);

    long long number = e->get_int();
    std::cout << "my_function_integer: " << number << std::endl; // 123456789
}

void my_function_boolean(webui::window::event* e) {

    // JavaScript:
    // webui.call('MyID_Three', true);

    bool status = e->get_bool(); // True
    if(status)
        std::cout << "my_function_boolean: True" << std::endl;
    else
        std::cout << "my_function_boolean: False" << std::endl;
}

void my_function_with_response(webui::window::event* e) {

    // JavaScript:
    // const result = webui.call('MyID_Four', number);

    long long number = e->get_int();
    number = number * 2;
    std::cout << "my_function_with_response: " << number << std::endl;

    // Send back the response to JavaScript
    e->return_int(number);
}

int main() {

    // HTML
    const std::string my_html = R"V0G0N(
      <html>
        <head>
          <script src="webui.js"></script>

          <title>Call C++ from JavaScript Example</title>
          <style>
            body {
              background: linear-gradient(to left, #36265a, #654da9);
              color: AliceBlue;
              font-size: 16px sans-serif;
              text-align: center;
              margin-top: 30px;
            }
            button {
              margin: 5px 0 10px;
            }
          </style>
        </head>
        <body>
          <h1>WebUI - Call C++ from JavaScript</h1>
          <p>Call C++ functions with arguments (<em>See the logs in your terminal</em>)</p>
          <button onclick="webui.call('MyID_One', 'Hello');">Call my_function_string()</button>
          <br>
          <button onclick="webui.call('MyID_Two', 123456789);">Call my_function_integer()</button>
          <br>
          <button onclick="webui.call('MyID_Three', true);">Call my_function_boolean()</button>
          <br>
          <p>Call a C++ function that returns a response</p>
          <button onclick="MyJS();">Call my_function_with_response()</button>
          <div>Double: <input type="text" id="MyInputID" value="2"></div>
          <script>
            function MyJS() {
              const MyInput = document.getElementById('MyInputID');
              const number = MyInput.value;
              webui.call('MyID_Four', number).then((response) => {
                MyInput.value = response;
              });
            }
          </script>
        </body>
      </html>
    )V0G0N";

    // Create a window
    webui::window my_window;

    // Bind HTML elements with C++ functions
    my_window.bind("MyID_One", my_function_string);
    my_window.bind("MyID_Two", my_function_integer);
    my_window.bind("MyID_Three", my_function_boolean);
    my_window.bind("MyID_Four", my_function_with_response);

    // Show the window
    my_window.show(my_html); // webui_show_browser(my_window, my_html, Chrome);

    // Wait until all windows get closed
    webui::wait();

    return 0;
}

#if defined(_MSC_VER)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
        main();
    }
#endif
