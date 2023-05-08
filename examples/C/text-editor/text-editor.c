// Text Editor in C using WebUI

#include "webui.h"

#include <stdio.h>
#include <stdlib.h>

void Close(webui_event_t* e) {
    // Close all opened windows
    printf("Exit.\n");
    webui_exit();
}

void Save(webui_event_t* e) {
    // Save data received from the UI
    printf("Save.\n");
}

void Open(webui_event_t* e) {
    // Open a new file
    printf("Open file.\n");
}

int main() {

    // Create new windows
    int MainWindow = webui_new_window();

    // Bind HTML element IDs with a C functions
    webui_bind(MainWindow, "Open", Open);
    webui_bind(MainWindow, "Save", Save);
    webui_bind(MainWindow, "Close", Close);

    // Show a new window
    webui_show(MainWindow, "ui/MainWindow.html");

    // Wait until all windows get closed
    webui_wait();

    return 0;
}

#if defined(_MSC_VER)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
        return main();
    }
#endif
