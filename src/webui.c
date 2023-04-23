/*
  WebUI Library 2.2.0
  http://_webui_core.me
  https://github.com/alifcommunity/webui
  Copyright (c) 2020-2023 Hassan Draga.
  Licensed under GNU General Public License v2.0.
  All rights reserved.
  Canada.
*/

// -- Third-party ---------------------
#include "mongoose.h"

// -- WebUI ---------------------------
#include "webui_core.h"

// -- Heap ----------------------------
static _webui_core_t _webui_core;

// -- WebUI JS-Bridge ---------
// This is a uncompressed version to make the debugging
// more easy in the browser using the builtin dev-tools
#ifdef WEBUI_LOG
    #define WEBUI_JS_LOG "true"
#else
    #define WEBUI_JS_LOG "false"
#endif
static const char* webui_javascript_bridge = 
"var _webui_log = " WEBUI_JS_LOG "; \n"
"var _webui_ws; \n"
"var _webui_ws_status = false; \n"
"var _webui_ws_status_once = false; \n"
"var _webui_close_reason = 0; \n"
"var _webui_close_value; \n"
"var _webui_has_events = false; \n"
"const _WEBUI_SIGNATURE = 221; \n"
"const _WEBUI_JS = 254; \n"
"const _WEBUI_JS_QUICK = 253; \n"
"const _WEBUI_CLICK = 252; \n"
"const _WEBUI_SWITCH = 251; \n"
"const _WEBUI_CLOSE = 250; \n"
"const _WEBUI_FUNCTION = 249; \n"
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
"        _webui_ws = new WebSocket('ws://localhost:' + _webui_port + '/_webui_ws_connect'); \n"
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
"                } else if(buffer8[1] === _WEBUI_JS_QUICK || buffer8[1] === _WEBUI_JS) { \n"
"                    data8utf8 = data8utf8.replace(/(?:\\r\\n|\\r|\\n)/g, \"\\\\n\"); \n"
"                    if(_webui_log) \n"
"                        console.log('WebUI -> JS [' + data8utf8 + ']'); \n"
"                    var FunReturn = 'undefined'; \n"
"                    var FunError = false; \n"
"                    try { FunReturn = eval('(() => {' + data8utf8 + '})()'); } catch (e) { FunError = true; FunReturn = e.message } \n"
"                    if(buffer8[1] === _WEBUI_JS_QUICK) return; \n"
"                    if(typeof FunReturn === 'undefined' || FunReturn === undefined) FunReturn = 'undefined'; \n"
"                    if(_webui_log && !FunError) console.log('WebUI -> JS -> Return [' + FunReturn + ']'); \n"
"                    if(_webui_log && FunError) console.log('WebUI -> JS -> Error [' + FunReturn + ']'); \n"
"                    var FunReturn8 = new TextEncoder('utf-8').encode(FunReturn); \n"
"                    var Return8 = new Uint8Array(4 + FunReturn8.length); \n"
"                    Return8[0] = _WEBUI_SIGNATURE; \n"
"                    Return8[1] = _WEBUI_JS; \n"
"                    Return8[2] = buffer8[2]; \n"
"                    if(FunError) Return8[3] = 0; \n"
"                    else Return8[3] = 1; \n"
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
"    if(typeof value == 'undefined') \n"
"        var value = ''; \n"
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
"   _webui_ws.close(); \n"
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
"if(typeof navigation !== 'undefined') { \n"
"    navigation.addEventListener('navigate', (event) => { \n"
"        const url = new URL(event.destination.url); \n"
"        _webui_send_event_navigation(url); \n"
"    }); \n"
"} \n"
"document.body.addEventListener('contextmenu', function(event){ event.preventDefault(); }); \n"
"var inputs = document.getElementsByTagName('input'); \n"
"for(var i = 0; i < inputs.length; i++){ inputs[i].addEventListener('contextmenu', function(event){ event.stopPropagation(); });} \n"
"// Load \n"
"window.addEventListener('load', _webui_start()); \n";

// -- Heap ----------------------------
static const char* webui_html_served = "<html><head><title>Access Denied</title><style>body{margin:0;background-repeat:no-repeat;background-attachment:fixed;background-color:#FF3CAC;background-image:linear-gradient(225deg,#FF3CAC 0%,#784BA0 45%,#2B86C5 100%);font-family:sans-serif;margin:20px;color:#fff}a{color:#fff}</style></head><body><h2>&#9888; Access Denied</h2><p>You can't access this content<br>because it's already processed.<br><br>The current security policy denies<br>multiple requests.</p><br><a href=\"https://www.webui.me\"><small>WebUI v" WEBUI_VERSION "<small></a></body></html>";
static const char* webui_html_res_not_available = "<html><head><title>Resource Not Available</title><style>body{margin:0;background-repeat:no-repeat;background-attachment:fixed;background-color:#FF3CAC;background-image:linear-gradient(225deg,#FF3CAC 0%,#784BA0 45%,#2B86C5 100%);font-family:sans-serif;margin:20px;color:#fff}a{color:#fff}</style></head><body><h2>&#9888; Resource Not Available</h2><p>The requested resource is not available.</p><br><a href=\"https://www.webui.me\"><small>WebUI v" WEBUI_VERSION "<small></a></body></html>";
static const char* webui_deno_not_found = "<html><head><title>Deno Not Found</title><style>body{margin:0;background-repeat:no-repeat;background-attachment:fixed;background-color:#FF3CAC;background-image:linear-gradient(225deg,#FF3CAC 0%,#784BA0 45%,#2B86C5 100%);font-family:sans-serif;margin:20px;color:#fff}a{color:#fff}</style></head><body><h2>&#9888; Deno Not Found</h2><p>Deno is not found on this system.<br>Please download it from <a href=\"https://github.com/denoland/deno/releases\">https://github.com/denoland/deno/releases</a></p><br><a href=\"https://www.webui.me\"><small>WebUI v" WEBUI_VERSION "<small></a></body></html>";
static const char* webui_nodejs_not_found = "<html><head><title>Node.js Not Found</title><style>body{margin:0;background-repeat:no-repeat;background-attachment:fixed;background-color:#FF3CAC;background-image:linear-gradient(225deg,#FF3CAC 0%,#784BA0 45%,#2B86C5 100%);font-family:sans-serif;margin:20px;color:#fff}a{color:#fff}</style></head><body><h2>&#9888; Node.js Not Found</h2><p>Node.js is not found on this system.<br>Please download it from <a href=\"https://nodejs.org/en/download/\">https://nodejs.org/en/download/</a></p><br><a href=\"https://www.webui.me\"><small>WebUI v" WEBUI_VERSION "<small></a></body></html>";
static const char* webui_def_icon = "<svg height=\"24\" width=\"24\" xmlns=\"http://www.w3.org/2000/svg\"><path d=\"M3 2c-1.105 0-2 .9-2 2v16c0 1.1.895 2 2 2h18c1.105 0 2-.9 2-2V4c0-1.1-.895-2-2-2H3z\" fill=\"#2c3e50\"/><path d=\"M3 21c-1.105 0-2-.9-2-2V7h22v12c0 1.1-.895 2-2 2H3z\" fill=\"#34495e\"/><path d=\"M4 4.5v1.1L6 8l-2 2.3v1.1L7 8 4 4.5z\" fill=\"#ecf0f1\"/><path d=\"M3 2a2 2 0 0 0-2 2v12h22V4a2 2 0 0 0-2-2H3z\" fill=\"#34495e\"/><path d=\"M4 5.125V6.25L7 8 4 9.75v1.125L9 8 4 5.125zM9 10v1h5v-1H9z\" fill=\"#ecf0f1\"/></svg>";
static const char* webui_def_icon_type = "Content-Type: image/svg+xml\r\n";
static const char* webui_js_empty = "ERR_WEBUI_NO_SCRIPT_FOUND";
static const char* webui_js_timeout = "ERR_WEBUI_TIMEOUT";
static const char* const webui_empty_string = ""; // In case the compiler optimization is disabled

// -- Functions -----------------------
bool webui_run(void* window, const char* script) {

    #ifdef WEBUI_LOG
        printf("[User] webui_run([%s])... \n", script);
    #endif

    size_t js_len = strlen(script);
    
    if(js_len < 1)
        return false;
    
    // Dereference
    _webui_window_t* win = (_webui_window_t*)window;

    // Initializing pipe
    unsigned char run_id = _webui_get_run_id();
    _webui_core.run_done[run_id] = false;
    _webui_core.run_error[run_id] = false;
    if((void *)_webui_core.run_responses[run_id] != NULL)
        _webui_free_mem((void *)_webui_core.run_responses[run_id]);
    
    // Prepare the packet
    size_t packet_len = 3 + js_len; // [header][js]
    char* packet = (char*) _webui_malloc(packet_len);
    packet[0] = WEBUI_HEADER_SIGNATURE; // Signature
    packet[1] = WEBUI_HEADER_JS_QUICK;  // Type
    packet[2] = run_id;                 // ID
    for(unsigned int i = 0; i < js_len; i++) // Data
        packet[i + 3] = script[i];
    
    // Send packets
    _webui_window_send(win, packet, packet_len);
    _webui_free_mem((void *)packet);

    return true;
}

bool webui_script(void* window, const char* script, unsigned int timeout_second, char* buffer, size_t buffer_length) {

    // Dereference
    _webui_window_t* win = (_webui_window_t*)window;

    #ifdef WEBUI_LOG
        printf("[User] webui_script()... \n", script);
        printf("[User] webui_script() -> Script [%s] \n", script);
        printf("[User] webui_script() -> Response Buffer @ 0x%p \n", buffer);
        printf("[User] webui_script() -> Response Buffer Size %lld bytes \n", buffer_length);
    #endif

    _webui_init();

    // Initializing response buffer
    if(buffer_length > 0)
        memset(buffer, 0, buffer_length);    

    size_t js_len = strlen(script);

    if(js_len < 1) {

        if(buffer != NULL && buffer_length > 1)
            snprintf(buffer, buffer_length, "%s", webui_js_empty);
        return false;
    }

    // Initializing pipe
    unsigned char run_id = _webui_get_run_id();
    _webui_core.run_done[run_id] = false;
    _webui_core.run_error[run_id] = false;
    if((void *)_webui_core.run_responses[run_id] != NULL)
        _webui_free_mem((void *)_webui_core.run_responses[run_id]);

    // Prepare the packet
    size_t packet_len = 3 + js_len;             // [header][js]
    char* packet = (char*) _webui_malloc(packet_len);
    packet[0] = WEBUI_HEADER_SIGNATURE;         // Signature
    packet[1] = WEBUI_HEADER_JS;                // Type
    packet[2] = run_id;                         // ID
    for(unsigned int i = 0; i < js_len; i++)    // Data
        packet[i + 3] = script[i];
    
    // Send packets
    _webui_window_send(win, packet, packet_len);
    _webui_free_mem((void *)packet);

    // Wait for UI response
    if(timeout_second < 1 || timeout_second > 86400) {

        // Wait forever
        for(;;) {

            if(_webui_core.run_done[run_id])
                break;
            
            _webui_sleep(1);
        }
    }
    else {

        // Using timeout
        for(unsigned int n = 0; n <= (timeout_second * 1000); n++) {

            if(_webui_core.run_done[run_id])
                break;
            
            _webui_sleep(1);
        }
    }

    if(_webui_core.run_responses[run_id] != NULL) {

        #ifdef WEBUI_LOG
            printf("[User] webui_script -> Response found [%s] \n", _webui_core.run_responses[run_id]);
        #endif

        // Response found
        if(buffer != NULL && buffer_length > 1) {

            // Copy response to the user's response buffer
            size_t response_len = strlen(_webui_core.run_responses[run_id]);
            size_t bytes_to_cpy = (response_len <= buffer_length ? response_len : buffer_length);
            snprintf(buffer, bytes_to_cpy, "%s", _webui_core.run_responses[run_id]);
        }

        _webui_free_mem((void *)_webui_core.run_responses[run_id]);

        return _webui_core.run_error[run_id];
    }

    return false;
}

void* webui_new_window(void) {

    #ifdef WEBUI_LOG
        printf("[User] webui_new_window()... \n");
    #endif

    _webui_init();

    _webui_window_t* win = (_webui_window_t*) _webui_malloc(sizeof(_webui_window_t));

    // Initialisation
    win->window_number = _webui_get_new_window_number();
    win->browser_path = (char*) _webui_malloc(WEBUI_MAX_PATH);
    win->profile_path = (char*) _webui_malloc(WEBUI_MAX_PATH);
    win->server_root_path = (char*) _webui_malloc(WEBUI_MAX_PATH);
    sprintf(win->server_root_path, "%s", WEBUI_DEFAULT_PATH);
    
    #ifdef WEBUI_LOG
        printf("[User] webui_new_window() -> New window @ 0x%p\n", win);
    #endif

    return (void*)win;
}

void webui_close(void* window) {

    // Dereference
    _webui_window_t* win = (_webui_window_t*)window;

    #ifdef WEBUI_LOG
        printf("[User] webui_close()... \n");
    #endif

    _webui_init();

    if(win->connected) {

        // Prepare packets
        char* packet = (char*) _webui_malloc(4);
        packet[0] = WEBUI_HEADER_SIGNATURE; // Signature
        packet[1] = WEBUI_HEADER_CLOSE;     // Type
        packet[2] = 0;                      // ID
        packet[3] = 0;                      // Data

        // Send packets
        _webui_window_send(win, packet, 4);
        _webui_free_mem((void *)packet);
    }
}

bool webui_is_shown(void* window) {

    // Dereference
    _webui_window_t* win = (_webui_window_t*)window;

    #ifdef WEBUI_LOG
        printf("[User] webui_is_shown()... \n");
    #endif

    return win->connected;
}

void webui_set_multi_access(void* window, bool status) {

    // Dereference
    _webui_window_t* win = (_webui_window_t*)window;

    #ifdef WEBUI_LOG
        printf("[User] webui_set_multi_access([%d])... \n", status);
    #endif

    win->multi_access = status;
}

void webui_set_icon(void* window, const char* icon, const char* type) {

    // Dereference
    _webui_window_t* win = (_webui_window_t*)window;

    #ifdef WEBUI_LOG
        printf("[User] webui_set_icon([%s], [%s])... \n", icon, type);
    #endif

    win->icon = icon;
    win->icon_type = type;
}

bool webui_show(void* window, const char* content) {

    // Dereference
    _webui_window_t* win = (_webui_window_t*)window;

    #ifdef WEBUI_LOG
        printf("[User] webui_show()... \n");
    #endif

    return _webui_show(win, content, AnyBrowser);
}

bool webui_show_browser(void* window, const char* content, unsigned int browser) {

    // Dereference
    _webui_window_t* win = (_webui_window_t*)window;

    #ifdef WEBUI_LOG
        printf("[User] webui_show_browser([%u])... \n", browser);
    #endif

    return _webui_show(win, content, browser);
}

unsigned int webui_bind(void* window, const char* element, void (*func)(webui_event_t* e)) {

    // Dereference
    _webui_window_t* win = (_webui_window_t*)window;

    #ifdef WEBUI_LOG
        printf("[User] webui_bind([%s], [0x%p])... \n", element, func);
    #endif

    _webui_init();

    int len = 0;
    if(_webui_is_empty(element))
        win->has_events = true;
    else
        len = strlen(element);

    // [win num][/][element]
    char* webui_internal_id = _webui_malloc(3 + 1 + len);
    sprintf(webui_internal_id, "%u/%s", win->window_number, element);

    unsigned int cb_index = _webui_get_cb_index(webui_internal_id);

    if(cb_index > 0) {

        // Replace a reference
        _webui_core.cb[cb_index] = func;

        _webui_free_mem((void *)webui_internal_id);
    }
    else {

        // New reference
        cb_index = _webui_set_cb_index(webui_internal_id);

        if(cb_index > 0)
            _webui_core.cb[cb_index] = func;
        else
            _webui_free_mem((void *)webui_internal_id);
    }

    return cb_index;
}

const char* webui_get_string(webui_event_t* e) {
    
    #ifdef WEBUI_LOG
        printf("[User] webui_get_string()... \n");
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
        printf("[User] webui_get_int()... \n");
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
        printf("[User] webui_get_bool()... \n");
    #endif

    const char* str = webui_get_string(e);
    if(str[0] == 't' || str[0] == 'T') // true || True
        return true;
    
        return false;
}

void webui_return_int(webui_event_t* e, long long int n) {

    #ifdef WEBUI_LOG
        printf("[User] webui_return_int([%lld])... \n", n);
    #endif

    // Free
    if(e->response != NULL)
        _webui_free_mem(e->response);

    // Int to Str
    // 64-bit max is -9,223,372,036,854,775,808 (20 character)
    char* buf = (char*) _webui_malloc(20);
    sprintf(buf, "%lld", n);

    // Set response
    e->response = buf;
}

void webui_return_string(webui_event_t* e, char* s) {

    #ifdef WEBUI_LOG
        printf("[User] webui_return_string([%s])... \n", s);
    #endif

    if(_webui_is_empty(s))
        return;
    
    // Free
    if(e->response != NULL)
        _webui_free_mem(e->response);

    // Copy Str
    int len = strlen(s);
    char* buf = (char*) _webui_malloc(len);
    memcpy(buf, s, len);

    // Set response
    e->response = buf;
}

void webui_return_bool(webui_event_t* e, bool b) {

    #ifdef WEBUI_LOG
        printf("[User] webui_return_bool([%d])... \n", b);
    #endif

    // Free
    if(e->response != NULL)
        _webui_free_mem(e->response);

    // Bool to Str
    int len = 1;
    char* buf = (char*) _webui_malloc(len);
    sprintf(buf, "%d", b);

    // Set response
    e->response = buf;
}

void webui_exit(void) {

    #ifdef WEBUI_LOG
        printf("[User] webui_exit()... \n");
    #endif

    _webui_core.exit_now = true;

    // Let's give other threads more time to 
    // safely exit and finish their cleaning up.
    _webui_sleep(100);
}

void webui_wait(void) {

    #ifdef WEBUI_LOG
        printf("[Loop] webui_wait()... \n");
    #endif

    _webui_init();

    if(_webui_core.startup_timeout > 0) {

        #ifdef WEBUI_LOG
            printf("[Loop] webui_wait() -> Using timeout %u second\n", _webui_core.startup_timeout);
        #endif

        // Wait for browser to start
        _webui_wait_for_startup();

        #ifdef WEBUI_LOG
            printf("[Loop] webui_wait() -> Wait for connected UI...\n");
        #endif

        while(_webui_core.servers > 0) {

            #ifdef WEBUI_LOG
                // printf("[%u/%u]", _webui_core.servers, _webui_core.connections);
            #endif
            _webui_sleep(50);
        }
    }
    else {

        #ifdef WEBUI_LOG
            printf("[Loop] webui_wait() -> Infinite wait...\n");
        #endif

        // Infinite wait
        while(!_webui_core.exit_now)
            _webui_sleep(50);
    }

    #ifdef WEBUI_LOG
        printf("[Loop] webui_wait() -> Wait finished.\n");
    #endif

    // Final cleaning
    _webui_clean();
}

void webui_set_timeout(unsigned int second) {

    #ifdef WEBUI_LOG
        printf("[User] webui_set_timeout([%u])... \n", second);
    #endif

    _webui_init();

    if(second > WEBUI_MAX_TIMEOUT)
        second = WEBUI_MAX_TIMEOUT;

    _webui_core.startup_timeout = second;
}

void webui_set_runtime(void* window, unsigned int runtime) {

    // Dereference
    _webui_window_t* win = (_webui_window_t*)window;

    #ifdef WEBUI_LOG
        printf("[User] webui_script_runtime(%u)... \n", runtime);
    #endif

    _webui_init();

    if(runtime != Deno && runtime != NodeJS)
        win->runtime = None;
    else
        win->runtime = runtime;
}

// -- Interface's Functions ----------------
void _webui_interface_bind_handler(webui_event_t* e) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_interface_bind_handler()... \n");
    #endif

    // Generate WebUI internal id
    char* webui_internal_id = _webui_generate_internal_id(e->window, e->element);
    unsigned int cb_index = _webui_get_cb_index(webui_internal_id);

    if(cb_index > 0 && _webui_core.cb_interface[cb_index] != NULL) {

        #ifdef WEBUI_LOG
            printf("[Core]\t\t_webui_interface_bind_handler() -> User callback @ 0x%p\n", _webui_core.cb_interface[cb_index]);
            printf("[Core]\t\t_webui_interface_bind_handler() -> Response set @ 0x%p\n", (char*)&e->response);
            printf("[Core]\t\t_webui_interface_bind_handler() -> type [%u]\n", e->type);
            printf("[Core]\t\t_webui_interface_bind_handler() -> data [%s]\n", e->data);
            printf("[Core]\t\t_webui_interface_bind_handler() -> element [%s]\n", e->element);
        #endif
        _webui_core.cb_interface[cb_index](e->window, e->type, e->element, e->data, (char*)&e->response);
    }
    
    if(_webui_is_empty((const char *)e->response))
        e->response = (char*)webui_empty_string;
    
    // Free
    _webui_free_mem((void *)webui_internal_id);

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_interface_bind_handler() -> user-callback response [%s] @ 0x%p\n", (const char *)e->response, e->response);
    #endif
}

unsigned int webui_interface_bind(void* window, const char* element, void (*func)(void*, unsigned int, char*, char*, char*)) {

    // Dereference
    _webui_window_t* win = (_webui_window_t*)window;

    #ifdef WEBUI_LOG
        printf("[User] webui_interface_bind()... \n");
    #endif

    // Bind
    unsigned int cb_index = webui_bind(win, element, _webui_interface_bind_handler);
    _webui_core.cb_interface[cb_index] = func;
    return cb_index;
}

void webui_interface_set_response(char* ptr, const char* response) {

    #ifdef WEBUI_LOG
        printf("[User] webui_interface_set_response()... \n");
        printf("[User] webui_interface_set_response() -> Pointer @ 0x%p \n", ptr);
        printf("[User] webui_interface_set_response() -> Response [%s] \n", response);
    #endif

    size_t len = strlen(response);

    if(len < 1)
        return;

    char* buf = (char*) _webui_malloc(len);
    char** _ptr = (char**)ptr;
    *_ptr = buf;
    strcpy(*_ptr, response);

    #ifdef WEBUI_LOG
        printf("[User] webui_interface_set_response() -> Internal buffer @ 0x%p \n", (*_ptr));
        printf("[User] webui_interface_set_response() -> Internal buffer [%s] \n", (*_ptr));
    #endif
}

bool webui_interface_is_app_running(void) {

    #ifdef WEBUI_LOG
        // printf("[User] webui_is_app_running()... \n");
    #endif

    static bool app_is_running = true;

    // Stop if already flagged
    if(!app_is_running) return false;

    // Initialization
    if(!_webui_core.initialized)
        _webui_init();
    
    // Get app status
    if(_webui_core.exit_now) {
        app_is_running = false;
    }
    else if(_webui_core.startup_timeout > 0) {
        if(_webui_core.servers < 1)
            app_is_running = false;
    }

    // Final cleaning
    if(!app_is_running) {
        #ifdef WEBUI_LOG
            printf("[User] webui_is_app_running() -> App Stopped.\n");
        #endif
        _webui_clean();
    }

    return app_is_running;
}

unsigned int webui_interface_get_window_id(void* window) {

    #ifdef WEBUI_LOG
        printf("[User] webui_interface_get_window_id()... \n");
    #endif

    // Dereference
    _webui_window_t* win = (_webui_window_t*)window;

    return win->window_number;
}

// -- Core's Functions ----------------
bool _webui_ptr_exist(void* ptr) {
    
    #ifdef WEBUI_LOG
        // printf("[Core]\t\t_webui_ptr_exist()... \n");
    #endif

    if(ptr == NULL)
        return false;
    
    for(unsigned int i = 0; i < _webui_core.ptr_position; i++) {

        if(_webui_core.ptr_list[i] == ptr)
            return true;
    }

    return false;
}

static void _webui_ptr_add(void* ptr, size_t size) {
    
    #ifdef WEBUI_LOG
        // printf("[Core]\t\t_webui_ptr_add(0x%p)... \n", ptr);
    #endif

    if(ptr == NULL)
        return;

    if(!_webui_ptr_exist(ptr)) {

        for(unsigned int i = 0; i < _webui_core.ptr_position; i++) {

            if(_webui_core.ptr_list[i] == NULL) {

                #ifdef WEBUI_LOG
                    printf("[Core]\t\t_webui_ptr_add(0x%p)... Allocate %d bytes\n", ptr, (int)size);
                #endif

                _webui_core.ptr_list[i] = ptr;
                _webui_core.ptr_size[i] = size;
                return;
            }
        }

        #ifdef WEBUI_LOG
            printf("[Core]\t\t_webui_ptr_add(0x%p)... Allocate %d bytes\n", ptr, (int)size);
        #endif

        _webui_core.ptr_list[_webui_core.ptr_position] = ptr;
        _webui_core.ptr_size[_webui_core.ptr_position] = size;
        _webui_core.ptr_position++;
        if(_webui_core.ptr_position >= WEBUI_MAX_ARRAY)
            _webui_core.ptr_position = (WEBUI_MAX_ARRAY - 1);
    }
}

static void _webui_free_mem(void* ptr) {
    
    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_free_mem(0x%p)... \n", ptr);
    #endif

    if(ptr == NULL)
        return;

    for(unsigned int i = 0; i < _webui_core.ptr_position; i++) {

        if(_webui_core.ptr_list[i] == ptr) {

            #ifdef WEBUI_LOG
                printf("[Core]\t\t_webui_free_mem(0x%p)... Free %d bytes\n", ptr, (int)_webui_core.ptr_size[i]);
            #endif

            memset(ptr, 0, _webui_core.ptr_size[i]);
            free(ptr);

            _webui_core.ptr_size[i] = 0;
            _webui_core.ptr_list[i] = NULL;
        }
    }

    for(int i = _webui_core.ptr_position; i >= 0; i--) {

        if(_webui_core.ptr_list[i] == NULL) {

            _webui_core.ptr_position = i;
            break;
        }
    }
}

static void _webui_free_all_mem(void) {
    
    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_free_all_mem()... \n");
    #endif

    // Makes sure we run this once
    static bool freed = false;
    if(freed) return;
    freed = true;

    void* ptr = NULL;
    for(unsigned int i = 0; i < _webui_core.ptr_position; i++) {

        ptr = _webui_core.ptr_list[i];

        if(ptr != NULL) {

            #ifdef WEBUI_LOG
                printf("[Core]\t\t_webui_free_all_mem()... Free %d bytes @ 0x%p\n", (int)_webui_core.ptr_size[i], ptr);
            #endif

            memset(ptr, 0, _webui_core.ptr_size[i]);
            free(ptr);
        }
    }
}

static void _webui_panic(void) {
    
    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_panic()... \n");
    #endif

    webui_exit();
    exit(EXIT_FAILURE);
}

size_t _webui_round_to_memory_block(int size) {

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
        printf("[Core]\t\t_webui_malloc([%d])... \n", size);
    #endif
    
    // Make sure we have the null
    // terminator if it's a string
    size++;

    size = _webui_round_to_memory_block(size);

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

    memset(block, 0, size);

    _webui_ptr_add((void *) block, size);

    return block;
}

static void _webui_sleep(long unsigned int ms) {
    
    #ifdef WEBUI_LOG
        // printf("[Core]\t\t_webui_sleep([%u])... \n", ms);
    #endif

    #ifdef _WIN32
        Sleep(ms);
    #else
        usleep(ms);
    #endif
}

long _webui_timer_diff(struct timespec *start, struct timespec *end) {

    #ifdef WEBUI_LOG
        // printf("[Core]\t\t_webui_timer_diff()... \n");
    #endif

    return (
        (long)(end->tv_sec * 1000) +
        (long)(end->tv_nsec / 1000000)) -
        ((long)(start->tv_sec * 1000) +
        (long)(start->tv_nsec / 1000000)
    );
}

static void _webui_timer_clock_gettime(struct timespec *spec) {

    #ifdef WEBUI_LOG
        // printf("[Core]\t\t_webui_timer_clock_gettime()... \n");
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

static void _webui_timer_start(_webui_timer_t* t) {

    #ifdef WEBUI_LOG
        // printf("[Core]\t\t_webui_timer_start()... \n");
    #endif
    
    _webui_timer_clock_gettime(&t->start);
}

bool _webui_timer_is_end(_webui_timer_t* t, unsigned int ms) {

    #ifdef WEBUI_LOG
        // printf("[Core]\t\t_webui_timer_is_end()... \n");
    #endif
    
    _webui_timer_clock_gettime(&t->now);

    unsigned int def = (unsigned int) _webui_timer_diff(&t->start, &t->now);
    if(def > ms)
        return true;
    return false;
}

bool _webui_is_empty(const char* s) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_is_empty()... \n");
    #endif

    if((s != NULL) && (s[0] != '\0'))
        return false;
    return true;
}

bool _webui_file_exist_mg(void *ev_data) {
    
    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_file_exist_mg()... \n");
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
    full_path = (char*) _webui_malloc(strlen(_webui_core.executable_path) + 1 + strlen(file));
    sprintf(full_path, "%s%s%s", _webui_core.executable_path, webui_sep, file);

    bool exist = _webui_file_exist(full_path);

    _webui_free_mem((void *)file);
    _webui_free_mem((void *)full_path);

    return exist;
}

bool _webui_file_exist(char* file) {
    
    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_file_exist([%s])... \n", file);
    #endif

    if(_webui_is_empty(file))
        return false;

    if(WEBUI_FILE_EXIST(file, 0) == 0)
        return true;
    return false;
}

const char* _webui_get_extension(const char*f) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_get_extension()... \n");
    #endif

    if(f == NULL)
        return webui_empty_string;

    const char*ext = strrchr(f, '.');

    if(ext == NULL || !ext || ext == f)
        return webui_empty_string;
    return ext + 1;
}

unsigned char _webui_get_run_id(void) {
    
    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_get_run_id()... \n");
    #endif

    return ++_webui_core.run_last_id;
}

bool _webui_socket_test_listen_mg(unsigned int port_num) {
    
    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_socket_test_listen_mg([%u])... \n", port_num);
    #endif

    struct mg_connection *c;
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);

    char url[32];
    sprintf(url, "http://localhost:%u", port_num);

    if((c = mg_http_listen(&mgr, url, NULL, &mgr)) == NULL) {

        // Cannot listen
        mg_mgr_free(&mgr);
        return false;
    }

    // Listening success
    mg_mgr_free(&mgr);

    return true;
}

bool _webui_port_is_used(unsigned int port_num) {
    
    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_port_is_used([%u])... \n", port_num);
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

static void _webui_serve_file(_webui_window_t* win, struct mg_connection *c, void *ev_data) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_serve_file()... \n");
    #endif

    // Serve a normal text based file
    // send with HTTP 200 status code

    struct mg_http_serve_opts opts = {

        .root_dir = win->server_root_path
    };

    mg_http_serve_dir(c, ev_data, &opts);
}

bool _webui_deno_exist(void) {
    
    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_deno_exist()... \n");
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
        printf("[Core]\t\t_webui_nodejs_exist()... \n");
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
        printf("[Core]\t\t_webui_interpret_command()... \n");
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
    _webui_free_mem((void *)line);

    return (const char*) out;
}

static void _webui_interpret_file(_webui_window_t* win, struct mg_connection *c, void *ev_data, char* index) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_interpret_file()... \n");
    #endif

    // Run the JavaScript / TypeScript runtime
    // and send back the output with HTTP 200 status code
    // otherwise, send the file as a normal text based one

    char* file;
    char* full_path;

    if(index != NULL && !_webui_is_empty(index)) {

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
        full_path = (char*) _webui_malloc(strlen(_webui_core.executable_path) + 1 + strlen(file));
        sprintf(full_path, "%s%s%s", _webui_core.executable_path, webui_sep, file);

        if(!_webui_file_exist(full_path)) {

            // File not exist - 404
            _webui_serve_file(win, c, ev_data);

            _webui_free_mem((void *)file);
            _webui_free_mem((void *)full_path);
            return;
        }
    }

    // Get file extension
    const char* extension = _webui_get_extension(file);

    if(strcmp(extension, "ts") == 0 || strcmp(extension, "js") == 0) {

        // TypeScript / JavaScript

        if(win->runtime == Deno) {

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

                _webui_free_mem((void *)cmd);
                _webui_free_mem((void *)out);
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
        else if(win->runtime == NodeJS) {

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

                _webui_free_mem((void *)cmd);
                _webui_free_mem((void *)out);
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

    _webui_free_mem((void *)file);
    _webui_free_mem((void *)full_path);
}

const char* _webui_generate_js_bridge(_webui_window_t* win) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_generate_js_bridge()... \n");
    #endif

    // Calculate the cb size
    size_t cb_mem_size = 64; // To hold 'const _webui_bind_list = ["elem1", "elem2",];'
    for(unsigned int i = 1; i < WEBUI_MAX_ARRAY; i++)
        if(_webui_core.html_elements[i] != NULL && !_webui_is_empty(_webui_core.html_elements[i]))
            cb_mem_size += strlen(_webui_core.html_elements[i]) + 3;
    
    // Generate the cb array
    char* event_cb_js_array = (char*) _webui_malloc(cb_mem_size);
    strcat(event_cb_js_array, "const _webui_bind_list = [");
    for(unsigned int i = 1; i < WEBUI_MAX_ARRAY; i++) {
        if(_webui_core.html_elements[i] != NULL && !_webui_is_empty(_webui_core.html_elements[i])) {
            strcat(event_cb_js_array, "\"");
            strcat(event_cb_js_array, _webui_core.html_elements[i]);
            strcat(event_cb_js_array, "\",");
        }
    }
    strcat(event_cb_js_array, "]; \n");

    // Generate the full WebUI JS-Bridge
    size_t len = cb_mem_size + strlen(webui_javascript_bridge);
    char* js = (char*) _webui_malloc(len);
    sprintf(js, 
        "_webui_port = %u; \n_webui_win_num = %u; \n%s \n%s \n",
        win->server_port, win->window_number, event_cb_js_array, webui_javascript_bridge
    );

    return js;
}

static void _webui_server_event_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {

    #ifdef WEBUI_LOG
        // printf("[Core]\t\t_webui_server_event_handler()... \n");
    #endif

    _webui_window_t* win = (_webui_window_t *) fn_data;

    if(ev == MG_EV_HTTP_MSG) {

        struct mg_http_message *hm = (struct mg_http_message *) ev_data;

        if(mg_http_match_uri(hm, "/_webui_ws_connect")) {

            // WebSocket

            #ifdef WEBUI_LOG
                printf("[Core]\t\t_webui_server_event_handler()... HTML Upgrade to WebSocket\n");
            #endif

            mg_ws_upgrade(c, hm, NULL);
        } 
        else if(mg_http_match_uri(hm, "/webui.js")) {

            // WebUI JS-Bridge

            #ifdef WEBUI_LOG
                printf("[Core]\t\t_webui_server_event_handler()... HTML WebUI JS\n");
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

            _webui_free_mem((void *)js);
        }
        else if(strncmp(hm->uri.ptr, "/WEBUI/FUNC/", 12) == 0 && hm->uri.len >= 15) {
            
            // Function Call (With response)

            // [/WEBUI/FUNC/ELEMENT_ID/DATA]
            // 0            12

            #ifdef WEBUI_LOG
                printf("[Core]\t\t_webui_server_event_handler()... CB start\n");
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
            e.element = element;
            e.window = win;
            e.data = data;
            e.response = NULL;
            e.type = WEBUI_EVENT_CALLBACK;

            unsigned int cb_index = _webui_get_cb_index(webui_internal_id);

            // Check for bind
            if(cb_index > 0 && _webui_core.cb[cb_index] != NULL) {

                // Call user cb
                _webui_core.cb[cb_index](&e);
            }

            if(_webui_is_empty(e.response))
                e.response = (char*)webui_empty_string;

            #ifdef WEBUI_LOG
                printf("[Core]\t\t_webui_server_event_handler()... user-callback response [%s]\n", (const char*)e.response);
            #endif

            // Send response
            mg_http_reply(
                c, 200,
                "",
                e.response
            );

            // Free
            _webui_free_mem((void *)packet);
            _webui_free_mem((void *)webui_internal_id);
            _webui_free_mem((void *)e.response);
        }
        else if(mg_http_match_uri(hm, "/")) {

            // [/]

            if(win->is_embedded_html) {

                // Main HTML

                if(!win->multi_access && win->html_handled) {

                    // Main HTML already handled.
                    // Forbidden 403

                    #ifdef WEBUI_LOG
                        printf("[Core]\t\t_webui_server_event_handler()... HTML Main Already Handled (403)\n");
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

                    win->html_handled = true;

                    #ifdef WEBUI_LOG
                        printf("[Core]\t\t_webui_server_event_handler()... HTML Main\n");
                    #endif

                    char* html = (char*) webui_empty_string;

                    if(win->html != NULL) {

                        // Generate the full WebUI JS-Bridge
                        const char* js = _webui_generate_js_bridge(win);

                        // Inject WebUI JS-Bridge into HTML
                        size_t len = strlen(win->html) + strlen(js) + 128;
                        html = (char*) _webui_malloc(len);
                        sprintf(html, 
                            "%s \n <script type = \"text/javascript\"> \n %s \n </script>",
                            win->html, js
                        );

                        _webui_free_mem((void *)js);
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
                    //     win->server_port, strlen(html)
                    // );

                    // Send
                    mg_http_reply(
                        c, 200,
                        "",
                        html
                    );

                    _webui_free_mem((void *)html);
                }
            }
            else {

                // Serve local files

                win->html_handled = true;

                #ifdef WEBUI_LOG
                    printf("[Core]\t\t_webui_server_event_handler()... HTML Root Index\n");
                #endif

                // Set full path
                // [Path][Sep][File Name]
                char* index = (char*) _webui_malloc(strlen(_webui_core.executable_path) + 1 + 8); 

                // Index.ts
                sprintf(index, "%s%sindex.ts", _webui_core.executable_path, webui_sep);
                if(_webui_file_exist(index)) {

                    // TypeScript Index
                    _webui_interpret_file(win, c, ev_data, index);

                   _webui_free_mem((void *)index);
                    return;
                }

                // Index.js
                sprintf(index, "%s%sindex.js", _webui_core.executable_path, webui_sep);
                if(_webui_file_exist(index)) {

                    // JavaScript Index
                    _webui_interpret_file(win, c, ev_data, index);

                    _webui_free_mem((void *)index);
                    return;
                }

                _webui_free_mem((void *)index);
                
                // Index.html
                // Serve as a normal HTML text-based file
                _webui_serve_file(win, c, ev_data);
            }
        }
        else if(mg_http_match_uri(hm, "/favicon.ico") || mg_http_match_uri(hm, "/favicon.svg")) {

            // Favicon

            if(win->icon != NULL && win->icon_type != NULL) {

                // Custom user icon

                char* icon_header = (char*) _webui_malloc(strlen(win->icon_type) + 32);
                sprintf(icon_header, "Content-Type: %s\r\n", win->icon_type);

                // User icon
                mg_http_reply(
                    c, 200,
                    icon_header,
                    win->icon
                );
            }
            else if(_webui_file_exist_mg(ev_data)) {

                // Local icon file
                _webui_serve_file(win, c, ev_data);
            }
            else {

                // Default embedded icon

                if(mg_http_match_uri(hm, "/favicon.ico")) {

                    mg_http_reply(c, 302, "Location: /favicon.svg\r\n", "");
                }
                else {

                    // TODO: Use webui_def_icon_type

                    // Header
                    // Content-Type: image/svg+xml
                    
                    // Default icon
                    mg_http_reply(
                        c, 200,
                        webui_def_icon_type,
                        webui_def_icon
                    );
                }
            }
        }
        else {

            // [/file]

            if(win->is_embedded_html) {

                if(win->runtime != None) {

                    // Interpret file

                    #ifdef WEBUI_LOG
                        printf("[Core]\t\t_webui_server_event_handler()... HTML Interpret file\n");
                    #endif

                    _webui_interpret_file(win, c, ev_data, NULL);
                }
                else {

                    // Serve local files

                    #ifdef WEBUI_LOG
                        printf("[Core]\t\t_webui_server_event_handler()... HTML Root file\n");
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
                        printf("[Core]\t\t_webui_server_event_handler()... HTML 404\n");
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
            printf("[Core]\t\t_webui_server_event_handler()... WebSocket Data\n");
        #endif

        struct mg_ws_message *wm = (struct mg_ws_message *) ev_data;

        // Parse the packet
        _webui_window_receive(win, wm->data.ptr, wm->data.len);
    }
    else if(ev == MG_EV_WS_OPEN) {

        #ifdef WEBUI_LOG
            printf("[Core]\t\t_webui_server_event_handler()... WebSocket Connected\n");
        #endif

        int event_type = WEBUI_EVENT_CONNECTED;

        if(!win->connected) {

            // First connection

            win->connected = true; // server thread
            _webui_core.connections++; // main loop
            _webui_core.mg_connections[win->window_number] = c; // websocket send func
        }
        else {

            if(win->multi_access) {

                // Multi connections
                win->connections++;
                event_type = WEBUI_EVENT_MULTI_CONNECTION;
            }
            else {

                // UNWANTED Multi connections

                #ifdef WEBUI_LOG
                    printf("[Core]\t\t_webui_server_event_handler() -> UNWANTED Multi Connections\n");
                #endif

                mg_close_conn(c);
                event_type = WEBUI_EVENT_UNWANTED_CONNECTION;
            }
        }

        // Events
        if(win->has_events) {

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
    }
    else if(ev == MG_EV_WS_CTL) {

        win->html_handled = false;

        #ifdef WEBUI_LOG
            printf("[Core]\t\t_webui_server_event_handler()... WebSocket Closed\n");
        #endif

        if(win->connected) {

            if(win->multi_access && win->connections > 0) {

                // Multi connections close
                win->connections--;
            }
            else {

                // Main connection close
                _webui_core.connections--;  // main loop
                win->connected = false;     // server thread                
            }
        }

        // Events
        if(win->has_events) {

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
}

bool _webui_browser_create_profile_folder(_webui_window_t* win, unsigned int browser) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_browser_create_profile_folder(%u)... \n", browser);
    #endif

    const char* temp = _webui_browser_get_temp_path(browser);

    if(browser == Chrome) {

        // Google Chrome
        sprintf(win->profile_path, "%s%s.WebUI%sWebUIChromeProfile", temp, webui_sep, webui_sep);
        return true;
    }
    else if(browser == Edge) {

        // Edge
        sprintf(win->profile_path, "%s%s.WebUI%sWebUIEdgeProfile", temp, webui_sep, webui_sep);
        return true;
    }
    else if(browser == Epic) {

        // Epic
        sprintf(win->profile_path, "%s%s.WebUI%sWebUIEpicProfile", temp, webui_sep, webui_sep);
        return true;
    }
    else if(browser == Vivaldi) {

        // Vivaldi
        sprintf(win->profile_path, "%s%s.WebUI%sWebUIVivaldiProfile", temp, webui_sep, webui_sep);
        return true;
    }
    else if(browser == Brave) {

        // Brave
        sprintf(win->profile_path, "%s%s.WebUI%sWebUIBraveProfile", temp, webui_sep, webui_sep);
        return true;
    }
    else if(browser == Yandex) {

        // Yandex
        sprintf(win->profile_path, "%s%s.WebUI%sWebUIYandexProfile", temp, webui_sep, webui_sep);
        return true;
    }
    else if(browser == Chromium) {

        // Chromium
        sprintf(win->profile_path, "%s%s.WebUI%sWebUIChromiumProfile", temp, webui_sep, webui_sep);
        return true;
    }
    else if(browser == Firefox) {

        // Firefox (We need to create a folder)

        char* profile_name = "WebUIFirefoxProfile";

        char firefox_profile_path[1024];
        sprintf(firefox_profile_path, "%s%s.WebUI%s%s", temp, webui_sep, webui_sep, profile_name);
        
        if(!_webui_folder_exist(firefox_profile_path)) {

            char buf[2048];

            sprintf(buf, "%s -CreateProfile \"WebUI %s\"", win->browser_path, firefox_profile_path);
            _webui_cmd_sync(buf, false);

            // Creating the browser profile
            for(unsigned int n = 0; n <= (_webui_core.startup_timeout * 4); n++) {

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

            sprintf(win->profile_path, "%s%s%s", temp, webui_sep, profile_name);
        }

        return true;
    }

    return false;
}

bool _webui_folder_exist(char* folder) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_folder_exist([%s])... \n", folder);
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

char* _webui_generate_internal_id(_webui_window_t* win, const char* element) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_generate_internal_id([%s])... \n", element);
    #endif

    // Generate WebUI internal id
    size_t element_len = strlen(element);
    size_t internal_id_size = 3 + 1 + element_len; // [win num][/][name]
    char* webui_internal_id = (char*) _webui_malloc(internal_id_size);
    sprintf(webui_internal_id, "%u/%s", win->window_number, element);

    return webui_internal_id;
}

const char* _webui_browser_get_temp_path(unsigned int browser) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_browser_get_temp_path([%u])... \n", browser);
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
        return WinUserProfile;
    #elif __APPLE__
        return MacUserProfile;
    #else
        return LinuxUserProfile;
    #endif
}

bool _webui_is_google_chrome_folder(const char* folder) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_is_google_chrome_folder([%s])... \n", folder);
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

bool _webui_browser_exist(_webui_window_t* win, unsigned int browser) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_browser_exist([%u])... \n", browser);
    #endif

    // Check if a web browser is installed on this machine

    if(browser == Chrome) {

        // Google Chrome

        #ifdef _WIN32

            // Google Chrome on Windows

            char browser_folder[WEBUI_MAX_PATH];

            // Search in `HKEY_LOCAL_MACHINE` (If Google Chrome installed for multi-user)
            if(_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\chrome.exe", "Path", browser_folder)) {

                // Make sure its Google Chrome and not Chromium
                if(_webui_is_google_chrome_folder(browser_folder)) {

                    // Google Chrome Found (multi-user)
                    sprintf(win->browser_path, "\"%s\\chrome.exe\"", browser_folder);
                    return true;
                }
            }

            // Search in `HKEY_CURRENT_USER` (If Google Chrome installed for one user)
            if(_webui_get_windows_reg_value(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\chrome.exe", "Path", browser_folder)) {

                // Make sure its Google Chrome and not Chromium
                if(_webui_is_google_chrome_folder(browser_folder)) {

                    // Google Chrome Found (one user)
                    sprintf(win->browser_path, "\"%s\\chrome.exe\"", browser_folder);
                    return true;
                }
            }

            return false;

        #elif __APPLE__

            // Google Chrome on macOS
            if(_webui_cmd_sync("open -R -a \"Google Chrome\"", false) == 0) {

                sprintf(win->browser_path, "open -W \"/Applications/Google Chrome.app\" --args");
                return true;
            }
            else
                return false;
        #else

            // Google Chrome on Linux
            if(_webui_cmd_sync("google-chrome --version", false) == 0) {

                sprintf(win->browser_path, "google-chrome");
                return true;
            }
            else if(_webui_cmd_sync("google-chrome-stable --version", false) == 0) {

                sprintf(win->browser_path, "google-chrome-stable");
                return true;
            }
            else
                return false;

        #endif
    }
    else if(browser == Edge) {

        // Edge

        #ifdef _WIN32

            // Edge on Windows

            char browser_fullpath[WEBUI_MAX_PATH];

            // Search in `HKEY_LOCAL_MACHINE` (If Edge installed for multi-user)
            if(_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\msedge.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Edge Found (multi-user)
                    sprintf(win->browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            // Search in `HKEY_CURRENT_USER` (If Edge installed for one user)
            if(_webui_get_windows_reg_value(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\msedge.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Edge Found (one user)
                    sprintf(win->browser_path, "\"%s\"", browser_fullpath);
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
    else if(browser == Epic) {

        // Epic Privacy Browser

        #ifdef _WIN32

            // Epic on Windows

            char browser_fullpath[WEBUI_MAX_PATH];

            // Search in `HKEY_CURRENT_USER` (If Epic installed for one user)
            if(_webui_get_windows_reg_value(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\epic.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Epic Found (one user)
                    sprintf(win->browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            // Search in `HKEY_LOCAL_MACHINE` (If Epic installed for multi-user)
            if(_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\epic.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Epic Found (multi-user)
                    sprintf(win->browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            return false;

        #elif __APPLE__

            // Epic on macOS
            if(_webui_cmd_sync("open -R -a \"Epic\"", false) == 0) {

                sprintf(win->browser_path, "open -W \"/Applications/Epic.app\" --args");
                return true;
            }
            else
                return false;
        #else

            // Epic on Linux
            if(_webui_cmd_sync("epic --version", false) == 0) {

                sprintf(win->browser_path, "epic");
                return true;
            }
            else
                return false;
        #endif
    }
    else if(browser == Vivaldi) {

        // Vivaldi Browser

        #ifdef _WIN32

            // Vivaldi on Windows

            char browser_fullpath[WEBUI_MAX_PATH];

            // Search in `HKEY_LOCAL_MACHINE` (If Vivaldi installed for multi-user)
            if(_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\vivaldi.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Vivaldi Found (multi-user)
                    sprintf(win->browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            // Search in `HKEY_CURRENT_USER` (If Vivaldi installed for one user)
            if(_webui_get_windows_reg_value(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\vivaldi.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Vivaldi Found (one user)
                    sprintf(win->browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            return false;

        #elif __APPLE__

            // Vivaldi on macOS
            if(_webui_cmd_sync("open -R -a \"Vivaldi\"", false) == 0) {

                sprintf(win->browser_path, "open -W \"/Applications/Vivaldi.app\" --args");
                return true;
            }
            else
                return false;
        #else

            // Vivaldi on Linux
            if(_webui_cmd_sync("vivaldi --version", false) == 0) {

                sprintf(win->browser_path, "vivaldi");
                return true;
            }
            else
                return false;
        #endif
    }
    else if(browser == Brave) {

        // Brave Browser

        #ifdef _WIN32

            // Brave on Windows

            char browser_fullpath[WEBUI_MAX_PATH];

            // Search in `HKEY_LOCAL_MACHINE` (If Brave installed for multi-user)
            if(_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\brave.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Brave Found (multi-user)
                    sprintf(win->browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            // Search in `HKEY_CURRENT_USER` (If Brave installed for one user)
            if(_webui_get_windows_reg_value(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\brave.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Brave Found (one user)
                    sprintf(win->browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            return false;

        #elif __APPLE__

            // Brave on macOS
            if(_webui_cmd_sync("open -R -a \"Brave\"", false) == 0) {

                sprintf(win->browser_path, "open -W \"/Applications/Brave.app\" --args");
                return true;
            }
            else
                return false;
        #else

            // Brave on Linux
            if(_webui_cmd_sync("brave-browser --version", false) == 0) {

                sprintf(win->browser_path, "brave-browser");
                return true;
            }
            else
                return false;
        #endif
    }
    else if(browser == Firefox) {

        // Firefox
        
        #ifdef _WIN32
        
            // Firefox on Windows

            char browser_fullpath[WEBUI_MAX_PATH];

            // Search in `HKEY_LOCAL_MACHINE` (If Firefox installed for multi-user)
            if(_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\firefox.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Firefox Found (multi-user)
                    sprintf(win->browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            // Search in `HKEY_CURRENT_USER` (If Firefox installed for one user)
            if(_webui_get_windows_reg_value(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\firefox.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Firefox Found (one user)
                    sprintf(win->browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            return false;

        #elif __APPLE__
            
            // Firefox on macOS
            if(_webui_cmd_sync("open -R -a \"firefox\"", false) == 0) {

                sprintf(win->browser_path, "open -W \"/Applications/Firefox.app\" --args");
                return true;
            }
            else
                return false;
        #else

            // Firefox on Linux

            if(_webui_cmd_sync("firefox -v", false) == 0) {

                sprintf(win->browser_path, "firefox");
                return true;
            }
            else
                return false;

        #endif

    }
    else if(browser == Yandex) {

        // Yandex Browser

        #ifdef _WIN32

            // Yandex on Windows

            char browser_fullpath[WEBUI_MAX_PATH];

            // Search in `HKEY_CURRENT_USER` (If Yandex installed for one user)
            if(_webui_get_windows_reg_value(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\browser.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Yandex Found (one user)
                    sprintf(win->browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            // Search in `HKEY_LOCAL_MACHINE` (If Yandex installed for multi-user)
            if(_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\browser.exe", "", browser_fullpath)) {

                // Make sure the browser executable file exist
                if(_webui_file_exist(browser_fullpath)) {

                    // Yandex Found (multi-user)
                    sprintf(win->browser_path, "\"%s\"", browser_fullpath);
                    return true;
                }
            }

            return false;

        #elif __APPLE__

            // Yandex on macOS
            if(_webui_cmd_sync("open -R -a \"Yandex\"", false) == 0) {

                sprintf(win->browser_path, "open -W \"/Applications/Yandex.app\" --args");
                return true;
            }
            else
                return false;
        #else

            // Yandex on Linux
            if(_webui_cmd_sync("yandex-browser --version", false) == 0) {

                sprintf(win->browser_path, "yandex-browser");
                return true;
            }
            else
                return false;
        #endif
    }
    else if(browser == Chromium) {

        // The Chromium Projects

        #ifdef _WIN32

            // Chromium on Windows

            char browser_folder[WEBUI_MAX_PATH];

            // Search in `HKEY_CURRENT_USER` (If Chromium installed for one user)
            if(_webui_get_windows_reg_value(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\chrome.exe", "Path", browser_folder)) {

                // Make sure its Chromium and not Google Chrome
                if(!_webui_is_google_chrome_folder(browser_folder)) {

                    // Chromium Found (one user)
                    sprintf(win->browser_path, "\"%s\\chrome.exe\"", browser_folder);
                    return true;
                }
            }

            // Search in `HKEY_LOCAL_MACHINE` (If Chromium installed for multi-user)
            if(_webui_get_windows_reg_value(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\chrome.exe", "Path", browser_folder)) {

                // Make sure its Chromium and not Google Chrome
                if(!_webui_is_google_chrome_folder(browser_folder)) {

                    // Chromium Found (multi-user)
                    sprintf(win->browser_path, "\"%s\\chrome.exe\"", browser_folder);
                    return true;
                }
            }

            return false;

        #elif __APPLE__

            // Chromium on macOS
            if(_webui_cmd_sync("open -R -a \"Chromium\"", false) == 0) {

                sprintf(win->browser_path, "open -W \"/Applications/Chromium.app\" --args");
                return true;
            }
            else
                return false;
        #else

            // Chromium on Linux
            if(_webui_cmd_sync("chromium-browser --version", false) == 0) {

                sprintf(win->browser_path, "chromium-browser");
                return true;
            }
            else
                return false;
        #endif
    }

    return false;
}

static void _webui_clean(void) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_clean()... \n");
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

int _webui_cmd_sync(char* cmd, bool show) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_cmd_sync()... \n");
    #endif

    // Run sync command and
    // return the exit code

    char buf[1024];

    #ifdef _WIN32
        sprintf(buf, "cmd /c \"%s\" > nul 2>&1", cmd);
        #ifdef WEBUI_LOG
            printf("[Core]\t\t_webui_cmd_sync() -> Running [%s] \n", buf);
        #endif
        return _webui_system_win32(buf, show);
    #else
        sprintf(buf, "%s >>/dev/null 2>>/dev/null ", cmd);
        #ifdef WEBUI_LOG
            printf("[Core]\t\t_webui_cmd_sync() -> Running [%s] \n", buf);
        #endif
        int r =  system(buf);
        r = (r != -1 && r != 127 && WIFEXITED(r)) ? WEXITSTATUS(r) : -1;
        return r;
    #endif
}

int _webui_cmd_async(char* cmd, bool show) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_cmd_async()... \n");
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

int _webui_run_browser(_webui_window_t* win, char* cmd) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_run_browser()... \n");
    #endif

    // Run a async command
    return _webui_cmd_async(cmd, false);
}

bool _webui_browser_start_chrome(_webui_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_browser_start_chrome([%s])... \n", address);
    #endif
    
    // -- Google Chrome ----------------------

    if(win->current_browser != 0 && win->current_browser != Chrome)
        return false;

    if(!_webui_browser_exist(win, Chrome))
        return false;
    
    if(!_webui_browser_create_profile_folder(win, Chrome))
        return false;
    
    char arg[1024];
    sprintf(arg, " --user-data-dir=\"%s\" --no-first-run --disable-gpu --disable-software-rasterizer --no-proxy-server --safe-mode --disable-extensions --disable-background-mode --disable-plugins --disable-plugins-discovery --disable-translate --bwsi --disable-sync --disable-sync-preferences --app=", win->profile_path);

    char full[1024];
    sprintf(full, "%s%s%s", win->browser_path, arg, address);

    if(_webui_run_browser(win, full) == 0) {

        win->current_browser = Chrome;
        _webui_core.current_browser = Chrome;
        return true;
    }
    else
        return false;
}

bool _webui_browser_start_edge(_webui_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_browser_start_edge([%s])... \n", address);
    #endif

    // -- Microsoft Edge ----------------------

    if(win->current_browser != 0 && win->current_browser != Edge)
        return false;

    if(!_webui_browser_exist(win, Edge))
        return false;
    
    if(!_webui_browser_create_profile_folder(win, Edge))
        return false;

    char arg[1024];
    sprintf(arg, " --user-data-dir=\"%s\" --no-first-run --disable-gpu --disable-software-rasterizer --no-proxy-server --safe-mode --disable-extensions --disable-background-mode --disable-plugins --disable-plugins-discovery --disable-translate --bwsi --disable-sync --disable-sync-preferences --app=", win->profile_path);

    char full[1024];
    sprintf(full, "%s%s%s", win->browser_path, arg, address);

    if(_webui_run_browser(win, full) == 0) {

        win->current_browser = Edge;
        _webui_core.current_browser = Edge;
        return true;
    }
    else
        return false;
}

bool _webui_browser_start_epic(_webui_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_browser_start_epic([%s])... \n", address);
    #endif

    // -- Epic Privacy Browser ----------------------

    if(win->current_browser != 0 && win->current_browser != Epic)
        return false;

    if(!_webui_browser_exist(win, Epic))
        return false;
    
    if(!_webui_browser_create_profile_folder(win, Epic))
        return false;

    char arg[1024];
    sprintf(arg, " --user-data-dir=\"%s\" --no-first-run --disable-gpu --disable-software-rasterizer --no-proxy-server --safe-mode --disable-extensions --disable-background-mode --disable-plugins --disable-plugins-discovery --disable-translate --bwsi --disable-sync --disable-sync-preferences --app=", win->profile_path);

    char full[1024];
    sprintf(full, "%s%s%s", win->browser_path, arg, address);

    if(_webui_run_browser(win, full) == 0) {

        win->current_browser = Epic;
        _webui_core.current_browser = Epic;
        return true;
    }
    else
        return false;
}

bool _webui_browser_start_vivaldi(_webui_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_browser_start_vivaldi([%s])... \n", address);
    #endif

    // -- Vivaldi Browser ----------------------

    if(win->current_browser != 0 && win->current_browser != Vivaldi)
        return false;

    if(!_webui_browser_exist(win, Vivaldi))
        return false;
    
    if(!_webui_browser_create_profile_folder(win, Vivaldi))
        return false;

    char arg[1024];
    sprintf(arg, " --user-data-dir=\"%s\" --no-first-run --disable-gpu --disable-software-rasterizer --no-proxy-server --safe-mode --disable-extensions --disable-background-mode --disable-plugins --disable-plugins-discovery --disable-translate --bwsi --disable-sync --disable-sync-preferences --app=", win->profile_path);

    char full[1024];
    sprintf(full, "%s%s%s", win->browser_path, arg, address);

    if(_webui_run_browser(win, full) == 0) {

        win->current_browser = Vivaldi;
        _webui_core.current_browser = Vivaldi;
        return true;
    }
    else
        return false;
}

bool _webui_browser_start_brave(_webui_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_browser_start_brave([%s])... \n", address);
    #endif

    // -- Brave Browser ----------------------

    if(win->current_browser != 0 && win->current_browser != Brave)
        return false;

    if(!_webui_browser_exist(win, Brave))
        return false;
    
    if(!_webui_browser_create_profile_folder(win, Brave))
        return false;

    char arg[1024];
    sprintf(arg, " --user-data-dir=\"%s\" --no-first-run --disable-gpu --disable-software-rasterizer --no-proxy-server --safe-mode --disable-extensions --disable-background-mode --disable-plugins --disable-plugins-discovery --disable-translate --bwsi --disable-sync --disable-sync-preferences --app=", win->profile_path);

    char full[1024];
    sprintf(full, "%s%s%s", win->browser_path, arg, address);

    if(_webui_run_browser(win, full) == 0) {

        win->current_browser = Brave;
        _webui_core.current_browser = Brave;
        return true;
    }
    else
        return false;
}

bool _webui_browser_start_firefox(_webui_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_browser_start_firefox([%s])... \n", address);
    #endif

    // -- Mozilla Firefox ----------------------

    if(win->current_browser != 0 && win->current_browser != Firefox)
        return false;

    if(!_webui_browser_exist(win, Firefox))
        return false;

    if(!_webui_browser_create_profile_folder(win, Firefox))
        return false;

    char full[1024];
    sprintf(full, "%s -P WebUI -purgecaches -new-window %s", win->browser_path, address);

    if(_webui_run_browser(win, full) == 0) {

        win->current_browser = Firefox;
        _webui_core.current_browser = Firefox;
        return true;
    }
    else
        return false;
}

bool _webui_browser_start_yandex(_webui_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_browser_start_yandex([%s])... \n", address);
    #endif

    // -- Yandex Browser ----------------------

    if(win->current_browser != 0 && win->current_browser != Yandex)
        return false;

    if(!_webui_browser_exist(win, Yandex))
        return false;
    
    if(!_webui_browser_create_profile_folder(win, Yandex))
        return false;

    char arg[1024];
    sprintf(arg, " --user-data-dir=\"%s\" --no-first-run --disable-gpu --disable-software-rasterizer --no-proxy-server --safe-mode --disable-extensions --disable-background-mode --disable-plugins --disable-plugins-discovery --disable-translate --bwsi --disable-sync --disable-sync-preferences --app=", win->profile_path);

    char full[1024];
    sprintf(full, "%s%s%s", win->browser_path, arg, address);

    if(_webui_run_browser(win, full) == 0) {

        win->current_browser = Yandex;
        _webui_core.current_browser = Yandex;
        return true;
    }
    else
        return false;
}

bool _webui_browser_start_chromium(_webui_window_t* win, const char* address) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_browser_start_chromium([%s])... \n", address);
    #endif
    
    // -- The Chromium Projects -------------------

    if (win->current_browser != 0 && win->current_browser != Chromium)
        return false;

    if (!_webui_browser_exist(win, Chromium))
        return false;
    
    if (!_webui_browser_create_profile_folder(win, Chromium))
        return false;
    
    char arg[1024];
    sprintf(arg, " --user-data-dir=\"%s\" --no-first-run --disable-gpu --disable-software-rasterizer --no-proxy-server --safe-mode --disable-extensions --disable-background-mode --disable-plugins --disable-plugins-discovery --disable-translate --bwsi --disable-sync --disable-sync-preferences --app=", win->profile_path);

    char full[1024];
    sprintf(full, "%s%s%s", win->browser_path, arg, address);

    if (_webui_run_browser(win, full) == 0) {

        win->current_browser = Chromium;
        _webui_core.current_browser = Chromium;
        return true;
    }
    else
        return false;
}

bool _webui_browser_start(_webui_window_t* win, const char* address, unsigned int browser) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_browser_start([%s], [%u])... \n", address, browser);
    #endif

    // Non existing browser
    if(browser > 10)
        return false;
    
    // Current browser
    if(browser == AnyBrowser && _webui_core.current_browser != 0)
        browser = _webui_core.current_browser;

    // TODO: Convert address from [/...] to [file://...]

    if(browser != 0) {

        // User specified browser

        if(browser == Chrome)
            return _webui_browser_start_chrome(win, address);
        else if(browser == Edge)
            return _webui_browser_start_edge(win, address);
        else if(browser == Epic)
            return _webui_browser_start_epic(win, address);
        else if(browser == Vivaldi)
            return _webui_browser_start_vivaldi(win, address);
        else if(browser == Brave)
            return _webui_browser_start_brave(win, address);
        else if(browser == Firefox)
            return _webui_browser_start_firefox(win, address);
        else if(browser == Yandex)
            return _webui_browser_start_yandex(win, address);
        else if(browser == Chromium)
            return _webui_browser_start_chromium(win, address);
        else
            return false;
    }
    else if(win->current_browser != 0) {

        // Already used browser

        if(win->current_browser == Chrome)
            return _webui_browser_start_chrome(win, address);
        else if(win->current_browser == Edge)
            return _webui_browser_start_edge(win, address);
        else if(win->current_browser == Epic)
            return _webui_browser_start_epic(win, address);
        else if(win->current_browser == Vivaldi)
            return _webui_browser_start_vivaldi(win, address);
        else if(win->current_browser == Brave)
            return _webui_browser_start_brave(win, address);
        else if(win->current_browser == Firefox)
            return _webui_browser_start_firefox(win, address);
        else if(win->current_browser == Yandex)
            return _webui_browser_start_yandex(win, address);
        else if(browser == Chromium)
            return _webui_browser_start_chromium(win, address);
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
                                            return false;
        #endif
    }

    return true;
}

bool _webui_set_root_folder(_webui_window_t* win, const char* path) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_set_root_folder([%s])... \n", path);
    #endif

    if((path == NULL) || (strlen(path) > WEBUI_MAX_PATH))
        return false;

    win->is_embedded_html = true;

    if(_webui_is_empty(path))
        sprintf(win->server_root_path, "%s", WEBUI_DEFAULT_PATH);
    else
        sprintf(win->server_root_path, "%s", path);
    
    webui_set_multi_access(win, true);

    return true;
}

bool _webui_show(_webui_window_t* win, const char* content, unsigned int browser) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_show([%u])... \n", browser);
    #endif

    if(_webui_is_empty(content))
        return false;

    // Some wrappers does not guarantee `content` to
    // stay valid, so, let's make our copy right now
    size_t content_len = strlen(content);
    const char* content_cpy = (const char*)_webui_malloc(content_len);
    memcpy((char*)content_cpy, content, content_len);

    if(strstr(content_cpy, "<html")) {

        // Embedded HTML
        #ifdef WEBUI_LOG
            printf("[Core]\t\t_webui_show() -> Embedded HTML:\n");
            printf("- - -[HTML]- - - - - - - - - -\n%s\n- - - - - - - - - - - - - - - -\n", content_cpy);
        #endif
        
        return _webui_show_window(win, content_cpy, true, browser);
    }
    else {

        // File
        #ifdef WEBUI_LOG
            printf("[User] webui_show() -> File: [%s]\n", content_cpy);
        #endif

        if(content_len > WEBUI_MAX_PATH || strstr(content_cpy, "<"))
            return false;
        
        return _webui_show_window(win, content_cpy, false, browser);
    }
}

bool _webui_show_window(_webui_window_t* win, const char* content, bool is_embedded_html, unsigned int browser) {

    #ifdef WEBUI_LOG
        if(is_embedded_html)
            printf("[Core]\t\t_webui_show_window(HTML, [%u])... \n", browser);
        else
            printf("[Core]\t\t_webui_show_window(FILE, [%u])... \n", browser);
    #endif

    _webui_init();

    char* url = NULL;
    unsigned int port = (win->server_port == 0 ? _webui_get_free_port() : win->server_port);

    // Initialization
    if(win->html != NULL)
        _webui_free_mem((void *)win->html);
    if(win->url != NULL)
        _webui_free_mem((void *)win->url);

    if(is_embedded_html) {

        // Show a window using the embedded HTML
        win->is_embedded_html = true;
        win->html = (content == NULL ? webui_empty_string : content);

        // Generate the URL
        size_t url_len = 32; // [http][domain][port]
        url = (char*) _webui_malloc(url_len);
        sprintf(url, "http://localhost:%u", port);
    }
    else {

        // Show a window using a local file
        win->is_embedded_html = false;
        win->html = NULL;

        // Generate the URL
        size_t url_len = 32 + strlen(content); // [http][domain][port][file]
        url = (char*) _webui_malloc(url_len);
        sprintf(url, "http://localhost:%u/%s", port, content);
    }

    // Set URL
    win->url = url;
    win->server_port = port;
    
    if(!webui_is_shown(win)) {

        // Start a new window

        // Run browser
        if(!_webui_browser_start(win, win->url, browser)) {

            // Browser not available
            _webui_free_mem((void *)win->html);
            _webui_free_mem((void *)win->url);
            _webui_free_port(win->server_port);
            return false;
        }
        
        // New server thread
        #ifdef _WIN32
            HANDLE thread = CreateThread(NULL, 0, _webui_server_start, (void *)win, 0, NULL);
            win->server_thread = thread;
            if(thread != NULL)
                CloseHandle(thread);
        #else
            pthread_t thread;
            pthread_create(&thread, NULL, &_webui_server_start, (void *)win);
            pthread_detach(thread);
            win->server_thread = thread;
        #endif
    }
    else {

        // Refresh an existing running window

        // Prepare packets
        size_t packet_len = 3 + strlen(url); // [header][url]
        char* packet = (char*) _webui_malloc(packet_len);
        packet[0] = WEBUI_HEADER_SIGNATURE; // Signature
        packet[1] = WEBUI_HEADER_SWITCH;    // Type
        packet[2] = 0;                      // ID
        for(unsigned int i = 0; i < strlen(win->url); i++) // URL
            packet[i + 3] = win->url[i];

        // Send the packet
        _webui_window_send(win, packet, packet_len);
        _webui_free_mem((void *)packet);
    }

    return true;
}

static void _webui_window_event(_webui_window_t* win, char* webui_internal_id, char* element, void* data, unsigned int data_len, int event_type) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_window_event([%s], [%s])... \n", webui_internal_id, element);
    #endif

    // Create a thread, and call the used cb function
    _webui_cb_t* arg = (_webui_cb_t*) _webui_malloc(sizeof(_webui_cb_t));
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

static void _webui_window_send(_webui_window_t* win, char* packet, size_t packets_size) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_window_send()... \n");
        printf("[Core]\t\t_webui_window_send() -> %d bytes \n", (int)packets_size);
        printf("[Core]\t\t_webui_window_send() -> [ ");
            _webui_print_hex(packet, 3);
        printf("]\n");
        printf("[Core]\t\t_webui_window_send() -> [%.*s] \n", (int)(packets_size - 3), (const char*)&packet[3]);
    #endif
    
    if(!win->connected ||
        _webui_core.mg_connections[win->window_number] == NULL ||
        packet == NULL ||
        packets_size < 4)
        return;

    struct mg_connection* c = _webui_core.mg_connections[win->window_number];
    mg_ws_send(
        c, 
        packet, 
        packets_size, 
        WEBSOCKET_OP_BINARY
    );
}

bool _webui_get_data(const char* packet, size_t packet_len, unsigned int pos, size_t* data_len, char** data) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_get_data()... \n");
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

static void _webui_window_receive(_webui_window_t* win, const char* packet, size_t len) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_window_receive()... \n");
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
        
        #ifdef WEBUI_LOG
            printf("[Core]\t\t_webui_window_receive() -> %d bytes \n", (int)element_len);
            printf("[Core]\t\t_webui_window_receive() -> [%s] \n", element);
        #endif

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

        #ifdef WEBUI_LOG
            printf("[Core]\t\t_webui_window_receive() -> run_id = 0x%02x \n", run_id);
            printf("[Core]\t\t_webui_window_receive() -> error = 0x%02x \n", error);
            printf("[Core]\t\t_webui_window_receive() -> %d bytes of data\n", (int)data_len);
            printf("[Core]\t\t_webui_window_receive() -> data = [%s] @ 0x%p\n", data, data);
        #endif

        // Initialize pipe
        if((void *)_webui_core.run_responses[run_id] != NULL)
            _webui_free_mem((void *)_webui_core.run_responses[run_id]);

        // Set pipe
        if(data_status && data_len > 0) {

            _webui_core.run_error[run_id] = error;
            _webui_core.run_responses[run_id] = data;
        }
        else {

            // Empty Result
            _webui_core.run_error[run_id] = error;
            _webui_core.run_responses[run_id] = webui_empty_string;
        }

        // Send ready signal to webui_script()
        _webui_core.run_done[run_id] = true;
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
        
        #ifdef WEBUI_LOG
            printf("[Core]\t\t_webui_window_receive() -> %d bytes \n", (int)element_len);
            printf("[Core]\t\t_webui_window_receive() -> [%s] \n", element);
        #endif

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

        // Events
        if(win->has_events) {

            // Get URL
            char* url;
            size_t url_len;
            if(!_webui_get_data(packet, len, 3, &url_len, &url))
                return;
            
            #ifdef WEBUI_LOG
                printf("[Core]\t\t_webui_window_receive() -> %d bytes \n", (int)url_len);
                printf("[Core]\t\t_webui_window_receive() -> [%s] \n", url);
            #endif

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
}

char* _webui_get_current_path(void) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_get_current_path()... \n");
    #endif

    char* path = (char*) _webui_malloc(WEBUI_MAX_PATH);
    if(WEBUI_GET_CURRENT_DIR (path, WEBUI_MAX_PATH) == NULL)
        path[0] = 0x00;

    return path;
}

static void _webui_free_port(unsigned int port) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_free_port([%u])... \n", port);
    #endif

    for(unsigned int i = 0; i < WEBUI_MAX_ARRAY; i++) {
        if(_webui_core.used_ports[i] == port) {
            _webui_core.used_ports[i] = 0;
            break;
        }
    }
}

static void _webui_wait_for_startup(void) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_wait_for_startup()... \n");
    #endif

    if(_webui_core.connections > 0)
        return;

    // Wait for the first connection
    for(unsigned int n = 0; n <= (_webui_core.startup_timeout * 10); n++) {

        if(_webui_core.connections > 0)
            break;
        
        _webui_sleep(50);
    }

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_wait_for_startup() -> Finish.\n");
    #endif
}

unsigned int _webui_get_new_window_number(void) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_get_new_window_number()... \n");
    #endif

    return ++_webui_core.last_window;
}

unsigned int _webui_get_free_port(void) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_get_free_port()... \n");
    #endif

    #ifdef _WIN32
        srand((unsigned int)time(NULL));
    #else
        srand(time(NULL));
    #endif

    unsigned int port = (rand() % (WEBUI_MAX_PORT + 1 - WEBUI_MIN_PORT)) + WEBUI_MIN_PORT;

    for(unsigned int i = WEBUI_MIN_PORT; i <= WEBUI_MAX_PORT; i++) {

        // Search [port] in [_webui_core.used_ports]
        bool found = false;
        for(unsigned int j = 0; j < WEBUI_MAX_ARRAY; j++) {
            if(_webui_core.used_ports[j] == port) {
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
        if(_webui_core.used_ports[i] == 0) {
            _webui_core.used_ports[i] = port;
            break;
        }
    }

    return port;
}

static void _webui_init(void) {

    if(_webui_core.initialized)
        return;    

    #ifdef WEBUI_LOG
        printf("[Core]\t\tWebUI v%s \n", WEBUI_VERSION);
        printf("[Core]\t\t_webui_init()... \n");
    #endif

    // Initializing
    memset(&_webui_core, 0, sizeof(_webui_core_t));

    _webui_core.initialized     = true;
    _webui_core.startup_timeout = WEBUI_DEF_TIMEOUT;
    _webui_core.executable_path = _webui_get_current_path();
}

unsigned int _webui_get_cb_index(char* webui_internal_id) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_get_cb_index([%s])... \n", webui_internal_id);
    #endif

    if(webui_internal_id != NULL) {

        for(unsigned int i = 1; i < WEBUI_MAX_ARRAY; i++) {

            if(_webui_core.html_elements[i] != NULL && !_webui_is_empty(_webui_core.html_elements[i])) 
                if(strcmp(_webui_core.html_elements[i], webui_internal_id) == 0)
                    return i;
        }
    }

    return 0;
}

unsigned int _webui_set_cb_index(char* webui_internal_id) {

    #ifdef WEBUI_LOG
        printf("[Core]\t\t_webui_set_cb_index([%s])... \n", webui_internal_id);
    #endif

    // Add
    for(unsigned int i = 1; i < WEBUI_MAX_ARRAY; i++) {

        if(_webui_is_empty(_webui_core.html_elements[i])) {

            _webui_core.html_elements[i] = webui_internal_id;

            return i;
        }
    }

    return 0;
}

#ifdef WEBUI_LOG
    static void _webui_print_hex(const char* data, size_t len) {

        for(size_t i = 0; i < len; i++) {

            printf("0x%02X ", (unsigned char) *data);
            data++;
        }
    }
#endif

WEBUI_SERVER_START
{
    _webui_window_t* win = (_webui_window_t*) arg;
    
    #ifdef WEBUI_LOG
        printf("[Core]\t\t[Thread] _webui_server_start(%s)... \n", win->url);
    #endif

    // Initialization
    _webui_core.servers++;
    win->server_running = true;
    if(_webui_core.startup_timeout < 1)
        _webui_core.startup_timeout = 0;
    if(_webui_core.startup_timeout > 30)
        _webui_core.startup_timeout = 30;

    // Start Server
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    _webui_core.mg_mgrs[win->window_number] = &mgr;

    if(mg_http_listen(&mgr, win->url, _webui_server_event_handler, (void *)win) != NULL) {

        if(_webui_core.startup_timeout > 0) {

            #ifdef WEBUI_LOG
                printf("[Core]\t\t[Thread] _webui_server_start()... Listening Success\n");
                printf("[Core]\t\t[Thread] _webui_server_start()... Timeout is %u seconds\n", _webui_core.startup_timeout);
            #endif

            bool stop = false;

            while(!stop) {

                if(!win->connected) {

                    // Wait for first connection
                    _webui_timer_t timer_1;
                    _webui_timer_start(&timer_1);
                    for(;;) {

                        // Stop if window is connected
                        mg_mgr_poll(&mgr, 1);
                        if(win->connected)
                            break;

                        // Stop if timer is finished
                        if(_webui_timer_is_end(&timer_1, (_webui_core.startup_timeout * 1000)))
                            break;
                    }

                    if(!win->connected && win->html_handled) {

                        // At this moment the browser is already started and HTML
                        // is already handled, so, let's wait more time to give
                        // the WebSocket an extra one second to connect.
                        
                        _webui_timer_t timer_2;
                        _webui_timer_start(&timer_2);
                        for(;;) {

                            // Stop if window is connected
                            mg_mgr_poll(&mgr, 1);
                            if(win->connected)
                                break;

                            // Stop if timer is finished
                            if(_webui_timer_is_end(&timer_2, 1000))
                                break;
                        }
                    }
                    
                    if(!win->connected)
                        stop = true; // First run failed
                }
                else {

                    // UI is connected

                    while(!stop) {

                        // Wait forever for disconnection

                        mg_mgr_poll(&mgr, 1);

                        // Exit signal
                        if(_webui_core.exit_now) {
                            stop = true;
                            break;
                        }

                        if(!win->connected) {

                            // The UI is just get disconnected
                            // probably the user did a refresh
                            // let's wait for re-connection...

                            _webui_timer_t timer_3;
                            _webui_timer_start(&timer_3);
                            for(;;) {

                                // Stop if window is re-connected
                                mg_mgr_poll(&mgr, 1);
                                if(win->connected)
                                    break;

                                // Stop if timer is finished
                                if(_webui_timer_is_end(&timer_3, 600))
                                    break;
                            }

                            if(!win->connected) {

                                stop = true;
                                break;
                            }
                        }
                    }
                }
            }
        }

        // Let's check the flag again, there is a change that
        // the flag has ben changed during the first loop for
        // example when set_timeout() get called later
        if(_webui_core.startup_timeout == 0) {

            #ifdef WEBUI_LOG
                printf("[Core]\t\t[Thread] _webui_server_start(%s)... Listening Success -> Infinite Loop... \n", win->url);
            #endif

            // Wait forever
            for(;;) {

                mg_mgr_poll(&mgr, 1);
                if(_webui_core.exit_now)
                    break;
            }
        }
    }
    else {

        #ifdef WEBUI_LOG
            printf("[Core]\t\t[Thread] _webui_server_start(%s)... Listening failed\n", win->url);
        #endif
    }

    // Stop server
    mg_mgr_free(&mgr);
    _webui_core.servers--;

    #ifdef WEBUI_LOG
        printf("[Core]\t\t[Thread] _webui_server_start()... Server Stop.\n");
    #endif

    // Clean
    win->server_running = false;
    win->html_handled = false;
    win->connected = false;
    _webui_core.mg_mgrs[win->window_number] = NULL;
    _webui_core.mg_connections[win->window_number] = NULL;
    _webui_free_port(win->server_port);

    THREAD_RETURN
}

WEBUI_CB
{
    _webui_cb_t* arg = (_webui_cb_t*) _arg;

    #ifdef WEBUI_LOG
        printf("[Core]\t\t[Thread] _webui_cb()... \n");
    #endif

    webui_event_t e;
    e.element = arg->element_name;
    e.window = arg->win;
    e.data = arg->data;
    e.response = NULL;
    e.type = arg->event_type;

    // Check for all events-binded function
    if(arg->win->has_events) {

        char* events_id = _webui_generate_internal_id(arg->win, "");
        unsigned int events_cb_index = _webui_get_cb_index(events_id);
        _webui_free_mem((void *)events_id);

        if(events_cb_index > 0 && _webui_core.cb[events_cb_index] != NULL) {

            // Call user all events cb
            _webui_core.cb[events_cb_index](&e);
        }
    }

    // Check for the binded function
    if(arg->element_name != NULL && !_webui_is_empty(arg->element_name)) {

        unsigned int cb_index = _webui_get_cb_index(arg->webui_internal_id);
        if(cb_index > 0 && _webui_core.cb[cb_index] != NULL) {

            // Call user cb
            _webui_core.cb[cb_index](&e);
        }
    }

    #ifdef WEBUI_LOG
        printf("[Core]\t\t[Thread] _webui_cb()... Stop.\n");
    #endif    

    // Free
    _webui_free_mem((void *)e.response);
    _webui_free_mem((void *)arg->webui_internal_id);
    _webui_free_mem((void *)arg->element_name);
    _webui_free_mem((void *)arg);

    THREAD_RETURN
}

#ifdef _WIN32

    bool _webui_socket_test_listen_win32(unsigned int port_num) {
    
        #ifdef WEBUI_LOG
            printf("[Core]\t\t_webui_socket_test_listen_win32([%u])... \n", port_num);
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
        sprintf(&the_port[0], "%u", port_num);
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

    int _webui_system_win32(char* cmd, bool show) {

        #ifdef WEBUI_LOG
            printf("[Core]\t\t_webui_system_win32()... \n");
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

    bool _webui_get_windows_reg_value(HKEY key, const char* reg, const char* value_name, char value[WEBUI_MAX_PATH]) {

        #ifdef WEBUI_LOG
            printf("[Core]\t\t_webui_get_windows_reg_value([%s])... \n", reg);
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

    BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
        return true;
    }
    
#endif
