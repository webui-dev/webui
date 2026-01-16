/*
  WebUI Library Extras
  https://webui.me
  https://github.com/webui-dev/webui
  Copyright (c) 2020-2025 Hassan Draga.
  Licensed under MIT License.
  All rights reserved.
  Canada.
*/

#ifndef _WEBUI_EXTRAS_H
#define _WEBUI_EXTRAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "webui.h"

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
WEBUI_EXPORT void webui_run_fmt(size_t window, const char* fmt, ...);

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
WEBUI_EXPORT bool webui_script_fmt(size_t window, size_t timeout,
    char* buffer, size_t buffer_length, 
    const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /*_WEBUI_EXTRAS_H */