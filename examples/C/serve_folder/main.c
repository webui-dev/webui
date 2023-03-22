
/*
    WebUI Library 2.0.7

	Serve a Folder Example
    
    http://webui.me
    https://github.com/alifcommunity/webui

    Licensed under GNU General Public License v3.0.
    Copyright (C)2023 Hassan DRAGA <https://github.com/hassandraga> - Canada.
*/

// Note:
// To see the console logs (printf) you need to build in debug mode
// example 'make debug', or 'nmake debug', or 'mingw32-make debug'

#include "webui.h"

void switch_to_second_page(webui_event_t* e) {

	// This function get called every time 
	// the user click on "SwitchToSecondPage" button
    webui_open(e->window, "second.html", webui.browser.any);
}

void exit_app(webui_event_t* e) {

    webui_exit();
}

int main() {

	// Create a new window
	webui_window_t* my_window;
	my_window = webui_new_window();

	// Bind am HTML element ID with a C function
	webui_bind(my_window, "SwitchToSecondPage", switch_to_second_page);
	webui_bind(my_window, "Exit", exit_app);

	// The root path. Leave it empty to let the WebUI 
	// automatically select the current working folder
	const char* root_path = "";

	// Create a new web server using WebUI
	const char* url = webui_new_server(my_window, root_path);

    // Show the window using the generated URL
	if(!webui_open(my_window, url, webui.browser.chrome))	// Run the window on Chrome
		webui_open(my_window, url, webui.browser.any);		// If not, run on any other installed web browser

    // Wait until all windows get closed
	webui_wait();

    return 0;
}

#if defined(_MSC_VER)
	int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
		main();
	}
#endif
