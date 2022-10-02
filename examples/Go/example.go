// WebUI Library 2.0.0
//
// http://webui.me
// https://github.com/alifcommunity/webui
//
// Licensed under GNU General Public License v3.0.
// Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.

package main

//--[WebUI]-------------------------------------
import (
	"C"
	"fmt"
	"syscall"
	"unsafe"
)

var (
	dll_webui, _ = syscall.LoadLibrary("webui-2-x64.dll")

	//webui_window_t* webui_new_window();
	dll_webui_new_window, _ = syscall.GetProcAddress(dll_webui, "webui_new_window")

	// bool webui_show(webui_window_t* win, const char* html, unsigned int browser);
	dll_webui_show, _ = syscall.GetProcAddress(dll_webui, "webui_show")

	// void webui_loop();
	dll_webui_loop, _ = syscall.GetProcAddress(dll_webui, "webui_loop")
)

func webui_new_window() (result uintptr) {
	var arg uintptr = 0
	ret, _, _ := syscall.Syscall(uintptr(dll_webui_new_window), arg, 0, 0, 0)
	result = ret
	return
}
func webui_show(win uintptr, html string, browser uint) (result bool) {
	chtml := C.CString(html)
	var arg uintptr = 3
	ret, _, _ := syscall.Syscall(uintptr(dll_webui_show), arg, win, uintptr(unsafe.Pointer((chtml))), uintptr(browser))
	if ret == 0x00 {
		return false
	}
	return true
}
func webui_loop() {
	var arg uintptr = 0
	syscall.Syscall(uintptr(dll_webui_loop), arg, 0, 0, 0)
}
func ini() {

	defer syscall.FreeLibrary(dll_webui)
}
func webui_test() (res int) {

	return 0
}

//----------------------------------------------

// Application Example

const my_html string = `<!DOCTYPE html>
<html>
  <head>
    <title>WebUI 2.0 Example</title>
    <style>
      body {
        color: white;
        background: #0F2027;
        background: -webkit-linear-gradient(to right, #2C5364, #203A43, #0F2027);
        background: linear-gradient(to right, #2C5364, #203A43, #0F2027);
        text-align: center;
        font-size: 18px;
        font-family: sans-serif;
      }
    </style>
  </head>
  <body>
    <h1>WebUI 2.0 Example</h1>
    <br>
    <input type="password" id="MyInput">
    <br>
    <br>
    <button id="MyButton1">Check Password</button> - <button id="MyButton2">Exit</button>
  </body>
</html>`

func main() {

	var my_window = webui_new_window()

	webui_show(my_window, my_html, 0)

	webui_loop()

	fmt.Print("Bye.")
}
