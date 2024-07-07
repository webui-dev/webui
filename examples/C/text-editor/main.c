// Text Editor in C using WebUI

#include "webui.h"

void close_app(webui_event_t* e) {
	printf("Exit.\n");

	// Close all opened windows
	webui_exit();
}

int main() {

	// Create a new window
	int MainWindow = webui_new_window();

	// Set the root folder for the UI
	webui_set_root_folder(MainWindow, "ui");

	// Bind HTML elements with the specified ID to C functions
	webui_bind(MainWindow, "close_app", close_app);

	// Show the window, preferably in a chromium based browser
	if (!webui_show_browser(MainWindow, "index.html", AnyBrowser))
		webui_show(MainWindow, "index.html");

	// Wait until all windows get closed
	webui_wait();

	// Free all memory resources (Optional)
	webui_clean();

	return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
