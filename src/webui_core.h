/*
  WebUI Library 2.4.0
  http://webui.me
  https://github.com/webui-dev/webui
  Copyright (c) 2020-2023 Hassan Draga.
  Licensed under MIT License.
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
#define WEBUI_HEADER_CALL_FUNC  0xF9        // Backend function call
#define WEBUI_HEADER_SEND_RAW   0xF8        // Send raw binary data to the UI
#define WEBUI_MAX_ARRAY         (512)       // Max threads, servers, windows, pointers..
#define WEBUI_MIN_PORT          (10000)     // Minimum socket port
#define WEBUI_MAX_PORT          (65500)     // Should be less than 65535
#define WEBUI_CMD_STDOUT_BUF    (10240)     // Command STDOUT output buffer size
#define WEBUI_DEFAULT_PATH      "."         // Default root path
#define WEBUI_DEF_TIMEOUT       (30)        // Default startup timeout in seconds
#define WEBUI_MAX_TIMEOUT       (60)        // Maximum startup timeout in seconds the user can set

// Mutex
#ifdef _WIN32
    typedef CRITICAL_SECTION webui_mutex_t;
    typedef CONDITION_VARIABLE webui_condition_t;
#else
    typedef pthread_mutex_t webui_mutex_t;
    typedef pthread_cond_t webui_condition_t;
#endif

// Timer
typedef struct _webui_timer_t {
    struct timespec start;
    struct timespec now;
} _webui_timer_t;

// Events
typedef struct webui_event_core_t {
    char* response; // Callback response
} webui_event_core_t;

// Window
typedef struct _webui_window_t {
    size_t window_number;
    volatile bool server_running;
    volatile bool connected;
    volatile bool file_handled;
    bool html_handled;
    bool server_handled;
    bool multi_access;
    bool is_embedded_html;
    size_t server_port;
    size_t ws_port;
    char* url;
    const char* html;
    const char* icon;
    const char* icon_type;
    size_t current_browser;
    char* browser_path;
    char* profile_path;
    size_t connections;
    size_t runtime;
    bool has_events;
    char* server_root_path;
    bool kiosk_mode;
    bool hide;
    size_t process_id;
    webui_event_core_t* event_core[WEBUI_MAX_ARRAY];
    #ifdef _WIN32
        HANDLE server_thread;
    #else
        pthread_t server_thread;
    #endif
    const void* (*files_handler)(const char* filename, int* length);
} _webui_window_t;

// Core
typedef struct _webui_core_t {
    volatile size_t servers;
    volatile size_t connections;
    char* html_elements[WEBUI_MAX_ARRAY];
    size_t used_ports[WEBUI_MAX_ARRAY];
    size_t startup_timeout;
    volatile bool exit_now;
    const char* run_responses[WEBUI_MAX_ARRAY];
    volatile bool run_done[WEBUI_MAX_ARRAY];
    bool run_error[WEBUI_MAX_ARRAY];
    unsigned char run_last_id;
    bool initialized;
    void (*cb[WEBUI_MAX_ARRAY])(webui_event_t* e);
    void (*cb_interface[WEBUI_MAX_ARRAY])(size_t, size_t, char*, char*, long long, size_t);
    char* executable_path;
    void *ptr_list[WEBUI_MAX_ARRAY];
    size_t ptr_position;
    size_t ptr_size[WEBUI_MAX_ARRAY];
    size_t current_browser;
    struct mg_connection* mg_connections[WEBUI_MAX_ARRAY];
    _webui_window_t* wins[WEBUI_MAX_ARRAY];
    size_t last_win_number;
    bool server_handled;
    webui_mutex_t mutex_server_start;
    webui_mutex_t mutex_send;
    webui_mutex_t mutex_receive;
    webui_mutex_t mutex_wait;
    webui_condition_t condition_wait;
    bool ui;
} _webui_core_t;

typedef struct _webui_cb_arg_t {
    // Event
    _webui_window_t* window;
    size_t event_type;
    char* element;
    char* data;
    size_t event_number;
    // Extras
    char* webui_internal_id;
} _webui_cb_arg_t;

typedef struct _webui_cmd_async_t {
    _webui_window_t* win;
    char* cmd;
} _webui_cmd_async_t;

// -- Definitions ---------------------
#ifdef _WIN32
    static const char* webui_sep = "\\";
    static DWORD WINAPI _webui_cb(LPVOID _arg);
    static DWORD WINAPI _webui_run_browser_task(LPVOID _arg);
    static int _webui_system_win32(_webui_window_t* win, char* cmd, bool show);
    static int _webui_system_win32_out(const char *cmd, char **output, bool show);
    static bool _webui_socket_test_listen_win32(size_t port_num);
    static bool _webui_get_windows_reg_value(HKEY key, LPCWSTR reg, LPCWSTR value_name, char value[WEBUI_MAX_PATH]);

    #define WEBUI_CB DWORD WINAPI _webui_cb(LPVOID _arg)
    #define WEBUI_SERVER_START DWORD WINAPI _webui_server_start(LPVOID arg)
    #define THREAD_RETURN return 0;
#else
    static const char* webui_sep = "/";
    static void* _webui_cb(void* _arg);
    static void* _webui_run_browser_task(void* _arg);

    #define WEBUI_CB void* _webui_cb(void* _arg)
    #define WEBUI_SERVER_START void* _webui_server_start(void* arg)
    #define THREAD_RETURN pthread_exit(NULL);
#endif

static void _webui_init(void);
static bool _webui_show(_webui_window_t* win, const char* content, size_t browser);
static size_t _webui_get_cb_index(char* webui_internal_id);
static size_t _webui_set_cb_index(char* webui_internal_id);
static size_t _webui_get_free_port(void);
static void _webui_wait_for_startup(void);
static void _webui_free_port(size_t port);
static char* _webui_get_current_path(void);
static void _webui_window_receive(_webui_window_t* win, const char* packet, size_t len);
static void _webui_window_send(_webui_window_t* win, char* packet, size_t packets_size);
static void _webui_window_event(_webui_window_t* win, int event_type, char* element, char* data, size_t event_number, char* webui_internal_id);
static int _webui_cmd_sync(_webui_window_t* win, char* cmd, bool show);
static int _webui_cmd_async(_webui_window_t* win, char* cmd, bool show);
static int _webui_run_browser(_webui_window_t* win, char* cmd);
static void _webui_clean(void);
static bool _webui_browser_exist(_webui_window_t* win, size_t browser);
static const char* _webui_browser_get_temp_path(size_t browser);
static bool _webui_folder_exist(char* folder);
static bool _webui_browser_create_profile_folder(_webui_window_t* win, size_t browser);
static bool _webui_browser_start_chrome(_webui_window_t* win, const char* address);
static bool _webui_browser_start_edge(_webui_window_t* win, const char* address);
static bool _webui_browser_start_epic(_webui_window_t* win, const char* address);
static bool _webui_browser_start_vivaldi(_webui_window_t* win, const char* address);
static bool _webui_browser_start_brave(_webui_window_t* win, const char* address);
static bool _webui_browser_start_firefox(_webui_window_t* win, const char* address);
static bool _webui_browser_start_yandex(_webui_window_t* win, const char* address);
static bool _webui_browser_start_chromium(_webui_window_t* win, const char* address);
static bool _webui_browser_start(_webui_window_t* win, const char* address, size_t browser);
static long _webui_timer_diff(struct timespec *start, struct timespec *end);
static void _webui_timer_start(_webui_timer_t* t);
static bool _webui_timer_is_end(_webui_timer_t* t, size_t ms);
static void _webui_timer_clock_gettime(struct timespec *spec);
static bool _webui_set_root_folder(_webui_window_t* win, const char* path);
static const char* _webui_generate_js_bridge(_webui_window_t* win);
static void _webui_print_hex(const char* data, size_t len);
static void _webui_free_mem(void* ptr);
static bool _webui_file_exist_mg(_webui_window_t* win, struct mg_connection *conn);
static bool _webui_file_exist(char* file);
static void _webui_free_all_mem(void);
static bool _webui_show_window(_webui_window_t* win, const char* content, bool is_embedded_html, size_t browser);
static char* _webui_generate_internal_id(_webui_window_t* win, const char* element);
static bool _webui_is_empty(const char* s);
static size_t _webui_strlen(const char* s);
static unsigned char _webui_get_run_id(void);
static void* _webui_malloc(size_t size);
static void _webui_sleep(long unsigned int ms);
static size_t _webui_find_the_best_browser(_webui_window_t* win);
static bool _webui_is_process_running(const char* process_name);
static size_t _webui_get_free_event_core_pos(_webui_window_t* win);
static void _webui_print_hex(const char* data, size_t len);
static void _webui_print_ascii(const char* data, size_t len);
static void _webui_panic(void);
static void _webui_kill_pid(size_t pid);
static _webui_window_t* _webui_dereference_win_ptr(void* ptr);

static void _webui_mutex_init(webui_mutex_t *mutex);
static void _webui_mutex_lock(webui_mutex_t *mutex);
static void _webui_mutex_unlock(webui_mutex_t *mutex);
static void _webui_mutex_destroy(webui_mutex_t *mutex);
static void _webui_condition_init(webui_condition_t *cond);
static void _webui_condition_wait(webui_condition_t *cond, webui_mutex_t *mutex);
static void _webui_condition_signal(webui_condition_t *cond);
static void _webui_condition_destroy(webui_condition_t *cond);

static void _webui_http_send(struct mg_connection *conn, const char* mime_type, const char* body);
static void _webui_http_send_error_page(struct mg_connection *conn, const char* body, int status);
static int _webui_http_log(const struct mg_connection *conn, const char* message);
static int _webui_http_handler(struct mg_connection *conn, void *_win);
static int _webui_ws_connect_handler(const struct mg_connection *conn, void *_win);
static void _webui_ws_ready_handler(struct mg_connection *conn, void *_win);
static int _webui_ws_data_handler(struct mg_connection *conn, int opcode, char* data, size_t datasize, void *_win);
static void _webui_ws_close_handler(const struct mg_connection *conn, void *_win);

static WEBUI_SERVER_START;
static WEBUI_CB;

#endif /* _WEBUI_CORE_H */
