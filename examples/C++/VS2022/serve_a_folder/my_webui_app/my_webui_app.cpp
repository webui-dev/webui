// Include the WebUI header
#include "webui.hpp"

// Include C++ STD
#include <iostream>

// Making this object global so show_second_window() can access it.
webui::window my_second_window;

// Example of a simple Class
class MyClass {
    public:

    // This method gets called every time the
    // user clicks on "OpenNewWindow"
    void show_second_window(webui::window::event* e) {

        // Show a new window, and navigate to `/second.html`
        // if the window is already opened, then switch in the same window
        my_second_window.show("second.html");
    }

    // This method gets called every time the
    // user clicks on "SwitchToSecondPage"
    void switch_to_second_page(webui::window::event* e) {

        // Switch to `/second.html` in the same opened window.
        e->window.show("second.html");
    }

    // Example of a simple function (Not a method)
    // This function receives all events because
    // it's get bind with an empty HTML ID.
    void events(webui::window::event* e) {

        if (e->event_type == webui::CONNECTED)
            std::cout << "Window Connected." << std::endl;
        else if (e->event_type == webui::DISCONNECTED)
            std::cout << "Window Disconnected." << std::endl;
        else if (e->event_type == webui::MOUSE_CLICK)
            std::cout << "Click on element: " << e->element << std::endl;
        else if (e->event_type == webui::NAVIGATION)
            std::cout << "Starting navigation to: " << e->data << std::endl;
    }

    // Example of a simple function (Not a method)
    void exit_app(webui::window::event* e) {

        // Close all opened windows
        webui::exit();
    }    
};

// Wrapper:
// Because WebUI is written in C, so it can not
// access `MyClass` directly. That's why we should
// create a simple C++ wrapper.
MyClass obj;
void show_second_window_wrp(webui::window::event* e) { obj.show_second_window(e); }
void switch_to_second_page_wrp(webui::window::event* e) { obj.switch_to_second_page(e); }
void events_wrp(webui::window::event* e) { obj.events(e); }
void exit_app_wrp(webui::window::event* e) { obj.exit_app(e); }

int main() {

    // Print logs (debug build only)
    std::cout << "Starting..." << std::endl;

    // Create a new window
    webui::window my_window;

    // Bind HTML element IDs with a C functions
    my_window.bind("SwitchToSecondPage", switch_to_second_page_wrp);
    my_window.bind("OpenNewWindow", show_second_window_wrp);
    my_window.bind("Exit", exit_app_wrp);
    my_second_window.bind("Exit", exit_app_wrp);

    // Bind all events
    my_window.bind("", events_wrp);

    // Show a new window
    my_window.show("index.html"); // my_window.show_browser("index.html", Chrome);

    // Wait until all windows get closed
    webui::wait();

    // Print logs (debug build only)
    std::cout << "Thank you." << std::endl;

    return 0;
}

#ifdef _WIN32
    // Release build
    int WINAPI wWinMain(
        _In_ HINSTANCE hInstance,
        _In_opt_ HINSTANCE hPrevInstance,
        _In_ LPWSTR lpCmdLine,
        _In_ int nShowCmd
    ) {
        return main();
    }
#endif
