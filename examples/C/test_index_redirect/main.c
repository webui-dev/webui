#include <stdbool.h>
#include <string.h>

#include "webui.h"
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
//   - webui_start_server(window, "") means "fallback mode", which probes
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
        if (strcmp(argv[i], "--server") == 0)
        {
            serverMode = true;
        }
        else if (strcmp(argv[i], "--vfs") == 0)
        {
            vfsEnabled = true;
        }
    }

    size_t window = webui_new_window();
    webui_set_config(use_cookies, false);

    if (vfsEnabled)
    {
        webui_set_file_handler(window, vfs);
    }
    else
    {
        webui_set_root_folder(window, "/home/gordon/Dokumenty/PlatformIO/Projects/webui/examples/C/test_index_redirect/ui/");
        
        // Absolute paths:
        // webui_set_root_folder(window, "/path/to/ui/");
        // webui_set_root_folder(window, "/path/to/ui");

        // Relative if ui folder is next to executable:
        // webui_set_root_folder(window, "ui/");
        // webui_set_root_folder(window, "ui");
    }

    if (serverMode)
    {
        webui_set_port(window, 8080);

        // Explicit custom entry file:
        (void)webui_start_server(window, "custom.html");

        // Redirect to the hardcoded index.* fallback list:
        // (void)webui_start_server(window, "");
    }
    else
    {
        // Explicit custom entry file:
        webui_show(window, "custom.html");
        // webui_show_browser(window, "custom.html", AnyBrowser);

        // Redirect to the hardcoded index.* fallback list:
        // webui_show(window, "");
        // webui_show_browser(window, "", AnyBrowser);
    }

    webui_wait();
    return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(__argc, __argv); }
#endif
