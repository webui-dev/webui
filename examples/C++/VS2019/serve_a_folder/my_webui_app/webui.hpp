/*
  WebUI Library 2.3.0
  http://webui.me
  https://github.com/alifcommunity/webui
  Copyright (c) 2020-2023 Hassan Draga.
  Licensed under MIT License.
  All rights reserved.
  Canada.
*/

#ifndef _WEBUI_HPP
#define _WEBUI_HPP

// C++ STD
#include <string>

// WebUI C Header
extern "C" {
	#include "webui.h"
}

namespace webui {

	// Create a new webui window object.
	size_t new_window(void) {
		return webui_new_window();
	}

	// Bind a specific html element click event with a function. Empty element means all events.
	unsigned int bind(size_t window, std::string element, void (*func)(webui_event_t* e)) {
		return webui_bind(window, element.c_str(), func);
	}

	// Show a window using a embedded HTML, or a file. If the window is already opened then it will be refreshed.
	bool show(size_t window, std::string content) {
		return webui_show(window, content.c_str());
	}

	// Same as show(). But with a specific web browser.
	bool show_browser(size_t window, std::string content, unsigned int browser) {
		return webui_show_browser(window, content.c_str(), browser);
	}

	// Wait until all opened windows get closed.
	void wait(void) {
		webui_wait();
	}

	// Close a specific window.
	void close(size_t window) {
		webui_close(window);
	}

	// Set the window in Kiosk mode (Full screen)
	void set_kiosk(size_t window, bool status) {
		webui_set_kiosk(window, status);
	}

	// Close all opened windows. wait() will break.
	void exit(void) {
		webui_exit();
	}

	// -- Other ---------------------------
	// Check a specific window if it's still running
	bool is_shown(size_t window) {
		return webui_is_shown(window);
	}

	// Set the maximum time in seconds to wait for browser to start
	void set_timeout(unsigned int second) {
		webui_set_timeout(second);
	}

	// Set the default embedded HTML favicon
	void set_icon(size_t window, std::string icon, std::string icon_type) {
		webui_set_icon(window, icon.c_str(), icon_type.c_str());
	}

	// Allow the window URL to be re-used in normal web browsers
	void set_multi_access(size_t window, bool status) {
		webui_set_multi_access(window, status);
	}

	// -- JavaScript ----------------------
	// Quickly run a JavaScript (no response waiting).
	bool run(size_t window, std::string script) {
		return webui_run(window, script.c_str());
	}

	// Run a JavaScript, and get the response back (Make sure your local buffer can hold the response).
	bool script(size_t window, std::string script, unsigned int timeout, char* buffer, size_t buffer_length) {
		return webui_script(window, script.c_str(), timeout, buffer, buffer_length);
	}

	// Chose between Deno and Nodejs runtime for .js and .ts files.
	void set_runtime(size_t window, unsigned int runtime) {
		webui_set_runtime(window, runtime);
	}

	// Parse argument as integer.
	long long int get_int(webui_event_t* e) {
		return webui_get_int(e);
	}

	// Parse argument as string.
	std::string get_string(webui_event_t* e) {
		return std::string(webui_get_string(e));
	}

	// Parse argument as boolean.
	bool get_bool(webui_event_t* e) {
		return webui_get_bool(e);
	}

	// Return the response to JavaScript as integer.
	void return_int(webui_event_t* e, long long int n) {
		webui_return_int(e, n);
	}

	// Return the response to JavaScript as string.
	void return_string(webui_event_t* e, std::string s) {
		webui_return_string(e, &s[0]);
	}

	// Return the response to JavaScript as boolean.
	void return_bool(webui_event_t* e, bool b) {
		webui_return_bool(e, b);
	}

	// -- Interface -----------------------
	// Bind a specific html element click event with a function. Empty element means all events. This replace bind(). The func is (Window, EventType, Element, Data, Response)
	unsigned int interface_bind(size_t window, std::string element, void (*func)(size_t, unsigned int, char*, char*, unsigned int)) {
		return webui_interface_bind(window, element.c_str(), func);
	}

	// When using `interface_bind()` you need this function to easily set your callback response.
	void interface_set_response(size_t window, webui_event_t* e, std::string response) {
		webui_interface_set_response(window, e->event_number, response.c_str());
	}

	// Check if the app still running or not. This replace wait().
	bool interface_is_app_running(void) {
		return webui_interface_is_app_running();
	}

	// Get window unique ID
	unsigned int interface_get_window_id(size_t window) {
		return webui_interface_get_window_id(window);
	}
}

#endif /* _WEBUI_HPP */
