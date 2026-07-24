// WebUI C Starter Kit
//
// A minimal C backend, WebView frontend,
// compiled into a single portable executable.
//
//   src/main.c   Your backend logic (this file)
//   ui/          Your frontend - embedded into the executable at build time
//   src/vfs.h    Generated from ui folder by the build system, do not edit.

#include <stdio.h>
#include <string.h>
#include "webui.h"
#include "vfs.h"

// Called from JavaScript: greet('name').then(...)
void greet(webui_event_t* e) {
	const char* name = webui_get_string(e);
	char reply[512];
	snprintf(reply, sizeof(reply),
		"Hello %s!, This reply was built in C (WebUI v" WEBUI_VERSION ")", name);
	webui_return_string(e, reply);
}

// Window controls, called from `ui/index.html`
void minimize(webui_event_t* e) {
	webui_minimize(e->window);
}

void maximize(webui_event_t* e) {
	webui_maximize(e->window);
}

void close_win(webui_event_t* e) {
	webui_close(e->window);
}

// Every HTTP request for the UI goes through this handler: embedded files
// are served by vfs() from the generated src/vfs.h, add your own routes and
// custom responses, here.
//
// Note: `webui.js` is virtual, and always served by WebUI and never reaches this handler.
const void* http_handler(const char* filename, int* length) {

	// Embedded files
	const void* response = vfs(filename, length);
	if (response != NULL)
		return response;

	// File not found in the virtual file system, return a 404 response.
	const char* body = "<html><body><h1>404 - Not Found</h1></body></html>";
	const char* http_header_template = "HTTP/1.1 404 Not Found\r\n"
	                                   "Content-Type: text/html\r\n"
	                                   "Content-Length: %d\r\n\r\n";
	
	// Build the full HTTP response (header + body)
	int body_length = (int)strlen(body);
	int header_length = snprintf(NULL, 0, http_header_template, body_length);
	*length = header_length + body_length;
	char* res = (char*)webui_malloc((size_t)*length + 1);
	snprintf(res, (size_t)header_length + 1, http_header_template, body_length);
	memcpy(res + header_length, body, (size_t)body_length);
	return res;
}

int main(void) {

	// Create a new window
	size_t win = webui_new_window();

	// Bind C functions - each becomes automatically a global async JavaScript function
	webui_bind(win, "greet", greet);
	webui_bind(win, "minimize", minimize);
	webui_bind(win, "maximize", maximize);
	webui_bind(win, "close_win", close_win);

	// All HTTP requests go through our handler (see http_handler above)
	webui_set_file_handler(win, http_handler);

	// Application icon
	#if defined(_WIN32) || defined(_WIN64)
	// Windows: use the embedded EXE `favicon.ico` as the taskbar icon.
	#elif defined(__linux__)
	// Linux: use the local `icon.png` file as the taskbar icon. 
	// Webui will pass the file to GTK.
	webui_set_icon_file(win, "icon.png");
	#else
	// macOS: the .app bundle provides the icon.
	#endif

	// Frameless window style
	webui_set_size(win, 800, 600);
	webui_set_frameless(win, true);
	webui_set_transparent(win, true);
	webui_set_resizable(win, true);
	webui_set_center(win);

	// Show the window using WebView. If no WebView is available,
	// fall back to showing it in a web browser (app mode).
	if (!webui_show_wv(win, "index.html")) {
		webui_show(win, "index.html");
	}

	// Wait until all windows are closed
	webui_wait();

	// Free all memory resources (optional)
	webui_clean();

	return 0;
}

#if defined(_MSC_VER)
// Release builds use the WINDOWS subsystem (no console window)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
