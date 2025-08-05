// Virtual File System Example C++

#include "webui.hpp"
#include "vfs.h"

void exit_app(webui::window::event* e) {
	webui::exit();
}

int main() {

	// Create new windows
	webui::window my_window;

	// Bind HTML element IDs with a C functions
	my_window.bind("Exit", exit_app);

	// VSF (Virtual File System) C++ Example
	//
	// 1. Run Python script to generate header file of a folder
	//    python vfs.py "/path/to/folder" "vfs.h"
	//
	// 2. Include header file in your C++ project
	//    #include "vfs.h"
	//
	// 3. use vfs in your custom files handler `webui::window::set_file_handler()`
	//    MyWindow.set_file_handler(vfs);

	// Set a custom files handler
	my_window.set_file_handler(vfs);

	// Show a new window
	// webui_show_browser(MyWindow, "index.html", Chrome);
	my_window.show("index.html");

	// Wait until all windows get closed
	webui::wait();

	// Free all memory resources (Optional)
	webui::clean();

	return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
