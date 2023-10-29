/*
  WebUI Library 2.4.0-Beta
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
#include <array>
#include <string>
#include <string_view>

// WebUI C Header
extern "C" {
#include "webui.h"
}

namespace webui {

static constexpr int DISCONNECTED = 0; // 0. Window disconnection event
static constexpr int CONNECTED = 1;    // 1. Window connection event
static constexpr int MOUSE_CLICK = 2;  // 2. Mouse click event
static constexpr int NAVIGATION = 3;   // 3. Window navigation event
static constexpr int CALLBACKS = 4;    // 4. Function call event

class window {
    private:
    size_t webui_window{webui_new_window()};

    public:
    // Event Struct
    class event : public webui_event_t {
        // Window object constructor that
        // initializes the reference, This
        // is to avoid creating copies.
        event(webui::window& window_obj, webui_event_t c_e) : webui_event_t(c_e) {

            reinterpret_cast<webui_event_t*>(this)->window = window_obj.webui_window;
        }

        public:
        class handler {

            public:
            using callback_t = void (*)(event*);

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

            static void add(size_t id, webui::window* win, callback_t func) {
                // Save window object
                window_list[id] = win;
                // Save callback
                callback_list[id] = func;
            }

            static void handle(webui_event_t* c_e) {
                // Get the binded unique ID
                const size_t id = c_e->bind_id;
                if (id > 0) {
                    // Create a new event struct
                    event e(*window_list[id], *c_e);
                    // Call the user callback
                    if (callback_list[id] != nullptr)
                        callback_list[id](&e);
                }
            }

            static webui::window& get_window(const size_t index) { return *window_list[index]; }
        };

        // Get an argument as integer at a specific index.
        long long int get_int(size_t index = 0) { return webui_get_int_at(this, index); }

        // Get the size in bytes of an argument at a specific index.
        size_t get_size(size_t index = 0) { return webui_get_size_at(this, index); }

        // Get an argument as string at a specific index.
        std::string get_string(size_t index = 0) { return std::string{webui_get_string_at(this, index)}; }

        // Get an argument as string_view at a specific index.
        std::string_view get_string_view(size_t index = 0) {
            return std::string_view{webui_get_string_at(this, index)};
        }

        // Get an argument as boolean at a specific index.
        bool get_bool(size_t index = 0) { return webui_get_bool_at(this, index); }

        // Return the response to JavaScript as integer.
        void return_int(long long int n) { webui_return_int(this, n); }

        // Return the response to JavaScript as string.
        void return_string(const std::string_view s) { webui_return_string(this, s.data()); }

        // Return the response to JavaScript as boolean.
        void return_bool(bool b) { webui_return_bool(this, b); }

        webui::window& get_window() { return event::handler::get_window(window); }

        size_t get_type() const { return event_type; }

        std::string_view get_element() const { return std::string_view{element}; }

        size_t number() const { return event_number; }
    };

    // Bind a specific html element click event with a function. Empty element means all events.
    void bind(const std::string_view element, event::handler::callback_t func) {
        // Get unique ID
        const size_t id = webui_bind(webui_window, element.data(), event::handler::handle);
        event::handler::add(id, this, func);
    }

    // Show a window using a embedded HTML, or a file. If the window is already opened then it will be
    // refreshed.
    bool show(const std::string_view content) const { return webui_show(webui_window, content.data()); }

    // Same as show(). But with a specific web browser.
    bool show_browser(const std::string_view content, unsigned int browser) const {
        return webui_show_browser(webui_window, content.data(), browser);
    }

    // Set the window in Kiosk mode (Full screen)
    void set_kiosk(bool status) const { webui_set_kiosk(webui_window, status); }

    // Close a specific window only. The window object will still exist.
    void close() const { webui_close(webui_window); }

    // Close a specific window and free all memory resources.
    void destroy() const { webui_destroy(webui_window); }

    // Check a specific window if it's still running
    bool is_shown() const { return webui_is_shown(webui_window); }

    // Set the default embedded HTML favicon
    void set_icon(const std::string_view icon, const std::string_view icon_type) const {
        webui_set_icon(webui_window, icon.data(), icon_type.data());
    }

    // Safely send raw data to the UI
    void send_raw(const std::string_view function, const void* raw, size_t size) const {
        webui_send_raw(webui_window, function.data(), raw, size);
    }

    // Run the window in hidden mode
    void set_hide(bool status) const { webui_set_hide(webui_window, status); }

    // Set window size
    void set_size(unsigned int width, unsigned int height) const { webui_set_size(webui_window, width, height); }

    // Set a custom web-server network port to be used by WebUI. This can be useful to determine the HTTP link of `webui.js`
    // in case you are trying to use WebUI with an external web-server like NGNIX
    void set_size(size_t port) const { webui_set_port(webui_window, port); }

    // Set window position
    void set_position(unsigned int x, unsigned int y) const { webui_set_position(webui_window, x, y); }

    // Delete a specific window web-browser local folder profile.
    void webui_delete_profile(size_t window) const { webui_delete_profile(webui_window); }

    // Get the ID of the parent process (The web browser may create another process for the window).
    size_t get_parent_process_id() const { return webui_get_parent_process_id(webui_window); }

    // Get the ID of the last child process spawned by the browser.
    size_t get_child_process_id() const { return webui_get_child_process_id(webui_window); }

    // Set the web-server root folder path for this specific window.
    bool set_root_folder(const std::string_view path) const {
        return webui_set_root_folder(webui_window, path.data());
    }

    // Set a custom handler to serve files.
    void set_file_handler(const void* (*handler)(const char* filename, int* length)) const {
        webui_set_file_handler(webui_window, handler);
    }

    // Set the web browser profile to use. An empty `name` and `path` means the default user profile. Need
    // to be called before `webui_show()`.
    void set_profile(const std::string_view name = {""}, const std::string_view path = {""}) const {
        webui_set_profile(webui_window, name.data(), path.data());
    }

    // Get the full current URL
    std::string_view get_url() const { return std::string_view{webui_get_url(webui_window)}; }

    // Navigate to a specific URL.
    void navigate(const std::string_view url) const { webui_navigate(webui_window, url.data()); }

    // -- JavaScript ----------------------

    // Quickly run a JavaScript (no response waiting).
    void run(const std::string_view script) const { webui_run(webui_window, script.data()); }

    // Run a JavaScript, and get the response back (Make sure your local buffer can hold the response).
    bool script(const std::string_view script, unsigned int timeout, char* buffer, size_t buffer_length) const {
        return webui_script(webui_window, script.data(), timeout, buffer, buffer_length);
    }

    // Chose between Deno and Nodejs runtime for .js and .ts files.
    void set_runtime(unsigned int runtime) const { webui_set_runtime(webui_window, runtime); }
};

// Wait until all opened windows get closed.
inline void wait() { webui_wait(); }

// Close all opened windows. wait() will break.
inline void exit() { webui_exit(); }

// Set the web-server root folder path for all windows.
inline bool set_default_root_folder(const std::string_view path) { return webui_set_default_root_folder(path.data()); }

// Set the maximum time in seconds to wait for browser to start
inline void set_timeout(unsigned int second) { webui_set_timeout(second); }

// Base64 encoding. Use this to safely send text based data to the UI. If it fails it will return NULL.
inline std::string encode(const std::string_view str) { return std::string{webui_encode(str.data())}; }

// Base64 decoding. Use this to safely decode received Base64 text from the UI. If it fails it will return NULL.
inline std::string decode(const std::string_view str) { return std::string{webui_decode(str.data())}; }

// Set the SSL/TLS certificate and the private key content, both in PEM format.
// This works only with `webui-2-secure` library. If set empty WebUI will generate a self-signed certificate.
inline void set_tls_certificate(const std::string_view certificate_pem, const std::string_view private_key_pem) {
    webui_set_tls_certificate(certificate_pem.data(), private_key_pem.data()); }

// Safely free a buffer allocated by WebUI, for example when using webui_encode().
inline void free(void* ptr) { webui_free(ptr); }

// Safely free a buffer allocated by WebUI, for example when using webui_encode().
inline void* malloc(size_t size) { return webui_malloc(size); }

// Free all memory resources. Should be called only at the end.
inline void clean() { webui_clean(); }

// Delete all local web-browser profiles folder. It should called at the end.
inline void delete_all_profiles() { webui_delete_all_profiles(); }
} // namespace webui

#endif /* _WEBUI_HPP */
