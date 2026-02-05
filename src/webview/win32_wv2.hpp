/*
  WebUI Library
  Win32 WebView2 C++ Implementation
  https://webui.me
  https://github.com/webui-dev/webui
  Copyright (c) 2020-2026 Hassan Draga.
  Licensed under MIT License.
  All rights reserved.
  Canada.
*/

#ifndef WEBUI_WIN32_WV2_HPP
#define WEBUI_WIN32_WV2_HPP

#ifdef _WIN32

#include <windows.h>
#include <wchar.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _webui_window_t _webui_window_t;
typedef void* _webui_win32_wv2_handle;

bool _webui_win32_wv2_check_loader_dll(void);
_webui_win32_wv2_handle _webui_win32_wv2_create(void);
void _webui_win32_wv2_free(_webui_win32_wv2_handle handle);
bool _webui_win32_wv2_navigate(_webui_win32_wv2_handle handle, wchar_t* url);
bool _webui_win32_wv2_set_position(_webui_win32_wv2_handle handle, int x, int y);
bool _webui_win32_wv2_set_size(_webui_win32_wv2_handle handle, int width, int height);
bool _webui_win32_wv2_maximize(_webui_win32_wv2_handle handle);
bool _webui_win32_wv2_minimize(_webui_win32_wv2_handle handle);
HWND _webui_win32_wv2_get_hwnd(_webui_win32_wv2_handle handle);
void _webui_win32_wv2_set_hwnd(_webui_win32_wv2_handle handle, HWND hwnd);
void _webui_win32_wv2_set_stop(_webui_win32_wv2_handle handle, bool stop);
bool _webui_win32_wv2_get_stop(_webui_win32_wv2_handle handle);
void _webui_win32_wv2_set_navigate_flag(_webui_win32_wv2_handle handle, bool navigate);
void _webui_win32_wv2_set_size_flag(_webui_win32_wv2_handle handle, bool size);
void _webui_win32_wv2_set_position_flag(_webui_win32_wv2_handle handle, bool position);
bool _webui_win32_wv2_get_navigate_flag(_webui_win32_wv2_handle handle);
bool _webui_win32_wv2_get_size_flag(_webui_win32_wv2_handle handle);
bool _webui_win32_wv2_get_position_flag(_webui_win32_wv2_handle handle);
void _webui_win32_wv2_set_url(_webui_win32_wv2_handle handle, wchar_t* url);
wchar_t* _webui_win32_wv2_get_url(_webui_win32_wv2_handle handle);
void _webui_win32_wv2_set_dimensions(_webui_win32_wv2_handle handle, int x, int y, int width, int height);
void _webui_win32_wv2_get_dimensions(_webui_win32_wv2_handle handle, int* x, int* y, int* width, int* height);
bool _webui_win32_wv2_create_environment(_webui_win32_wv2_handle handle, wchar_t* cacheFolder);
void* _webui_win32_wv2_get_controller(_webui_win32_wv2_handle handle);

#ifdef __cplusplus
}
#endif

#endif // _WIN32
#endif // WEBUI_WIN32_WV2_HPP
