
/*
    WebUI Library 2.x

	Serve a Folder Example
    
    http://webui.me
    https://github.com/alifcommunity/webui

    Licensed under GNU General Public License v3.0.
    Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.
*/

#include "webui.h"

void all_clicks(webui_event_t* e) {
    printf("You clicked on '%s' element.\n", e->element_name);
}

int main() {

	// Crate new window
	webui_window_t* my_window;
	my_window = webui_new_window();

	// Bind all clicks
	webui_bind_all(my_window, all_clicks);

	// Create a new web server using WebUI
	const char* url = webui_new_server(my_window, "");

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
