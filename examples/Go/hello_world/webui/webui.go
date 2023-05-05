/*
  WebUI Library 2.3.0
  http://_webui_core.me
  https://github.com/alifcommunity/webui
  Copyright (c) 2020-2023 Hassan Draga.
  Licensed under MIT License.
  All rights reserved.
  Canada.
*/

package webui

/*
#cgo CFLAGS: -I ./
#cgo windows LDFLAGS: -L ./ -lwebui-2-static-x64 -lws2_32
#cgo darwin LDFLAGS: -L ./ -lwebui-2-static-x64 -lpthread -lm
#cgo linux LDFLAGS: -L ./ -lwebui-2-static-x64 -lpthread -lm
#include <webui.h>
extern void GoWebuiEvent(size_t _window, unsigned int _event_type, char* _element, char* _data, unsigned int _event_number);
static void GoWebuiEvents_handler(webui_event_t* e) {
    GoWebuiEvent(e->window, e->event_type, e->element, e->data, e->event_number);
}
static void go_webui_bind(size_t win, const char* element) {
    webui_bind(win, element, GoWebuiEvents_handler);
}
*/
import "C"

import (
	"bytes"
	"strconv"
	"unsafe"
)

// Heap
var isIni bool = false

// User Go Callback Functions list
var fun_list map[string]func(Event) string

// Web browsers enum
const AnyBrowser uint = 0 // 0. Default recommended web browser
const Chrome uint = 1     // 1. Google Chrome
const Firefox uint = 2    // 2. Mozilla Firefox
const Edge uint = 3       // 3. Microsoft Edge
const Safari uint = 4     // 4. Apple Safari
const Chromium uint = 5   // 5. The Chromium Project
const Opera uint = 6      // 6. Opera Browser
const Brave uint = 7      // 7. The Brave Browser
const Vivaldi uint = 8    // 8. The Vivaldi Browser
const Epic uint = 9       // 9. The Epic Browser
const Yandex uint = 10    // 10. The Yandex Browser

// Runtimes enum
const None uint = 0
const Deno uint = 1
const Nodejs uint = 2

// Events enum
const WEBUI_EVENT_DISCONNECTED uint = 0        // 0. Window disconnection event
const WEBUI_EVENT_CONNECTED uint = 1           // 1. Window connection event
const WEBUI_EVENT_MULTI_CONNECTION uint = 2    // 2. New window connection event
const WEBUI_EVENT_UNWANTED_CONNECTION uint = 3 // 3. New unwanted window connection event
const WEBUI_EVENT_MOUSE_CLICK uint = 4         // 4. Mouse click event
const WEBUI_EVENT_NAVIGATION uint = 5          // 5. Window navigation event
const WEBUI_EVENT_CALLBACK uint = 6            // 6. Function call event

// Events struct
type Event struct {
	Window    uint
	EventType uint
	Element   string
	Data      string
}

// JavaScript struct
type JavaScript struct {
	Timeout    uint
	BufferSize uint
	Response   string
}

// Initilializing
func Ini() {
	if isIni {
		return
	}
	isIni = true
	fun_list = make(map[string]func(Event) string)
}

// This function receives all events
//
//export GoWebuiEvent
func GoWebuiEvent(window C.size_t, _event_type C.uint, _element *C.char, _data *C.char, _event_number C.uint) {
	Ini()

	// Create a new event struct
	var event_type uint = uint(_event_type)
	var element string = C.GoString(_element)
	var data string = C.GoString(_data)
	e := Event{
		Window:    uint(window),
		EventType: event_type,
		Element:   element,
		Data:      data,
	}

	// Call user callback function
	var window_id uint = uint(C.webui_interface_get_window_id(window))
	var func_id string = strconv.Itoa(int(window_id)) + element
	response := string(fun_list[func_id](e))

	// Set the response back
	if len(response) > 0 {
		c_response := C.CString(response)
		C.webui_interface_set_response(window, _event_number, c_response)
	}
}

// -- Public APIs --

// JavaScript object constructor
func NewJavaScript() JavaScript {
	Ini()

	js := JavaScript{
		Timeout:    0,
		BufferSize: (1024 * 8),
		Response:   "",
	}
	return js
}

// Run a JavaScript, and get the response back (Make sure your local buffer can hold the response).
func Script(window uint, js *JavaScript, script string) bool {
	Ini()

	// Convert the JavaScript from Go-String to C-String
	c_script := C.CString(script)

	// Create a local buffer to hold the response
	ResponseBuffer := make([]byte, uint64(js.BufferSize))

	// Create a pointer to the local buffer
	ptr := (*C.char)(unsafe.Pointer(&ResponseBuffer[0]))

	// Run the JavaScript and wait for response
	status := C.webui_script(C.size_t(window), c_script, C.uint(js.Timeout), ptr, C.size_t(uint64(js.BufferSize)))

	// Copy the response to the users struct
	ResponseLen := bytes.IndexByte(ResponseBuffer[:], 0)
	js.Response = string(ResponseBuffer[:ResponseLen])

	// return the status of the JavaScript execution
	// True: No JavaScript error.
	// False: JavaScript error.
	return bool(status)
}

// Run JavaScript quickly with no waiting for the response.
func Run(window uint, script string) {
	Ini()

	// Convert the JavaScript from Go-String to C-String
	c_script := C.CString(script)

	// Run the JavaScript
	C.webui_run(C.size_t(window), c_script)
}

// Chose between Deno and Nodejs runtime for .js and .ts files.
func SetRuntime(window uint, runtime uint) {
	Ini()

	C.webui_set_runtime(C.size_t(window), C.uint(runtime))
}

// Create a new window object
func NewWindow() uint {
	Ini()

	// Create a new window object
	// this return a (size_t) and we should
	// never change it. It's only managed by WebUI
	return uint(C.size_t(C.webui_new_window()))
}

// Check a specific window if it's still running
func IsShown(window uint) bool {
	Ini()

	status := C.webui_is_shown(C.size_t(window))
	return bool(status)
}

// Close a specific window.
func Close(window uint) {
	Ini()

	C.webui_close(C.size_t(window))
}

// Set the maximum time in seconds to wait for browser to start
func SetTimeout(seconds uint) {
	Ini()

	C.webui_set_timeout(C.uint(seconds))
}

// Allow the window URL to be re-used in normal web browsers
func SetMultiAccess(window uint, access bool) {
	Ini()

	C.webui_set_multi_access(C.size_t(window), C._Bool(access))
}

// Close all opened windows
func Exit() {
	Ini()

	C.webui_exit()
}

// Show a window using a embedded HTML, or a file. If the window is already opened then it will be refreshed.
func Show(window uint, content string) {
	Ini()

	c_content := C.CString(content)
	C.webui_show(C.size_t(window), c_content)
}

// Same as Show(). But with a specific web browser.
func ShowBrowser(window uint, content string, browser uint) {
	Ini()

	c_content := C.CString(content)
	C.webui_show_browser(C.size_t(window), c_content, C.uint(browser))
}

// Wait until all opened windows get closed.
func Wait() {
	Ini()

	C.webui_wait()
}

// Bind a specific html element click event with a function. Empty element means all events.
func Bind(window uint, element string, callback func(Event) string) {
	Ini()

	// Convert element from Go-String to C-String
	c_element := C.CString(element)
	C.go_webui_bind(C.size_t(window), c_element)

	// Get the window ID
	var window_id uint = uint(C.webui_interface_get_window_id(C.size_t(window)))

	// Generate a unique ID for this element
	var func_id string = strconv.Itoa(int(window_id)) + element

	// Add the user callback function to the list
	fun_list[func_id] = callback
}
