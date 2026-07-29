// WebUI C++ Starter Kit
//
// A minimal, professional template: a C++ backend + an embedded web frontend,
// compiled into a single portable executable.
//
//   src/main.cpp  Your backend logic (this file)
//   ui/           Your frontend - embedded into the executable at build time
//   src/vfs.h     Generated from ui/ by tools/vfs.c (do not edit)

#include <string>
#include "webui.hpp"
#include "vfs.h"

// The application: owns the WebUI window, serves the embedded UI, and
// exposes backend methods to JavaScript.
class App {
	public:

	App() {

		// Bind methods - each becomes a global async JavaScript function
		win.bind("greet", this, &App::greet);
		win.bind("minimize", this, &App::minimize);
		win.bind("maximize", this, &App::maximize);
		win.bind("close_win", this, &App::close_win);

		// All HTTP requests go through our handler (see http_handler below)
		win.set_file_handler(http_handler);

		// Icon.
		// Windows:
		//		icon.png does not exist, so WebUI will ignore it,
		//		Windows will use `favicon.ico` embedded in the executable.
		// Linux:
		//		icon.png should exist so WebUI can pass it to GTK.
		// macOS:
		//		the .app bundle provides the icon (see GNUmakefile).
		win.set_icon_file("icon.png");

		// Frameless window style
		win.set_size(800, 600);
		win.set_frameless(true);
		win.set_transparent(true);
		win.set_resizable(true);
		win.set_center();
	}

	int run() {

		// Show the window using the native WebView (WebView2 on Windows,
		// WKWebView on macOS, GTK WebKit on Linux). If no WebView is
		// available, fall back to showing it in a web browser (app mode).
		if (!win.show_wv("index.html")) {
			win.show("index.html");
		}

		// Wait until all windows are closed
		webui::wait();

		// Free all memory resources (optional)
		webui::clean();

		return 0;
	}

	private:

	// The WebUI window
	webui::window win;

	// Called from JavaScript: greet('name').then(...)
	void greet(webui::window::event* e) {
		std::string name = e->get_string();
		std::string reply = "Hello " + name +
			"! This reply was built in C++ (WebUI v" WEBUI_VERSION ")";
		e->return_string(reply);
	}

	// Window controls, called from the custom title bar (ui/index.html)
	void minimize(webui::window::event*) {
		win.minimize();
	}

	void maximize(webui::window::event*) {
		win.maximize();
	}

	void close_win(webui::window::event*) {
		win.close();
	}

	// Every HTTP request for the UI goes through this handler: embedded
	// files (the ui/ folder) are served by vfs() from the generated
	// src/vfs.h, and everything else gets a 404. Tweak requests, or add
	// your own routes and custom responses, here. Static because WebUI
	// takes a plain function pointer. Note: `webui.js` is always served
	// by WebUI itself and never reaches this handler.
	static const void* http_handler(const char* filename, int* length) {

		// Embedded UI files
		const void* response = vfs(filename, length);
		if (response != nullptr)
			return response;

		// Error 404
		const char* body = "<html><body><h1>404 - Not Found</h1></body></html>";
		const char* http_header_template = "HTTP/1.1 404 Not Found\r\n"
		                                   "Content-Type: text/html\r\n"
		                                   "Content-Length: %d\r\n\r\n";

		// Build the full HTTP response with header + body
		int body_length = (int)strlen(body);
		int header_length = snprintf(NULL, 0, http_header_template, body_length);
		*length = header_length + body_length;
		char* res = (char*)webui_malloc((size_t)*length + 1);
		snprintf(res, (size_t)header_length + 1, http_header_template, body_length);
		memcpy(res + header_length, body, (size_t)body_length);
		return res;
	}
};

int main() {
	App app;
	return app.run();
}

#if defined(_MSC_VER)
// Release builds use the WINDOWS subsystem (no console window)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
