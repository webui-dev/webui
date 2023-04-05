/*
    WebUI Library 2.1.1
    
    http://webui.me
    https://github.com/alifcommunity/webui

    Copyright (c) 2020-2023 Hassan Draga.
    Licensed under GNU General Public License v2.0.
    All rights reserved.
    Canada.
*/

// -- Third-party ---------------------
#include "mongoose.h"

// -- WebUI ---------------------------
#include "webui.h"

// -- Log -----------------------------
// #define WEBUI_LOG

// -- Heap ----------------------------
webui_t webui;

#ifdef WEBUI_LOG
    #define WEBUI_JS_LOG "true"
#else
    #define WEBUI_JS_LOG "false"
#endif

// -- WebUI JS-Bridge ---------
// This is a uncompressed version to make the debugging
// more easy in the browser using the builtin dev-tools
static const char* webui_javascript_bridge = 
"var _webui_log = " WEBUI_JS_LOG "; \n"
"var _webui_ws; \n"
"var _webui_ws_status = false; \n"
"var _webui_ws_status_once = false; \n"
"var _webui_close_reason = 0; \n"
"var _webui_close_value; \n"
"var _webui_has_events = false; \n"
"const _WEBUI_SIGNATURE = 255; \n"
"const _WEBUI_JS = 254; \n"
"const _WEBUI_CLICK = 253; \n"
"const _WEBUI_SWITCH = 252; \n"
"const _WEBUI_CLOSE = 251; \n"
"const _WEBUI_FUNCTION = 250; \n"
"function _webui_close(reason = 0, value = 0) { \n"
"    _webui_send_event_navigation(value); \n"
"    _webui_ws_status = false; \n"
"    _webui_close_reason = reason; \n"
"    _webui_close_value = value; \n"
"    _webui_ws.close(); \n"
"} \n"
"function _webui_freeze_ui() { \n"
"    document.body.style.filter = 'contrast(1%)'; \n"
"} \n"
"function _webui_start() { \n"
"    if('WebSocket' in window) { \n"
"        if(_webui_bind_list.includes(_webui_win_num + '/')) _webui_has_events = true; \n"
"        _webui_ws = new WebSocket('ws://localhost:' + _webui_port + '/_ws'); \n"
"        _webui_ws.binaryType = 'arraybuffer'; \n"
"        _webui_ws.onopen = function () { \n"
"            _webui_ws.binaryType = 'arraybuffer'; \n"
"            _webui_ws_status = true; \n"
"            _webui_ws_status_once = true; \n"
"            if(_webui_log) \n"
"                console.log('WebUI -> Connected'); \n"
"            _webui_clicks_listener(); \n"
"        }; \n"
"        _webui_ws.onerror = function () { \n"
"            if(_webui_log) \n"
"                console.log('WebUI -> Connection Failed'); \n"
"            _webui_freeze_ui(); \n"
"        }; \n"
"        _webui_ws.onclose = function (evt) { \n"
"            _webui_ws_status = false; \n"
"            if(_webui_close_reason === _WEBUI_SWITCH) { \n"
"                if(_webui_log) \n"
"                    console.log('WebUI -> Refresh UI'); \n"
"                window.location.replace(_webui_close_value); \n"
"            } else { \n"
"                if(_webui_log) \n"
"                    console.log('WebUI -> Connection lost (' + evt.code + ')'); \n"
"                if(!_webui_log && evt.code != 1005) window.close(); \n"
"                else _webui_freeze_ui(); \n"
"            } \n"
"        }; \n"
"        _webui_ws.onmessage = function (evt) { \n"
"                const buffer8 = new Uint8Array(evt.data); \n"
"                if(buffer8.length < 4) return; \n"
"                if(buffer8[0] !== _WEBUI_SIGNATURE) \n"
"                    return; \n"
"                var len = buffer8.length - 3; \n"
"                if(buffer8[buffer8.length - 1] === 0) \n"
"                   len--; // Null byte (0x00) can break eval() \n"
"                data8 = new Uint8Array(len); \n"
"                for (i = 0; i < len; i++) data8[i] = buffer8[i + 3]; \n"
"                var data8utf8 = new TextDecoder('utf-8').decode(data8); \n"
"                // Process Command \n"
"                if(buffer8[1] === _WEBUI_SWITCH) { \n"
"                    _webui_close(_WEBUI_SWITCH, data8utf8); \n"
"                } else if(buffer8[1] === _WEBUI_CLOSE) { \n"
"                    _webui_close(_WEBUI_CLOSE); \n"
"                } else if(buffer8[1] === _WEBUI_JS) { \n"
"                    data8utf8 = data8utf8.replace(/(?:\\r\\n|\\r|\\n)/g, \"\\\\n\"); \n"
"                    if(_webui_log) \n"
"                        console.log('WebUI -> JS [' + data8utf8 + ']'); \n"
"                    var FunReturn = 'undefined'; \n"
"                    var FunError = false; \n"
"                    try { FunReturn = eval('(() => {' + data8utf8 + '})()'); } catch (e) { FunError = true; FunReturn = e.message } \n"
"                    if(typeof FunReturn === 'undefined' || FunReturn === undefined) FunReturn = 'undefined'; \n"
"                    if(_webui_log && !FunError) console.log('WebUI -> JS -> Return [' + FunReturn + ']'); \n"
"                    if(_webui_log && FunError) console.log('WebUI -> JS -> Error [' + FunReturn + ']'); \n"
"                    var FunReturn8 = new TextEncoder('utf-8').encode(FunReturn); \n"
"                    var Return8 = new Uint8Array(4 + FunReturn8.length); \n"
"                    Return8[0] = _WEBUI_SIGNATURE; \n"
"                    Return8[1] = _WEBUI_JS; \n"
"                    Return8[2] = buffer8[2]; \n"
"                    if(FunError) Return8[3] = 1; \n"
"                    else Return8[3] = 0; \n"
"                    var p = -1; \n"
"                    for (i = 4; i < FunReturn8.length + 4; i++) Return8[i] = FunReturn8[++p]; \n"
"                    if(_webui_ws_status) _webui_ws.send(Return8.buffer); \n"
"                } \n"
"        }; \n"
"    } else { \n"
"        alert('Sorry. WebSocket not supported by your Browser.'); \n"
"        if(!_webui_log) window.close(); \n"
"    } \n"
"} \n"
"function _webui_clicks_listener() { \n"
"    Object.keys(window).forEach(key=>{ \n"
"        if(/^on(click)/.test(key)) { \n"
"            window.addEventListener(key.slice(2),event=>{ \n"
"                if(event.target.id !== '') { \n"
"                    if(_webui_has_events || _webui_bind_list.includes(_webui_win_num + '/' + event.target.id)) \n"
"                        _webui_send_click(event.target.id); \n"
"                } \n"
"            }); \n"
"        } \n"
"    }); \n"
"} \n"
"function _webui_send_click(elem) { \n"
"    if(_webui_ws_status && elem !== '') { \n"
"        var elem8 = new TextEncoder('utf-8').encode(elem); \n"
"        var packet = new Uint8Array(3 + elem8.length); \n"
"        packet[0] = _WEBUI_SIGNATURE; \n"
"        packet[1] = _WEBUI_CLICK; \n"
"        packet[2] = 0; \n"
"        var p = -1; \n"
"        for (i = 3; i < elem8.length + 3; i++) \n"
"            packet[i] = elem8[++p]; \n"
"        _webui_ws.send(packet.buffer); \n"
"        if(_webui_log) \n"
"            console.log('WebUI -> Click [' + elem + ']'); \n"
"    } \n"
"} \n"
"function _webui_send_event_navigation(url) { \n"
"    if(_webui_ws_status && url !== '') { \n"
"        var url8 = new TextEncoder('utf-8').encode(url); \n"
"        var packet = new Uint8Array(3 + url8.length); \n"
"        packet[0] = _WEBUI_SIGNATURE; \n"
"        packet[1] = _WEBUI_SWITCH; \n"
"        packet[2] = 0; \n"
"        var p = -1; \n"
"        for (i = 3; i < url8.length + 3; i++) \n"
"            packet[i] = url8[++p]; \n"
"        _webui_ws.send(packet.buffer); \n"
"        if(_webui_log) \n"
"            console.log('WebUI -> Navigation [' + url + ']'); \n"
"    } \n"
"} \n"
" // -- APIs -------------------------- \n"
"function webui_fn(fn, value) { \n"
"    if(!_webui_has_events && !_webui_bind_list.includes(_webui_win_num + '/' + fn)) \n"
"        return; \n"
"    var data = ''; \n"
"    if(_webui_ws_status && fn !== '') { \n"
"        if(_webui_log) \n"
"            console.log('WebUI -> Func [' + fn + ']'); \n"
"        var xmlHttp = new XMLHttpRequest(); \n"
"        xmlHttp.open('GET', ('http://localhost:' + _webui_port + '/WEBUI/FUNC/' + fn + '/' + value), false); \n"
"        xmlHttp.send(null); \n"
"        if(xmlHttp.status == 200) \n"
"           data = String(xmlHttp.responseText); \n"
"    } \n"
"    return data; \n"
"} \n"
"function webui_log(status) { \n"
"    if(status) { \n"
"        console.log('WebUI -> Log Enabled.'); \n"
"        _webui_log = true; \n"
"    } else { \n"
"        console.log('WebUI -> Log Disabled.'); \n"
"        _webui_log = false; \n"
"    } \n"
"} \n"
" // -- DOM --------------------------- \n"
"document.addEventListener('keydown', function (e) { \n"
"    // Disable F5 \n"
"    if(e.keyCode === 116) { \n"
"        e.preventDefault(); \n"
"        e.returnValue = false; \n"
"        e.keyCode = 0; \n"
"        return false; \n"
"    } \n"
"}); \n"
"window.onbeforeunload = function () { \n"
"   //_webui_ws.close(); \n"
"}; \n"
"setTimeout(function () { \n"
"    if(!_webui_ws_status_once) { \n"
"        _webui_freeze_ui(); \n"
"        alert('WebUI failed to connect to the background application. Please try again.'); \n"
"        if(!_webui_log) window.close(); \n"
"    } \n"
"}, 1500); \n"
"window.addEventListener('unload', unload_handler, false); \n"
"function unload_handler() { \n"
"    // Unload for 'back' & 'forward' navigation \n"
"    window.removeEventListener('unload', unload_handler, false); \n"
"} \n"
"// Links \n"
"document.addEventListener('click', e => { \n"
"    const attribute = e.target.closest('a'); \n"
"    if(attribute) { \n"
"        const link = attribute.href; \n"
"        e.preventDefault(); \n"
"        _webui_close(_WEBUI_SWITCH, link); \n"
"    } \n"
"}); \n"
"navigation.addEventListener('navigate', (event) => { \n"
"    const url = new URL(event.destination.url); \n"
"    _webui_send_event_navigation(url); \n"
"}); \n"
"// Load \n"
"window.addEventListener('load', _webui_start()); \n";

// -- Heap ----------------------------
static const char* webui_html_served = "<html><head><title>Access Denied</title><style>body{margin:0;background-repeat:no-repeat;background-attachment:fixed;background-color:#FF3CAC;background-image:linear-gradient(225deg,#FF3CAC 0%,#784BA0 45%,#2B86C5 100%);font-family:sans-serif;margin:20px;color:#fff}a{color:#fff}</style></head><body><h2>&#9888; Access Denied</h2><p>You can't access this content<br>because it's already processed.<br><br>The current security policy denies<br>multiple requests.</p><br><a href=\"https://www.webui.me\"><small>WebUI v" WEBUI_VERSION "<small></a></body></html>";
static const char* webui_html_res_not_available = "<html><head><title>Resource Not Available</title><style>body{margin:0;background-repeat:no-repeat;background-attachment:fixed;background-color:#FF3CAC;background-image:linear-gradient(225deg,#FF3CAC 0%,#784BA0 45%,#2B86C5 100%);font-family:sans-serif;margin:20px;color:#fff}a{color:#fff}</style></head><body><h2>&#9888; Resource Not Available</h2><p>The requested resource is not available.</p><br><a href=\"https://www.webui.me\"><small>WebUI v" WEBUI_VERSION "<small></a></body></html>";
static const char* webui_deno_not_found = "<html><head><title>Deno Not Found</title><style>body{margin:0;background-repeat:no-repeat;background-attachment:fixed;background-color:#FF3CAC;background-image:linear-gradient(225deg,#FF3CAC 0%,#784BA0 45%,#2B86C5 100%);font-family:sans-serif;margin:20px;color:#fff}a{color:#fff}</style></head><body><h2>&#9888; Deno Not Found</h2><p>Deno is not found on this system.<br>Please download it from <a href=\"https://github.com/denoland/deno/releases\">https://github.com/denoland/deno/releases</a></p><br><a href=\"https://www.webui.me\"><small>WebUI v" WEBUI_VERSION "<small></a></body></html>";
static const char* webui_nodejs_not_found = "<html><head><title>Node.js Not Found</title><style>body{margin:0;background-repeat:no-repeat;background-attachment:fixed;background-color:#FF3CAC;background-image:linear-gradient(225deg,#FF3CAC 0%,#784BA0 45%,#2B86C5 100%);font-family:sans-serif;margin:20px;color:#fff}a{color:#fff}</style></head><body><h2>&#9888; Node.js Not Found</h2><p>Node.js is not found on this system.<br>Please download it from <a href=\"https://nodejs.org/en/download/\">https://nodejs.org/en/download/</a></p><br><a href=\"https://www.webui.me\"><small>WebUI v" WEBUI_VERSION "<small></a></body></html>";
static const char* webui_def_icon = "<?xml version=\"1.0\" ?><svg height=\"24\" version=\"1.1\" width=\"24\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:cc=\"http://creativecommons.org/ns#\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"><g transform=\"translate(0 -1028.4)\"><path d=\"m3 1030.4c-1.1046 0-2 0.9-2 2v7 2 7c0 1.1 0.8954 2 2 2h9 9c1.105 0 2-0.9 2-2v-7-2-7c0-1.1-0.895-2-2-2h-9-9z\" fill=\"#2c3e50\"/><path d=\"m3 2c-1.1046 0-2 0.8954-2 2v3 3 1 1 1 3 3c0 1.105 0.8954 2 2 2h9 9c1.105 0 2-0.895 2-2v-3-4-2-3-3c0-1.1046-0.895-2-2-2h-9-9z\" fill=\"#34495e\" transform=\"translate(0 1028.4)\"/><path d=\"m4 5.125v1.125l3 1.75-3 1.75v1.125l5-2.875-5-2.875zm5 4.875v1h5v-1h-5z\" fill=\"#ecf0f1\" transform=\"translate(0 1028.4)\"/></g></svg>";
static const char* webui_def_icon_type = "image/svg+xml";
static const char* webui_js_empty = "WEBUI_JS_EMPTY";
static const char* webui_js_timeout = "WEBUI_JS_TIMEOUT";
static const char* const webui_empty_string = ""; // In case the compiler optimization is disabled

#ifdef _WIN32
    static const char* webui_sep = "\\";
#else
    static const char* webui_sep = "/";
#endif

#define WEBUI_NON_EXIST_BROWSER (100)

// -- Functions -----------------------
bool _webui_ptr_exist(void *p) {
    
    #ifdef WEBUI_LOG
        // printf("[0] _webui_ptr_exist()... \n");
    #endif

    if(p == NULL)
        return false;
    
    for(unsigned int i = 0; i < webui.ptr_position; i++) {

        if(webui.ptr_list[i] == p)
            return true;
    }

    return false;
}

void _webui_ptr_add(void *p, size_t size) {
    
    #ifdef WEBUI_LOG
        // printf("[0] _webui_ptr_add(0x%p)... \n", p);
    #endif

    if(p == NULL)
        return;

    if(!_webui_ptr_exist(p)) {

        for(unsigned int i = 0; i < webui.ptr_position; i++) {

            if(webui.ptr_list[i] == NULL) {

                #ifdef WEBUI_LOG
                    printf("[0] _webui_ptr_add(0x%p)... Allocate %d bytes\n", p, (int)size);
                #endif

                webui.ptr_list[i] = p;
                webui.ptr_size[i] = size;
                return;
            }
        }

        #ifdef WEBUI_LOG
            printf("[0] _webui_ptr_add(0x%p)... Allocate %d bytes\n", p, (int)size);
        #endif

        webui.ptr_list[webui.ptr_position] = p;
        webui.ptr_size[webui.ptr_position] = size;
        webui.ptr_position++;
        if(webui.ptr_position >= WEBUI_MAX_ARRAY)
            webui.ptr_position = (WEBUI_MAX_ARRAY - 1);
    }
}

void webui_clean_mem(void* p) {

    #ifdef WEBUI_LOG
        printf("[0] webui_clean_mem(0x%p)... \n", p);
    #endif

    _webui_free_mem((void *) &p);
}

void _webui_free_mem(void **p) {
    
    #ifdef WEBUI_LOG
        // printf("[0] _webui_free_mem(0x%p)... \n", *p);
    #endif

    if(p == NULL || *p == NULL)
        return;

    for(unsigned int i = 0; i < webui.ptr_position; i++) {

        if(webui.ptr_list[i] == *p) {

            #ifdef WEBUI_LOG
                printf("[0] _webui_free_mem(0x%p)... Free %d bytes\n", *p, (int)webui.ptr_size[i]);
            #endif

            memset(*p, 0x00, webui.ptr_size[i]);
            free(*p);

            webui.ptr_size[i] = 0;
            webui.ptr_list[i] = NULL;
        }
    }

    for(int i = webui.ptr_position; i >= 0; i--) {

        if(webui.ptr_list[i] == NULL) {

            webui.ptr_position = i;
            break;
        }
    }

    *p = NULL;
}

void _webui_free_all_mem(void) {
    
    #ifdef WEBUI_LOG
        printf("[0] _webui_free_all_mem()... \n");
    #endif

    // Makes sure we run this once
    static bool freed = false;
    if(freed) return;
    freed = true;

    void* ptr = NULL;
    for(unsigned int i = 0; i < webui.ptr_position; i++) {

        ptr = webui.ptr_list[i];

        if(ptr != NULL) {

            #ifdef WEBUI_LOG
                printf("[0] _webui_free_all_mem()... Free %d bytes @ 0x%p\n", (int)webui.ptr_size[i], ptr);
            #endif

            memset(ptr, 0x00, webui.ptr_size[i]);
            free(ptr);
        }
    }
}

void _webui_panic(void) {
    
    #ifdef WEBUI_LOG
        printf("[0] _webui_panic()... \n");
    #endif

    webui_exit();
    exit(EXIT_FAILURE);
}

size_t round_to_memory_block(int size) {

    // If size is negative
    if(size < 4)
        size = 4;

    // If size is already a full block
    // we should return the same block
    size--;

    int block_size = 4;
    while (block_size <= size)
        block_size *= 2;

    return (size_t)block_size;
}

void* _webui_malloc(int size) {
    
    #ifdef WEBUI_LOG
        // printf("[0] _webui_malloc([%d])... \n", size);
    #endif
    
    // Make sure we have the null
    // terminator if it's a string
    size++;

    size = round_to_memory_block(size);

    void* block = NULL;
    for(unsigned int i = 0; i < 8; i++) {

        if(size > WEBUI_MAX_BUF)
            size = WEBUI_MAX_BUF;

        block = malloc(size);

        if(block == NULL)
            size++;
        else
            break;
    }

    if(block == NULL) {

        _webui_panic();
        return NULL;
    }

    memset(block, 0x00, size);

    _webui_ptr_add((void *) block, size);

    return block;
}

void _webui_sleep(long unsigned int ms) {
    
    #ifdef WEBUI_LOG
        // printf("[0] _webui_sleep([%d])... \n", ms);
    #endif

    #ifdef _WIN32
        Sleep(ms);
    #else
        usleep(ms);
    #endif
}

long _webui_timer_diff(struct timespec *start, struct timespec *end) {

    #ifdef WEBUI_LOG
        // printf("[0] _webui_timer_diff()... \n");
    #endif

    return (
        (long)(end->tv_sec * 1000) +
        (long)(end->tv_nsec / 1000000)) -
        ((long)(start->tv_sec * 1000) +
        (long)(start->tv_nsec / 1000000)
    );
}

void _webui_timer_clock_gettime(struct timespec *spec) {

    #ifdef WEBUI_LOG
        // printf("[0] _webui_timer_clock_gettime()... \n");
    #endif

    #ifdef _WIN32
        __int64 wintime;
        GetSystemTimeAsFileTime((FILETIME*)&wintime);
        wintime      -= ((__int64)116444736000000000);
        spec->tv_sec  = wintime / ((__int64)10000000);
        spec->tv_nsec = wintime % ((__int64)10000000) * 100;
    #else
        clock_gettime(CLOCK_MONOTONIC, spec);
    #endif
}

void _webui_timer_start(webui_timer_t* t) {

    #ifdef WEBUI_LOG
        // printf("[0] _webui_timer_start()... \n");
    #endif
    
    _webui_timer_clock_gettime(&t->start);
}

bool _webui_timer_is_end(webui_timer_t* t, unsigned int ms) {

    #ifdef WEBUI_LOG
        // printf("[0] _webui_timer_is_end()... \n");
    #endif
    
    _webui_timer_clock_gettime(&t->now);

    unsigned int def = (unsigned int) _webui_timer_diff(&t->start, &t->now);
    if(def > ms)
        return true;
    return false;
}

#ifdef WEBUI_LOG
    void _webui_print_hex(const char* data, size_t len) {

        for(size_t i = 0; i < len; i++) {

            printf("0x%02X ", (unsigned char) *data);
            data++;
        }
    }
#endif

bool _webui_is_empty(const char* s) {

    #ifdef WEBUI_LOG
        // printf("[0] _webui_is_empty()... \n");
    #endif

    if((s != NULL) && (s[0] != '\0'))
        return false;
    return true;
}

bool _webui_file_exist_mg(void *ev_data) {
    
    #ifdef WEBUI_LOG
        printf("[0] _webui_file_exist_mg()... \n");
    #endif

    char* file;
    char* full_path;

    struct mg_http_message *hm = (struct mg_http_message *) ev_data;

    // Get file name
    file = (char*) _webui_malloc(hm->uri.len);
    const char* p = hm->uri.ptr;
    p++; // Skip "/"
    sprintf(file, "%.*s", (int)(hm->uri.len - 1), p);

    // Get full path
    // [current folder][/][file]
    full_path = (char*) _webui_malloc(strlen(webui.executable_path) + 1 + strlen(file));
    sprintf(full_path, "%s%s%s", webui.executable_path, webui_sep, file);

    bool exist = _webui_file_exist(full_path);

    _webui_free_mem((void *) &file);
    _webui_free_mem((void *) &full_path);

    return exist;
}

bool _webui_file_exist(char* file) {
    
    #ifdef WEBUI_LOG
        printf("[0] _webui_file_exist([%s])... \n", file);
    #endif

    if(_webui_is_empty(file))
        return false;

    if(WEBUI_FILE_EXIST(file, 0) == 0)
        return true;
    return false;
}

const char* _webui_get_extension(const char*f) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_get_extension()... \n");
    #endif

    if(f == NULL)
        return webui_empty_string;

    const char*ext = strrchr(f, '.');

    if(ext == NULL || !ext || ext == f)
        return webui_empty_string;
    return ext + 1;
}

unsigned int _webui_get_run_id(void) {
    
    #ifdef WEBUI_LOG
        printf("[0] _webui_get_run_id()... \n");
    #endif

    return ++webui.run_last_id;
}

bool _webui_socket_test_listen_mg(unsigned int port_num) {
    
    #ifdef WEBUI_LOG
        printf("[0] _webui_socket_test_listen_mg([%d])... \n", port_num);
    #endif

    struct mg_connection *c;
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);

    char url[32];
    sprintf(url, "http://localhost:%d", port_num);

    if((c = mg_http_listen(&mgr, url, NULL, &mgr)) == NULL) {

        // Cannot listen
        mg_mgr_free(&mgr);
        return false;
    }

    // Listening success
    mg_mgr_free(&mgr);

    return true;
}

#ifdef _WIN32
    bool _webui_socket_test_listen_win32(unsigned int port_num) {
    
        #ifdef WEBUI_LOG
            printf("[0] _webui_socket_test_listen_win32([%d])... \n", port_num);
        #endif

        WSADATA wsaData;
        unsigned int iResult;
        SOCKET ListenSocket = INVALID_SOCKET;
        struct addrinfo *result = NULL;
        struct addrinfo hints;

        // Initialize Winsock
        iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if(iResult != 0) {
            // WSAStartup failed
            return false;
        }
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        // Resolve the server address and port
        char the_port[16];
        sprintf(&the_port[0], "%d", port_num);
        iResult = getaddrinfo("127.0.0.1", &the_port[0], &hints, &result);
        if(iResult != 0) {
            // WSACleanup();
            return false;
        }

        // Create a SOCKET for the server to listen for client connections.
        ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if(ListenSocket == INVALID_SOCKET) {
            freeaddrinfo(result);
            // WSACleanup();
            return false;
        }

        // Setup the TCP listening socket
        iResult = bind(ListenSocket, result->ai_addr, (unsigned int)result->ai_addrlen);
        if(iResult == SOCKET_ERROR) {
            freeaddrinfo(result);
            closesocket(ListenSocket);
            shutdown(ListenSocket, SD_BOTH);
            // WSACleanup();
            return false;
        }

        // Clean
        freeaddrinfo(result);
        closesocket(ListenSocket);
        shutdown(ListenSocket, SD_BOTH);
        // WSACleanup();

        // Listening Success
        return true;
    }
#endif

bool _webui_port_is_used(unsigned int port_num) {
    
    #ifdef WEBUI_LOG
        printf("[0] _webui_port_is_used([%d])... \n", port_num);
    #endif

    #ifdef _WIN32
        // Listener test
        if(!_webui_socket_test_listen_win32(port_num))
            return true; // Port is already used
        return false; // Port is not in use
    #else
        // Listener test MG
        if(!_webui_socket_test_listen_mg(port_num))
            return true; // Port is already used
        return false; // Port is not in use
    #endif
}

void _webui_serve_file(webui_window_t* win, struct mg_connection *c, void *ev_data) {

    #ifdef WEBUI_LOG
        printf("[%d] _webui_serve_file()... \n", win->core.window_number);
    #endif

    // Serve a normal text based file
    // send with HTTP 200 status code

    struct mg_http_serve_opts opts = {

        .root_dir = win->path
    };

    mg_http_serve_dir(c, ev_data, &opts);
}

bool _webui_deno_exist(void) {
    
    #ifdef WEBUI_LOG
        printf("[0] _webui_deno_exist()... \n");
    #endif

    static bool found = false;

    if(found)
        return true;

    if(_webui_cmd_sync("deno --version", false) == 0) {

        found = true;
        return true;
    }
    else
        return false;
}

bool _webui_nodejs_exist(void) {
    
    #ifdef WEBUI_LOG
        printf("[0] _webui_nodejs_exist()... \n");
    #endif

    static bool found = false;

    if(found)
        return true;

    if(_webui_cmd_sync("node -v", false) == 0) {

        found = true;
        return true;
    }
    else
        return false;
}

const char* _webui_interpret_command(const char* cmd) {
    
    #ifdef WEBUI_LOG
        printf("[0] _webui_interpret_command()... \n");
    #endif

    // Redirect stderr to stdout
    char cmd_redirected[1024];
    sprintf(cmd_redirected, "%s 2>&1", cmd);

    FILE *runtime = WEBUI_POPEN(cmd_redirected, "r");

    if(runtime == NULL)
        return NULL;

    // Get STDOUT length
    // int c;
    // while ((c = fgetc(runtime)) != EOF)
    //     len++;
    int len = 1024 * 8;

    // Read STDOUT
    char* out = (char*) _webui_malloc(len);
    char* line = (char*) _webui_malloc(4000);
    while(fgets(line, 4000, runtime) != NULL)
        strcat(out, line);

    WEBUI_PCLOSE(runtime);
    _webui_free_mem((void *) &line);

    return (const char*) out;
}

void _webui_interpret_file(webui_window_t* win, struct mg_connection *c, void *ev_data, char* index) {

    #ifdef WEBUI_LOG
        printf("[%d] _webui_interpret_file()... \n", win->core.window_number);
    #endif

    // Run the JavaScript / TypeScript runtime
    // and send back the output with HTTP 200 status code
    // otherwise, send the file as a normal text based one

    char* file;
    char* full_path;

    if(!_webui_is_empty(index)) {

        // Parse index file
        file = index;
        full_path = index;
    }
    else {

        // Parse other files

        struct mg_http_message *hm = (struct mg_http_message *) ev_data;

        // Get file name
        file = (char*) _webui_malloc(hm->uri.len);
        const char* p = hm->uri.ptr;
        p++; // Skip "/"
        sprintf(file, "%.*s", (int)(hm->uri.len - 1), p);

        // Get full path
        // [current folder][/][file]
        full_path = (char*) _webui_malloc(strlen(webui.executable_path) + 1 + strlen(file));
        sprintf(full_path, "%s%s%s", webui.executable_path, webui_sep, file);

        if(!_webui_file_exist(full_path)) {

            // File not exist - 404
            _webui_serve_file(win, c, ev_data);

            _webui_free_mem((void *) &file);
            _webui_free_mem((void *) &full_path);
            return;
        }
    }

    // Get file extension
    const char* extension = _webui_get_extension(file);

    if(strcmp(extension, "ts") == 0 || strcmp(extension, "js") == 0) {

        // TypeScript / JavaScript

        if(win->core.runtime == webui.runtime.deno) {

            // Use Deno

            if(_webui_deno_exist()) {

                // Set command
                // [disable coloring][file]
                char* cmd = (char*) _webui_malloc(64 + strlen(full_path));
                #ifdef _WIN32
                    sprintf(cmd, "Set NO_COLOR=1 & deno run --allow-all \"%s\"", full_path);
                #else
                    sprintf(cmd, "NO_COLOR=1 & deno run --allow-all \"%s\"", full_path);
                #endif

                // Run command
                const char* out = _webui_interpret_command(cmd);

                if(out != NULL) {

                    // Send deno output
                    mg_http_reply(
                        c, 200,
                        "",
                        out
                    );
                }
                else {

                    // Deno failed.
                    // Serve as a normal text-based file
                    _webui_serve_file(win, c, ev_data);
                }

                _webui_free_mem((void *) &cmd);
                _webui_free_mem((void *) &out);
            }
            else {

                // Deno not installed

                mg_http_reply(
                    c, 200,
                    "",
                    webui_deno_not_found
                );
            }
        }
        else if(win->core.runtime == webui.runtime.nodejs) {

            // Use Nodejs

            if(_webui_nodejs_exist()) {

                // Set command
                // [node][file]
                char* cmd = (char*) _webui_malloc(16 + strlen(full_path));
                sprintf(cmd, "node \"%s\"", full_path);

                // Run command
                const char* out = _webui_interpret_command(cmd);

                if(out != NULL) {

                    // Send Node.js output
                    mg_http_reply(
                        c, 200,
                        "",
                        out
                    );
                }
                else {

                    // Node.js failed.
                    // Serve as a normal text-based file
                    _webui_serve_file(win, c, ev_data);
                }

                _webui_free_mem((void *) &cmd);
                _webui_free_mem((void *) &out);
            }
            else {

                // Node.js not installed

                mg_http_reply(
                    c, 200,
                    "",
                    webui_nodejs_not_found
                );
            }
        }
        else {

            // Unknown runtime
            // Serve as a normal text-based file
            _webui_serve_file(win, c, ev_data);
        }
    }
    else {

        // Unknown file extension
        // Serve as a normal text-based file
        _webui_serve_file(win, c, ev_data);
    }

    _webui_free_mem((void *) &file);
    _webui_free_mem((void *) &full_path);
}

const char* _webui_generate_js_bridge(webui_window_t* win) {

    #ifdef WEBUI_LOG
        printf("[%d] _webui_generate_js_bridge()... \n", win->core.window_number);
    #endif

    // Calculate the cb size
    size_t cb_mem_size = 64; // To hold 'const _webui_bind_list = ["elem1", "elem2",];'
    for(unsigned int i = 1; i < WEBUI_MAX_ARRAY; i++)
        if(!_webui_is_empty(webui.html_elements[i]))
            cb_mem_size += strlen(webui.html_elements[i]) + 3;
    
    // Generate the cb array
    char* event_cb_js_array = (char*) _webui_malloc(cb_mem_size);
    strcat(event_cb_js_array, "const _webui_bind_list = [");
    for(unsigned int i = 1; i < WEBUI_MAX_ARRAY; i++) {
        if(!_webui_is_empty(webui.html_elements[i])) {
            strcat(event_cb_js_array, "\"");
            strcat(event_cb_js_array, webui.html_elements[i]);
            strcat(event_cb_js_array, "\",");
        }
    }
    strcat(event_cb_js_array, "]; \n");

    // Generate the full WebUI JS-Bridge
    size_t len = cb_mem_size + strlen(webui_javascript_bridge) + 1;
    char* js = (char*) _webui_malloc(len);
    sprintf(js, 
        "_webui_port = %d; \n _webui_win_num = %d; \n %s \n %s \n",
        win->core.server_port, win->core.window_number, event_cb_js_array, webui_javascript_bridge
    );

    return js;
}

static void _webui_server_event_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {

    webui_window_t* win = (webui_window_t *) fn_data;
    
    #ifdef WEBUI_LOG
        // printf("[%d] _webui_server_event_handler()... \n", win->core.window_number);
    #endif

    if(ev == MG_EV_OPEN) {

        // c->is_hexdumping = 1;
    }
    else if(ev == MG_EV_HTTP_MSG) {

        struct mg_http_message *hm = (struct mg_http_message *) ev_data;

        if(mg_http_match_uri(hm, "/_ws")) {

            // WebSocket

            #ifdef WEBUI_LOG
                printf("[%d] _webui_server_event_handler()... HTML Upgrade to WebSocket\n", win->core.window_number);
            #endif

            mg_ws_upgrade(c, hm, NULL);
        } 
        else if(mg_http_match_uri(hm, "/webui.js")) {

            // WebUI JS-Bridge

            #ifdef WEBUI_LOG
                printf("[%d] _webui_server_event_handler()... HTML WebUI JS\n", win->core.window_number);
            #endif

            // Generate JavaScript bridge
            const char* js = _webui_generate_js_bridge(win);

            // Header
            // Content-Type: text/javascript

            // Send
            mg_http_reply(
                c, 200,
                "",
                js
            );

            _webui_free_mem((void *) &js);
        }
        else if(mg_http_match_uri(hm, "/favicon.ico") || 
                mg_http_match_uri(hm, "/favicon.svg") || 
                mg_http_match_uri(hm, "/favicon.png")) {

            // Send favicon

            if(win->core.icon) {

                // Custom icon
                
                // TODO: Add core.icon_type to the header

                // Header
                // ...

                // User icon
                mg_http_reply(
                    c, 200,
                    "",
                    win->core.icon
                );
            }
            else if(win->core.server_root) {

                // Local icon file
                _webui_serve_file(win, c, ev_data);
            }
            else {

                // Default embedded icon

                // TODO: Use webui_def_icon_type

                // Header
                // Content-Type: image/svg+xml
                
                // Default icon
                mg_http_reply(
                    c, 200,
                    "",
                    webui_def_icon
                );
            }
        }
        else if(mg_http_match_uri(hm, "/")) {

            // [/]

            if(win->core.server_root) {

                // Serve local files

                #ifdef WEBUI_LOG
                    printf("[%d] _webui_server_event_handler()... HTML Root Index\n", win->core.window_number);
                #endif

                win->core.server_handled = true;

                // Set full path
                // [Path][Sep][File Name]
                char* index = (char*) _webui_malloc(strlen(webui.executable_path) + 1 + 8); 

                // Index.ts
                sprintf(index, "%s%sindex.ts", webui.executable_path, webui_sep);
                if(_webui_file_exist(index)) {

                    // TypeScript Index
                    _webui_interpret_file(win, c, ev_data, index);

                   _webui_free_mem((void *) &index);
                    return;
                }

                // Index.js
                sprintf(index, "%s%sindex.js", webui.executable_path, webui_sep);
                if(_webui_file_exist(index)) {

                    // JavaScript Index
                    _webui_interpret_file(win, c, ev_data, index);

                    _webui_free_mem((void *) &index);
                    return;
                }

                _webui_free_mem((void *) &index);
                
                // Index.html
                // Serve as a normal HTML text-based file
                _webui_serve_file(win, c, ev_data);
            }
            else {

                // Main HTML

                if(!win->core.multi_access && win->core.server_handled) {

                    // Main HTML already handled.
                    // Forbidden 403

                    #ifdef WEBUI_LOG
                        printf("[%d] _webui_server_event_handler()... HTML Main Already Handled (403)\n", win->core.window_number);
                    #endif

                    // Header
                    // text/html; charset=utf-8

                    mg_http_reply(
                        c, 403,
                        "",
                        webui_html_served
                    );
                }
                else {

                    // Send main HTML

                    #ifdef WEBUI_LOG
                        printf("[%d] _webui_server_event_handler()... HTML Main\n", win->core.window_number);
                    #endif

                    win->core.server_handled = true;
                    char* html = (char*) webui_empty_string;

                    if(win->core.html != NULL) {

                        // Generate the full WebUI JS-Bridge
                        const char* js = _webui_generate_js_bridge(win);

                        // Inject WebUI JS-Bridge into HTML
                        size_t len = strlen(win->core.html) + strlen(js) + 128;
                        html = (char*) _webui_malloc(len);
                        sprintf(html, 
                            "%s \n <script type = \"text/javascript\"> \n %s \n </script>",
                            win->core.html, js
                        );

                        _webui_free_mem((void *) &js);
                    }

                    // // HTTP Header
                    // char header[512];
                    // memset(header, 0x00, 512);
                    // sprintf(header,
                    //     "HTTP/1.1 200 OK\r\n"
                    //     "Content-Type: text/html; charset=utf-8\r\n"
                    //     "Host: localhost:%d\r\n"
                    //     "Cache-Control: no-cache\r\n"
                    //     "Content-Length: %d\r\n"
                    //     "Connection: close\r\n\r\n",
                    //     win->core.server_port, strlen(html)
                    // );

                    // Send
                    mg_http_reply(
                        c, 200,
                        "",
                        html
                    );

                    _webui_free_mem((void *) &html);
                }
            }
        }
        else if(strncmp(hm->uri.ptr, "/WEBUI/FUNC/", 12) == 0 && hm->uri.len >= 15) {
            
            // Function Call (With response)

            // [/WEBUI/FUNC/ELEMENT_ID/DATA]
            // 0            12

            #ifdef WEBUI_LOG
                printf("[%d] _webui_server_event_handler()... CB start\n", win->core.window_number);
            #endif

            // Copy packet
            size_t len = hm->uri.len;
            char* packet = (char*) _webui_malloc(len);
            memcpy(packet, hm->uri.ptr, len);

            // Get html element id
            char* element = &packet[12];
            size_t element_len = 0;
            for (size_t i = 12; i < len; i++) {
                if(packet[i] == '/') {
                    packet[i] = '\0';
                    break;
                }
                element_len++;
            }

            // [/WEBUI/FUNC/ELEMENT_ID DATA]
            // 0            12

            // Get data
            void* data = &packet[11 + element_len + 2];
            size_t data_len = strlen(data);

            // Generate WebUI internal id
            char* webui_internal_id = _webui_generate_internal_id(win, element);

            // Call user function
            webui_event_t e;
            e.window_id = win->core.window_number;
            e.element_name = element;
            e.window = win;
            e.data = data;
            e.response = NULL;
            e.type = WEBUI_EVENT_CALLBACK;

            unsigned int cb_index = _webui_get_cb_index(webui_internal_id);

            // Check for bind
            if(cb_index > 0 && webui.cb[cb_index] != NULL) {

                // Call user cb
                e.element_id = cb_index;
                webui.cb[cb_index](&e);
            }

            if(_webui_is_empty(e.response))
                e.response = (char*)webui_empty_string;

            #ifdef WEBUI_LOG
                printf("[%d] _webui_server_event_handler()... user-callback response [%s]\n", win->core.window_number, (const char*)e.response);
            #endif

            // Send response
            mg_http_reply(
                c, 200,
                "",
                e.response
            );

            // Free
            _webui_free_mem((void *) &packet);
            _webui_free_mem((void *) &webui_internal_id);

            // Free data allocated by user callback
            if(e.response != NULL) {
                if(_webui_ptr_exist(e.response)) {
                    // This block of memory is allocated by WebUI
                    // for example the user callback used webui_return_int()
                    // It's totally safe to free it right now
                    _webui_free_mem((void *) &e.response);
                }
                else {
                    // This block of memory is allocated by 
                    // the user-callback in another language
                    // for example Python, Rust, Golang...
                    // We should not free it, it's unsafe.
                    e.response = NULL;
                }
            }
        }
        else {

            // [/file]

            if(win->core.server_root) {

                if(win->core.runtime != webui.runtime.none) {

                    // Interpret file

                    #ifdef WEBUI_LOG
                        printf("[%d] _webui_server_event_handler()... HTML Interpret file\n", win->core.window_number);
                    #endif

                    _webui_interpret_file(win, c, ev_data, NULL);
                }
                else {

                    // Serve local files

                    #ifdef WEBUI_LOG
                        printf("[%d] _webui_server_event_handler()... HTML Root file\n", win->core.window_number);
                    #endif

                    // Serve as a normal text-based file
                    _webui_serve_file(win, c, ev_data);
                }
            }
            else {

                // This is a non-server-folder mode
                // but the HTML body request a local file
                // this request can be css, js, image, etc...

                if(_webui_file_exist_mg(ev_data)) {

                    // Serve as a normal text-based file
                    _webui_serve_file(win, c, ev_data);
                }
                else {

                    // 404

                    #ifdef WEBUI_LOG
                        printf("[%d] _webui_server_event_handler()... HTML 404\n", win->core.window_number);
                    #endif

                    // Header
                    // text/html; charset=utf-8

                    mg_http_reply(
                        c, 404,
                        "",
                        webui_html_res_not_available
                    );
                }
            }
        }
    }
    else if(ev == MG_EV_WS_MSG) {

        #ifdef WEBUI_LOG
            printf("[%d] _webui_server_event_handler()... WebSocket Data\n", win->core.window_number);
        #endif

        struct mg_ws_message *wm = (struct mg_ws_message *) ev_data;

        // Parse the packet
        _webui_window_receive(win, wm->data.ptr, wm->data.len);
    }
    else if(ev == MG_EV_WS_OPEN) {

        #ifdef WEBUI_LOG
            printf("[%d] _webui_server_event_handler()... WebSocket Connected\n", win->core.window_number);
        #endif

        int event_type = WEBUI_EVENT_CONNECTED;

        if(!win->core.connected) {

            // First connection

            win->core.connected = true;                         // server thread
            webui.connections++;                                // main loop
            webui.mg_connections[win->core.window_number] = c;  // websocket send func
        }
        else {

            if(win->core.multi_access) {

                // Multi connections
                win->core.connections++;
                event_type = WEBUI_EVENT_MULTI_CONNECTION;
            }
            else {

                // UNWANTED Multi connections

                #ifdef WEBUI_LOG
                    printf("[%d] _webui_server_event_handler() -> UNWANTED Multi Connections\n", win->core.window_number);
                #endif

                mg_close_conn(c);
                event_type = WEBUI_EVENT_UNWANTED_CONNECTION;
            }
        }

        // Generate WebUI internal id
        char* webui_internal_id = _webui_generate_internal_id(win, "");

        _webui_window_event(
            win,                // Window
            webui_internal_id,  // WebUI Internal ID
            "",                 // User HTML ID
            NULL,               // User Custom Data
            0,                  // User Data Len
            event_type          // Type of this event
        );
    }
    else if(ev == MG_EV_WS_CTL) {

        #ifdef WEBUI_LOG
            printf("[%d] _webui_server_event_handler()... WebSocket Closed\n", win->core.window_number);
        #endif

        if(win->core.connected) {

            if(win->core.multi_access && win->core.connections > 0) {

                // Multi connections close
                win->core.connections--;
            }
            else {

                // Main connection close
                webui.connections--;            // main loop
                win->core.connected = false;    // server thread                
            }
        }

        // Generate WebUI internal id
        char* webui_internal_id = _webui_generate_internal_id(win, "");

        _webui_window_event(
            win,                        // Window
            webui_internal_id,          // WebUI Internal ID
            "",                         // User HTML ID
            NULL,                       // User Custom Data
            0,                          // User Data Len
            WEBUI_EVENT_DISCONNECTED    // Type of this event
        );
    }
}

#ifdef _WIN32
    DWORD WINAPI webui_server_start(LPVOID arg) 
#else
    void* webui_server_start(void* arg)
#endif
{
    webui_window_t* win = (webui_window_t*) arg;
    
    #ifdef WEBUI_LOG
        printf("[%d] [Thread] webui_server_start(%s)... \n", win->core.window_number, win->core.url);
    #endif

    // Initialization
    webui.servers++;
    win->core.server_running = true;
    unsigned int timeout = webui.startup_timeout;
    if(timeout < 1)
        timeout = 1;

    // Start Server
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    webui.mg_mgrs[win->core.window_number] = &mgr;

    if(mg_http_listen(&mgr, win->core.url, _webui_server_event_handler, (void *)win) != NULL) {

        if(webui.use_timeout) {

            #ifdef WEBUI_LOG
                printf("[%d] [Thread] webui_server_start(%s)... Listening Success -> Loop (%d seconds timeout)... \n", win->core.window_number, win->core.url, timeout);
            #endif

            bool stop = false;

            for(;;) {

                if(!win->core.server_handled) {

                    // Wait for first connection
                    webui_timer_t timer;
                    _webui_timer_start(&timer);
                    for(;;) {

                        // Stop if window is connected
                        mg_mgr_poll(&mgr, 1);
                        if(win->core.connected)
                            break;

                        // Stop if timer is finished
                        if(_webui_timer_is_end(&timer, (timeout * 1000)))
                            break;
                    }

                    if(!win->core.connected && webui.timeout_extra && win->core.server_handled) {

                        // At this moment the browser is already started and HTML
                        // is already handled, so, let's wait more time to give
                        // the WebSocket an extra time to connect
                        
                        _webui_timer_start(&timer);
                        for(;;) {

                            // Stop if window is connected
                            mg_mgr_poll(&mgr, 1);
                            if(win->core.connected)
                                break;

                            // Stop if timer is finished
                            if(_webui_timer_is_end(&timer, (timeout * 1000)))
                                break;
                        }
                    }
                    
                    if(!win->core.connected)
                        stop = true;
                }
                else {

                    for(;;) {

                        // Wait forever for disconnection

                        mg_mgr_poll(&mgr, 1);

                        // Exit signal
                        if(webui.exit_now) {
                            stop = true;
                            break;
                        }

                        if(!win->core.connected) {

                            // The UI is just get disconnected
                            // let's wait for re-connection...

                            webui_timer_t timer;
                            _webui_timer_start(&timer);
                            for(;;) {

                                // Stop if window is re-connected
                                mg_mgr_poll(&mgr, 1);
                                if(win->core.connected)
                                    break;

                                // Stop if all process get closed
                                if(webui.process < 1) {

                                    stop = true;
                                    break;
                                }

                                // Stop if timer is finished
                                if(_webui_timer_is_end(&timer, 2500))
                                    break;
                            }

                            if(!win->core.connected)
                                break;
                        }
                    }

                    if(win->core.server_handled)
                        stop = true;
                }

                if(stop)
                    break;
            }
        }

        // Let's check the flag again, there is a change
        // that the flag is changed during the first loop
        // for example when set_timeout() get called later
        if(!webui.use_timeout) {

            #ifdef WEBUI_LOG
                printf("[%d] [Thread] webui_server_start(%s)... Listening Success -> Infinite Loop... \n", win->core.window_number, win->core.url);
            #endif

            // Wait forever
            for(;;) {

                mg_mgr_poll(&mgr, 1);
                if(webui.exit_now)
                    break;
            }
        }
    }
    else {

        #ifdef WEBUI_LOG
            printf("[%d] [Thread] webui_server_start(%s)... Listening failed\n", win->core.window_number, win->core.url);
        #endif
    }

    // Stop server
    mg_mgr_free(&mgr);
    webui.servers--;

    #ifdef WEBUI_LOG
        printf("[%d] [Thread] webui_server_start()... Server Stop.\n", win->core.window_number);
    #endif

    // Clean
    win->core.server_running = false;
    win->core.server_handled = false;
    win->core.connected = false;
    webui.mg_mgrs[win->core.window_number] = NULL;
    webui.mg_connections[win->core.window_number] = NULL;
    _webui_free_port(win->core.server_port);

    #ifdef _WIN32
        return 0;
    #else
        pthread_exit(NULL);
    #endif
}

bool _webui_browser_create_profile_folder(webui_window_t* win, unsigned int browser) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_browser_create_profile_folder(%d)... \n", browser);
    #endif
    
    // Custom Browser
    if(browser == webui.browser.custom) {
        // Check the struct pointer
        if(webui.custom_browser == NULL)
            return false;
        return true;
    }

    const char* temp = _webui_browser_get_temp_path(browser);

    if(browser == webui.browser.chrome) {

        // Google Chrome
        sprintf(win->core.profile_path, "%s%s.WebUI%sWebUIChromeProfile", temp, webui_sep, webui_sep);
        return true;
    }
    else if(browser == webui.browser.edge) {

        // Edge
        sprintf(win->core.profile_path, "%s%s.WebUI%sWebUIEdgeProfile", temp, webui_sep, webui_sep);
        return true;
    }
    else if(browser == webui.browser.epic) {

        // Epic
        sprintf(win->core.profile_path, "%s%s.WebUI%sWebUIEpicProfile", temp, webui_sep, webui_sep);
        return true;
    }
    else if(browser == webui.browser.vivaldi) {

        // Vivaldi
        sprintf(win->core.profile_path, "%s%s.WebUI%sWebUIVivaldiProfile", temp, webui_sep, webui_sep);
        return true;
    }
    else if(browser == webui.browser.brave) {

        // Brave
        sprintf(win->core.profile_path, "%s%s.WebUI%sWebUIBraveProfile", temp, webui_sep, webui_sep);
        return true;
    }
    else if(browser == webui.browser.yandex) {

        // Yandex
        sprintf(win->core.profile_path, "%s%s.WebUI%sWebUIYandexProfile", temp, webui_sep, webui_sep);
        return true;
    }
    else if(browser == webui.browser.chromium) {

        // Chromium
        sprintf(win->core.profile_path, "%s%s.WebUI%sWebUIChromiumProfile", temp, webui_sep, webui_sep);
        return true;
    }
    else if(browser == webui.browser.firefox) {

        // Firefox (We need to create a folder)

        char* profile_name = "WebUIFirefoxProfile";

        char firefox_profile_path[1024];
        sprintf(firefox_profile_path, "%s%s.WebUI%s%s", temp, webui_sep, webui_sep, profile_name);
        
        if(!_webui_folder_exist(firefox_profile_path)) {

            char buf[2048];

            sprintf(buf, "%s -CreateProfile \"WebUI %s\"", win->core.browser_path, firefox_profile_path);
            _webui_cmd_sync(buf, false);

            // Creating the browser profile folders timeout...
            for(unsigned int n = 0; n <= (webui.startup_timeout * 4); n++) {

                if(_webui_folder_exist(firefox_profile_path))
                    break;
                
                _webui_sleep(250);
            }

            if(!_webui_folder_exist(firefox_profile_path))
                return false;

            // prefs.js
            FILE *file;
            sprintf(buf, "%s%sprefs.js", firefox_profile_path, webui_sep);
            file = fopen(buf, "a");
            if(file == NULL)
                return false;
            fputs("user_pref(\"toolkit.legacyUserProfileCustomizations.stylesheets\", true); ", file);
            fputs("user_pref(\"browser.shell.checkDefaultBrowser\", false); ", file);
            fputs("user_pref(\"browser.tabs.warnOnClose\", false); ", file);
            fclose(file);

            // userChrome.css
            sprintf(buf, "\"%s%schrome%s\"", firefox_profile_path, webui_sep, webui_sep);
            if(!_webui_folder_exist(buf)) {

                sprintf(buf, "mkdir \"%s%schrome%s\"", firefox_profile_path, webui_sep, webui_sep);
                _webui_cmd_sync(buf, false); // Create directory
            }
            sprintf(buf, "%s%schrome%suserChrome.css", firefox_profile_path, webui_sep, webui_sep);
            file = fopen(buf, "a");
            if(file == NULL)
                return false;
            #ifdef _WIN32
                fputs(":root{--uc-toolbar-height:32px}:root:not([uidensity=\"compact\"]) {--uc-toolbar-height:38px}#TabsToolbar{visibility:collapse!important}:root:not([inFullscreen]) #nav-bar{margin-top:calc(0px - var(--uc-toolbar-height))}#toolbar-menubar{min-height:unset!important;height:var(--uc-toolbar-height)!important;position:relative}#main-menubar{-moz-box-flex:1;background-color:var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor);background-clip:padding-box;border-right:30px solid transparent;border-image:linear-gradient(to left,transparent,var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor) 30px) 20 / 30px}#toolbar-menubar:not([inactive]) {z-index:2}#toolbar-menubar[inactive] > #menubar-items{opacity:0;pointer-events:none;margin-left:var(--uc-window-drag-space-width,0px)}#nav-bar{visibility:collapse}@-moz-document url(chrome://browser/content/browser.xhtml) {:root:not([sizemode=\"fullscreen\"]) > head{display: block;position: fixed;width: calc(200vw - 440px);text-align: left;z-index: 9;pointer-events: none;}head > *{ display: none }head > title{display: -moz-inline-box;padding: 4px;max-width: 50vw;overflow-x: hidden;text-overflow: ellipsis;}}", file);
            #elif __APPLE__
                fputs(":root{--uc-toolbar-height:32px}:root:not([uidensity=\"compact\"]) {--uc-toolbar-height:38px}#TabsToolbar{visibility:collapse!important}:root:not([inFullscreen]) #nav-bar{margin-top:calc(0px - var(--uc-toolbar-height))}#toolbar-menubar{min-height:unset!important;height:var(--uc-toolbar-height)!important;position:relative}#main-menubar{-moz-box-flex:1;background-color:var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor);background-clip:padding-box;border-right:30px solid transparent;border-image:linear-gradient(to left,transparent,var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor) 30px) 20 / 30px}#toolbar-menubar:not([inactive]) {z-index:2}#toolbar-menubar[inactive] > #menubar-items{opacity:0;pointer-events:none;margin-left:var(--uc-window-drag-space-width,0px)}#nav-bar{visibility:collapse}@-moz-document url(chrome://browser/content/browser.xhtml) {:root:not([sizemode=\"fullscreen\"]) > head{display: block;position: fixed;width: calc(200vw - 440px);text-align: left;z-index: 9;pointer-events: none;}head > *{ display: none }head > title{display: -moz-inline-box;padding: 4px;max-width: 50vw;overflow-x: hidden;text-overflow: ellipsis;}}", file);
            #else
                fputs(":root{--uc-toolbar-height:32px}:root:not([uidensity=\"compact\"]) {--uc-toolbar-height:38px}#TabsToolbar{visibility:collapse!important}:root:not([inFullscreen]) #nav-bar{margin-top:calc(0px - var(--uc-toolbar-height))}#toolbar-menubar{min-height:unset!important;height:var(--uc-toolbar-height)!important;position:relative}#main-menubar{-moz-box-flex:1;background-color:var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor);background-clip:padding-box;border-right:30px solid transparent;border-image:linear-gradient(to left,transparent,var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor) 30px) 20 / 30px}#toolbar-menubar:not([inactive]) {z-index:2}#toolbar-menubar[inactive] > #menubar-items{opacity:0;pointer-events:none;margin-left:var(--uc-window-drag-space-width,0px)}#nav-bar{visibility:collapse}@-moz-document url(chrome://browser/content/browser.xhtml) {:root:not([sizemode=\"fullscreen\"]) > head{display: block;position: fixed;width: calc(200vw - 440px);text-align: left;z-index: 9;pointer-events: none;}head > *{ display: none }head > title{display: -moz-inline-box;padding: 4px;max-width: 50vw;overflow-x: hidden;text-overflow: ellipsis;}}", file);
            #endif
            fclose(file);

            sprintf(win->core.profile_path, "%s%s%s", temp, webui_sep, profile_name);
        }

        return true;
    }

    return false;
}

bool _webui_folder_exist(char* folder) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_folder_exist([%s])... \n", folder);
    #endif

    #if defined(_MSC_VER)
        if(GetFileAttributesA(folder) != INVALID_FILE_ATTRIBUTES)
            return true;
    #else
        DIR* dir = opendir(folder);
        if(dir) {
            closedir(dir);
            return true;
        }
    #endif

    return false;
}

char* _webui_generate_internal_id(webui_window_t* win, const char* element) {

    #ifdef WEBUI_LOG
        printf("[%d] _webui_generate_internal_id([%s])... \n", win->core.window_number, element);
    #endif

    // Generate WebUI internal id
    size_t element_len = strlen(element);
    size_t internal_id_size = 3 + 1 + element_len; // [win num][/][name]
    char* webui_internal_id = (char*) _webui_malloc(internal_id_size);
    sprintf(webui_internal_id, "%d/%s", win->core.window_number, element);

    return webui_internal_id;
}

const char* _webui_browser_get_temp_path(unsigned int browser) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_browser_get_temp_path([%d])... \n", browser);
    #endif

    #ifdef _WIN32
        // Resolve %USERPROFILE%
        #ifdef _MSC_VER
            char* WinUserProfile = NULL;
            size_t sz = 0;
            if(_dupenv_s(&WinUserProfile, &sz, "USERPROFILE") != 0 || WinUserProfile == NULL)
                return webui_empty_string;
        #else
            char* WinUserProfile = getenv("USERPROFILE");
            if(WinUserProfile == NULL)
                return webui_empty_string;
        #endif
    #elif __APPLE__
        // Resolve $HOME
        char* MacUserProfile = getenv("HOME");
        if(MacUserProfile == NULL)
            return webui_empty_string;
    #else
        // Resolve $HOME
        char* LinuxUserProfile = getenv("HOME");
        if(LinuxUserProfile == NULL)
            return webui_empty_string;
    #endif

    #ifdef _WIN32
        // WinUserProfile is already a complete path
        return WinUserProfile;
    #elif __APPLE__
        return MacUserProfile;
    #else
        return LinuxUserProfile;
    #endif
}

#ifdef _WIN32
    bool _webui_get_windows_reg_value(HKEY key, const char* reg, const char* value_name, char value[WEBUI_MAX_PATH]) {

        #ifdef WEBUI_LOG
            printf("[0] _webui_get_windows_reg_value([%s])... \n", reg);
        #endif

        HKEY hKey;

        if(RegOpenKeyEx(key, reg, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {

            DWORD valueSize = WEBUI_MAX_PATH;
            // If `value_name` is empty then
            // will read the "(default)" reg-key
            if(RegQueryValueEx(hKey, value_name, NULL, NULL, (LPBYTE)value, &valueSize) == ERROR_SUCCESS) {

                RegCloseKey(hKey);
                return true;
            }
        }

        return false;
    }
#endif

bool _webui_is_google_chrome_folder(const char* folder) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_is_google_chrome_folder([%s])... \n", folder);
    #endif

    char browser_full_path[WEBUI_MAX_PATH];

    // Make sure this folder is Google Chrome setup and not Chromium
    // by checking if `master_preferences` file exist or `initial_preferences`
    // Ref: https://support.google.com/chrome/a/answer/187948?hl=en

    sprintf(browser_full_path, "%s\\master_preferences", folder);
    if(!_webui_file_exist(browser_full_path)) {

        sprintf(browser_full_path, "%s\\initial_preferences", folder);
        if(!_webui_file_exist(browser_full_path))
            return false; // This is Chromium or something else
    }

    // Make sure the browser executable file exist
    sprintf(browser_full_path, "%s\\chrome.exe", folder);
    if(!_webui_file_exist(browser_full_path))
        return false;
    
    return true;
}

bool _webui_browser_exist(webui_window_t* win, unsigned int browser) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_browser_exist([%d])... \n", browser);
    #endif

    // Check if a web browser is installed on this machine

    // Custom Browser
    if(browser == webui.browser.custom) {
        // Check the struct pointer
        if(webui.custom_browser == NULL)
            return false;
        return true;
    }

    if(browser == webui.browser.chrome) {

        // Google Chrome

        #ifdef _WIN32

            // Google Chrome on Windows

            char browser_folder[WEBUI_MAX_PATH];

            // Search in `HKEY_LOCAL_MACHINE` (If Google Chrome installed for multi-user)
            if(_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\chrome.exe", "Path", browser_folder)) {

                // Make sure its Google Chrome and not Chromium
                if(_webui_is_google_chrome_folder(browser_folder)) {

                    // Google Chrome Found (multi-user)
                    sprintf(win->core.browser_path, "\"%s\\chrome.exe\"", browser_folder);
                    return true;
                }
            }

            // Search in `HKEY_CURRENT_USER` (If Google Chrome installed for one user)
            if(_webui_get_windows_reg_value(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\chrome.exe", "Path", browser_folder)) {

                // Make sure its Google Chrome and not Chromium
                if(_webui_is_google_chrome_folder(browser_folder)) {

                    // Google Chrome Found (one user)
                    sprintf(win->core.browser_path, "\"%s\\chrome.exe\"", browser_folder);
                    return true;
                }
            }

            return false;

        #elif __APPLE__

            // Google Chrome on macOS
            if(_webui_cmd_sync("open -R -a \"Google Chrome\"", false) == 0) {

                sprintf(win->core.browser_path, "open -W \"/Applications/Google Chrome.app\" --args");
                return true;
            }
            else
                return false;
        #else

            // Google Chrome on Linux
            if(_webui_cmd_sync("google-chrome --version", false) == 0) {

                sprintf(win->core.browser_path, "google-chrome");
                return true;
            }
            else
                return false;

        #endif
    }
    else if(browser == webui.browser.edge) {

        // Edge

        #ifdef _WIN32

            // Edge on Windows

            char browser_fullpath[WEBUI_MAX_PATH];

            // Search in `HKEY_LOCAL_MACHINE` (If Edge installed for multi-user)
            if(_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\msedge.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Edge Found (multi-user)
                    sprintf(win->core.browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            // Search in `HKEY_CURRENT_USER` (If Edge installed for one user)
            if(_webui_get_windows_reg_value(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\msedge.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Edge Found (one user)
                    sprintf(win->core.browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            return false;

        #elif __APPLE__

            // Edge on macOS
            return false;

        #else

            // Edge on Linux
            return false;

        #endif
    }
    else if(browser == webui.browser.epic) {

        // Epic Privacy Browser

        #ifdef _WIN32

            // Epic on Windows

            char browser_fullpath[WEBUI_MAX_PATH];

            // Search in `HKEY_CURRENT_USER` (If Epic installed for one user)
            if(_webui_get_windows_reg_value(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\epic.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Epic Found (one user)
                    sprintf(win->core.browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            // Search in `HKEY_LOCAL_MACHINE` (If Epic installed for multi-user)
            if(_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\epic.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Epic Found (multi-user)
                    sprintf(win->core.browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            return false;

        #elif __APPLE__

            // Epic on macOS
            if(_webui_cmd_sync("open -R -a \"Epic\"", false) == 0) {

                sprintf(win->core.browser_path, "open -W \"/Applications/Epic.app\" --args");
                return true;
            }
            else
                return false;
        #else

            // Epic on Linux
            if(_webui_cmd_sync("epic --version", false) == 0) {

                sprintf(win->core.browser_path, "epic");
                return true;
            }
            else
                return false;
        #endif
    }
    else if(browser == webui.browser.vivaldi) {

        // Vivaldi Browser

        #ifdef _WIN32

            // Vivaldi on Windows

            char browser_fullpath[WEBUI_MAX_PATH];

            // Search in `HKEY_LOCAL_MACHINE` (If Vivaldi installed for multi-user)
            if(_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\vivaldi.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Vivaldi Found (multi-user)
                    sprintf(win->core.browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            // Search in `HKEY_CURRENT_USER` (If Vivaldi installed for one user)
            if(_webui_get_windows_reg_value(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\vivaldi.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Vivaldi Found (one user)
                    sprintf(win->core.browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            return false;

        #elif __APPLE__

            // Vivaldi on macOS
            if(_webui_cmd_sync("open -R -a \"Vivaldi\"", false) == 0) {

                sprintf(win->core.browser_path, "open -W \"/Applications/Vivaldi.app\" --args");
                return true;
            }
            else
                return false;
        #else

            // Vivaldi on Linux
            if(_webui_cmd_sync("vivaldi --version", false) == 0) {

                sprintf(win->core.browser_path, "vivaldi");
                return true;
            }
            else
                return false;
        #endif
    }
    else if(browser == webui.browser.brave) {

        // Brave Browser

        #ifdef _WIN32

            // Brave on Windows

            char browser_fullpath[WEBUI_MAX_PATH];

            // Search in `HKEY_LOCAL_MACHINE` (If Brave installed for multi-user)
            if(_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\brave.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Brave Found (multi-user)
                    sprintf(win->core.browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            // Search in `HKEY_CURRENT_USER` (If Brave installed for one user)
            if(_webui_get_windows_reg_value(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\brave.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Brave Found (one user)
                    sprintf(win->core.browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            return false;

        #elif __APPLE__

            // Brave on macOS
            if(_webui_cmd_sync("open -R -a \"Brave\"", false) == 0) {

                sprintf(win->core.browser_path, "open -W \"/Applications/Brave.app\" --args");
                return true;
            }
            else
                return false;
        #else

            // Brave on Linux
            if(_webui_cmd_sync("brave-browser --version", false) == 0) {

                sprintf(win->core.browser_path, "brave-browser");
                return true;
            }
            else
                return false;
        #endif
    }
    else if(browser == webui.browser.firefox) {

        // Firefox
        
        #ifdef _WIN32
        
            // Firefox on Windows

            char browser_fullpath[WEBUI_MAX_PATH];

            // Search in `HKEY_LOCAL_MACHINE` (If Firefox installed for multi-user)
            if(_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\firefox.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Firefox Found (multi-user)
                    sprintf(win->core.browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            // Search in `HKEY_CURRENT_USER` (If Firefox installed for one user)
            if(_webui_get_windows_reg_value(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\firefox.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Firefox Found (one user)
                    sprintf(win->core.browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            return false;

        #elif __APPLE__
            
            // Firefox on macOS
            if(_webui_cmd_sync("open -R -a \"firefox\"", false) == 0) {

                sprintf(win->core.browser_path, "open -W \"/Applications/Firefox.app\" --args");
                return true;
            }
            else
                return false;
        #else

            // Firefox on Linux

            if(_webui_cmd_sync("firefox -v", false) == 0) {

                sprintf(win->core.browser_path, "firefox");
                return true;
            }
            else
                return false;

        #endif

    }
    else if(browser == webui.browser.yandex) {

        // Yandex Browser

        #ifdef _WIN32

            // Yandex on Windows

            char browser_fullpath[WEBUI_MAX_PATH];

            // Search in `HKEY_CURRENT_USER` (If Yandex installed for one user)
            if(_webui_get_windows_reg_value(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\browser.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Yandex Found (one user)
                    sprintf(win->core.browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            // Search in `HKEY_LOCAL_MACHINE` (If Yandex installed for multi-user)
            if(_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\browser.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Yandex Found (multi-user)
                    sprintf(win->core.browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            return false;

        #elif __APPLE__

            // Yandex on macOS
            if(_webui_cmd_sync("open -R -a \"Yandex\"", false) == 0) {

                sprintf(win->core.browser_path, "open -W \"/Applications/Yandex.app\" --args");
                return true;
            }
            else
                return false;
        #else

            // Yandex on Linux
            if(_webui_cmd_sync("yandex-browser --version", false) == 0) {

                sprintf(win->core.browser_path, "yandex-browser");
                return true;
            }
            else
                return false;
        #endif
    }
    else if(browser == webui.browser.chromium) {

        // The Chromium Projects

        #ifdef _WIN32

            // Chromium on Windows

            char browser_folder[WEBUI_MAX_PATH];

            // Search in `HKEY_CURRENT_USER` (If Chromium installed for one user)
            if(_webui_get_windows_reg_value(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\chrome.exe", "Path", browser_folder)) {

                // Make sure its Chromium and not Google Chrome
                if(!_webui_is_google_chrome_folder(browser_folder)) {

                    // Chromium Found (one user)
                    sprintf(win->core.browser_path, "\"%s\\chrome.exe\"", browser_folder);
                    return true;
                }
            }

            // Search in `HKEY_LOCAL_MACHINE` (If Chromium installed for multi-user)
            if(_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\chrome.exe", "Path", browser_folder)) {

                // Make sure its Chromium and not Google Chrome
                if(!_webui_is_google_chrome_folder(browser_folder)) {

                    // Chromium Found (multi-user)
                    sprintf(win->core.browser_path, "\"%s\\chrome.exe\"", browser_folder);
                    return true;
                }
            }

            return false;

        #elif __APPLE__

            // Chromium on macOS
            if(_webui_cmd_sync("open -R -a \"Chromium\"", false) == 0) {

                sprintf(win->core.browser_path, "open -W \"/Applications/Chromium.app\" --args");
                return true;
            }
            else
                return false;
        #else

            // Chromium on Linux
            if(_webui_cmd_sync("chromium-browser --version", false) == 0) {

                sprintf(win->core.browser_path, "chromium-browser");
                return true;
            }
            else
                return false;
        #endif
    }

    return false;
}

void _webui_clean(void) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_clean()... \n");
    #endif

    static bool cleaned = false;
    if(cleaned) return;
    cleaned = true;

    // Let's give other threads more time to safely exit
    // and finish their cleaning up.
    _webui_sleep(120);

    // TODO: Add option to let the user decide if
    // WebUI should delete the web browser profile
    // folder or not.

    // Free all non-freed memory allocations
    _webui_free_all_mem();
}

#ifdef _WIN32
    int _webui_system_win32(char* cmd, bool show) {

        #ifdef WEBUI_LOG
            printf("[0] _webui_system_win32()... \n");
        #endif

        DWORD Return = 0;
        DWORD CreationFlags = CREATE_NO_WINDOW;

        /*
        We should not kill this process, because may had many child
        process of other WebUI app instances. Unfortunately, this is
        how modern browsers save memory by combine all windows into one
        single parent process, and we can't control this behavior.

        // Automatically close the browser process when the
        // parent process (this app) get closed. If this fail
        // webui.js will try to close the window.
        HANDLE JobObject = CreateJobObject(NULL, NULL);
        JOB_OBJECT_EXTENDED_LIMIT_INFORMATION ExtendedInfo = { 0 };
        ExtendedInfo.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION | JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        SetInformationJobObject(JobObject, JobObjectExtendedLimitInformation, &ExtendedInfo, sizeof(ExtendedInfo));
        */

        if(show)
            CreationFlags = SW_SHOW;

        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        if(!CreateProcessA(
            NULL,               // No module name (use command line)
            cmd,                // Command line
            NULL,               // Process handle not inheritable
            NULL,               // Thread handle not inheritable
            FALSE,              // Set handle inheritance to FALSE
            CreationFlags,      // Creation flags
            NULL,               // Use parent's environment block
            NULL,               // Use parent's starting directory 
            &si,                // Pointer to STARTUP INFO structure
            &pi))               // Pointer to PROCESS_INFORMATION structure
        {
            // CreateProcess failed
            return -1;
        }

        SetFocus(pi.hProcess);
        // AssignProcessToJobObject(JobObject, pi.hProcess);
        WaitForSingleObject(pi.hProcess, INFINITE);
        GetExitCodeProcess(pi.hProcess, &Return);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        if(Return == 0)
            return 0;
        else
            return -1;
    }
#endif

int _webui_cmd_sync(char* cmd, bool show) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_cmd_sync()... \n");
    #endif

    // Run sync command and
    // return the exit code

    char buf[1024];

    #ifdef _WIN32
        sprintf(buf, "cmd /c \"%s\" > nul 2>&1", cmd);
        #ifdef WEBUI_LOG
            printf("[0] _webui_cmd_sync() -> Running [%s] \n", buf);
        #endif
        return _webui_system_win32(buf, show);
    #else
        sprintf(buf, "%s >>/dev/null 2>>/dev/null ", cmd);
        #ifdef WEBUI_LOG
            printf("[0] _webui_cmd_sync() -> Running [%s] \n", buf);
        #endif
        int r =  system(buf);
        r = (r != -1 && r != 127 && WIFEXITED(r)) ? WEXITSTATUS(r) : -1;
        return r;
    #endif
}

int _webui_cmd_async(char* cmd, bool show) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_cmd_async()... \n");
    #endif

    // Run a async command
    // and return immediately

    char buf[1024];
    int res = 0;

    // Asynchronous command
    #ifdef _WIN32
        sprintf(buf, "START \"\" %s", cmd);
        res = _webui_cmd_sync(buf, show);
    #else
        sprintf(buf, "%s >>/dev/null 2>>/dev/null &", cmd);
        res = _webui_cmd_sync(buf, show);
    #endif

    return res;
}

#ifdef _WIN32
    DWORD WINAPI _webui_run_browser_detect_proc_task(LPVOID _arg)
#else
    void* _webui_run_browser_detect_proc_task(void* _arg)
#endif
{
    webui_cmd_async_t* arg = (webui_cmd_async_t*) _arg;

    #ifdef WEBUI_LOG
        printf("[%d] _webui_run_browser_detect_proc_task()... \n", arg->win->core.window_number);
    #endif

    // Prevent the main loop from closing
    webui.process++;

    // Run command
    _webui_cmd_sync(arg->cmd, false);

    #ifdef WEBUI_LOG
        printf("[%d] _webui_run_browser_detect_proc_task() -> Process closed.\n", arg->win->core.window_number);
    #endif

    // Free memory
    _webui_free_mem((void *) &arg->cmd);

    // The browser process just get closed
    // let the main loop break if there is
    // no other running browser process.
    webui.process--;
    _webui_sleep(250);
    if((webui.process < 1) && (!arg->win->core.connected))
        webui.exit_now = true;

    #ifdef _WIN32
        return 0;
    #else
        pthread_exit(NULL);
    #endif
}

int _webui_run_browser(webui_window_t* win, char* cmd) {

    #ifdef WEBUI_LOG
        printf("[%d] _webui_run_browser()... \n", win->core.window_number);
    #endif

    int res = 0;

    if(win->core.detect_process_close) {

        // Detect window closing event by the browser
        // process, instead of the WS connection status

        webui_cmd_async_t* arg = (webui_cmd_async_t*) _webui_malloc(sizeof(webui_cmd_async_t));
        arg->win = win;
        arg->cmd = (char*) _webui_malloc(strlen(cmd));
        strcpy(arg->cmd, cmd);

        #ifdef _WIN32
            HANDLE user_fun_thread = CreateThread(NULL, 0, _webui_run_browser_detect_proc_task, (void *) arg, 0, NULL);
            if(user_fun_thread != NULL)
                CloseHandle(user_fun_thread);
        #else
            pthread_t thread;
            pthread_create(&thread, NULL, &_webui_run_browser_detect_proc_task, (void *) arg);
            pthread_detach(thread);
        #endif

        // TODO: We need to set 'res = -1' when _webui_run_browser_detect_proc_task() fails. 
    }
    else {

        // Run a async command
        res = _webui_cmd_async(cmd, false);
    }

    return res;
}

bool _webui_browser_start_chrome(webui_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_browser_start_chrome([%s])... \n", address);
    #endif
    
    // -- Google Chrome ----------------------

    if(win->core.CurrentBrowser != 0 && win->core.CurrentBrowser != webui.browser.chrome)
        return false;

    if(!_webui_browser_exist(win, webui.browser.chrome))
        return false;
    
    if(!_webui_browser_create_profile_folder(win, webui.browser.chrome))
        return false;
    
    char arg[1024];
    sprintf(arg, " --user-data-dir=\"%s\" --no-first-run --disable-gpu --disable-software-rasterizer --no-proxy-server --safe-mode --disable-extensions --disable-background-mode --disable-plugins --disable-plugins-discovery --disable-translate --bwsi --disable-sync --incognito --app=", win->core.profile_path);

    char full[1024];
    sprintf(full, "%s%s%s", win->core.browser_path, arg, address);

    if(_webui_run_browser(win, full) == 0) {

        win->core.CurrentBrowser = webui.browser.chrome;
        webui.browser.current = webui.browser.chrome;
        return true;
    }
    else
        return false;
}

bool _webui_browser_start_edge(webui_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_browser_start_edge([%s])... \n", address);
    #endif

    // -- Microsoft Edge ----------------------

    if(win->core.CurrentBrowser != 0 && win->core.CurrentBrowser != webui.browser.edge)
        return false;

    if(!_webui_browser_exist(win, webui.browser.edge))
        return false;
    
    if(!_webui_browser_create_profile_folder(win, webui.browser.edge))
        return false;

    // TODO: We need to disable the Sync message in the first run,
    // we fix it using `--inprivate`, but it add "" in the title bar.

    char arg[1024];
    sprintf(arg, " --user-data-dir=\"%s\" --no-first-run --disable-gpu --disable-software-rasterizer --no-proxy-server --safe-mode --disable-extensions --disable-background-mode --disable-plugins --disable-plugins-discovery --disable-translate --bwsi --disable-sync --inprivate --app=", win->core.profile_path);

    char full[1024];
    sprintf(full, "%s%s%s", win->core.browser_path, arg, address);

    if(_webui_run_browser(win, full) == 0) {

        win->core.CurrentBrowser = webui.browser.edge;
        webui.browser.current = webui.browser.edge;
        return true;
    }
    else
        return false;
}

bool _webui_browser_start_epic(webui_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_browser_start_epic([%s])... \n", address);
    #endif

    // -- Epic Privacy Browser ----------------------

    if(win->core.CurrentBrowser != 0 && win->core.CurrentBrowser != webui.browser.epic)
        return false;

    if(!_webui_browser_exist(win, webui.browser.epic))
        return false;
    
    if(!_webui_browser_create_profile_folder(win, webui.browser.epic))
        return false;

    char arg[1024];
    sprintf(arg, " --user-data-dir=\"%s\" --no-first-run --disable-gpu --disable-software-rasterizer --no-proxy-server --safe-mode --disable-extensions --disable-background-mode --disable-plugins --disable-plugins-discovery --disable-translate --bwsi --disable-sync --incognito --app=", win->core.profile_path);

    char full[1024];
    sprintf(full, "%s%s%s", win->core.browser_path, arg, address);

    if(_webui_run_browser(win, full) == 0) {

        win->core.CurrentBrowser = webui.browser.epic;
        webui.browser.current = webui.browser.epic;
        return true;
    }
    else
        return false;
}

bool _webui_browser_start_vivaldi(webui_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_browser_start_vivaldi([%s])... \n", address);
    #endif

    // -- Vivaldi Browser ----------------------

    if(win->core.CurrentBrowser != 0 && win->core.CurrentBrowser != webui.browser.vivaldi)
        return false;

    if(!_webui_browser_exist(win, webui.browser.vivaldi))
        return false;
    
    if(!_webui_browser_create_profile_folder(win, webui.browser.vivaldi))
        return false;

    char arg[1024];
    sprintf(arg, " --user-data-dir=\"%s\" --no-first-run --disable-gpu --disable-software-rasterizer --no-proxy-server --safe-mode --disable-extensions --disable-background-mode --disable-plugins --disable-plugins-discovery --disable-translate --bwsi --disable-sync --incognito --app=", win->core.profile_path);

    char full[1024];
    sprintf(full, "%s%s%s", win->core.browser_path, arg, address);

    if(_webui_run_browser(win, full) == 0) {

        win->core.CurrentBrowser = webui.browser.vivaldi;
        webui.browser.current = webui.browser.vivaldi;
        return true;
    }
    else
        return false;
}

bool _webui_browser_start_brave(webui_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_browser_start_brave([%s])... \n", address);
    #endif

    // -- Brave Browser ----------------------

    if(win->core.CurrentBrowser != 0 && win->core.CurrentBrowser != webui.browser.brave)
        return false;

    if(!_webui_browser_exist(win, webui.browser.brave))
        return false;
    
    if(!_webui_browser_create_profile_folder(win, webui.browser.brave))
        return false;

    char arg[1024];
    sprintf(arg, " --user-data-dir=\"%s\" --no-first-run --disable-gpu --disable-software-rasterizer --no-proxy-server --safe-mode --disable-extensions --disable-background-mode --disable-plugins --disable-plugins-discovery --disable-translate --bwsi --disable-sync --incognito --app=", win->core.profile_path);

    char full[1024];
    sprintf(full, "%s%s%s", win->core.browser_path, arg, address);

    if(_webui_run_browser(win, full) == 0) {

        win->core.CurrentBrowser = webui.browser.brave;
        webui.browser.current = webui.browser.brave;
        return true;
    }
    else
        return false;
}

bool _webui_browser_start_firefox(webui_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_browser_start_firefox([%s])... \n", address);
    #endif

    // -- Mozilla Firefox ----------------------

    if(win->core.CurrentBrowser != 0 && win->core.CurrentBrowser != webui.browser.firefox)
        return false;

    if(!_webui_browser_exist(win, webui.browser.firefox))
        return false;

    if(!_webui_browser_create_profile_folder(win, webui.browser.firefox))
        return false;

    char full[1024];
    sprintf(full, "%s -P WebUI -purgecaches -new-window -private-window %s", win->core.browser_path, address);

    if(_webui_run_browser(win, full) == 0) {

        win->core.CurrentBrowser = webui.browser.firefox;
        webui.browser.current = webui.browser.firefox;
        return true;
    }
    else
        return false;
}

bool _webui_browser_start_yandex(webui_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_browser_start_yandex([%s])... \n", address);
    #endif

    // -- Yandex Browser ----------------------

    if(win->core.CurrentBrowser != 0 && win->core.CurrentBrowser != webui.browser.yandex)
        return false;

    if(!_webui_browser_exist(win, webui.browser.yandex))
        return false;
    
    if(!_webui_browser_create_profile_folder(win, webui.browser.yandex))
        return false;

    char arg[1024];
    sprintf(arg, " --user-data-dir=\"%s\" --no-first-run --disable-gpu --disable-software-rasterizer --no-proxy-server --safe-mode --disable-extensions --disable-background-mode --disable-plugins --disable-plugins-discovery --disable-translate --bwsi --disable-sync --incognito --app=", win->core.profile_path);

    char full[1024];
    sprintf(full, "%s%s%s", win->core.browser_path, arg, address);

    if(_webui_run_browser(win, full) == 0) {

        win->core.CurrentBrowser = webui.browser.yandex;
        webui.browser.current = webui.browser.yandex;
        return true;
    }
    else
        return false;
}

bool _webui_browser_start_chromium(webui_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_browser_start_chromium([%s])... \n", address);
    #endif
    
    // -- The Chromium Projects -------------------

    if (win->core.CurrentBrowser != 0 && win->core.CurrentBrowser != webui.browser.chromium)
        return false;

    if (!_webui_browser_exist(win, webui.browser.chromium))
        return false;
    
    if (!_webui_browser_create_profile_folder(win, webui.browser.chromium))
        return false;
    
    char arg[1024];
    sprintf(arg, " --user-data-dir=\"%s\" --no-first-run --disable-gpu --disable-software-rasterizer --no-proxy-server --safe-mode --disable-extensions --disable-background-mode --disable-plugins --disable-plugins-discovery --disable-translate --bwsi --disable-sync --incognito --app=", win->core.profile_path);

    char full[1024];
    sprintf(full, "%s%s%s", win->core.browser_path, arg, address);

    if (_webui_run_browser(win, full) == 0) {

        win->core.CurrentBrowser = webui.browser.chromium;
        webui.browser.current = webui.browser.chromium;
        return true;
    }
    else
        return false;
}

bool _webui_browser_start_custom(webui_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_browser_start_custom([%s])... \n", address);
    #endif
    
    // -- Custom Browser ----------------------

    if(win->core.CurrentBrowser != 0 && win->core.CurrentBrowser != webui.browser.custom)
        return false;

    if(!_webui_browser_exist(win, webui.browser.custom))
        return false;
    
    if(!_webui_browser_create_profile_folder(win, webui.browser.custom))
        return false;

    char full[1024];
    if(webui.custom_browser->auto_link)
        sprintf(full, "%s %s%s", webui.custom_browser->app, webui.custom_browser->arg, address);
    else
        sprintf(full, "%s %s", webui.custom_browser->app, webui.custom_browser->arg);

    if(_webui_run_browser(win, full) == 0) {

        win->core.CurrentBrowser = webui.browser.custom;
        webui.browser.current = webui.browser.custom;
        return true;
    }
    else
        return false;
}

bool _webui_browser_start(webui_window_t* win, const char* address, unsigned int browser) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_browser_start([%s], [%d])... \n", address, browser);
    #endif

    // Non existing browser
    if(browser > 10)
        return false;
    
    // Current browser
    if(browser == webui.browser.any && webui.browser.current != 0)
        browser = webui.browser.current;

    // TODO: Convert address from [/...] to [file://...]

    if(browser != 0) {

        // User specified browser

        if(browser == webui.browser.chrome)
            return _webui_browser_start_chrome(win, address);
        else if(browser == webui.browser.edge)
            return _webui_browser_start_edge(win, address);
        else if(browser == webui.browser.epic)
            return _webui_browser_start_epic(win, address);
        else if(browser == webui.browser.vivaldi)
            return _webui_browser_start_vivaldi(win, address);
        else if(browser == webui.browser.brave)
            return _webui_browser_start_brave(win, address);
        else if(browser == webui.browser.firefox)
            return _webui_browser_start_firefox(win, address);
        else if(browser == webui.browser.yandex)
            return _webui_browser_start_yandex(win, address);
        else if(browser == webui.browser.chromium)
            return _webui_browser_start_chromium(win, address);
        else if(browser == webui.browser.custom)
            return _webui_browser_start_custom(win, address);
        else
            return false;
    }
    else if(win->core.CurrentBrowser != 0) {

        // Already used browser

        if(win->core.CurrentBrowser == webui.browser.chrome)
            return _webui_browser_start_chrome(win, address);
        else if(win->core.CurrentBrowser == webui.browser.edge)
            return _webui_browser_start_edge(win, address);
        else if(win->core.CurrentBrowser == webui.browser.epic)
            return _webui_browser_start_epic(win, address);
        else if(win->core.CurrentBrowser == webui.browser.vivaldi)
            return _webui_browser_start_vivaldi(win, address);
        else if(win->core.CurrentBrowser == webui.browser.brave)
            return _webui_browser_start_brave(win, address);
        else if(win->core.CurrentBrowser == webui.browser.firefox)
            return _webui_browser_start_firefox(win, address);
        else if(win->core.CurrentBrowser == webui.browser.yandex)
            return _webui_browser_start_yandex(win, address);
        else if(browser == webui.browser.chromium)
            return _webui_browser_start_chromium(win, address);
        else if(win->core.CurrentBrowser == webui.browser.custom)
            return _webui_browser_start_custom(win, address);
        else
            return false;
    }
    else {

        // Default OS browser

        #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
            // Windows
            if(!_webui_browser_start_chrome(win, address))
                if(!_webui_browser_start_edge(win, address))
                    if(!_webui_browser_start_epic(win, address))
                        if(!_webui_browser_start_vivaldi(win, address))
                            if(!_webui_browser_start_brave(win, address))
                                if(!_webui_browser_start_firefox(win, address))
                                    if(!_webui_browser_start_yandex(win, address))
                                        if(!_webui_browser_start_chromium(win, address))
                                            if(!_webui_browser_start_custom(win, address))
                                                return false;
        #elif __APPLE__
            // macOS
            if(!_webui_browser_start_chrome(win, address))
                if(!_webui_browser_start_edge(win, address))
                    if(!_webui_browser_start_epic(win, address))
                        if(!_webui_browser_start_vivaldi(win, address))
                            if(!_webui_browser_start_brave(win, address))
                                if(!_webui_browser_start_firefox(win, address))
                                    if(!_webui_browser_start_yandex(win, address))
                                        if(!_webui_browser_start_chromium(win, address))
                                            if(!_webui_browser_start_custom(win, address))
                                                return false;
        #else
            // Linux
            if(!_webui_browser_start_chrome(win, address))
                if(!_webui_browser_start_edge(win, address))
                    if(!_webui_browser_start_epic(win, address))
                        if(!_webui_browser_start_vivaldi(win, address))
                            if(!_webui_browser_start_brave(win, address))
                                if(!_webui_browser_start_firefox(win, address))
                                    if(!_webui_browser_start_yandex(win, address))
                                        if(!_webui_browser_start_chromium(win, address))
                                            if(!_webui_browser_start_custom(win, address))
                                                return false;
        #endif
    }

    return true;
}

void _webui_window_open(webui_window_t* win, char* link, unsigned int browser) {

    #ifdef WEBUI_LOG
        printf("[%d] _webui_window_open([%s], [%d])... \n", win->core.window_number, link, browser);
    #endif

    // Just open an app-mode window using this link
    _webui_browser_start(win, link, browser);
}

void webui_script_cleanup(webui_script_t* script) {

    _webui_free_mem((void *) &script->result.data);
    _webui_free_mem((void *) &script->script);
}

void webui_script(webui_window_t* win, webui_script_t* script) {

    #ifdef WEBUI_LOG
        printf("[%d] webui_script([%s])... \n", win->core.window_number, script->script);
    #endif

    _webui_init();

    size_t js_len = strlen(script->script);

    if(js_len < 1) {

        _webui_free_mem((void *) &script->result.data);
        script->result.data = webui_js_empty;
        script->result.length = (unsigned int) strlen(webui_js_empty);
        script->result.error = true;
        return;
    }

    // Initializing js result
    _webui_free_mem((void *) &script->result.data);
    script->result.data = webui_js_timeout;
    script->result.length = (unsigned int) strlen(webui_js_timeout);
    script->result.error = true;
    
    // Initializing pipe
    unsigned int run_id = _webui_get_run_id();
    webui.run_done[run_id] = false;
    webui.run_error[run_id] = false;
    _webui_free_mem((void *) &webui.run_responses[run_id]);

    // Prepare the packet
    size_t packet_len = 3 + js_len + 1; // [header][js]
    char* packet = (char*) _webui_malloc(packet_len);
    packet[0] = WEBUI_HEADER_SIGNATURE; // Signature
    packet[1] = WEBUI_HEADER_JS;        // Type
    packet[2] = run_id;                 // ID
    for(unsigned int i = 0; i < js_len; i++)     // Data
        packet[i + 3] = script->script[i];
    
    // Send packets
    _webui_window_send(win, packet, packet_len);
    _webui_free_mem((void *) &packet);

    // Wait for UI response
    if(script->timeout < 1 || script->timeout > 86400) {

        for(;;) {

            if(webui.run_done[run_id])
                break;
            
            _webui_sleep(1);
        }
    }
    else {

        for(unsigned int n = 0; n <= (script->timeout * 1000); n++) {

            if(webui.run_done[run_id])
                break;
            
            _webui_sleep(1);
        }
    }

    if(webui.run_responses[run_id] != NULL) {

        script->result.data = webui.run_responses[run_id];
        script->result.length = (unsigned int) strlen(webui.run_responses[run_id]);
        script->result.error = webui.run_error[run_id];
    }
}

webui_window_t* webui_new_window(void) {

    #ifdef WEBUI_LOG
        printf("[0] webui_new_window()... \n");
    #endif

    _webui_init();

    webui_window_t* win = (webui_window_t*) _webui_malloc(sizeof(webui_window_t));

    // Initialisation
    win->core.window_number = _webui_get_new_window_number();
    win->core.browser_path = (char*) _webui_malloc(WEBUI_MAX_PATH);
    win->core.profile_path = (char*) _webui_malloc(WEBUI_MAX_PATH);
    win->path = (char*) _webui_malloc(WEBUI_MAX_PATH);
    sprintf(win->path, "%s", WEBUI_DEFAULT_PATH);
    
    #ifdef WEBUI_LOG
        printf("[0] webui_new_window() -> New window @ 0x%p\n", win);
    #endif

    return win;
}

void webui_close(webui_window_t* win) {

    #ifdef WEBUI_LOG
        printf("[%d] webui_close()... \n", win->core.window_number);
    #endif

    _webui_init();

    if(win->core.connected) {

        // Prepare packets
        char* packet = (char*) _webui_malloc(4);
        packet[0] = WEBUI_HEADER_SIGNATURE; // Signature
        packet[1] = WEBUI_HEADER_CLOSE;     // Type
        packet[2] = 0;                      // ID
        packet[3] = 0;                      // Data

        // Send packets
        _webui_window_send(win, packet, 4);
        _webui_free_mem((void *) &packet);
    }
}

bool webui_is_shown(webui_window_t* win) {

    #ifdef WEBUI_LOG
        printf("[%d] webui_is_shown()... \n", win->core.window_number);
    #endif

    return win->core.connected;
}

bool webui_is_any_window_running(void) {

    #ifdef WEBUI_LOG
        printf("[0] webui_is_any_window_running()... \n");
    #endif

    _webui_init();
    
    if(webui.connections > 0)
        return true;
    
    return false;
}

unsigned int _webui_window_get_number(webui_window_t* win) {

    #ifdef WEBUI_LOG
        printf("[%d] _webui_window_get_number()... \n", win->core.window_number);
    #endif
    
    return win->core.window_number;
}

const char* webui_new_server(webui_window_t* win, const char* path) {

    #ifdef WEBUI_LOG
        printf("[%d] webui_new_server()... \n", win->core.window_number);
    #endif

    _webui_init();

    // Root folder to serve
    if(!_webui_set_root_folder(win, path))
        return webui_empty_string;
    
    // Prevent the server from using the
    // timeout-mode while waiting for connections
    webui_set_timeout(0);
    
    // WEBUI_NON_EXIST_BROWSER is to prevent
    // any browser from running. Because we want 
    // to only to run a web-server this time.
    _webui_show_window(win, NULL, WEBUI_NON_EXIST_BROWSER);

    // Wait for server to start
    for(unsigned int n = 0; n < 500; n++) {

        if(win->core.server_running)
            break;
        
        _webui_sleep(1);
    }

    return (const char*) win->core.url;
}

bool _webui_set_root_folder(webui_window_t* win, const char* path) {

    #ifdef WEBUI_LOG
        printf("[%d] _webui_set_root_folder([%s])... \n", win->core.window_number, path);
    #endif

    if((path == NULL) || (strlen(path) > WEBUI_MAX_PATH))
        return false;

    win->core.server_root = true;

    if(_webui_is_empty(path))
        sprintf(win->path, "%s", WEBUI_DEFAULT_PATH);
    else
        sprintf(win->path, "%s", path);
    
    webui_multi_access(win, true);

    return true;
}

void webui_multi_access(webui_window_t* win, bool status) {

    #ifdef WEBUI_LOG
        printf("[%d] webui_multi_access([%d])... \n", win->core.window_number, status);
    #endif

    win->core.multi_access = status;
}

void webui_set_icon(webui_window_t* win, const char* icon_s, const char* type_s) {

    #ifdef WEBUI_LOG
        printf("[%d] webui_set_icon([%s], [%s])... \n", win->core.window_number, icon_s, type_s);
    #endif

    win->core.icon = icon_s;
    win->core.icon_type = type_s;
}

bool webui_open(webui_window_t* win, const char* url, unsigned int browser) {

    #ifdef WEBUI_LOG
        printf("[%d] webui_open()... \n", win->core.window_number);
    #endif

    // Just open an app-mode window using the link
    // webui_set_timeout(0);
    _webui_wait_process(win, true);

    if(webui_is_shown(win)) {

        // Refresh an existing running window

        #ifdef WEBUI_LOG
            printf("[%d] webui_open()... Refresh the running window to [%s]...\n", win->core.window_number, url);
        #endif

        // Prepare packets
        size_t packet_len = 3 + strlen(url) + 1; // [header][url]
        char* packet = (char*) _webui_malloc(packet_len);
        packet[0] = WEBUI_HEADER_SIGNATURE; // Signature
        packet[1] = WEBUI_HEADER_SWITCH;    // Type
        packet[2] = 0;                      // ID
        for(unsigned int i = 0; i < strlen(url); i++)  // Data
            packet[i + 3] = url[i];

        // Send the packet
        _webui_window_send(win, packet, packet_len);
        _webui_free_mem((void *) &packet);

        return true;
    }
    
    // New window
    return _webui_browser_start(win, url, browser);
}

bool webui_show(webui_window_t* win, const char* content) {

    #ifdef WEBUI_LOG
        printf("[%d] webui_show()... \n", win->core.window_number);
    #endif

    size_t content_len = strlen(content);

    // Some wrappers does not guarantee `content` to
    // stay valid, so, let's make a copy right now.
    char* content_cpy = (char*) webui_empty_string;
    if(content_len > 1) {
        content_cpy = _webui_malloc(content_len);
        memcpy(content_cpy, content, content_len);
    }

    // Check if this is an HTML script or a file name
    if(strstr(content_cpy, "<html")) {

        // Handel the static HTML script
        #ifdef WEBUI_LOG
            printf("[%d] webui_show()... -> Static HTML Script:\n", win->core.window_number);
            printf("- - -[HTML]- - - - - - - - - -\n%s\n- - - - - - - - - - - - - - - -\n", content_cpy);
        #endif
        win->core.server_root = false;
        return _webui_show_window(win, content_cpy, webui.browser.any);
    }
    
    // Handel the file
    #ifdef WEBUI_LOG
        printf("[%d] webui_show()... -> File: [%s]\n", win->core.window_number, content_cpy);
    #endif
    if(content_len > WEBUI_MAX_PATH || strstr(content_cpy, "<"))
        return false;
    if(win->core.url == NULL)
        webui_new_server(win, "");
    // URL: [localhost:port][/][filename]
    char* url = (char*) _webui_malloc(strlen(win->core.url) + 1 + content_len);
    sprintf(url, "%s/%s", win->core.url, content_cpy);
    return webui_open(win, url, win->core.CurrentBrowser);
}

bool _webui_show_window(webui_window_t* win, const char* html, unsigned int browser) {

    #ifdef WEBUI_LOG
        printf("[%d] _webui_show_window(html, [%d])... \n", win->core.window_number, browser);
    #endif

    _webui_init();

    // Initializing
    win->core.html = (html == NULL ? webui_empty_string : html);
    win->core.server_handled = false;
    webui.wait_for_socket_window = true;

    // Detect window closing event by the browser
    // process, instead of the WS connection status
    _webui_wait_process(win, true);

    if(!webui_is_shown(win)) {

        // Start a new window

        // Get new port
        unsigned int port = _webui_get_free_port();
        win->core.server_port = port;
        _webui_free_mem((void *) &win->core.url);
        win->core.url = (char*) _webui_malloc(2000);
        sprintf(win->core.url, "http://localhost:%d", port);

        // Run browser
        bool browser_started = _webui_browser_start(win, win->core.url, browser);
        if(browser != WEBUI_NON_EXIST_BROWSER && !browser_started) {

            // Browser not available
            _webui_free_port(win->core.server_port);
            return false;
        }
        
        // New Server Thread
        #ifdef _WIN32
            HANDLE thread = CreateThread(NULL, 0, webui_server_start, (void *) win, 0, NULL);
            win->core.server_thread = thread;
            if(thread != NULL)
                CloseHandle(thread);
        #else
            pthread_t thread;
            pthread_create(&thread, NULL, &webui_server_start, (void *) win);
            pthread_detach(thread);
            win->core.server_thread = thread;
        #endif
    }
    else {

        // Refresh an existing running window

        // Prepare packets
        size_t packet_len = 3 + strlen(win->core.url) + 1; // [header][url]
        char* packet = (char*) _webui_malloc(packet_len);
        packet[0] = WEBUI_HEADER_SIGNATURE; // Signature
        packet[1] = WEBUI_HEADER_SWITCH;    // Type
        packet[2] = 0;                      // ID
        for(unsigned int i = 0; i < strlen(win->core.url); i++)  // Data
            packet[i + 3] = win->core.url[i];

        // Send the packet
        _webui_window_send(win, packet, packet_len);
        _webui_free_mem((void *) &packet);
    }

    return true;
}

unsigned int webui_bind(webui_window_t* win, const char* element, void (*func)(webui_event_t* e)) {

    #ifdef WEBUI_LOG
        printf("[%d] webui_bind([%s], [0x%p])... \n", win->core.window_number, element, func);
    #endif

    _webui_init();

    int len = 0;
    if(_webui_is_empty(element))
        win->core.has_events = true;
    else
        len = strlen(element);

    // [win num][/][element]
    char* webui_internal_id = _webui_malloc(3 + 1 + len);
    sprintf(webui_internal_id, "%d/%s", win->core.window_number, element);

    unsigned int cb_index = _webui_get_cb_index(webui_internal_id);

    if(cb_index > 0) {

        // Replace a reference
        webui.cb[cb_index] = func;

        _webui_free_mem((void *) &webui_internal_id);
    }
    else {

        // New reference
        cb_index = _webui_set_cb_index(webui_internal_id);

        if(cb_index > 0)
            webui.cb[cb_index] = func;
        else
            _webui_free_mem((void *) &webui_internal_id);
    }

    return cb_index;
}

#ifdef _WIN32
    DWORD WINAPI _webui_cb(LPVOID _arg)
#else
    void* _webui_cb(void* _arg)
#endif
{
    webui_cb_t* arg = (webui_cb_t*) _arg;

    #ifdef WEBUI_LOG
        printf("[%d] [Thread] _webui_cb()... \n", arg->win->core.window_number);
    #endif

    webui_event_t e;
    e.window_id = arg->win->core.window_number;
    e.element_name = arg->element_name;
    e.window = arg->win;
    e.data = arg->data;
    e.response = NULL;
    e.type = arg->event_type;

    // Check for the events-bind function
    if(arg->win->core.has_events) {

        char* events_id = _webui_generate_internal_id(arg->win, "");
        unsigned int events_cb_index = _webui_get_cb_index(events_id);
        _webui_free_mem((void *) &events_id);

        if(events_cb_index > 0 && webui.cb[events_cb_index] != NULL) {

            // Call user events cb
            e.element_id = 0;
            webui.cb[events_cb_index](&e);
        }
    }

    // Check for the bind function
    if(!_webui_is_empty(arg->element_name)) {

        unsigned int cb_index = _webui_get_cb_index(arg->webui_internal_id);
        if(cb_index > 0 && webui.cb[cb_index] != NULL) {

            // Call user cb
            e.element_id = cb_index;
            webui.cb[cb_index](&e);
        }
    }

    #ifdef WEBUI_LOG
        printf("[%d] [Thread] _webui_cb()... Stop.\n", arg->win->core.window_number);
    #endif    

    // Free
    _webui_free_mem((void *) &arg->webui_internal_id);
    _webui_free_mem((void *) &arg->element_name);
    _webui_free_mem((void *) &arg);

    #ifdef _WIN32
        return 0;
    #else
        pthread_exit(NULL);
    #endif
}

void _webui_window_event(webui_window_t* win, char* webui_internal_id, char* element, void* data, unsigned int data_len, int event_type) {

    #ifdef WEBUI_LOG
        printf("[%d] _webui_window_event([%s], [%s])... \n", win->core.window_number, webui_internal_id, element);
    #endif

    // Create a thread, and call the used cb function
    webui_cb_t* arg = (webui_cb_t*) _webui_malloc(sizeof(webui_cb_t));
    arg->win = win;
    arg->webui_internal_id = webui_internal_id;
    arg->element_name = element;
    arg->event_type = event_type;
    if(data != NULL) {
        arg->data = data;
        arg->data_len = data_len;
    }
    else {
        arg->data = (void*) webui_empty_string;
        arg->data_len = 0;
    }

    #ifdef _WIN32
        HANDLE user_fun_thread = CreateThread(NULL, 0, _webui_cb, (void *) arg, 0, NULL);
        if(user_fun_thread != NULL)
            CloseHandle(user_fun_thread); 
    #else
        pthread_t thread;
        pthread_create(&thread, NULL, &_webui_cb, (void *) arg);
        pthread_detach(thread);
    #endif
}

void _webui_window_send(webui_window_t* win, char* packet, size_t packets_size) {

    #ifdef WEBUI_LOG
        printf("[%d] _webui_window_send([%.*s], [%d])... [ ", win->core.window_number, (int)packets_size, packet, (int)packets_size);
            _webui_print_hex(packet, packets_size);
        printf("]\n");
    #endif
    
    if(!win->core.connected ||
        webui.mg_connections[win->core.window_number] == NULL ||
        packet == NULL ||
        packets_size < 4)
        return;

    struct mg_connection* c = webui.mg_connections[win->core.window_number];
    mg_ws_send(
        c, 
        packet, 
        packets_size, 
        WEBSOCKET_OP_BINARY
    );
}

bool _webui_get_data(const char* packet, size_t packet_len, unsigned int pos, size_t* data_len, char** data) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_get_data()... \n");
    #endif

    if((pos + 1) > packet_len) {

        *data = NULL;
        data_len = 0;
        return false;
    }

    *data = (char*) _webui_malloc((packet_len - pos));

    // Check mem
    if(*data == NULL) {

        data_len = 0;
        return false;
    }

    // Copy data part
    char* p = *data;
    for(unsigned int i = pos; i < packet_len; i++) {

        memcpy(p, &packet[i], 1);
        p++;
    }

    // Check data size
    *data_len = strlen(*data);
    if(*data_len < 1) {

        _webui_free_mem((void *) data);
        *data = NULL;
        data_len = 0;
        return false;
    }

    return true;
}

void _webui_window_receive(webui_window_t* win, const char* packet, size_t len) {

    #ifdef WEBUI_LOG
        printf("[%d] _webui_window_receive([%.*s], [%d])... [ ", win->core.window_number, (int)len, packet, (int)len);
            _webui_print_hex(packet, len);
        printf("]\n");
    #endif

    if((unsigned char) packet[0] != WEBUI_HEADER_SIGNATURE || len < 4)
        return;

    if((unsigned char) packet[1] == WEBUI_HEADER_CLICK) {

        // Click Event

        // 0: [Signature]
        // 1: [Type]
        // 2: 
        // 3: [Data]

        // Get html element id
        char* element;
        size_t element_len;
        if(!_webui_get_data(packet, len, 3, &element_len, &element))
            return;

        // Generate WebUI internal id
        char* webui_internal_id = _webui_generate_internal_id(win, element);

        _webui_window_event(
            win,                // Window
            webui_internal_id,  // WebUI Internal ID
            element,            // User HTML ID
            NULL,               // User Custom Data
            0,                  // User Data Len
            WEBUI_EVENT_MOUSE_CLICK // Type of this event
        );
    }
    else if((unsigned char) packet[1] == WEBUI_HEADER_JS) {

        // JS Result

        // 0: [Signature]
        // 1: [Type]
        // 2: [ID]
        // 3: [Error]
        // 4: [Data]

        // Get pipe id
        unsigned char run_id = packet[2];
        if(run_id < 0x01) {

            // Fatal.
            // The pipe ID is not valid
            // we can't send the ready signal to webui_script()
            return;
        }

        // Get data part
        char* data;
        size_t data_len;
        bool data_status = _webui_get_data(packet, len, 4, &data_len, &data);

        // Get js-error
        bool error = true;
        if((unsigned char) packet[3] == 0x00)
            error = false;

        // Initialize pipe
        _webui_free_mem((void *) &webui.run_responses[run_id]);

        // Set pipe
        if(data_status && data_len > 0) {

            webui.run_error[run_id] = error;
            webui.run_responses[run_id] = data;
        }
        else {

            // Empty Result
            webui.run_error[run_id] = error;
            webui.run_responses[run_id] = webui_empty_string;
        }

        // Send ready signal to webui_script()
        webui.run_done[run_id] = true;
    }
    else if((unsigned char) packet[1] == WEBUI_HEADER_CALL_FUNC) {

        // Function Call (No response)

        // 0: [Signature]
        // 1: [Type]
        // 2: 
        // 3: [ID, Null, Data]

        // Get html element id
        char* element;
        size_t element_len;
        if(!_webui_get_data(packet, len, 3, &element_len, &element))
            return;

        // Get data
        void* data;
        size_t data_len;
        if(!_webui_get_data(packet, len, (3 + element_len + 1), &data_len, (char **) &data))
            return;

        // Generate WebUI internal id
        char* webui_internal_id = _webui_generate_internal_id(win, element);

        _webui_window_event(
            win,                // Window
            webui_internal_id,  // WebUI Internal ID
            element,            // User HTML ID
            data,               // User Custom Data
            data_len,           // User Data Len
            WEBUI_EVENT_CALLBACK    // Type of this event
        );
    }
    else if((unsigned char) packet[1] == WEBUI_HEADER_SWITCH) {

        // Navigation Event

        // 0: [Signature]
        // 1: [Type]
        // 2: 
        // 3: [URL]

        // Get URL
        char* url;
        size_t url_len;
        if(!_webui_get_data(packet, len, 3, &url_len, &url))
            return;

        // Generate WebUI internal id
        char* webui_internal_id = _webui_generate_internal_id(win, "");

        _webui_window_event(
            win,                // Window
            webui_internal_id,  // WebUI Internal ID
            "",                 // HTML ID
            url,                // URL
            url_len,            // URL Len
            WEBUI_EVENT_NAVIGATION // Type of this event
        );
    }
}

const char* webui_get_string(webui_event_t* e) {
    
    #ifdef WEBUI_LOG
        printf("[0] webui_get_string()... \n");
    #endif

    if(e->data != NULL) {
        size_t len = strlen(e->data);
        if(len > 0 && len <= WEBUI_MAX_BUF)
            return (const char *) e->data;
    }

    return webui_empty_string;
}

long long int webui_get_int(webui_event_t* e) {
    
    #ifdef WEBUI_LOG
        printf("[0] webui_get_int()... \n");
    #endif

    char *endptr;

    if(e->data != NULL) {
        size_t len = strlen(e->data);
        if(len > 0 && len <= 20) // 64-bit max is -9,223,372,036,854,775,808 (20 character)
            return strtoll((const char *) e->data, &endptr, 10);
    }
    
    return 0;
}

bool webui_get_bool(webui_event_t* e) {

    #ifdef WEBUI_LOG
        printf("[0] webui_get_bool()... \n");
    #endif

    const char* str = webui_get_string(e);
    if(str[0] == 't' || str[0] == 'T') // true || True
        return true;
    
        return false;
}

void webui_return_int(webui_event_t* e, long long int n) {

    #ifdef WEBUI_LOG
        printf("[%d] webui_return_int([%lld])... \n", e->window_id, n);
    #endif

    // Int to Str
    // 64-bit max is -9,223,372,036,854,775,808 (20 character)
    char* buf = (char*) _webui_malloc(20);
    sprintf(buf, "%lld", n);

    // Set response
    e->response = buf;
}

void webui_return_string(webui_event_t* e, char* s) {

    #ifdef WEBUI_LOG
        printf("[%d] webui_return_string([%s])... \n", e->window_id, s);
    #endif

    if(_webui_is_empty(s))
        return;

    // Copy Str
    int len = strlen(s);
    char* buf = (char*) _webui_malloc(len);
    memcpy(buf, s, len);

    // Set response
    e->response = buf;
}

void webui_return_bool(webui_event_t* e, bool b) {

    #ifdef WEBUI_LOG
        printf("[%d] webui_return_bool([%d])... \n", e->window_id, b);
    #endif

    // Bool to Str
    int len = 1;
    char* buf = (char*) _webui_malloc(len);
    sprintf(buf, "%d", b);

    // Set response
    e->response = buf;
}

void _webui_wait_process(webui_window_t* win, bool status) {

    #ifdef WEBUI_LOG
        printf("[%d] _webui_wait_process()... \n", win->core.window_number);
    #endif

    win->core.detect_process_close = status;
}

char* _webui_get_current_path(void) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_get_current_path()... \n");
    #endif

    char* path = (char*) _webui_malloc(WEBUI_MAX_PATH);
    if(WEBUI_GET_CURRENT_DIR (path, WEBUI_MAX_PATH) == NULL)
        path[0] = 0x00;

    return path;
}

void _webui_set_custom_browser(webui_custom_browser_t* p) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_set_custom_browser()... \n");
    #endif

    webui.custom_browser = p;
}

void webui_exit(void) {

    #ifdef WEBUI_LOG
        printf("[0] webui_exit()... \n");
    #endif

    webui.wait_for_socket_window = false;
    webui.exit_now = true;

    // Let's give other threads more time to 
    // safely exit and finish their cleaning up.
    _webui_sleep(100);
}

bool webui_is_app_running(void) {

    #ifdef WEBUI_LOG
        // printf("[0] webui_is_app_running()... \n");
    #endif

    static bool app_is_running = true;

    // Stop if already flagged
    if(!app_is_running) return false;

    // Initialization
    if(!webui.initialized)
    _webui_init();
    
    // Get app status
    if(webui.exit_now) {
        app_is_running = false;
    }
    else if(webui.use_timeout) {
        if(webui.wait_for_socket_window) {
            if(webui.servers < 1)
                app_is_running = false;
        }
    }
    else {

        // Unknown status!
        app_is_running = false;
    }

    // Final cleaning
    if(!app_is_running)
        _webui_clean();

    return app_is_running;
}

void webui_wait(void) {

    #ifdef WEBUI_LOG
        printf("[L] webui_wait()... \n");
    #endif

    _webui_init();

    if(webui.use_timeout) {

        #ifdef WEBUI_LOG
            printf("[L] webui_wait() -> Using timeout %d second\n", webui.startup_timeout);
        #endif

        // Wait for browser to start
        _webui_wait_for_startup();

        if(webui.wait_for_socket_window) {

            #ifdef WEBUI_LOG
                printf("[L] webui_wait() -> Wait for connected UI...\n");
            #endif

            while(webui.servers > 0) {

                #ifdef WEBUI_LOG
                    // printf("[%d/%d]", webui.servers, webui.connections);
                #endif
                _webui_sleep(50);
            }
        }
        else {

            // Probably the app didn't use the show() function
            // so, there is no window running.

            #ifdef WEBUI_LOG
                printf("[L] webui_wait() -> Ignoring connected UI.\n");
            #endif
        }
    }
    else {

        #ifdef WEBUI_LOG
            printf("[L] webui_wait() -> Infinite wait...\n");
        #endif

        // Infinite wait
        while(!webui.exit_now)
            _webui_sleep(50);
    }

    #ifdef WEBUI_LOG
        printf("[L] webui_wait() -> Wait finished.\n");
    #endif

    // Final cleaning
    _webui_clean();
}

void _webui_free_port(unsigned int port) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_free_port([%d])... \n", port);
    #endif

    for(unsigned int i = 0; i < WEBUI_MAX_ARRAY; i++) {
        if(webui.used_ports[i] == port) {
            webui.used_ports[i] = 0;
            break;
        }
    }
}

void _webui_wait_for_startup(void) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_wait_for_startup()... \n");
    #endif

    if(webui.connections > 0)
        return;

    // Wait for a specific time
    for(unsigned int n = 0; n <= (webui.startup_timeout * 10); n++) {

        if(webui.connections > 0)
            break;
        
        // We should wait 100ms, but the server thread
        // may add an extras 3 second to the main loop.
        _webui_sleep(50);
    }

    #ifdef WEBUI_LOG
        printf("[0] _webui_wait_for_startup() -> Finish.\n");
    #endif
}

void webui_set_timeout(unsigned int second) {

    #ifdef WEBUI_LOG
        printf("[0] webui_set_timeout([%d])... \n", second);
    #endif

    _webui_init();

    if(second < 1) {

        webui.use_timeout = false;
    }
    else {

        webui.use_timeout = true;
        webui.startup_timeout = second;
        webui.timeout_extra = false;
    }
}

unsigned int _webui_get_new_window_number(void) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_get_new_window_number()... \n");
    #endif

    return ++webui.last_window;
}

unsigned int _webui_get_free_port(void) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_get_free_port()... \n");
    #endif

    #ifdef _WIN32
        srand((unsigned int)time(NULL));
    #else
        srand(time(NULL));
    #endif

    unsigned int port = (rand() % (WEBUI_MAX_PORT + 1 - WEBUI_MIN_PORT)) + WEBUI_MIN_PORT;

    for(unsigned int i = WEBUI_MIN_PORT; i <= WEBUI_MAX_PORT; i++) {

        // Search [port] in [webui.used_ports]
        bool found = false;
        for(unsigned int j = 0; j < WEBUI_MAX_ARRAY; j++) {
            if(webui.used_ports[j] == port) {
                found = true;
                break;
            }
        }

        if(found)
            // Port used by local window
            port = (rand() % (WEBUI_MAX_PORT + 1 - WEBUI_MIN_PORT)) + WEBUI_MIN_PORT;
        else {
            if(_webui_port_is_used(port))
                // Port used by an external app
                port = (rand() % (WEBUI_MAX_PORT + 1 - WEBUI_MIN_PORT)) + WEBUI_MIN_PORT;
            else
                // Port is free
                break;
        }
    }

    // Add
    for(unsigned int i = 0; i < WEBUI_MAX_ARRAY; i++) {
        if(webui.used_ports[i] == 0) {
            webui.used_ports[i] = port;
            break;
        }
    }

    return port;
}

void webui_script_runtime(webui_window_t* win, unsigned int runtime) {

    #ifdef WEBUI_LOG
        printf("[%d] webui_script_runtime(%d)... \n", win->core.window_number, runtime);
    #endif

    _webui_init();

    if(runtime != webui.runtime.deno && runtime != webui.runtime.nodejs)
        win->core.runtime = webui.runtime.none;
    else
        win->core.runtime = runtime;
}

void _webui_init(void) {

    if(webui.initialized)
        return;    

    #ifdef WEBUI_LOG
        printf("[0] _webui_init()... \n");
    #endif

    // Initializing
    memset(&webui, 0x0, sizeof(webui_t));
    webui.initialized           = true;
    webui.use_timeout           = true;
    webui.startup_timeout       = WEBUI_DEF_TIMEOUT;
    webui.timeout_extra         = true;
    webui.browser.chrome        = 1;
    webui.browser.firefox       = 2;
    webui.browser.edge          = 3;
    webui.browser.safari        = 4;
    webui.browser.chromium      = 5;
    webui.browser.opera         = 6;
    webui.browser.brave         = 7;
    webui.browser.vivaldi       = 8;
    webui.browser.epic          = 9;
    webui.browser.yandex        = 10;
    webui.browser.custom        = 99;
    webui.runtime.deno          = 1;
    webui.runtime.nodejs        = 2;
    webui.executable_path       = _webui_get_current_path();
}

unsigned int _webui_get_cb_index(char* webui_internal_id) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_get_cb_index([%s])... \n", webui_internal_id);
    #endif

    if(webui_internal_id != NULL) {

        for(unsigned int i = 1; i < WEBUI_MAX_ARRAY; i++) {

            if(!_webui_is_empty(webui.html_elements[i])) 
                if(strcmp(webui.html_elements[i], webui_internal_id) == 0)
                    return i;
        }
    }

    return 0;
}

unsigned int _webui_set_cb_index(char* webui_internal_id) {

    #ifdef WEBUI_LOG
        printf("[0] _webui_set_cb_index([%s])... \n", webui_internal_id);
    #endif

    // Add
    for(unsigned int i = 1; i < WEBUI_MAX_ARRAY; i++) {

        if(_webui_is_empty(webui.html_elements[i])) {

            webui.html_elements[i] = webui_internal_id;

            return i;
        }
    }

    return 0;
}

// --[Interface]---------------

void webui_bind_interface_handler(webui_event_t* e) {

    #ifdef WEBUI_LOG
        printf("[%d] webui_bind_interface_handler()... \n", e->window_id);
    #endif

    unsigned int cb_index = e->element_id;

    if(cb_index > 0 && webui.cb_interface[cb_index] != NULL)
        webui.cb_interface[cb_index](e->element_id, e->window_id, e->element_name, e->window, (char*)e->data, (char**)&e->response);
    
    if(_webui_is_empty((const char *)e->response)) {
        e->response = (void*)webui_empty_string;
    }
    else {

        // The response pointer is not guaranteed to stay live
        // so let's make our own copy.
        size_t len = strlen((const char *)e->response);
        char* new_cpy = (char*)_webui_malloc(len);
        memcpy(new_cpy, e->response, len);
        e->response = new_cpy;
    }

    #ifdef WEBUI_LOG
        printf("[%d] webui_bind_interface_handler()... user-callback response [%s]\n", e->window_id, (const char *)e->response);
    #endif
}

unsigned int webui_bind_interface(webui_window_t* win, const char* element, void (*func)(unsigned int, unsigned int, char*, webui_window_t*, char*, char**)) {

    #ifdef WEBUI_LOG
        printf("[%d] webui_bind_interface()... \n", win->core.window_number);
    #endif

    // Bind
    unsigned int cb_index = webui_bind(win, element, webui_bind_interface_handler);
    webui.cb_interface[cb_index] = func;
    return cb_index;
}

void webui_script_interface(webui_window_t* win, const char* script, unsigned int timeout, bool* error, unsigned int* length, char** data) {

    #ifdef WEBUI_LOG
        printf("[%d] webui_script_interface()... \n", win->core.window_number);
    #endif

    webui_script_t js = {
        .script = script,
        .timeout = timeout
    };

    webui_script(win, &js);

    *data = (char*) js.result.data;
    *error = js.result.error;
    *length = js.result.length;
}

void webui_script_interface_struct(webui_window_t* win, webui_script_interface_t* js_int) {

    #ifdef WEBUI_LOG
        printf("[%d] webui_script_interface_struct()... \n", win->core.window_number);
    #endif

    webui_script_t js = {
        .script = js_int->script,
        .timeout = js_int->timeout
    };

    webui_script(win, &js);
    
    js_int->data = js.result.data;
    js_int->error = js.result.error;
    js_int->length = js.result.length;
}

#ifdef _WIN32
    BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
        return true;
    }
#endif
