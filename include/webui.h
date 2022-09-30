/*
    WebUI Library 2.0.0
    
    http://webui.me
    https://github.com/alifcommunity/webui

    Licensed under GNU General Public License v3.0.
    Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.
*/

#ifndef _WEBUI_H
#define _WEBUI_H

#if defined(_MSC_VER) || defined(__TINYC__)
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT extern
#endif

#define WEBUI_HEADER_SIGNATURE  0xFF        // All packets should start with this 8bit
#define WEBUI_HEADER_JS         0xFE        // Javascript result in frontend
#define WEBUI_HEADER_CLICK      0xFD        // Click event
#define WEBUI_HEADER_SWITCH     0xFC        // Frontend refresh
#define WEBUI_HEADER_CLOSE      0xFB        // Close window
#define WEBUI_HEADER_CALL_FUNC  0xFA        // Call a backend function
#define WEBUI_MAX_ARRAY         (32)        // Max thread, servers, windows..
#define WEBUI_MIN_PORT          (8080)      // Minimum socket port
#define WEBUI_MAX_PORT          (8335)      // Should be less than 65535
#define WEBUI_MAX_BUF           (512000)    // 512 Kb max dynamic memory
#define WEBUI_DEFAULT_PATH      "."         // Default root path

// -- C STD -----------------------------------
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
#if defined(__GNUC__) || defined(__TINYC__)
    #include <dirent.h>
#endif

// -- Windows ---------------------------------
#ifdef _WIN32
    // #include <SDKDDKVer.h> // Fix _WIN32_WINNT warning
    #include <winsock2.h>
    #include <windows.h>
    #include <ws2tcpip.h>
    #include <direct.h>
    #include <io.h>
    #include <tchar.h>
    #define WEBUI_GET_CURRENT_DIR _getcwd
    #define WEBUI_FILE_EXIST _access
    #define WEBUI_POPEN _popen
    #define WEBUI_PCLOSE _pclose
#endif

// -- Linux -----------------------------------
#ifdef __linux__
    #include <pthread.h> // POSIX threading
    #include <unistd.h>
    #define WEBUI_GET_CURRENT_DIR getcwd
    #define WEBUI_FILE_EXIST _access
    #define WEBUI_POPEN popen
    #define WEBUI_PCLOSE pclose
#endif

// -- macOS -----------------------------------
// ...

typedef struct webui_event_t {

    unsigned int window_id;
    unsigned int element_id;
    char* element_name;

} webui_event_t;

typedef struct webui_window_core_t {

    unsigned int window_number;
    bool server_running;
    bool connected;
    bool server_handled;
    bool multi_access;
    bool server_root;
    unsigned int server_port;
    bool is_bind_all;
    char* url;
    void (*cb_all[1]) (webui_event_t e);
    const char* html;
    const char* icon;
    const char* icon_type;
    unsigned int CurrentBrowser;
    char* browser_path;
    char* profile_path;
    unsigned int connections;
    unsigned int runtime;
    bool detect_process_close;
    #ifdef _WIN32
        HANDLE server_thread;
    #else
        unsigned int server_thread;
    #endif    

} webui_window_core_t;

typedef struct webui_window_t {

    webui_window_core_t core;
    char* path;

} webui_window_t;

typedef struct webui_javascript_result_t {

    bool error;
    unsigned int length;
    const char* data;

} webui_javascript_result_t;

typedef struct webui_javascript_t {

    char* script;
    unsigned int timeout;
    webui_javascript_result_t result;

} webui_javascript_t;

typedef struct webui_cb_t {

    webui_window_t* win;
    char* element_id;
    char* element_name;

} webui_cb_t;

typedef struct webui_cmd_async_t {

    webui_window_t* win;
    char* cmd;

} webui_cmd_async_t;

typedef struct webui_custom_browser_t {

    char* app;
    char* arg;
    bool auto_link;

} webui_custom_browser_t;

typedef struct webui_browser_t {

    unsigned int any;       // 0
    unsigned int chrome;    // 1
    unsigned int firefox;   // 2
    unsigned int edge;      // 3
    unsigned int safari;    // 4
    unsigned int chromium;  // 5
    unsigned int custom;    // 99

} webui_browser_t;

typedef struct webui_runtime_t {

    unsigned int none;      // 0
    unsigned int deno;      // 1
    unsigned int nodejs;    // 2

} webui_runtime_t;

typedef struct webui_t {

    unsigned int servers;
    unsigned int connections;
    webui_custom_browser_t *custom_browser;
    bool wait_for_socket_window;
    char* html_elements[WEBUI_MAX_ARRAY];
    unsigned int used_ports[WEBUI_MAX_ARRAY];
    unsigned int last_window;
    unsigned int startup_timeout;
    bool use_timeout;
    bool timeout_extra;
    bool exit_now;
    char* run_responses[WEBUI_MAX_ARRAY];
    bool run_done[WEBUI_MAX_ARRAY];
    bool run_error[WEBUI_MAX_ARRAY];
    unsigned int run_last_id;
    struct mg_mgr* mg_mgrs[WEBUI_MAX_ARRAY];
    struct mg_connection* mg_connections[WEBUI_MAX_ARRAY];
    webui_browser_t browser;
    webui_runtime_t runtime;
    bool initialized;
    void (*cb[WEBUI_MAX_ARRAY]) (webui_event_t e);
    void (*cb_py[WEBUI_MAX_ARRAY])(unsigned int, unsigned int, char*);
    char* executable_path;

    // Pointers Tracker
    void *ptr_list[WEBUI_MAX_ARRAY];
    size_t ptr_position;
    size_t ptr_size[WEBUI_MAX_ARRAY];

} webui_t;

typedef struct webui_javascript_py_t {

    char* script;
    unsigned int timeout;
    bool error;
    unsigned int length;
    const char* data;

} webui_javascript_py_t;

// -- Definitions --------------------

EXPORT webui_t webui;
EXPORT void webui_loop();
EXPORT void webui_exit();
EXPORT bool webui_any_window_is_open();
EXPORT void webui_set_timeout(unsigned int second);
EXPORT webui_window_t* webui_new_window();
EXPORT bool webui_show(webui_window_t* win, const char* html, unsigned int browser);
EXPORT void webui_set_icon(webui_window_t* win, const char* icon_s, const char* type_s);
EXPORT void webui_allow_multi_access(webui_window_t* win, bool status);
EXPORT bool webui_set_root_folder(webui_window_t* win, const char* path);
EXPORT const char* webui_new_server(webui_window_t* win, const char* path, const char* index_html);
EXPORT void webui_close(webui_window_t* win);
EXPORT bool webui_is_show(webui_window_t* win);
EXPORT void webui_run_js(webui_window_t* win, webui_javascript_t* javascript);
EXPORT unsigned int webui_bind(webui_window_t* win, const char* element, void (*func) (webui_event_t e));
EXPORT void webui_bind_all(webui_window_t* win, void (*func) (webui_event_t e));
EXPORT bool webui_open(webui_window_t* win, const char* url, unsigned int browser);
EXPORT void webui_free_js(webui_javascript_t* javascript);
EXPORT void webui_runtime(webui_window_t* win, unsigned int runtime);
EXPORT void webui_detect_process_close(webui_window_t* win, bool status);

// Python Interface
EXPORT unsigned int webui_bind_py(webui_window_t* win, const char* element, void (*func)(unsigned int, unsigned int, char*));
EXPORT void webui_run_js_py(webui_window_t* win, webui_javascript_py_t* js_py);

// Core
EXPORT void _webui_ini();
EXPORT unsigned int _webui_get_cb_index(char* element);
EXPORT unsigned int _webui_set_cb_index(char* element);
EXPORT unsigned int _webui_get_free_port();
EXPORT unsigned int _webui_get_new_window_number();
EXPORT void _webui_wait_for_startup();
EXPORT void _webui_free_port(unsigned int port);
EXPORT void _webui_set_custom_browser(webui_custom_browser_t* p);
EXPORT char* _webui_get_current_path();
EXPORT void _webui_window_receive(webui_window_t* win, const char* packet, size_t len);
EXPORT void _webui_window_send(webui_window_t* win, char* packet, size_t packets_size);
EXPORT void _webui_window_event(webui_window_t* win, char* element_id, char* element);
EXPORT unsigned int _webui_window_get_window_number(webui_window_t* win);
EXPORT void _webui_window_open(webui_window_t* win, char* link, unsigned int browser);
EXPORT int _webui_cmd_sync(char* cmd);
EXPORT int _webui_cmd_async(char* cmd);
EXPORT int _webui_run_browser(webui_window_t* win, char* cmd);
EXPORT void _webui_browser_clean();
EXPORT bool _webui_browser_exist(webui_window_t* win, unsigned int browser);
EXPORT char* _webui_browser_get_temp_path(unsigned int browser);
EXPORT bool _webui_folder_exist(char* folder);
EXPORT bool _webui_browser_create_profile_folder(webui_window_t* win, unsigned int browser);
EXPORT bool _webui_browser_start_edge(webui_window_t* win, const char* address);
EXPORT bool _webui_browser_start_firefox(webui_window_t* win, const char* address);
EXPORT bool _webui_browser_start_custom(webui_window_t* win, const char* address);
EXPORT bool _webui_browser_start_chrome(webui_window_t* win, const char* address);
EXPORT bool _webui_browser_start(webui_window_t* win, const char* address, unsigned int browser);
#ifdef _WIN32
    EXPORT DWORD WINAPI _webui_cb(LPVOID _arg);
    EXPORT DWORD WINAPI _webui_run_browser_task(LPVOID _arg);
#else
    EXPORT void _webui_cb(void* _arg);
    EXPORT void _webui_run_browser_task(void* _arg);
#endif

#endif /* _WEBUI_H */
