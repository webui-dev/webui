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

    const int DISCONNECTED = 0; // 0. Window disconnection event
    const int CONNECTED = 1; // 1. Window connection event
    const int MULTI_CONNECTION = 2; // 2. New window connection event
    const int UNWANTED_CONNECTION = 3; // 3. New unwanted window connection event
    const int MOUSE_CLICK = 4; // 4. Mouse click event
    const int NAVIGATION = 5; // 5. Window navigation event
    const int CALLBACKS = 6; // 6. Function call event

    class window;

    // Event struct
    struct event {
        webui::window& window; // The window object
        unsigned int event_type; // Event type
        std::string element; // HTML element ID
        std::string data; // JavaScript data
        unsigned int event_number; // Internal WebUI

        // Window object constructor that
        // initializes the reference, This
        // is to avoid creating copies.
        event(webui::window& window_obj) : window(window_obj) {}
    };

    // List of callbacks: user_function(webui::event* e)
    void (*callback_list[512])(webui::event*);

    // List of window objects: webui::window
    webui::window* window_list[512];

    // Event handler
    // WebUI is written in C. So there is no way
    // to make C call a C++ class member. That's
    // why this function should be outside class
    void event_handler(webui_event_t* c_e) {

        // Get a unique ID. Same ID as `webui_bind()`. Return > 0 if bind exist.
        unsigned int id = webui_interface_get_bind_id(c_e->window, c_e->element);

        if(id < 1)
            return;

        // Create a new event struct
        webui::event e(*window_list[id]);
        // `e.window` is already initialized by `e` constructor
        e.event_type = c_e->event_type;
        e.element = (c_e->element != NULL ? std::string(c_e->element) : std::string(""));
        e.data = (c_e->data != NULL ? std::string(c_e->data) : std::string(""));
        e.event_number = c_e->event_number;

        // Call the user callback
        if(callback_list[id] != NULL)
            callback_list[id](&e);
    }

    class window {
    private:
        size_t webui_window = 0;

        webui_event_t* convert_event_to_webui_event(webui::event* e) {
            // Convert C++ `webui::event` to C `webui_event_t`
            webui_event_t* c_e = new webui_event_t;
            c_e->window = this->webui_window;
            c_e->event_type = e->event_type;
            c_e->element = (char*)e->element.c_str();
            c_e->data = (char*)e->data.c_str();
            c_e->event_number = e->event_number;
            return c_e;
        }
    public:
        // Constructor
        window() {
            this->webui_window = webui_new_window();
        }

        // Destructor
        ~window() {
            // Nothing to do.
            // No needs to call `webui_close()`
        }

        // Bind a specific html element click event with a function. Empty element means all events.
        void bind(std::string element, void (*func)(webui::event* e)) {

            // Get unique ID
            unsigned int id = webui_bind(this->webui_window, element.c_str(), webui::event_handler);

            // Save window object
            window_list[id] = this;

            // Save callback
            callback_list[id] = func;
        }

        // Show a window using a embedded HTML, or a file. If the window is already opened then it will be refreshed.
        bool show(std::string content) {
            return webui_show(this->webui_window, content.c_str());
        }

        // Same as show(). But with a specific web browser.
        bool show_browser(std::string content, unsigned int browser) {
            return webui_show_browser(this->webui_window, content.c_str(), browser);
        }

        // Close a specific window.
        void close() {
            webui_close(this->webui_window);
        }

        // Set the window in Kiosk mode (Full screen)
        void set_kiosk(bool status) {
            webui_set_kiosk(this->webui_window, status);
        }

        // -- Other ---------------------------
        // Check a specific window if it's still running
        bool is_shown() {
            return webui_is_shown(this->webui_window);
        }

        // Set the default embedded HTML favicon
        void set_icon(std::string icon, std::string icon_type) {
            webui_set_icon(this->webui_window, icon.c_str(), icon_type.c_str());
        }

        // Allow the window URL to be re-used in normal web browsers
        void set_multi_access(bool status) {
            webui_set_multi_access(this->webui_window, status);
        }

        // -- JavaScript ----------------------
        // Quickly run a JavaScript (no response waiting).
        bool run(std::string script) {
            return webui_run(this->webui_window, script.c_str());
        }

        // Run a JavaScript, and get the response back (Make sure your local buffer can hold the response).
        bool script(std::string script, unsigned int timeout, char* buffer, size_t buffer_length) {
            return webui_script(this->webui_window, script.c_str(), timeout, buffer, buffer_length);
        }

        // Chose between Deno and Nodejs runtime for .js and .ts files.
        void set_runtime(unsigned int runtime) {
            webui_set_runtime(this->webui_window, runtime);
        }

        // Parse argument as integer.
        long long int get_int(webui::event* e) {
            webui_event_t* c_e = convert_event_to_webui_event(e);
            long long int ret = webui_get_int(c_e);
            delete c_e;
            return ret;
        }

        // Parse argument as string.
        std::string get_string(webui::event* e) {
            webui_event_t* c_e = convert_event_to_webui_event(e);
            std::string ret = std::string(webui_get_string(c_e));
            delete c_e;
            return ret;
        }

        // Parse argument as boolean.
        bool get_bool(webui::event* e) {
            webui_event_t* c_e = convert_event_to_webui_event(e);
            bool ret = webui_get_bool(c_e);
            delete c_e;
            return ret;
        }

        // Return the response to JavaScript as integer.
        void return_int(webui::event* e, long long int n) {
            webui_event_t* c_e = convert_event_to_webui_event(e);
            webui_return_int(c_e, n);
            delete c_e;
        }

        // Return the response to JavaScript as string.
        void return_string(webui::event* e, std::string s) {
            webui_event_t* c_e = convert_event_to_webui_event(e);
            webui_return_string(c_e, &s[0]);
            delete c_e;
        }

        // Return the response to JavaScript as boolean.
        void return_bool(webui::event* e, bool b) {
            webui_event_t* c_e = convert_event_to_webui_event(e);
            webui_return_bool(c_e, b);
            delete c_e;
        }
    };

    // Wait until all opened windows get closed.
    void wait(void) {
        webui_wait();
    }

    // Close all opened windows. wait() will break.
    void exit(void) {
        webui_exit();
    }

    // Set the maximum time in seconds to wait for browser to start
    void set_timeout(unsigned int second) {
        webui_set_timeout(second);
    }
}

#endif /* _WEBUI_HPP */
