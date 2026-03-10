#include "webui.hpp"
#include "vfs.h"

// This example is used to verify redirect and fallback behavior for:
//   /, /sub, /sub/foo
// in both storage modes:
//   1) set_root_folder(...)
//   2) set_file_handler(vfs)
//
// Server mode is the primary redirect test path because it exposes the raw
// HTTP behavior (302 Location targets) without mixing in window lifecycle.
//
// The expected server behavior is:
//   - start_server("custom.html") prefers custom.html in every directory
//   - an explicit filename is strict; if it does not exist, the request fails
//   - start_server() or start_server("") means "fallback mode", which probes
//     index.html, index.htm, index.ts, then index.js
//
// Note: index.ts and index.js can still be valid fallback targets for
// runtime/script-first scenarios, but they are not equivalent to an HTML GUI
// entry page. For normal GUI testing, HTML/HTM are the page-like entries.
int main(int argc, char *argv[])
{
    bool serverMode = false;
    bool vfsEnabled = false;
    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]) == "--server")
        {
            serverMode = true;
        }
        else if (std::string(argv[i]) == "--vfs")
        {
            vfsEnabled = true;
        }
    }

    webui::window window_;
    webui::set_config(use_cookies, false);

    if (vfsEnabled)
    {
        window_.set_file_handler(vfs);
    }
    else
    {
        window_.set_root_folder("/home/gordon/Dokumenty/PlatformIO/Projects/webui/examples/C++/test_index_redirect/ui/");

        // Absolute paths:
        // window_.set_root_folder("/path/to/ui/");
        // window_.set_root_folder("/path/to/ui");

        // Relative if ui folder is next to executable:
        // window_.set_root_folder("ui/");
        // window_.set_root_folder("ui");
    }

    if (serverMode)
    {
        window_.set_port(8080);

        // Explicit custom entry file:
        window_.start_server("custom.html");

        // Redirect to the hardcoded index.* fallback list:
        // window_.start_server();
        
    }
    else
    {
        // Explicit custom entry file:
        window_.show("custom.html");
        // window_.show_browser("custom.html"); // AnyBrowser is the default second argument in C++.

        // Redirect to the hardcoded index.* fallback list:
        // window_.show();
        // window_.show_browser();
    }

    webui::wait();

    return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(__argc, __argv); }
#endif
