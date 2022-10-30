package webui

// WebUI Library 2.0.3
//
// http://webui.me
// https://github.com/alifcommunity/webui
//
// Licensed under GNU General Public License v3.0.
// Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.

/*
// [?] Change the library path as you need
#cgo CFLAGS: -I ./ -I ../../../../include
#cgo windows LDFLAGS: -L ./ -L ../../../../build/Windows/GCC/ -L ../../../../build/Windows/MSVC/ -lwebui-2-static-x64 -lws2_32
#cgo darwin LDFLAGS: -L ./ -L ../../../../build/macOS/GCC/ -L ../../../../build/macOS/Clang/ -lwebui-2-static-x64 -lpthread
#cgo linux LDFLAGS: -L ./ -L ../../../../build/Linux/GCC/ -L ../../../../build/Linux/Clang/ -lwebui-2-static-x64 -lpthread
#include <webui.h>
extern void webui_go_handler(webui_window_t* _window, unsigned int _element_id, unsigned int _window_id, char* _element_name);
static void webui_bind_go_handler(webui_event_t* e) {
    webui_go_handler(e->window, e->element_id, e->window_id, e->element_name);
}
static unsigned int webui_bind_go(webui_window_t* win, const char* element) {
    return webui_bind(win, element, webui_bind_go_handler);
}
*/
import (
	"C"
)

const AnyBrowser uint = 0
const Chrome uint = 1
const Firefox uint = 2
const Edge uint = 3
const Safari uint = 4
const Chromium uint = 5
const Custom uint = 99

// Event Struct
type Event struct {
	Window      *C.webui_window_t
	ElementID   uint
	WindowID    uint
	ElementName string
}

// JavaScript Struct
type JavaScript struct {
	Timeout uint
	Script  string
	Error   bool
	Length  uint
	Data    string
}

// User Go Callback Functions list
var fun_list [64][256]func(Event)

//export webui_go_handler
func webui_go_handler(_window *C.webui_window_t, _element_id C.uint, _window_id C.uint, _element_name *C.char) {

	var window *C.webui_window_t = (*C.webui_window_t)(_window)
	var element_id uint = uint(_element_id)
	var window_id uint = uint(_window_id)
	var element_name string = C.GoString(_element_name)

	e := Event{
		Window:      window,
		ElementID:   element_id,
		WindowID:    window_id,
		ElementName: element_name,
	}

	fun_list[window_id][element_id](e)
}

func RunJavaScript(window *C.webui_window_t, js *JavaScript) {

	// Interface
	c_js := C.webui_script_interface_t{
		script:  C.CString(js.Script),
		timeout: 30, // uint(js.Timeout),
		error:   C.bool(false),
		// length:  uint(0),
		// data:    C.CString(nil),
	}

	C.webui_script_interface_struct(window, &c_js)

	js.Error = bool(c_js.error)
	js.Data = C.GoString(c_js.data)
}

func NewWindow() *C.webui_window_t {

	return C.webui_new_window()
}

func Exit() {

	C.webui_exit()
}

func Show(window *C.webui_window_t, html string, browser uint) {

	c_html := C.CString(html)
	C.webui_show(window, c_html, C.uint(browser))
}

func Wait() {

	C.webui_wait()
}

func Bind(window *C.webui_window_t, element string, callback func(Event)) {

	c_element := C.CString(element)
	var window_id uint = uint(C._webui_window_get_number(window))
	var cb_index uint = uint(C.webui_bind_go(window, c_element))

	fun_list[window_id][cb_index] = callback
}
