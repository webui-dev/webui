/*
  WebUI Library 2.2.0
  http://webui.me
  https://github.com/alifcommunity/webui
  Copyright (c) 2020-2023 Hassan Draga.
  Licensed under GNU General Public License v2.0.
  All rights reserved.
  Canada.
*/

#ifndef _WEBUI_H
#define _WEBUI_H

#define WEBUI_VERSION "2.2.0"

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
    #ifndef WEBUI_NO_TLHELPER32
        #include <tlhelp32.h>
    #endif
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
    void* window; // Pointer to the window object
    unsigned int event_type; // Event type
    char* element; // HTML element ID
    char* data; // JavaScript data
    unsigned int event_number; // Internal WebUI
} webui_event_t;

// -- Definitions ---------------------
// Create a new webui window object.
WEBUI_EXPORT void* webui_new_window(void);
// Bind a specific html element click event with a function. Empty element means all events.
WEBUI_EXPORT unsigned int webui_bind(void* window, const char* element, void (*func)(webui_event_t* e));
// Show a window using a embedded HTML, or a file. If the window is already opened then it will be refreshed.
WEBUI_EXPORT bool webui_show(void* window, const char* content);
// Same as webui_show(). But with a specific web browser.
WEBUI_EXPORT bool webui_show_browser(void* window, const char* content, unsigned int browser);
// Wait until all opened windows get closed.
WEBUI_EXPORT void webui_wait(void);
// Close a specific window.
WEBUI_EXPORT void webui_close(void* window);
// Close all opened windows. webui_wait() will break.
WEBUI_EXPORT void webui_exit(void);

// -- Other ---------------------------
// Check a specific window if it's still running
WEBUI_EXPORT bool webui_is_shown(void* window);
// Set the maximum time in seconds to wait for browser to start
WEBUI_EXPORT void webui_set_timeout(unsigned int second);
// Set the default embedded HTML favicon
WEBUI_EXPORT void webui_set_icon(void* window, const char* icon, const char* icon_type);
// Allow the window URL to be re-used in normal web browsers
WEBUI_EXPORT void webui_set_multi_access(void* window, bool status);

// -- JavaScript ----------------------
// Run JavaScript quickly with no waiting for the response.
WEBUI_EXPORT bool webui_run(void* window, const char* script);
// Run a JavaScript, and get the response back (Make sure your local buffer can hold the response).
WEBUI_EXPORT bool webui_script(void* window, const char* script, unsigned int timeout, char* buffer, size_t buffer_length);
// Chose between Deno and Nodejs runtime for .js and .ts files.
WEBUI_EXPORT void webui_set_runtime(void* window, unsigned int runtime);
// Parse argument as integer.
WEBUI_EXPORT long long int webui_get_int(webui_event_t* e);
// Parse argument as string.
WEBUI_EXPORT const char* webui_get_string(webui_event_t* e);
// Parse argument as boolean.
WEBUI_EXPORT bool webui_get_bool(webui_event_t* e);
// Return the response to JavaScript as integer.
WEBUI_EXPORT void webui_return_int(webui_event_t* e, long long int n);
// Return the response to JavaScript as string.
WEBUI_EXPORT void webui_return_string(webui_event_t* e, char* s);
// Return the response to JavaScript as boolean.
WEBUI_EXPORT void webui_return_bool(webui_event_t* e, bool b);

// -- Interface -----------------------
// Bind a specific html element click event with a function. Empty element means all events. This replace webui_bind(). The func is (Window, EventType, Element, Data, EventNumber)
WEBUI_EXPORT unsigned int webui_interface_bind(void* window, const char* element, void (*func)(void*, unsigned int, char*, char*, unsigned int));
// When using `webui_interface_bind()` you may need this function to easily set your callback response.
WEBUI_EXPORT void webui_interface_set_response(void* window, unsigned int event_number, const char* response);
// Check if the app still running or not. This replace webui_wait().
WEBUI_EXPORT bool webui_interface_is_app_running(void);
// Get window unique ID
WEBUI_EXPORT unsigned int webui_interface_get_window_id(void* window);

#endif /* _WEBUI_H */
