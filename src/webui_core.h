/*
  WebUI Library 2.2.0
  http://webui.me
  https://github.com/alifcommunity/webui
  Copyright (c) 2020-2023 Hassan Draga.
  Licensed under GNU General Public License v2.0.
  All rights reserved.
  Canada.
*/

#ifndef _WEBUI_CORE_H
#define _WEBUI_CORE_H

#include "webui.h"

#define WEBUI_HEADER_SIGNATURE  0xDD        // All packets should start with this 8bit
#define WEBUI_HEADER_JS         0xFE        // JavaScript result in frontend
#define WEBUI_HEADER_JS_QUICK   0xFD        // JavaScript result in frontend
#define WEBUI_HEADER_CLICK      0xFC        // Click event
#define WEBUI_HEADER_SWITCH     0xFB        // Frontend refresh
#define WEBUI_HEADER_CLOSE      0xFA        // Close window
#define WEBUI_HEADER_CALL_FUNC  0xF9        // Call a backend function
#define WEBUI_MAX_ARRAY         (512)       // Max threads, servers, windows, pointers..
#define WEBUI_MIN_PORT          (10000)     // Minimum socket port
#define WEBUI_MAX_PORT          (65500)     // Should be less than 65535
#define WEBUI_MAX_BUF           (1024000)   // 1024 Kb max dynamic memory allocation
#define WEBUI_DEFAULT_PATH      "."         // Default root path
#define WEBUI_DEF_TIMEOUT       (10)        // Default startup timeout in seconds
#define WEBUI_MAX_TIMEOUT       (30)        // Maximum startup timeout in seconds

typedef struct _webui_timer_t {
    struct timespec start;
    struct timespec now;
} _webui_timer_t;

typedef struct _webui_window_t {
    unsigned int window_number;
    bool server_running;
    bool connected;
    bool html_handled;
    bool multi_access;
    bool is_embedded_html;
    unsigned int server_port;
    char* url;
    const char* html;
    const char* icon;
    const char* icon_type;
    unsigned int current_browser;
    char* browser_path;
    char* profile_path;
    unsigned int connections;
    unsigned int runtime;
    bool has_events;
    char* server_root_path;
    #ifdef _WIN32
        HANDLE server_thread;
    #else
        pthread_t server_thread;
    #endif
} _webui_window_t;

typedef struct _webui_core_t {
    unsigned int servers;
    unsigned int connections;
    char* html_elements[WEBUI_MAX_ARRAY];
    unsigned int used_ports[WEBUI_MAX_ARRAY];
    unsigned int last_window;
    unsigned int startup_timeout;
    bool exit_now;
    const char* run_responses[WEBUI_MAX_ARRAY];
    bool run_done[WEBUI_MAX_ARRAY];
    bool run_error[WEBUI_MAX_ARRAY];
    unsigned char run_last_id;
    struct mg_mgr* mg_mgrs[WEBUI_MAX_ARRAY];
    struct mg_connection* mg_connections[WEBUI_MAX_ARRAY];
    bool initialized;
    void (*cb[WEBUI_MAX_ARRAY])(webui_event_t* e);
    void (*cb_interface[WEBUI_MAX_ARRAY])(void*, unsigned int, char*, char*, char*);
    char* executable_path;
    void *ptr_list[WEBUI_MAX_ARRAY];
    unsigned int ptr_position;
    size_t ptr_size[WEBUI_MAX_ARRAY];
    unsigned int current_browser;
} _webui_core_t;

typedef struct _webui_cb_t {
    _webui_window_t* win;
    char* webui_internal_id;
    char* element_name;
    void* data;
    unsigned int data_len;
    int event_type;
} _webui_cb_t;

typedef struct _webui_mg_handler_t {
    struct mg_connection* c;
    int ev;
    void* ev_data;
    void* fn_data;
} _webui_mg_handler_t;

typedef struct _webui_cmd_async_t {
    _webui_window_t* win;
    char* cmd;
} _webui_cmd_async_t;

// -- Definitions ---------------------
#ifdef _WIN32
    static const char* webui_sep = "\\";
    DWORD WINAPI _webui_cb(LPVOID _arg);
    DWORD WINAPI _webui_run_browser_task(LPVOID _arg);
    int _webui_system_win32(char* cmd, bool show);
    bool _webui_socket_test_listen_win32(unsigned int port_num);
    bool _webui_get_windows_reg_value(HKEY key, const char* reg, const char* value_name, char value[WEBUI_MAX_PATH]);

    #define WEBUI_CB DWORD WINAPI _webui_cb(LPVOID _arg)
    #define WEBUI_SERVER_START DWORD WINAPI _webui_server_start(LPVOID arg)
    #define THREAD_RETURN return 0;
#else
    static const char* webui_sep = "/";
    void* _webui_cb(void* _arg);
    void* _webui_run_browser_task(void* _arg);

    #define WEBUI_CB void* _webui_cb(void* _arg)
    #define WEBUI_SERVER_START void* _webui_server_start(void* arg)
    #define THREAD_RETURN pthread_exit(NULL);
#endif

static void _webui_init(void);
bool _webui_show(_webui_window_t* window, const char* content, unsigned int browser);
unsigned int _webui_get_cb_index(char* webui_internal_id);
unsigned int _webui_set_cb_index(char* webui_internal_id);
unsigned int _webui_get_free_port(void);
unsigned int _webui_get_new_window_number(void);
static void _webui_wait_for_startup(void);
static void _webui_free_port(unsigned int port);
char* _webui_get_current_path(void);
static void _webui_window_receive(_webui_window_t* win, const char* packet, size_t len);
static void _webui_window_send(_webui_window_t* win, char* packet, size_t packets_size);
static void _webui_window_event(_webui_window_t* win, char* element_id, char* element, void* data, unsigned int data_len, int event_type);
unsigned int _webui_window_get_number(_webui_window_t* win);
static void _webui_window_open(_webui_window_t* win, char* link, unsigned int browser);
int _webui_cmd_sync(char* cmd, bool show);
int _webui_cmd_async(char* cmd, bool show);
int _webui_run_browser(_webui_window_t* win, char* cmd);
static void _webui_clean(void);
bool _webui_browser_exist(_webui_window_t* win, unsigned int browser);
const char* _webui_browser_get_temp_path(unsigned int browser);
bool _webui_folder_exist(char* folder);
bool _webui_browser_create_profile_folder(_webui_window_t* win, unsigned int browser);
bool _webui_browser_start_chrome(_webui_window_t* win, const char* address);
bool _webui_browser_start_edge(_webui_window_t* win, const char* address);
bool _webui_browser_start_epic(_webui_window_t* win, const char* address);
bool _webui_browser_start_vivaldi(_webui_window_t* win, const char* address);
bool _webui_browser_start_brave(_webui_window_t* win, const char* address);
bool _webui_browser_start_firefox(_webui_window_t* win, const char* address);
bool _webui_browser_start_yandex(_webui_window_t* win, const char* address);
bool _webui_browser_start_chromium(_webui_window_t* win, const char* address);
bool _webui_browser_start(_webui_window_t* win, const char* address, unsigned int browser);
long _webui_timer_diff(struct timespec *start, struct timespec *end);
static void _webui_timer_start(_webui_timer_t* t);
bool _webui_timer_is_end(_webui_timer_t* t, unsigned int ms);
static void _webui_timer_clock_gettime(struct timespec *spec);
bool _webui_set_root_folder(_webui_window_t* win, const char* path);
const char* _webui_generate_js_bridge(_webui_window_t* win);
static void _webui_print_hex(const char* data, size_t len);
static void _webui_free_mem(void* ptr);
bool _webui_file_exist_mg(void *ev_data);
bool _webui_file_exist(char* file);
static void _webui_free_all_mem(void);
bool _webui_show_window(_webui_window_t* win, const char* content, bool is_embedded_html, unsigned int browser);
char* _webui_generate_internal_id(_webui_window_t* win, const char* element);
bool _webui_is_empty(const char* s);
unsigned char _webui_get_run_id(void);
void* _webui_malloc(int size);
static void _webui_sleep(long unsigned int ms);

WEBUI_SERVER_START;
WEBUI_CB;

#endif /* _WEBUI_CORE_H */
