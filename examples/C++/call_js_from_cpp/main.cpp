// Call JavaScript from C++ Example

// Include the WebUI header
#include "webui.hpp"

// Include C++ STD
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

void my_function_exit(webui::window::event* e) {

    // Close all opened windows
    webui::exit();
}

void my_function_count(webui::window::event* e) {

    // This function gets called every time the user clicks on "MyButton1"

    // Create a buffer to hold the response
    char response[64];

    // This is another way to create a buffer:
    //  std::string buffer;
    //  buffer.reserve(64);
    //  my_window.script(..., ..., &buffer[0], 64);

    // Run JavaScript
    if(!e->get_window().script("return GetCount();", 0, response, 64)) {

        std::cout << "JavaScript Error: " << response << std::endl;
        return;
    }

    // Get the count
    int count = std::stoi(response);

    // Increment
    count++;

    // Generate a JavaScript
    std::stringstream js;
    js << "SetCount(" << count << ");";

    // Run JavaScript (Quick Way)
    e->get_window().run(js.str());
}

int main() {

    // HTML
    const std::string my_html = R"V0G0N(
    <html>
      <head>
        <title>Call JavaScript from C++ Example</title>
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
        <h1>WebUI - Call JavaScript from C++</h1>
        <br>
        <button id="MyButton1">Count <span id="count">0</span>!</button>
        <br>
        <button id="MyButton2">Exit</button>
        <script>
          let count = 0;
          function GetCount() {
            return count;
          }
          function SetCount(number) {
            document.getElementById('count').innerHTML = number;
            count = number;
          }
        </script>
      </body>
    </html>
    )V0G0N";

    // Create a window
    webui::window my_window;

    // Bind HTML elements with C++ functions
    my_window.bind("MyButton1", my_function_count);
    my_window.bind("MyButton2", my_function_exit);

    // Show the window
    my_window.show(my_html); // my_window.show_browser(my_html, Chrome);

    // Wait until all windows get closed
    webui::wait();

    return 0;
}

#if defined(_MSC_VER)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
        main();
    }
#endif
