/*
  WebUI Library
  https://webui.me
  https://github.com/webui-dev/webui
  Copyright (c) 2020-2026 Hassan Draga.
  Licensed under MIT License.
  All rights reserved.
  Canada.
*/

#ifndef _WEBUI_EXTENSIONS_H
#define _WEBUI_EXTENSIONS_H

#include "webui.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Construct a JavaScript string from a format string,
 * then execute it without waiting for the response. All clients.
 *
 * @param window The window number
 * @param fmt The JavaScript format string to be run
 *
 * @warning This function DOES NOT handle escape characters, proceed with caution.
 *     For example, when passing string arguments that contain quotes or backslashes, 
 *     it may lead to unexpected behavior
 * 
 * @note This function internally uses vsnprintf
 *
 * @example webui_run_fmt(myWindow, "alert('Hello %s');", "World");
 */
void webui_run_fmt(size_t window, const char* fmt, ...) {

    int len;
    char* buf;

    // Precalculate actual string length
    va_list args;
    va_start(args, fmt);
    len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (len < 1) return;

    buf = (char*)webui_malloc(len + 1);
    if (buf == NULL) return;

    // Format and execute
    va_start(args, fmt);
    vsnprintf(buf, len + 1, fmt, args);
    va_end(args);

    webui_run(window, buf);

    webui_free((void*)buf);
}

/**
 * @brief Construct a JavaScript string from a format string,
 * then execute it and get the response back. Work only in single client mode.
 * Make sure your local buffer can hold the response.
 *
 * @param window The window number
 * @param timeout The execution timeout in seconds
 * @param buffer The local buffer to hold the response
 * @param buffer_length The local buffer size
 * @param fmt The JavaScript format string to be run
 *
 * @return Returns True if there is no execution error
 *
 * @warning This function DOES NOT handle escape characters, proceed with caution.
 *     For example, when passing string arguments that contain quotes or backslashes,
 *     it may lead to unexpected behavior
 *
 * @note This function internally uses vsnprintf
 *
 * @example bool err = webui_script_fmt(myWindow, 0, myBuffer, myBufferSize,
 *     "return %d + %d;", 4, 6);
 */
bool webui_script_fmt(
    size_t window,
    size_t timeout,
    char* buffer,
    size_t buffer_length,
    const char* fmt, ...) {

    int len;
    char* buf;
    bool status;

    // Precalculate actual string length
    va_list args;
    va_start(args, fmt);
    len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (len < 1) return false;

    buf = (char*)webui_malloc(len + 1);
    if (buf == NULL) return false;

    // Format and execute
    va_start(args, fmt);
    vsnprintf(buf, len + 1, fmt, args);
    va_end(args);

    status = webui_script(window, buf, timeout, buffer, buffer_length);

    webui_free((void*)buf);

    return status;
}

#ifdef __cplusplus
}
#endif

#endif /* _WEBUI_EXTENSIONS_H */
