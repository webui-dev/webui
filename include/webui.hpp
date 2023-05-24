/*
  WebUI Library 2.3.0
  http://webui.me
  https://github.com/webui-dev/webui
  Copyright (c) 2020-2023 Hassan Draga.
  Licensed under MIT License.
  All rights reserved.
  Canada.
*/

#ifndef _WEBUI_HPP
#define _WEBUI_HPP

// C++ STD
#include <string>
#include <string_view>
#include <array>

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

    class window {
    private:
        size_t webui_window {webui_new_window()};

    public:
    // Event struct
        struct event : public webui_event_t{
            using webui_event_t::webui_event_t;
            webui::window& window; // The window object

            // Window object constructor that
            // initializes the reference, This
            // is to avoid creating copies.
            event(webui::window& window_obj, webui_event_t c_e = {}) : webui_event_t(c_e),
                window(window_obj) {
                    reinterpret_cast<webui_event_t*>(this)->window = window_obj.webui_window;
                }

            class handler{
                public:
                using callback_t = void(*)(event*);

                private:
                static inline std::array<callback_t, 512> callback_list{};

                // List of window objects: webui::window
                static inline std::array<webui::window*, 512> window_list{};

                public:
                handler() = delete;
                handler(const handler&) = delete;
                handler(handler&&) = delete;
                handler& operator=(const handler&) = delete;
                handler& operator=(handler&&) = delete;
                ~handler() = delete;

                static void add(size_t id, webui::window* win, callback_t func){
                    window_list[id] = win;

                    // Save callback
                    callback_list[id] = func;
                }

                static void handle(webui_event_t* c_e) {

                    // Get a unique ID. Same ID as `webui_bind()`. Return > 0 if bind exist.
                    const size_t id = webui_interface_get_bind_id(c_e->window, c_e->element);

                    if(id < 1){
                        return;
                    }

                    // Create a new event struct
                    event e(*window_list[id], *c_e);

                    // Call the user callback
                    if(callback_list[id] != nullptr)
                        callback_list[id](&e);
                }
            };

            // Parse argument as integer.
            long long int get_int()  {
                return webui_get_int(this);
            }

            // Parse argument as string.
            std::string get_string()  {
                return std::string{webui_get_string(this)};
            }

            // Parse argument as boolean.
            bool get_bool()  {
                return webui_get_bool(this);
            }

            // Return the response to JavaScript as integer.
            void return_int(long long int n)  {
                webui_return_int(this, n);
            }

            // Return the response to JavaScript as string.
            void return_string(const std::string_view s)  {
                webui_return_string(this, s.data());
            }

            // Return the response to JavaScript as boolean.
            void return_bool(bool b)  {
                webui_return_bool(this, b);
            }
        };

        // Bind a specific html element click event with a function. Empty element means all events.
        void bind(const std::string_view element, event::handler::callback_t func) {

            // Get unique ID
            const size_t id = webui_bind(webui_window, element.data(), event::handler::handle);

            event::handler::add(id, this, func);
        }

        // Show a window using a embedded HTML, or a file. If the window is already opened then it will be refreshed.
        bool show(const std::string_view content) const {
            return webui_show(webui_window, content.data());
        }

        // Same as show(). But with a specific web browser.
        bool show_browser(const std::string_view content, unsigned int browser) const {
            return webui_show_browser(webui_window, content.data(), browser);
        }

        // Close a specific window.
        void close() const {
            webui_close(webui_window);
        }

        // Set the window in Kiosk mode (Full screen)
        void set_kiosk(bool status) const {
            webui_set_kiosk(webui_window, status);
        }

        // -- Other ---------------------------
        // Check a specific window if it's still running
        bool is_shown() const {
            return webui_is_shown(webui_window);
        }

        // Set the default embedded HTML favicon
        void set_icon(const std::string_view icon, const std::string& icon_type) const {
            webui_set_icon(webui_window, icon.data(), icon_type.c_str());
        }

        // Allow the window URL to be re-used in normal web browsers
        void set_multi_access(bool status) const {
            webui_set_multi_access(webui_window, status);
        }

        // -- JavaScript ----------------------
        // Quickly run a JavaScript (no response waiting).
        void run(const std::string_view script) const {
            webui_run(webui_window, script.data());
        }

        // Run a JavaScript, and get the response back (Make sure your local buffer can hold the response).
        bool script(const std::string_view script, unsigned int timeout, char* buffer, size_t buffer_length) const {
            return webui_script(webui_window, script.data(), timeout, buffer, buffer_length);
        }

        // Chose between Deno and Nodejs runtime for .js and .ts files.
        void set_runtime(unsigned int runtime) const {
            webui_set_runtime(webui_window, runtime);
        }
    };

    // Wait until all opened windows get closed.
    inline void wait() {
        webui_wait();
    }

    // Close all opened windows. wait() will break.
    inline void exit(){
        webui_exit();
    }

    // Set the maximum time in seconds to wait for browser to start
    inline void set_timeout(unsigned int second) {
        webui_set_timeout(second);
    }

    // Base64 encoding. Use this to safely send text based data to the UI. If it fails it will return NULL.
    inline std::string encode(const std::string_view str) {
        std::string ret = std::string(webui_encode(str.data()));
        return ret;
    }

    // Base64 decoding. Use this to safely decode received Base64 text from the UI. If it fails it will return NULL.
    inline std::string decode(const std::string_view str) {
        std::string ret = std::string(webui_decode(str.data()));
        return ret;
    }

    // Safely free a buffer allocated by WebUI, for example when using webui_encode().
    inline void free(void* ptr) {
        webui_free(ptr);
    }
}

#endif /* _WEBUI_HPP */
