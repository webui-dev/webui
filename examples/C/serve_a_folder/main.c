// Serve a Folder Example

#include "webui.h"

void* my_window;
void* my_second_window;

void exit_app(webui_event_t* e) {

    // Close all opened windows
    webui_exit();
}

void events(webui_event_t* e) {

    // This function gets called every time
    // there is an event

    if(e->type == WEBUI_EVENT_CONNECTED)
        printf("Connected. \n");
    else if(e->type == WEBUI_EVENT_DISCONNECTED)
        printf("Disconnected. \n");
    else if(e->type == WEBUI_EVENT_MOUSE_CLICK)
        printf("Click. \n");
    else if(e->type == WEBUI_EVENT_NAVIGATION)
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
    webui_show(my_second_window, "second.html");
}

int main() {

    // Create new windows
    my_window = webui_new_window();
    my_second_window = webui_new_window();

    // Bind HTML element IDs with a C functions
    webui_bind(my_window, "SwitchToSecondPage", switch_to_second_page);
    webui_bind(my_window, "OpenNewWindow", show_second_window);
    webui_bind(my_window, "Exit", exit_app);
    webui_bind(my_second_window, "Exit", exit_app);

    // Bind events
    webui_bind(my_window, "", events);

    // Make Deno as the `.ts` and `.js` interpreter
    webui_set_runtime(my_window, Deno);

    // Show a new window
    webui_show(my_window, "index.html"); // webui_show_browser(my_window, "index.html", Chrome);

    // Wait until all windows get closed
    webui_wait();

    return 0;
}

#if defined(_MSC_VER)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
        return main();
    }
#endif
