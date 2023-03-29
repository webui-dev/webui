


#include "webui.h"

int main() {

	// Create a new window
	webui_window_t* my_window;
	my_window = webui_new_window();

	// Chose your preferable runtime for .js files
	// Deno: webui.runtime.deno
	// Node.js: webui.runtime.nodejs
	webui_script_runtime(my_window, webui.runtime.nodejs);

	// Create a new web server using WebUI
	const char* url = webui_new_server(my_window, "");

    // Show the window
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
