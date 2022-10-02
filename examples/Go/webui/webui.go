package webui

import (
	"C"
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
