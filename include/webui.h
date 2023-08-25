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
    AnyBrowser = 0, // 0. Default recommended web browser
    Chrome, // 1. Google Chrome
    Firefox, // 2. Mozilla Firefox
    Edge, // 3. Microsoft Edge
    Safari, // 4. Apple Safari
    Chromium, // 5. The Chromium Project
    Opera, // 6. Opera Browser
    Brave, // 7. The Brave Browser
    Vivaldi, // 8. The Vivaldi Browser
    Epic, // 9. The Epic Browser
    Yandex, // 10. The Yandex Browser
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
    long long int size; // JavaScript data len
    size_t event_number; // Internal WebUI
} webui_event_t;

// -- Definitions ---------------------
// Create a new webui window object.
WEBUI_EXPORT size_t webui_new_window(void);
// Create a new webui window object.
WEBUI_EXPORT void webui_new_window_id(size_t window_number);
// Get a free window ID that can be used with `webui_new_window_id()`
WEBUI_EXPORT size_t webui_get_new_window_id(void);
// Bind a specific html element click event with a function. Empty element means all events.
WEBUI_EXPORT size_t webui_bind(size_t window, const char* element, void (*func)(webui_event_t* e));
// Show a window using a embedded HTML, or a file. If the window is already opened then it will be refreshed.
WEBUI_EXPORT bool webui_show(size_t window, const char* content);
// Same as webui_show(). But with a specific web browser.
WEBUI_EXPORT bool webui_show_browser(size_t window, const char* content, size_t browser);
// Set the window in Kiosk mode (Full screen)
WEBUI_EXPORT void webui_set_kiosk(size_t window, bool status);
// Wait until all opened windows get closed.
WEBUI_EXPORT void webui_wait(void);
// Close a specific window only. The window object will still exist.
WEBUI_EXPORT void webui_close(size_t window);
// Close a specific window and free all memory resources.
WEBUI_EXPORT void webui_destroy(size_t window);
// Close all opened windows. webui_wait() will break.
WEBUI_EXPORT void webui_exit(void);
// Set the web-server root folder path for a specific window.
WEBUI_EXPORT bool webui_set_root_folder(size_t window, const char* path);
// Set the web-server root folder path for all windows.
WEBUI_EXPORT bool webui_set_default_root_folder(const char* path);
// Set a custom handler to serve files
WEBUI_EXPORT void webui_set_file_handler(size_t window, const void* (*handler)(const char* filename, int* length));

// -- Other ---------------------------
// Check a specific window if it's still running
WEBUI_EXPORT bool webui_is_shown(size_t window);
// Set the maximum time in seconds to wait for browser to start
WEBUI_EXPORT void webui_set_timeout(size_t second);
// Set the default embedded HTML favicon
WEBUI_EXPORT void webui_set_icon(size_t window, const char* icon, const char* icon_type);
// Allow the window URL to be re-used in normal web browsers
WEBUI_EXPORT void webui_set_multi_access(size_t window, bool status);

// -- JavaScript ----------------------
// Run JavaScript quickly with no waiting for the response.
WEBUI_EXPORT void webui_run(size_t window, const char* script);
// Run a JavaScript, and get the response back (Make sure your local buffer can hold the response).
WEBUI_EXPORT bool webui_script(size_t window, const char* script, size_t timeout, char* buffer, size_t buffer_length);
// Chose between Deno and Nodejs runtime for .js and .ts files.
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
// Base64 encoding. Use this to safely send text based data to the UI. If it fails it will return NULL.
WEBUI_EXPORT char* webui_encode(const char* str);
// Base64 decoding. Use this to safely decode received Base64 text from the UI. If it fails it will return NULL.
WEBUI_EXPORT char* webui_decode(const char* str);
// Safely free a buffer allocated by WebUI, for example when using webui_encode().
WEBUI_EXPORT void webui_free(void* ptr);
// Safely allocate memory using the WebUI memory management system. It can be safely free using webui_free().
WEBUI_EXPORT void* webui_malloc(size_t size);
// Safely send raw data to the UI.
WEBUI_EXPORT void webui_send_raw(size_t window, const char* function, const void* raw, size_t size);
// Run the window in hidden mode.
WEBUI_EXPORT void webui_set_hide(size_t window, bool status);

// -- Interface -----------------------
// Bind a specific html element click event with a function. Empty element means all events. This replace webui_bind(). The func is (Window, EventType, Element, Data, DataSize, EventNumber)
WEBUI_EXPORT size_t webui_interface_bind(size_t window, const char* element, void (*func)(size_t, size_t, char*, char*, long long, size_t));
// When using `webui_interface_bind()` you may need this function to easily set your callback response.
WEBUI_EXPORT void webui_interface_set_response(size_t window, size_t event_number, const char* response);
// Check if the app still running or not. This replace webui_wait().
WEBUI_EXPORT bool webui_interface_is_app_running(void);
// Get window unique ID
WEBUI_EXPORT size_t webui_interface_get_window_id(size_t window);
// Get a unique ID. Same ID as `webui_bind()`. Return > 0 if bind exist.
WEBUI_EXPORT size_t webui_interface_get_bind_id(size_t window, const char* element);

#endif /* _WEBUI_H */
