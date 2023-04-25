#pragma once
/*
  WebUI Library 2.2.0
  http://webui.me
  https://github.com/alifcommunity/webui
  Copyright (c) 2020-2023 Hassan Draga.
  Licensed under GNU General Public License v2.0.
  All rights reserved.
  Canada.
*/

// This is a C++ wrapper for WebUI.

// C++ STD
#include <string>

// Include the original WebUI header (C99)
extern "C" {
	#include "webui.h"
}

namespace webui {

	// Create a new webui window object.
	void* new_window(void) {
		return webui_new_window();
	}

	// Bind a specific html element click event with a function. Empty element means all events.
	unsigned int bind(void* window, std::string element, void (*func)(webui_event_t* e)) {
		return webui_bind(window, element.c_str(), func);
	}

	// Show a window using a embedded HTML, or a file. If the window is already opened then it will be refreshed.
	bool show(void* window, std::string content) {
		return webui_show(window, content.c_str());
	}

	// Same as show(). But with a specific web browser.
	bool show_browser(void* window, std::string content, unsigned int browser) {
		return webui_show_browser(window, content.c_str(), browser);
	}

	// Wait until all opened windows get closed.
	void wait(void) {
		webui_wait();
	}

	// Close a specific window.
	void close(void* window) {
		webui_close(window);
	}

	// Close all opened windows. wait() will break.
	void exit(void) {
		webui_exit();
	}

	// -- Other ---------------------------
	bool is_shown(void* window) {
		return webui_is_shown(window);
	}

	void set_timeout(unsigned int second) {
		webui_set_timeout(second);
	}

	void set_icon(void* window, std::string icon, std::string type) {
		webui_set_icon(window, icon.c_str(), type.c_str());
	}

	void set_multi_access(void* window, bool status) {
		webui_set_multi_access(window, status);
	}

	// -- JavaScript ----------------------
	// Quickly run a JavaScript (no response waiting).
	bool run(void* window, std::string script) {
		return webui_run(window, script.c_str());
	}

	// Run a JavaScript, and get the response back (Make sure your local buffer can hold the response).
	bool script(void* window, std::string script, unsigned int timeout, char* buffer, size_t buffer_length) {
		return webui_script(window, script.c_str(), timeout, buffer, buffer_length);
	}

	// Chose between Deno and Nodejs runtime for .js and .ts files.
	void set_runtime(void* window, unsigned int runtime) {
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
	unsigned int interface_bind(void* window, std::string element, void (*func)(void*, unsigned int, char*, char*, char*)) {
		return webui_interface_bind(window, element.c_str(), func);
	}

	// When using `interface_bind()` you need this function to easily set your callback response.
	void interface_set_response(std::string ptr, std::string response) {
		webui_interface_set_response(&ptr[0], response.c_str());
	}

	// Check if the app still running or not. This replace wait().
	bool interface_is_app_running(void) {
		return webui_interface_is_app_running();
	}

	// Get window unique ID
	unsigned int interface_get_window_id(void* window) {
		return webui_interface_get_window_id(window);
	}
}
