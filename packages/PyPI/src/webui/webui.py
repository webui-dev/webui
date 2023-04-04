
# WebUI Library 2.1.1
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


WebUI = None
WebUI_Path = os.path.dirname(__file__)
PTR_CHAR = ctypes.POINTER(ctypes.c_char)
PTR_PTR_CHAR = ctypes.POINTER(PTR_CHAR)


# Event
class event:
    element_id = 0
    window_id = 0
    element_name = ""
    data = ""
    window = None
    c_window = None


# WebUI C-Struct
class webui_script_interface_t(ctypes.Structure):
    _fields_ = [
        ("script", c_char_p),
        ("timeout", c_uint),
        ("error", c_bool),
        ("length", c_uint),
        ("data", c_char_p)
    ]


# JavaScript
class javascript:
    script = ""
    timeout = 10
    error = False
    length = 0
    data = ""


# Scripts Runtime
class runtime:
    none = 0
    deno = 1
    nodejs = 2


# The window class
class window:

    window = None
    c_events = None
    cb_fun_list = [64]

    def __init__(self):
        global WebUI
        try:
            # Load WebUI Shared Library
            load_library()
            # Check library if correctly loaded
            if WebUI is None:
                print(
                    'Please download the latest WebUI lib from https://webui.me')
                sys.exit(1)
            # Create new WebUI window
            webui_wrapper = None
            webui_wrapper = WebUI.webui_new_window
            webui_wrapper.restype = c_void_p
            self.window = c_void_p(webui_wrapper())
            # Initializing events() to be used by
            # WebUI library as a callback
            py_fun = ctypes.CFUNCTYPE(
                ctypes.c_void_p,
                ctypes.c_uint,
                ctypes.c_uint,
                ctypes.c_char_p,
                ctypes.c_void_p,
                ctypes.c_char_p,
                PTR_PTR_CHAR)
            self.c_events = py_fun(self.events)
        except OSError as e:
            print(
                "WebUI Exception: %s" % e)
            sys.exit(1)


    # def __del__(self):
    #     global WebUI
    #     if self.window is not None and WebUI is not None:
    #         WebUI.webui_close(self.window)


    def events(self, element_id, window_id, 
                element_name, window, data, response):
        if self.cb_fun_list[int(element_id)] is None:
            print('WebUI Error: Callback is None.')
            return
        # Create event
        e = event()
        e.element_id = int(element_id)
        e.window_id = int(window_id)
        e.data = data.decode('utf-8')
        e.element_name = element_name.decode('utf-8')
        e.window = self
        e.c_window = window
        # User callback
        cb_res = str(self.cb_fun_list[element_id](e))
        cb_res_encode = cb_res.encode('utf-8')
        # Allocate a new memory
        buffer = ctypes.create_string_buffer(cb_res_encode)
        # Set the response
        response[0] = buffer


    def bind(self, element, func):
        global WebUI
        if self.window is None:
            err_window_is_none('bind')
            return
        if WebUI is None:
            err_library_not_found('bind')
            return
        cb_index = int(
            WebUI.webui_bind_interface(
                self.window,
                element.encode('utf-8'),
                self.c_events))
        self.cb_fun_list.insert(cb_index, func)


    def show(self, html="<html></html>"):
        global WebUI
        if self.window is None:
            err_window_is_none('show')
            return
        if WebUI is None:
            err_library_not_found('show')
            return
        WebUI.webui_show(self.window, html.encode('utf-8'))


    def open(self, url, browser=0):
        global WebUI
        if self.window is None:
            err_window_is_none('open')
            return
        if WebUI is None:
            err_library_not_found('open')
            return
        WebUI.webui_open(self.window, 
                        url.encode('utf-8'), ctypes.c_uint(browser))


    def script_runtime(self, rt=runtime.deno):
        global WebUI
        if self.window is None:
            err_window_is_none('script_runtime')
            return
        if WebUI is None:
            err_library_not_found('script_runtime')
            return
        WebUI.webui_script_runtime(self.window, 
                        ctypes.c_uint(rt))


    def new_server(self, path=""):
        global WebUI
        if self.window is None:
            err_window_is_none('new_server')
            return
        if WebUI is None:
            err_library_not_found('new_server')
            return
        webui_wrapper = None
        webui_wrapper = WebUI.webui_new_server
        webui_wrapper.argtypes = [c_void_p, c_void_p]
        webui_wrapper.restype = c_char_p
        url = c_char_p(webui_wrapper(self.window,
            path.encode('utf-8'))).value.decode('utf-8')
        return url


    def multi_access(self, status=False):
        global WebUI
        if self.window is None:
            err_window_is_none('multi_access')
            return
        if WebUI is None:
            err_library_not_found('multi_access')
            return
        WebUI.webui_multi_access(self.window, 
                        ctypes.c_bool(status))


    def close(self):
        global WebUI
        if WebUI is None:
            err_library_not_found('close')
            return
        WebUI.webui_close(self.window)


    def is_any_window_running(self):
        global WebUI
        if WebUI is None:
            err_library_not_found('close')
            return
        r = bool(WebUI.webui_is_any_window_running())
        return r

    def is_shown(self):
        global WebUI
        if WebUI is None:
            err_library_not_found('close')
            return
        r = bool(WebUI.webui_is_shown(self.window))
        return r

    def run_js(self, script, timeout=0) -> javascript:
        global WebUI
        if self.window is None:
            err_window_is_none('show')
            return
        if WebUI is None:
            err_library_not_found('show')
            return
        # Create Struct
        js = webui_script_interface_t()
        # Initializing
        js.script = ctypes.c_char_p(script.encode('utf-8'))
        js.timeout = ctypes.c_uint(timeout)
        js.error = ctypes.c_bool(False)
        js.length = ctypes.c_uint(0)
        js.data = ctypes.c_char_p("".encode('utf-8'))
        # Initializing Result
        res = javascript()
        res.script = script
        res.timeout = timeout
        res.error = True
        res.length = 7
        res.data = "UNKNOWN"
        # Run JavaScript
        WebUI.webui_script_interface_struct(self.window, 
                                            ctypes.byref(js))
        res.length = int(js.length)
        res.data = js.data.decode('utf-8')
        res.error = js.error
        return res


def get_library_path() -> str:
    global WebUI_Path
    if platform.system() == 'Darwin':
        file = '/webui-2-x64.dylib'
        path = os.getcwd() + file
        if os.path.exists(path):
            return path
        path = WebUI_Path + file
        if os.path.exists(path):
            return path
        return path
    elif platform.system() == 'Windows':
        file = '\\webui-2-x64.dll'
        path = os.getcwd() + file
        if os.path.exists(path):
            return path
        path = WebUI_Path + file
        if os.path.exists(path):
            return path
        return path
    elif platform.system() == 'Linux':
        file = '/webui-2-x64.so'
        path = os.getcwd() + file
        if os.path.exists(path):
            return path
        path = WebUI_Path + file
        if os.path.exists(path):
            return path
        return path
    else:
        return ""


# Load WebUI Dynamic Library
def load_library():
    global WebUI
    global WebUI_Path
    if WebUI is not None:
        return
    if platform.system() == 'Darwin':
        WebUI = ctypes.CDLL(get_library_path())
        if WebUI is None:
            print(
                "WebUI Error: Failed to load WebUI lib.")
    elif platform.system() == 'Windows':
        if sys.version_info.major==3 and sys.version_info.minor<=8:
            os.chdir(os.getcwd())
            os.add_dll_directory(os.getcwd())
            WebUI = ctypes.CDLL(get_library_path())
        else:
            os.chdir(os.getcwd())
            os.add_dll_directory(os.getcwd())
            WebUI = cdll.LoadLibrary(get_library_path())
        if WebUI is None:
            print("WebUI Error: Failed to load WebUI lib.")
    elif platform.system() == 'Linux':
        WebUI = ctypes.CDLL(get_library_path())
        if WebUI is None:
            print("WebUI Error: Failed to load WebUI lib.")
    else:
        print("WebUI Error: Unsupported OS")


# Exit app
def exit():
    global WebUI
    if WebUI is None:
        load_library()
        if WebUI is None:
            err_library_not_found('exit')
            return
    WebUI.webui_exit()


# Set startup timeout
def set_timeout(second):
    global WebUI
    if WebUI is None:
        load_library()
        if WebUI is None:
            err_library_not_found('set_timeout')
            return
    WebUI.webui_set_timeout(ctypes.c_uint(second))


def is_app_running():
    global WebUI
    if WebUI is None:
        load_library()
        if WebUI is None:
            err_library_not_found('is_app_running')
            return
    r = bool(WebUI.webui_is_app_running())
    return r


# Wait until all windows get closed
def wait():
    global WebUI
    if WebUI is None:
        load_library()
        if WebUI is None:
            err_library_not_found('wait')
            return
    WebUI.webui_wait()
    try:
        shutil.rmtree(os.getcwd() + '/__intcache__/')
    except OSError:
        pass


# 
def err_library_not_found(f):
    print('WebUI ' + f + '(): Library Not Found.')


#
def err_window_is_none(f):
    print('WebUI ' + f + '(): Window is None.')


# Set the path to the WebUI prebuilt dynamic lib
def set_library_path(Path):
    global WebUI_Path
    WebUI_Path = Path
