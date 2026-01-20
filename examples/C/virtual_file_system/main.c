// Virtual File System Example

#include "webui.h"
#include "vfs.h"

void exit_app(webui_event_t* e) {
	webui_exit();
}

int main() {

	// Create new windows
	size_t MyWindow = webui_new_window();

	// Bind HTML element IDs with a C functions
	webui_bind(MyWindow, "Exit", exit_app);

	// VSF (Virtual File System) Example
	//
	// 1. Run Python script to generate header file of a folder
	//    python vfs.py "/path/to/folder" "vfs.h" "index.html"
	//
	// 2. Include header file in your C project
	//    #include "vfs.h"
	//
	// 3. use vfs in your custom files handler `webui_set_file_handler()`
	//    webui_set_file_handler(MyWindow, vfs);

	// Set a custom files handler
	webui_set_file_handler(MyWindow, vfs);

	// Show a new window
	// webui_show_browser(MyWindow, "index.html", Chrome);
	webui_show(MyWindow, "index.html");

	// Wait until all windows get closed
	webui_wait();

	// Free all memory resources (Optional)
	webui_clean();

	return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
