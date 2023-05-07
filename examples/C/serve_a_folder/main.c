// Serve a Folder Example

#include "webui.h"

// Those defines is to avoid using global variables
// you can also use `const size_t MyWindow;`.
// A window ID can be between 1 and 512 (WEBUI_MAX_IDS)
#define MyWindow (1)
#define MySecondWindow (2)

void exit_app(webui_event_t* e) {

    // Close all opened windows
    webui_exit();
}

void events(webui_event_t* e) {

    // This function gets called every time
    // there is an event

    if(e->event_type == WEBUI_EVENT_CONNECTED)
        printf("Connected. \n");
    else if(e->event_type == WEBUI_EVENT_DISCONNECTED)
        printf("Disconnected. \n");
    else if(e->event_type == WEBUI_EVENT_MOUSE_CLICK)
        printf("Click. \n");
    else if(e->event_type == WEBUI_EVENT_NAVIGATION)
        printf("Starting navigation to: %s \n", (char *)e->data);
}

void switch_to_second_page(webui_event_t* e) {

    // This function gets called every
    // time the user clicks on "SwitchToSecondPage"

    // Switch to `/second.html` in the same opened window.
    webui_show(e->window, "second.html");
}

void show_second_window(webui_event_t* e) {

    // This function gets called every
    // time the user clicks on "OpenNewWindow"

    // Show a new window, and navigate to `/second.html`
    // if it's already open, then switch in the same window
    webui_show(MySecondWindow, "second.html");
}

int main() {

    // Create new windows
    webui_new_window_id(MyWindow);
    webui_new_window_id(MySecondWindow);

    // Bind HTML element IDs with a C functions
    webui_bind(MyWindow, "SwitchToSecondPage", switch_to_second_page);
    webui_bind(MyWindow, "OpenNewWindow", show_second_window);
    webui_bind(MyWindow, "Exit", exit_app);
    webui_bind(MySecondWindow, "Exit", exit_app);

    // Bind events
    webui_bind(MyWindow, "", events);

    // Make Deno as the `.ts` and `.js` interpreter
    webui_set_runtime(MyWindow, Deno);

    // Show a new window
    webui_show(MyWindow, "index.html"); // webui_show_browser(MyWindow, "index.html", Chrome);

    // Wait until all windows get closed
    webui_wait();

    return 0;
}

#if defined(_MSC_VER)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
        return main();
    }
#endif
