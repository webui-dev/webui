#include "webui.hpp"
#include <iostream>

void* my_window;
void* my_second_window;

void exit_app(webui_event_t* e) {

    // Close all opened windows
    webui::exit();
}

void events(webui_event_t* e) {

    // This function gets called every time
    // there is an event

    if (e->type == WEBUI_EVENT_CONNECTED)
        std::cout << "Connected. " << std::endl;
    else if (e->type == WEBUI_EVENT_DISCONNECTED)
        std::cout << "Disconnected. " << std::endl;
    else if (e->type == WEBUI_EVENT_MOUSE_CLICK)
        std::cout << "Click. " << std::endl;
    else if (e->type == WEBUI_EVENT_NAVIGATION)
        std::cout << "Starting navigation to: " << e->data << std::endl;
}

void switch_to_second_page(webui_event_t* e) {

    // This function gets called every
    // time the user clicks on "SwitchToSecondPage"

    // Switch to `/second.html` in the same opened window.
    webui::show(e->window, "second.html");
}

void show_second_window(webui_event_t* e) {

    // This function gets called every
    // time the user clicks on "OpenNewWindow"

    // Show a new window, and navigate to `/second.html`
    // if it's already open, then switch in the same window
    webui::show(my_second_window, "second.html");
}

int main()
{
    // Print logs (debug build only)
    std::cout << "Starting..." << std::endl;

    // Create new windows
    my_window = webui::new_window();
    my_second_window = webui::new_window();

    // Bind HTML element IDs with a C functions
    webui::bind(my_window, "SwitchToSecondPage", switch_to_second_page);
    webui::bind(my_window, "OpenNewWindow", show_second_window);
    webui::bind(my_window, "Exit", exit_app);
    webui::bind(my_second_window, "Exit", exit_app);

    // Bind events
    webui::bind(my_window, "", events);

    // Show a new window
    webui::show(my_window, "index.html"); // webui::show_browser(my_window, "index.html", Chrome);

    // Wait until all windows get closed
    webui::wait();

    // Print logs (debug build only)
    std::cout << "Thank you." << std::endl;

    return 0;
}

// Release build
int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
) {
	return main();
}
