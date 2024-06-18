// Serve a Folder Example

#include "webui.h"

void events(webui_event_t* e) {

	// This function gets called every time
	// there is an event

	if (e->event_type == WEBUI_EVENT_CONNECTED)
		printf("Connected. \n");
	else if (e->event_type == WEBUI_EVENT_DISCONNECTED)
		printf("Disconnected. \n");
	else if (e->event_type == WEBUI_EVENT_MOUSE_CLICK)
		printf("Click. \n");
	else if (e->event_type == WEBUI_EVENT_NAVIGATION) {
		const char* url = webui_get_string(e);
		printf("Starting navigation to: %s \n", url);

		// Because we used `webui_bind(MyWindow, "", events);`
		// WebUI will block all `href` link clicks and sent here instead.
		// We can then control the behaviour of links as needed.
		webui_navigate(e->window, url);
	}
}

void my_backend_func(webui_event_t* e) {

	// JavaScript:
	// my_backend_func(123, 456, 789);
	// or webui.my_backend_func(...);

	long long number_1 = webui_get_int_at(e, 0);
	long long number_2 = webui_get_int_at(e, 1);
	long long number_3 = webui_get_int_at(e, 2);

	printf("my_backend_func 1: %lld\n", number_1); // 123
	printf("my_backend_func 2: %lld\n", number_2); // 456
	printf("my_backend_func 3: %lld\n", number_3); // 789
}

int main() {

	// Create new windows
	size_t window = webui_new_window();

	// Bind all events
	webui_bind(window, "", events);

	// Bind HTML elements with C functions
	webui_bind(window, "my_backend_func", my_backend_func);

	// Set the web-server/WebSocket port that WebUI should
	// use. This means `webui.js` will be available at:
	// http://localhost:MY_PORT_NUMBER/webui.js
	webui_set_port(window, 8081);

	// Show a new window and show our custom web server
	// Assuming the custom web server is running on port
	// 8080...
	webui_show(window, "http://localhost:8080/");

	// Wait until all windows get closed
	webui_wait();

	// Free all memory resources (Optional)
	webui_clean();

	return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
