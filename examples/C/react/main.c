// React Example

#include "webui.h"
#include "vfs.h"

void exit_app(webui_event_t* e) {
    webui_exit();
}

int main() {

    // Create new windows
    size_t react_window = webui_new_window();

    // Set window size
    webui_set_size(react_window, 550, 450);

    // Set window position
    webui_set_position(react_window, 250, 250);

    // Allow multi-user connection to WebUI window
    webui_set_config(multi_client, true);

    // Disable WebUI's cookies
    webui_set_config(use_cookies, false);

    // Bind React HTML element IDs with a C functions
    webui_bind(react_window, "Exit", exit_app);

    // VSF (Virtual File System) Example
    //
    // 1. Run Python script to generate header file of a folder
    //    python vfs.py "/path/to/folder" "vfs.h" "index.html"
    //
    // 2. Include header file in your C project
    //    #include "vfs.h"
    //
    // 3. use vfs in your custom files handler `webui_set_file_handler()`
    //    webui_set_file_handler(react_window, vfs);

    // Set a custom files handler
    webui_set_file_handler(react_window, vfs);

    // Show the React window
    // webui_show_browser(react_window, "index.html", Chrome);
    webui_show(react_window, "index.html");

    // Wait until all windows get closed
    webui_wait();

    // Free all memory resources (Optional)
    webui_clean();

    return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
