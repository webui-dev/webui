var _webui_log = _webui_log ?? false; //If webui.c define _webui_log then use it, instead set it to false
var _webui_ws; 
var _webui_ws_status = false; 
var _webui_ws_status_once = false; 
var _webui_close_reason = 0; 
var _webui_close_value; 
var _webui_has_events = false; 
var _webui_fn_id = new Uint8Array(1); 
var _webui_fn_promise_resolve = []; 
const WEBUI_HEADER_SIGNATURE = 221; 
const WEBUI_HEADER_JS = 254; 
const WEBUI_HEADER_JS_QUICK = 253; 
const WEBUI_HEADER_CLICK = 252; 
const WEBUI_HEADER_SWITCH = 251; 
const WEBUI_HEADER_CLOSE = 250; 
const WEBUI_HEADER_CALL_FUNC = 249; 
function _webui_close(reason = 0, value = 0) { 
        if(reason == WEBUI_HEADER_SWITCH) _webui_send_event_navigation(value); 
        _webui_ws_status = false; 
        _webui_close_reason = reason; 
        _webui_close_value = value; 
        _webui_ws.close(); 
} 
function _webui_freeze_ui() { 
        document.body.style.filter = 'contrast(1%)'; 
} 
function _webui_start() { 
        if('WebSocket' in window) { 
            if(_webui_bind_list.includes(_webui_win_num + '/')) _webui_has_events = true; 
        _webui_ws = new WebSocket('ws://localhost:' + _webui_port + '/_webui_ws_connect'); 
        _webui_ws.binaryType = 'arraybuffer'; 
        _webui_ws.onopen = function () { 
                _webui_ws.binaryType = 'arraybuffer'; 
            _webui_ws_status = true; 
            _webui_ws_status_once = true; 
            _webui_fn_id[0] = 1; 
            if(_webui_log) 
                console.log('WebUI -> Connected'); 
            _webui_clicks_listener(); 
        }; 
        _webui_ws.onerror = function () { 
                if(_webui_log) 
                console.log('WebUI -> Connection Failed'); 
            _webui_freeze_ui(); 
        }; 
        _webui_ws.onclose = function (evt) { 
                _webui_ws_status = false; 
            if(_webui_close_reason === WEBUI_HEADER_SWITCH) { 
                    if(_webui_log) 
                    console.log('WebUI -> Connection lost -> Navigation to [' + _webui_close_value + ']'); 
                window.location.replace(_webui_close_value); 
            } else { 
                    if(_webui_log) { 
                        console.log('WebUI -> Connection lost (' + evt.code + ')'); 
                    _webui_freeze_ui(); 
                } else _webui_close_window_timer(); 
            } 
        }; 
        _webui_ws.onmessage = function (evt) { 
                    const buffer8 = new Uint8Array(evt.data); 
                if(buffer8.length < 4) return; 
                if(buffer8[0] !== WEBUI_HEADER_SIGNATURE) 
                    return; 
                var len = buffer8.length - 3; 
                if(buffer8[buffer8.length - 1] === 0) 
                   len--; // Null byte (0x00) can break eval() 
                data8 = new Uint8Array(len); 
                for (i = 0; i < len; i++) data8[i] = buffer8[i + 3]; 
                var data8utf8 = new TextDecoder('utf-8').decode(data8); 
                // Process Command 
                if(buffer8[1] === WEBUI_HEADER_CALL_FUNC) { 
                        const call_id = buffer8[2];
                    if(_webui_log) 
                        console.log('WebUI -> Func Reponse [' + data8utf8 + ']'); 
                    if (_webui_fn_promise_resolve[call_id]) { 
                            if(_webui_log) 
                            console.log('WebUI -> Resolving reponse #' + call_id + '...'); 
                        _webui_fn_promise_resolve[call_id](data8utf8); 
                        _webui_fn_promise_resolve[call_id] = null; 
                    } 
                } else if(buffer8[1] === WEBUI_HEADER_SWITCH) { 
                        _webui_close(WEBUI_HEADER_SWITCH, data8utf8); 
                } else if(buffer8[1] === WEBUI_HEADER_CLOSE) { 
                        window.close(); 
                } else if(buffer8[1] === WEBUI_HEADER_JS_QUICK || buffer8[1] === WEBUI_HEADER_JS) { 
                        data8utf8 = data8utf8.replace(/(?:\r\n|\r|\n)/g, "\n"); 
                    if(_webui_log) 
                        console.log('WebUI -> JS [' + data8utf8 + ']'); 
                    var FunReturn = 'undefined'; 
                    var FunError = false; 
                    try { FunReturn = eval('(() => {' + data8utf8 + '})()'); } catch (e) { FunError = true; FunReturn = e.message } 
                    if(buffer8[1] === WEBUI_HEADER_JS_QUICK) return; 
                    if(typeof FunReturn === 'undefined' || FunReturn === undefined) FunReturn = 'undefined'; 
                    if(_webui_log && !FunError) console.log('WebUI -> JS -> Return [' + FunReturn + ']'); 
                    if(_webui_log && FunError) console.log('WebUI -> JS -> Error [' + FunReturn + ']'); 
                    const FunReturn8 = new TextEncoder('utf-8').encode(FunReturn); 
                    var Return8 = new Uint8Array(4 + FunReturn8.length); 
                    Return8[0] = WEBUI_HEADER_SIGNATURE; 
                    Return8[1] = WEBUI_HEADER_JS; 
                    Return8[2] = buffer8[2]; 
                    if(FunError) Return8[3] = 0; 
                    else Return8[3] = 1; 
                    var p = -1; 
                    for (i = 4; i < FunReturn8.length + 4; i++) Return8[i] = FunReturn8[++p]; 
                    if(_webui_ws_status) _webui_ws.send(Return8.buffer); 
                } 
        }; 
    } else { 
            alert('Sorry. WebSocket not supported by your Browser.'); 
        if(!_webui_log) window.close(); 
    } 
} 
function _webui_clicks_listener() { 
        Object.keys(window).forEach(key=>{ 
            if(/^on(click)/.test(key)) { 
                window.addEventListener(key.slice(2),event=>{ 
                    if(_webui_has_events || ((event.target.id !== '') && (_webui_bind_list.includes(_webui_win_num + '/' + event.target.id)))) { 
                        _webui_send_click(event.target.id); 
                } 
            }); 
        } 
    }); 
} 
function _webui_send_click(elem) { 
        if(_webui_ws_status) { 
            var packet; 
        if(elem !== '') { 
                const elem8 = new TextEncoder('utf-8').encode(elem); 
            packet = new Uint8Array(3 + elem8.length); 
            packet[0] = WEBUI_HEADER_SIGNATURE; 
            packet[1] = WEBUI_HEADER_CLICK; 
            packet[2] = 0; 
            var p = -1; 
            for (i = 3; i < elem8.length + 3; i++) 
                packet[i] = elem8[++p]; 
        } else { 
                packet = new Uint8Array(4); 
            packet[0] = WEBUI_HEADER_SIGNATURE; 
            packet[1] = WEBUI_HEADER_CLICK; 
            packet[2] = 0; 
            packet[3] = 0; 
        } 
        _webui_ws.send(packet.buffer); 
        if(_webui_log) 
            console.log('WebUI -> Click [' + elem + ']'); 
    } 
} 
function _webui_send_event_navigation(url) { 
        if(_webui_has_events && _webui_ws_status && url !== '') { 
            const url8 = new TextEncoder('utf-8').encode(url); 
        var packet = new Uint8Array(3 + url8.length); 
        packet[0] = WEBUI_HEADER_SIGNATURE; 
        packet[1] = WEBUI_HEADER_SWITCH; 
        packet[2] = 0; 
        var p = -1; 
        for (i = 3; i < url8.length + 3; i++) 
            packet[i] = url8[++p]; 
        _webui_ws.send(packet.buffer); 
        if(_webui_log) 
            console.log('WebUI -> Navigation [' + url + ']'); 
    } 
} 
function _webui_is_external_link(url) { 
        const currentUrl = new URL(window.location.href); 
    const targetUrl = new URL(url, window.location.href); 
    currentUrl.hash = ''; 
    targetUrl.hash = ''; 
    if (url.startsWith('#') || url === currentUrl.href + '#' || currentUrl.href === targetUrl.href) { 
            return false; 
    } 
        return true; 
} 
function _webui_close_window_timer() { 
        setTimeout(function(){window.close();},1000); 
} 
async function _webui_fn_promise(fn, value) { 
        if(_webui_log) 
        console.log('WebUI -> Func [' + fn + '](' + value + ')'); 
    const fn8 = new TextEncoder('utf-8').encode(fn); 
    const value8 = new TextEncoder('utf-8').encode(value); 
    var packet = new Uint8Array(3 + fn8.length + 1 + value8.length); 
    const call_id = _webui_fn_id[0]++; 
    packet[0] = WEBUI_HEADER_SIGNATURE; 
    packet[1] = WEBUI_HEADER_CALL_FUNC; 
    packet[2] = call_id; 
    var p = 3; 
    for (var i = 0; i < fn8.length; i++) 
        { packet[p] = fn8[i]; p++; } 
    packet[p] = 0; 
    p++; 
    if(value8.length > 0) { 
            for (var i = 0; i < value8.length; i++) 
            { packet[p] = value8[i]; p++; } 
    } else { packet[p] = 0; } 
    return new Promise((resolve) => { 
            _webui_fn_promise_resolve[call_id] = resolve; 
        _webui_ws.send(packet.buffer); 
    }); 
} 
 // -- APIs -------------------------- 
function webui_fn(fn, value) { 
        if(!fn || !_webui_ws_status) 
        return Promise.resolve(); 
    if(typeof value == 'undefined') 
        var value = ''; 
    if(!_webui_has_events && !_webui_bind_list.includes(_webui_win_num + '/' + fn)) 
        return Promise.resolve(); 
    return _webui_fn_promise(fn, value); 
} 
function webui_log(status) { 
        if(status) { 
            console.log('WebUI -> Log Enabled.'); 
        _webui_log = true; 
    } else { 
            console.log('WebUI -> Log Disabled.'); 
        _webui_log = false; 
    } 
} 
function webui_encode(str) { 
         return btoa(str); 
} 
function webui_decode(str) { 
         return atob(str); 
} 
 // -- DOM --------------------------- 
document.addEventListener('keydown', function (e) { 
        // Disable F5 
    if(_webui_log) return; 
    if(e.keyCode === 116) { 
            e.preventDefault(); 
        e.returnValue = false; 
        e.keyCode = 0; 
        return false; 
    } 
}); 
window.onbeforeunload = function () { 
       _webui_ws.close(); 
}; 
setTimeout(function () { 
        if(!_webui_ws_status_once) { 
            _webui_freeze_ui(); 
        alert('WebUI failed to connect to the background application. Please try again.'); 
        if(!_webui_log) window.close(); 
    } 
}, 1500); 
window.addEventListener('unload', unload_handler, false); 
function unload_handler() { 
        // Unload for 'back' & 'forward' navigation 
    window.removeEventListener('unload', unload_handler, false); 
} 
// Links 
document.addEventListener('click', e => { 
        const attribute = e.target.closest('a'); 
    if(attribute) { 
            const link = attribute.href; 
        if(_webui_is_external_link(link)) { 
                e.preventDefault(); 
            _webui_close(WEBUI_HEADER_SWITCH, link); 
        } 
    } 
});

if(typeof navigation !== 'undefined') { 
        navigation.addEventListener('navigate', (event) => { 
            const url = new URL(event.destination.url); 
            _webui_send_event_navigation(url); 
    }); 
}

const inputs = document.getElementsByTagName('input'); 

for(var i = 0; i < inputs.length; i++) {
    inputs[i].addEventListener('contextmenu', function(event){ event.stopPropagation(); });
} 
// Load 
window.addEventListener('load', () => {
    _webui_start()
    document.body.addEventListener('contextmenu', function(event){ event.preventDefault(); }); 
});