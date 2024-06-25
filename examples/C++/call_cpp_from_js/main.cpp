// Call C++ from JavaScript Example

// Include the WebUI header
#include "webui.hpp"

// Include C++ STD
#include <iostream>

void my_function_string(webui::window::event* e) {

	// JavaScript:
	// my_function_string('Hello', 'World`);

	std::string str_1 = e->get_string(); // Or e->get_string(0);
	std::string str_2 = e->get_string(1);

	std::cout << "my_function_string 1: " << str_1 << std::endl; // Hello
	std::cout << "my_function_string 2: " << str_2 << std::endl; // World
}

void my_function_integer(webui::window::event* e) {

	// JavaScript:
	// my_function_integer(123, 456, 789);

	long long number_1 = e->get_int(); // Or e->get_int(0);
	long long number_2 = e->get_int(1);
	long long number_3 = e->get_int(2);

	std::cout << "my_function_integer 1: " << number_1 << std::endl; // 123
	std::cout << "my_function_integer 2: " << number_2 << std::endl; // 456
	std::cout << "my_function_integer 3: " << number_3 << std::endl; // 789
}

void my_function_boolean(webui::window::event* e) {

	// JavaScript:
	// my_function_boolean(true, false);

	bool status_1 = e->get_bool(); // Or e->get_bool(0);
	bool status_2 = e->get_bool(1);

	std::cout << "my_function_boolean 1: " << (status_1 ? "True" : "False") << std::endl;
	std::cout << "my_function_boolean 2: " << (status_2 ? "True" : "False") << std::endl;
}

void my_function_with_response(webui::window::event* e) {

	// JavaScript:
	// my_function_with_response(number, 2).then(...)

	long long number = e->get_int(0);
	long long times = e->get_int(1);

	long long res = number * times;

	std::cout << "my_function_with_response: " << number << " * " << times << " = " << res << std::endl;

	// Send back the response to JavaScript
	e->return_int(res);
}

int main() {

	// HTML
	const std::string my_html = R"V0G0N(
      <html>
        <head>
          <meta charset="UTF-8">
          <script src="/webui.js"></script>

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
          <button onclick="my_function_string('Hello', 'World');">Call my_function_string()</button>
          <br>
          <button onclick="my_function_integer(123, 456, 789);">Call my_function_integer()</button>
          <br>
          <button onclick="my_function_boolean(true, false);">Call my_function_boolean()</button>
          <br>
          <p>Call a C++ function that returns a response</p>
          <button onclick="MyJS();">Call my_function_with_response()</button>
          <div>Double: <input type="text" id="MyInputID" value="2"></div>
          <script>
            function MyJS() {
              const MyInput = document.getElementById('MyInputID');
              const number = MyInput.value;
              my_function_with_response(number, 2).then((response) => {
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
	my_window.bind("my_function_string", my_function_string);
	my_window.bind("my_function_integer", my_function_integer);
	my_window.bind("my_function_boolean", my_function_boolean);
	my_window.bind("my_function_with_response", my_function_with_response);

	// Show the window
	my_window.show(my_html); // webui_show_browser(my_window, my_html, Chrome);

	// Wait until all windows get closed
	webui::wait();

	return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { main(); }
#endif
