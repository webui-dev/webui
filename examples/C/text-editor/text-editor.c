// Text Editor in C using WebUI

#include "webui.h"

void Close(webui_event_t* e) {
    printf("Exit.\n");

    // Close all opened windows
    webui_exit();
}

int main() {

    // Create new windows
    int MainWindow = webui_new_window();

    // Bind HTML element IDs with a C functions
    webui_bind(MainWindow, "close-button", Close);

    // Show a new window
    webui_set_root_folder(MainWindow, "ui");
    if (!webui_show_browser(MainWindow, "MainWindow.html", ChromiumBased))
        webui_show(MainWindow, "MainWindow.html");

    // Wait until all windows get closed
    webui_wait();

    return 0;
}

#if defined(_MSC_VER)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
        return main();
    }
#endif
