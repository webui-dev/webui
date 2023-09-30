/*
  WebUI Library 2.4.0
  http://webui.me
  https://github.com/webui-dev/webui
  Copyright (c) 2020-2023 Hassan Draga.
  Licensed under MIT License.
  All rights reserved.
  Canada.
*/

#ifndef _WEBUI_H
#define _WEBUI_H

#define WEBUI_VERSION "2.4.0"

// Max windows, servers and threads
#define WEBUI_MAX_IDS (512)

// Dynamic Library Exports
#if defined(_MSC_VER) || defined(__TINYC__)
    #ifndef WEBUI_EXPORT
        #define WEBUI_EXPORT __declspec(dllexport)
    #endif
#else
    #ifndef WEBUI_EXPORT
        #define WEBUI_EXPORT extern
    #endif
#endif

// -- C STD ---------------------------
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stddef.h>
#include <time.h>
#include <errno.h>
#include <math.h>
#if defined(__GNUC__) || defined(__TINYC__)
    #include <dirent.h>
#endif

// -- Windows -------------------------
#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <direct.h>
    #include <io.h>
    #include <tchar.h>
    #include <tlhelp32.h>
    #define WEBUI_GET_CURRENT_DIR _getcwd
    #define WEBUI_FILE_EXIST _access
    #define WEBUI_POPEN _popen
    #define WEBUI_PCLOSE _pclose
    #define WEBUI_MAX_PATH MAX_PATH
#endif

// -- Linux ---------------------------
#ifdef __linux__
    #include <unistd.h>
    #include <pthread.h>
    #include <unistd.h>
    #include <limits.h>
    #include <dirent.h>
    #include <sys/socket.h>
    #include <fcntl.h>
    #include <poll.h>
    #include <sys/time.h>
    #include <signal.h>
    #define WEBUI_GET_CURRENT_DIR getcwd
    #define WEBUI_FILE_EXIST access
    #define WEBUI_POPEN popen
    #define WEBUI_PCLOSE pclose
    #define WEBUI_MAX_PATH PATH_MAX
#endif

// -- Apple ---------------------------
#ifdef __APPLE__
    #include <pthread.h>
    #include <unistd.h>
    #include <limits.h>
    #include <dirent.h>
    #include <sys/socket.h>
    #include <fcntl.h>
    #include <poll.h>
    #include <sys/syslimits.h>
    #include <sys/time.h>
    #include <sys/sysctl.h>
    #include <signal.h>
    #define WEBUI_GET_CURRENT_DIR getcwd
    #define WEBUI_FILE_EXIST access
    #define WEBUI_POPEN popen
    #define WEBUI_PCLOSE pclose
    #define WEBUI_MAX_PATH PATH_MAX
#endif

// -- Enums ---------------------------
enum webui_browsers {
    NoBrowser = 0, // 0. No web browser
    AnyBrowser = 1, // 1. Default recommended web browser
    Chrome, // 2. Google Chrome
    Firefox, // 3. Mozilla Firefox
    Edge, // 4. Microsoft Edge
    Safari, // 5. Apple Safari
    Chromium, // 6. The Chromium Project
    Opera, // 7. Opera Browser
    Brave, // 8. The Brave Browser
    Vivaldi, // 9. The Vivaldi Browser
    Epic, // 10. The Epic Browser
    Yandex, // 11. The Yandex Browser
    ChromiumBased, // 12. Any Chromium based browser
};

enum webui_runtimes {
    None = 0, // 0. Prevent WebUI from using any runtime for .js and .ts files
    Deno, // 1. Use Deno runtime for .js and .ts files
    NodeJS, // 2. Use Nodejs runtime for .js files
};

enum webui_events {
    WEBUI_EVENT_DISCONNECTED = 0, // 0. Window disconnection event
    WEBUI_EVENT_CONNECTED, // 1. Window connection event
    WEBUI_EVENT_MULTI_CONNECTION, // 2. New window connection event
    WEBUI_EVENT_UNWANTED_CONNECTION, // 3. New unwanted window connection event
    WEBUI_EVENT_MOUSE_CLICK, // 4. Mouse click event
    WEBUI_EVENT_NAVIGATION, // 5. Window navigation event
    WEBUI_EVENT_CALLBACK, // 6. Function call event
};

// -- Structs -------------------------
typedef struct webui_event_t {
    size_t window; // The window object number
    size_t event_type; // Event type
    char* element; // HTML element ID
    char* data; // JavaScript data
    size_t size; // JavaScript data len
    size_t event_number; // Internal WebUI
} webui_event_t;

// -- Definitions ---------------------

/**
 * @brief Create a new WebUI window object.
 * 
 * @return Returns the window number.
 * 
 * @example size_t myWindow = webui_new_window();
 */
WEBUI_EXPORT size_t webui_new_window(void);

/**
 * @brief Create a new webui window object using a specified window number.
 * 
 * @param window_number The window number (should be > 0, and < WEBUI_MAX_IDS)
 * 
 * @return Returns the window number.
 * 
 * @example size_t myWindow = webui_new_window_id(123);
 */
WEBUI_EXPORT size_t webui_new_window_id(size_t window_number);

/**
 * @brief Get a free window number that can be used with `webui_new_window_id()`.
 * 
 * @return Returns the first available free window number. Starting from 1.
 * 
 * @example size_t myWindowNumber = webui_get_new_window_id();
 */
WEBUI_EXPORT size_t webui_get_new_window_id(void);

/**
 * @brief Bind a specific html element click event with a function. Empty element means all events.
 * 
 * @param window The window number
 * @param element The HTML ID
 * @param func The callback function
 * 
 * @return Returns a unique bind ID.
 * 
 * @example webui_bind(myWindow, "myID", myFunction);
 */
WEBUI_EXPORT size_t webui_bind(size_t window, const char* element, void (*func)(webui_event_t* e));

/**
 * @brief Show a window using embedded HTML, or a file. If the window is already open, it will be refreshed.
 * 
 * @param window The window number
 * @param content The HTML, Or a local file
 * 
 * @return Returns True if showing the window is successed.
 * 
 * @example webui_show(myWindow, "<html>...</html>"); | webui_show(myWindow, "index.html");
 */
WEBUI_EXPORT bool webui_show(size_t window, const char* content);

/**
 * @brief Same as `webui_show()`. But using a specific web browser.
 * 
 * @param window The window number
 * @param content The HTML, Or a local file
 * @param browser The web browser to be used
 * 
 * @return Returns True if showing the window is successed.
 * 
 * @example webui_show_browser(myWindow, "<html>...</html>", Chrome); | webui_show(myWindow, "index.html", Firefox);
 */
WEBUI_EXPORT bool webui_show_browser(size_t window, const char* content, size_t browser);

/**
 * @brief Set the window in Kiosk mode (Full screen)
 * 
 * @param window The window number
 * @param status True or False
 * 
 * @example webui_set_kiosk(myWindow, true);
 */
WEBUI_EXPORT void webui_set_kiosk(size_t window, bool status);

/**
 * @brief Wait until all opened windows get closed.
 * 
 * @example webui_wait();
 */
WEBUI_EXPORT void webui_wait(void);

/**
 * @brief Close a specific window only. The window object will still exist.
 * 
 * @param window The window number
 * 
 * @example webui_close(myWindow);
 */
WEBUI_EXPORT void webui_close(size_t window);

/**
 * @brief Close a specific window and free all memory resources.
 * 
 * @param window The window number
 * 
 * @example webui_destroy(myWindow);
 */
WEBUI_EXPORT void webui_destroy(size_t window);

/**
 * @brief Close all open windows. `webui_wait()` will return (Break).
 * 
 * @example webui_exit();
 */
WEBUI_EXPORT void webui_exit(void);

/**
 * @brief Set the web-server root folder path for a specific window.
 * 
 * @param window The window number
 * @param path The local folder full path
 * 
 * @example webui_set_root_folder(myWindow, "/home/Foo/Bar/");
 */
WEBUI_EXPORT bool webui_set_root_folder(size_t window, const char* path);

/**
 * @brief Set the web-server root folder path for all windows. Should be used before `webui_show()`.
 * 
 * @param path The local folder full path
 * 
 * @example webui_set_default_root_folder("/home/Foo/Bar/");
 */
WEBUI_EXPORT bool webui_set_default_root_folder(const char* path);

/**
 * @brief Set a custom handler to serve files.
 * 
 * @param window The window number
 * @param handler The handler function: `void myHandler(const char* filename, int* length)`
 * 
 * @return Returns a unique bind ID.
 * 
 * @example webui_set_file_handler(myWindow, myHandlerFunction);
 */
WEBUI_EXPORT void webui_set_file_handler(size_t window, const void* (*handler)(const char* filename, int* length));

/**
 * @brief Check if the specified window is still running.
 * 
 * @param window The window number
 * 
 * @example webui_is_shown(myWindow);
 */
WEBUI_EXPORT bool webui_is_shown(size_t window);

/**
 * @brief Set the maximum time in seconds to wait for the browser to start.
 * 
 * @param second The timeout in seconds
 * 
 * @example webui_set_timeout(30);
 */
WEBUI_EXPORT void webui_set_timeout(size_t second);

/**
 * @brief Set the default embedded HTML favicon.
 * 
 * @param window The window number
 * @param icon The icon as string: `<svg>...</svg>`
 * @param icon_type The icon type: `image/svg+xml`
 * 
 * @example webui_set_icon(myWindow, "<svg>...</svg>", "image/svg+xml");
 */
WEBUI_EXPORT void webui_set_icon(size_t window, const char* icon, const char* icon_type);

/**
 * @brief Allow the window URL to be re-used in normal web browsers.
 * 
 * @param window The window number
 * @param status The status: True or False
 * 
 * @example webui_set_multi_access(myWindow, true);
 */
WEBUI_EXPORT void webui_set_multi_access(size_t window, bool status);

/**
 * @brief Base64 encoding. Use this to safely send text based data to the UI. If it fails it will return NULL.
 * 
 * @param str The string to encode (Should be null terminated)
 * 
 * @example webui_encode("Hello");
 */
WEBUI_EXPORT char* webui_encode(const char* str);

/**
 * @brief Base64 decoding. Use this to safely decode received Base64 text from the UI. If it fails it will return NULL.
 * 
 * @param str The string to decode (Should be null terminated)
 * 
 * @example webui_encode("SGVsbG8=");
 */
WEBUI_EXPORT char* webui_decode(const char* str);

/**
 * @brief Safely free a buffer allocated by WebUI using `webui_malloc()`.
 * 
 * @param ptr The buffer to be freed
 * 
 * @example webui_free(myBuffer);
 */
WEBUI_EXPORT void webui_free(void* ptr);

/**
 * @brief Safely allocate memory using the WebUI memory management system. It can be safely freed using `webui_free()` at any time.
 * 
 * @param size The size of memory in bytes
 * 
 * @example webui_malloc(1024);
 */
WEBUI_EXPORT void* webui_malloc(size_t size);

/**
 * @brief Safely send raw data to the UI.
 * 
 * @param window The window number
 * @param function The JavaScript function to receive raw data: `function myFunc(myData){}`
 * @param raw The raw data buffer
 * @param size The raw data size in bytes
 * 
 * @example webui_send_raw(myWindow, "myJavascriptFunction", myBuffer, 64);
 */
WEBUI_EXPORT void webui_send_raw(size_t window, const char* function, const void* raw, size_t size);

/**
 * @brief Set a window in hidden mode. Should be called before `webui_show()`.
 * 
 * @param window The window number
 * @param status The status: True or False
 * 
 * @example webui_set_hide(myWindow, True);
 */
WEBUI_EXPORT void webui_set_hide(size_t window, bool status);

/**
 * @brief Set the window size.
 * 
 * @param window The window number
 * @param width The window width
 * @param height The window height
 * 
 * @example webui_set_size(myWindow, 800, 600);
 */
WEBUI_EXPORT void webui_set_size(size_t window, unsigned int width, unsigned int height);

/**
 * @brief Set the window position.
 * 
 * @param window The window number
 * @param x The window X
 * @param y The window Y
 * 
 * @example webui_set_position(myWindow, 100, 100);
 */
WEBUI_EXPORT void webui_set_position(size_t window, unsigned int x, unsigned int y);

/**
 * @brief Set the web browser profile to use. An empty `name` and `path` means the default user profile. Need to be called before `webui_show()`.
 * 
 * @param window The window number
 * @param name The web browser profile name
 * @param path The web browser profile full path
 * 
 * @example webui_set_profile(myWindow, "Bar", "/Home/Foo/Bar"); | webui_set_profile(myWindow, "", "");
 */
WEBUI_EXPORT void webui_set_profile(size_t window, const char* name, const char* path);

/**
 * @brief Get the full current URL.
 * 
 * @param window The window number
 *
 * @return Returns the full URL string
 * 
 * @example const char* url = webui_get_url(myWindow);
 */
WEBUI_EXPORT const char* webui_get_url(size_t window);

/**
 * @brief Navigate to a specific URL
 * 
 * @param window The window number
 * @param url Full HTTP URL
 * 
 * @example webui_navigate(myWindow, "http://domain.com");
 */
WEBUI_EXPORT void webui_navigate(size_t window, const char* url);

/**
 * @brief Free all memory resources. Should be called only at the end.
 * 
 * @example
 * webui_wait();
 * webui_clean();
 */
WEBUI_EXPORT void webui_clean();

/**
 * @brief Delete all local web-browser profile's folder. Should be called at the end.
 * 
 * @example
 * webui_wait();
 * webui_delete_all_profiles();
 * webui_clean();
 */
WEBUI_EXPORT void webui_delete_all_profiles();

/**
 * @brief Delete a specific window web-browser local folder profile.
 * 
 * @param window The window number
 * 
 * @example
 * webui_wait();
 * webui_delete_profile(myWindow);
 * webui_clean();
 * 
 * @note This can break functionality of other windows if using the same web-browser.
 */
WEBUI_EXPORT void webui_delete_profile(size_t window);

// -- JavaScript ----------------------

// Run JavaScript without waiting for the response.
WEBUI_EXPORT void webui_run(size_t window, const char* script);

// Run JavaScript and get the response back (Make sure your local buffer can hold the response).
WEBUI_EXPORT bool webui_script(size_t window, const char* script, size_t timeout, char* buffer, size_t buffer_length);

// Chose between Deno and Nodejs as runtime for .js and .ts files.
WEBUI_EXPORT void webui_set_runtime(size_t window, size_t runtime);

// Parse argument as integer.
WEBUI_EXPORT long long int webui_get_int(webui_event_t* e);

// Parse argument as string.
WEBUI_EXPORT const char* webui_get_string(webui_event_t* e);

// Parse argument as boolean.
WEBUI_EXPORT bool webui_get_bool(webui_event_t* e);

// Return the response to JavaScript as integer.
WEBUI_EXPORT void webui_return_int(webui_event_t* e, long long int n);

// Return the response to JavaScript as string.
WEBUI_EXPORT void webui_return_string(webui_event_t* e, const char* s);

// Return the response to JavaScript as boolean.
WEBUI_EXPORT void webui_return_bool(webui_event_t* e, bool b);

// Get process id (The web browser may create another process for the window)
WEBUI_EXPORT size_t webui_get_child_process_id(size_t window);
WEBUI_EXPORT size_t webui_get_parent_process_id(size_t window);

// -- Wrapper's Interface -------------

// Bind a specific html element click event with a function. Empty element means all events. This replaces `webui_bind()`. The func is (Window, EventType, Element, Data, DataSize, EventNumber).
WEBUI_EXPORT size_t webui_interface_bind(size_t window, const char* element, void (*func)(size_t, size_t, char*, char*, size_t, size_t));

// When using `webui_interface_bind()`, you may need this function to easily set your callback response.
WEBUI_EXPORT void webui_interface_set_response(size_t window, size_t event_number, const char* response);

/**
 * @brief Check if the app still running.
 * 
 * @example if (webui_interface_is_app_running()) ...
 */
WEBUI_EXPORT bool webui_interface_is_app_running(void);

// Get a unique window ID.
WEBUI_EXPORT size_t webui_interface_get_window_id(size_t window);

// Get a unique ID. Same ID as `webui_bind()`. Return > 0 if bind exist.
WEBUI_EXPORT size_t webui_interface_get_bind_id(size_t window, const char* element);

#endif /* _WEBUI_H */
