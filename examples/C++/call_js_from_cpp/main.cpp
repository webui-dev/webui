// Call JavaScript from C++ Example

#include "webui.hpp"
#include <iostream>
#include <sstream>
#include <string>

class CounterApp {
public:
    CounterApp() {
        // Bind HTML element IDs with class methods
        win.bind("my_function_count", this, &CounterApp::my_function_count);
        win.bind("Exit", this, &CounterApp::exit_app);
    }

    void run() {
        const std::string html = R"V0G0N(
        <html>
        <head>
            <meta charset="UTF-8">
            <script src="/webui.js"></script>
            <title>Call JavaScript from C++ Example</title>
            <style>
            body {
                background: linear-gradient(to left, #36265a, #654da9);
                color: AliceBlue;
                font: 16px sans-serif;
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
            <h1 id="count">0</h1>
            <br>
            <button id="ManualBtn" OnClick="my_function_count();">Manual Count</button>
            <br>
            <button id="AutoBtn" OnClick="AutoTest();">Auto Count (Every 10ms)</button>
            <br>
            <button id="Exit" OnClick="this.disabled=true;">Exit</button>
            <script>
            let count = 0;
            let auto_running = false;

            function GetCount() {
                return count;
            }
            function SetCount(number) {
                document.getElementById('count').innerHTML = number;
                count = number;
            }
            function AutoTest(number) {
                if (auto_running) return;
                auto_running = true;
                document.getElementById('AutoBtn').disabled = true;
                document.getElementById('ManualBtn').disabled = true;

                setInterval(function() {
                my_function_count();
                }, 10);
            }
            </script>
        </body>
        </html>
        )V0G0N";

        // Set WebUI configuration to process UI events one at a time.
        // This example calls C++ from JavaScript every 10ms, so it's 
        // recommended to set this to `true` to avoid race conditions.
        webui::set_config(ui_event_blocking, true);

        // Show the HTML page in the WebView/Browser window
        win.show(html);
            // win.show_browser(html, webui_browser::Chrome);
            // win.show_wv(html);

        // Wait until the window gets closed
        webui::wait();
    }

private:
    webui::window win;

    void my_function_count(webui::window::event* e) {
        // Create a buffer to hold the response
        char response[64];

        // Run JavaScript to get the current count value from the HTML page
        if (!e->get_window().script("return GetCount();", 0, response, 64)) {
            if (!e->get_window().is_shown())
                std::cout << "Window closed." << std::endl;
            else
                std::cout << "JavaScript Error: " << response << std::endl;
            return;
        }

        // Get the count, increment, and send it back
        int count = std::stoi(response) + 1;
        std::stringstream js;
        js << "SetCount(" << count << ");";
        // Run JavaScript (no response) to update the count in the HTML page
        e->get_window().run(js.str());
    }

    void exit_app(webui::window::event* e) {
        // Exit the application, and close all windows. `webui::wait()` will return (Break).
        webui::exit();
    }
};

int main() {
    CounterApp app;
    app.run();
    return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
