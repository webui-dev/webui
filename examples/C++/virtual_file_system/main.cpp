// Virtual File System Example C++

#include "webui.hpp"
#include "vfs.h"

class VirtualFileSystemApp {
public:
    VirtualFileSystemApp() {

        // Bind the "Exit" button in the HTML to the exit_app() method in C++
        win.bind("Exit", this, &VirtualFileSystemApp::exit_app);

        // VSF (Virtual File System) C++ Example
        //
        // 1. Run Python script to generate header file of a folder
        //    python vfs.py "/path/to/folder" "vfs.h"
        //
        // 2. Include header file in your C++ project
        //    #include "vfs.h"
        //
        // 3. Use vfs in your custom files handler `webui::window::set_file_handler()`
        //    win.set_file_handler(vfs);

        win.set_file_handler(vfs);
    }

    void run() {
        // Show the main page (index.html) in the WebView/Browser window
        win.show("index.html");
            // win.show_browser("index.html", webui_browser::Chrome);
            // win.show_wv("index.html");

        // Wait until the window gets closed
        webui::wait();

        // Clean up WebUI resources. This is optional, but recommended.
        webui::clean();
    }

private:
    webui::window win;

    void exit_app(webui::window::event* e) {
        // Exit the application, and close all windows. `webui::wait()` will return (Break).
        webui::exit();
    }
};

int main() {
    VirtualFileSystemApp app;
    app.run();
    return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
