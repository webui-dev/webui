
# webui_lib Library 2.2.0
#
# http://webui.me
# https://github.com/alifcommunity/webui
#
# Copyright (c) 2020-2023 Hassan Draga.
# Licensed under GNU General Public License v2.0.
# All rights reserved.
# Canada.


import os
import platform
import sys
import ctypes
from ctypes import *
import shutil


webui_lib = None
webui_path = os.path.dirname(__file__)
PTR_CHAR = ctypes.POINTER(ctypes.c_char)
PTR_PTR_CHAR = ctypes.POINTER(PTR_CHAR)

# Scripts Runtime
class browser:
    any:int = 0 # Default recommended web browser
    chrome:int = 1 # Google Chrome
    firefox:int = 2 # Mozilla Firefox
    edge:int = 3 # Microsoft Edge
    safari:int = 4 # Apple Safari
    chromium:int = 5 # The Chromium Project
    opera:int = 6 # Opera Browser
    brave:int = 7 # The Brave Browser
    vivaldi:int = 8 # The Vivaldi Browser
    epic:int = 9 # The Epic Browser
    yandex:int = 10 # The Yandex Browser

# event
class event:
    window = None
    type = 0
    element = ""
    data = ""


# JavaScript
class javascript:
    error = False
    response = ""


# Scripts Runtime
class runtime:
    none = 0
    deno = 1
    nodejs = 2


# The window class
class window:

    window = None
    window_id = ""
    c_events = None
    cb_fun_list = {}

    def __init__(self):
        global webui_lib
        try:
            # Load webui_lib Shared Library
            _load_library()
            # Check library if correctly loaded
            if webui_lib is None:
                print(
                    'Please download the latest webui_lib lib from https://webui.me')
                sys.exit(1)
            # Create new webui_lib window
            webui_wrapper = None
            webui_wrapper = webui_lib.webui_new_window
            webui_wrapper.restype = c_void_p
            self.window = c_void_p(webui_wrapper())
            # Get the window unique ID
            window_unique_id = int(
                webui_lib.webui_interface_get_window_id(
                self.window))
            self.window_id = str(window_unique_id)
            # Initializing events() to be used by
            # webui_lib library as a callback
            py_fun = ctypes.CFUNCTYPE(
                ctypes.c_void_p, # RESERVED
                ctypes.c_void_p, # window
                ctypes.c_uint, # type
                ctypes.c_char_p, # element
                ctypes.c_char_p, # data
                PTR_CHAR) # response
            self.c_events = py_fun(self._events)
        except OSError as e:
            print(
                "webui_lib Exception: %s" % e)
            sys.exit(1)


    # def __del__(self):
    #     global webui_lib
    #     if self.window is not None and webui_lib is not None:
    #         webui_lib.webui_close(self.window)


    def _events(self, window: ctypes.c_void_p,
               event_type: ctypes.c_uint,
               _element: ctypes.c_char_p,
               data: ctypes.c_char_p,
               response_ptr: PTR_CHAR):
        element = _element.decode('utf-8')
        func_id = self.window_id + element
        if self.cb_fun_list[func_id] is None:
            print('webui_lib error: Callback is None.')
            return
        # Create event
        e = event()
        e.window = self
        e.type = int(event_type)
        e.element = element
        e.data = data.decode('utf-8')
        # User callback
        cb_result = self.cb_fun_list[func_id](e)
        if cb_result is not None:
            cb_result_str = str(cb_result)
            cb_result_encode = cb_result_str.encode('utf-8')
            # Set the response
            webui_lib.webui_interface_set_response(response_ptr, cb_result_encode)


    # Bind a specific html element click event with a function. Empty element means all events.
    def bind(self, element, func):
        global webui_lib
        if self.window is None:
            _err_window_is_none('bind')
            return
        if webui_lib is None:
            _err_library_not_found('bind')
            return
        # Bind
        webui_lib.webui_interface_bind(
            self.window,
            element.encode('utf-8'),
            self.c_events)
        # Add CB to the list
        func_id = self.window_id + element
        self.cb_fun_list[func_id] = func


    # Show a window using a embedded HTML, or a file. If the window is already opened then it will be refreshed.
    def show(self, content="<html></html>", browser:int=0):
        global webui_lib
        if self.window is None:
            _err_window_is_none('show')
            return
        if webui_lib is None:
            _err_library_not_found('show')
            return
        # Show the window
        webui_lib.webui_show_browser(self.window, content.encode('utf-8'), ctypes.c_uint(browser))


    # Chose between Deno and Nodejs runtime for .js and .ts files.
    def set_runtime(self, rt=runtime.deno):
        global webui_lib
        if self.window is None:
            _err_window_is_none('set_runtime')
            return
        if webui_lib is None:
            _err_library_not_found('set_runtime')
            return
        webui_lib.webui_set_runtime(self.window, 
                        ctypes.c_uint(rt))


    def set_multi_access(self, status=False):
        global webui_lib
        if self.window is None:
            _err_window_is_none('set_multi_access')
            return
        if webui_lib is None:
            _err_library_not_found('set_multi_access')
            return
        webui_lib.webui_set_multi_access(self.window, 
                        ctypes.c_bool(status))


    # Close the window.
    def close(self):
        global webui_lib
        if webui_lib is None:
            _err_library_not_found('close')
            return
        webui_lib.webui_close(self.window)


    def is_shown(self):
        global webui_lib
        if webui_lib is None:
            _err_library_not_found('is_shown')
            return
        r = bool(webui_lib.webui_is_shown(self.window))
        return r

    # Run a JavaScript, and get the response back (Make sure your local buffer can hold the response).
    def script(self, script, timeout=0, response_size=(1024 * 8)) -> javascript:
        global webui_lib
        if self.window is None:
            _err_window_is_none('script')
            return
        if webui_lib is None:
            _err_library_not_found('script')
            return
        # Create Buffer
        buffer = ctypes.create_string_buffer(response_size)
        buffer.value = b""
        # Create a pointer to the buffer
        buffer_ptr = ctypes.pointer(buffer)
        # Run JavaScript
        status = bool(webui_lib.webui_script(self.window, 
            ctypes.c_char_p(script.encode('utf-8')), 
            ctypes.c_uint(timeout), buffer_ptr,
            ctypes.c_uint(response_size)))
        # Initializing Result
        res = javascript()
        res.data = buffer.value.decode('utf-8')
        res.error = not status
        return res

    # Quickly run a JavaScript (no response waiting).
    def run(self, script):
        global webui_lib
        if self.window is None:
            _err_window_is_none('run')
            return
        if webui_lib is None:
            _err_library_not_found('run')
            return
        # Run JavaScript
        webui_lib.webui_run(self.window, 
            ctypes.c_char_p(script.encode('utf-8')))


def _get_library_path() -> str:
    global webui_path
    if platform.system() == 'Darwin':
        file = '/webui-2-x64.dylib'
        path = os.getcwd() + file
        if os.path.exists(path):
            return path
        path = webui_path + file
        if os.path.exists(path):
            return path
        return path
    elif platform.system() == 'Windows':
        file = '\\webui-2-x64.dll'
        path = os.getcwd() + file
        if os.path.exists(path):
            return path
        path = webui_path + file
        if os.path.exists(path):
            return path
        return path
    elif platform.system() == 'Linux':
        file = '/webui-2-x64.so'
        path = os.getcwd() + file
        if os.path.exists(path):
            return path
        path = webui_path + file
        if os.path.exists(path):
            return path
        return path
    else:
        return ""


# Load webui_lib Dynamic Library
def _load_library():
    global webui_lib
    global webui_path
    if webui_lib is not None:
        return
    if platform.system() == 'Darwin':
        webui_lib = ctypes.CDLL(_get_library_path())
        if webui_lib is None:
            print(
                "webui_lib error: Failed to load webui_lib lib.")
    elif platform.system() == 'Windows':
        if sys.version_info.major==3 and sys.version_info.minor<=8:
            os.chdir(os.getcwd())
            os.add_dll_directory(os.getcwd())
            webui_lib = ctypes.CDLL(_get_library_path())
        else:
            os.chdir(os.getcwd())
            os.add_dll_directory(os.getcwd())
            webui_lib = cdll.LoadLibrary(_get_library_path())
        if webui_lib is None:
            print("webui_lib error: Failed to load webui_lib lib.")
    elif platform.system() == 'Linux':
        webui_lib = ctypes.CDLL(_get_library_path())
        if webui_lib is None:
            print("webui_lib error: Failed to load webui_lib lib.")
    else:
        print("webui_lib error: Unsupported OS")


# Close all opened windows. webui_wait() will break.
def exit():
    global webui_lib
    if webui_lib is not None:
        webui_lib.webui_exit()


# Set startup timeout
def set_timeout(second):
    global webui_lib
    if webui_lib is None:
        _load_library()
        if webui_lib is None:
            _err_library_not_found('set_timeout')
            return
    webui_lib.webui_set_timeout(ctypes.c_uint(second))


def is_app_running():
    global webui_lib
    if webui_lib is None:
        _load_library()
        if webui_lib is None:
            _err_library_not_found('is_app_running')
            return
    r = bool(webui_lib.webui_interface_is_app_running())
    return r


# Wait until all opened windows get closed.
def wait():
    global webui_lib
    if webui_lib is None:
        _load_library()
        if webui_lib is None:
            _err_library_not_found('wait')
            return
    webui_lib.webui_wait()
    try:
        shutil.rmtree(os.getcwd() + '/__intcache__/')
    except OSError:
        pass


# 
def _err_library_not_found(f):
    print('webui_lib ' + f + '(): Library Not Found.')


#
def _err_window_is_none(f):
    print('webui_lib ' + f + '(): window is None.')


# Set the path to the webui_lib prebuilt dynamic lib
def set_library_path(Path):
    global webui_path
    webui_path = Path
