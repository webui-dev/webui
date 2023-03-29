


#include "webui.h"

int main(int argc, char* argv[]) {

	// argc should be exactly 2
	if(argc != 2)
		return 1;
	
	// Get the Nodejs server url
	const char *url = argv[1];

	// Create a new window
	webui_window_t* my_window;
	my_window = webui_new_window();

    // Show the window
	if(!webui_open(my_window, url, webui.browser.chrome))	// Run the window on Chrome
		webui_open(my_window, url, webui.browser.any);		// If not, run on any other installed web browser

    // Wait until all windows get closed
	webui_wait();

    return 0;
}
