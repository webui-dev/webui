
# WebUI Library 2.2.0
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
    Window = None
    EventType = 0
    Element = ""
    Data = ""


# JavaScript
class javascript:
    Error = False
    Response = ""


# Scripts Runtime
class runtime:
    none = 0
    Deno = 1
    Nodejs = 2


# The window class
class window:

    window = None
    window_id = None
    c_events = None
    cb_fun_list = {}

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
            # Get the window unique ID
            self.window_id = int(
                WebUI.webui_interface_get_window_id(
                self.window))
            # Initializing events() to be used by
            # WebUI library as a callback
            py_fun = ctypes.CFUNCTYPE(
                ctypes.c_void_p, # Window
                ctypes.c_uint, # EventType
                ctypes.c_char_p, # Element
                ctypes.c_char_p, # Data
                ctypes.c_char_p) # Response
            self.c_events = py_fun(self.events)
        except OSError as e:
            print(
                "WebUI Exception: %s" % e)
            sys.exit(1)


    # def __del__(self):
    #     global WebUI
    #     if self.window is not None and WebUI is not None:
    #         WebUI.webui_close(self.window)


    def events(self, window, event_type, 
                element, data, response):
        if self.cb_fun_list[self.window_id][element] is None:
            print('WebUI Error: Callback is None.')
            return
        # Create event
        e = event()
        e.Window = self
        e.EventType = int(event_type)
        e.Element = element.decode('utf-8')
        e.Data = data.decode('utf-8')
        # User callback
        cb_res = str(self.cb_fun_list[self.window_id][element](e))
        cb_res_encode = cb_res.encode('utf-8')
        # Set the response
        WebUI.webui_interface_bind(response, cb_res_encode)


    def bind(self, element, func):
        global WebUI
        if self.window is None:
            err_window_is_none('bind')
            return
        if WebUI is None:
            err_library_not_found('bind')
            return
        # Bind
        WebUI.webui_interface_bind(
            self.window,
            element.encode('utf-8'),
            self.c_events)
        # Add CB to the list
        self.cb_fun_list[self.window_id] = {element: func}


    def show(self, content="<html></html>"):
        global WebUI
        if self.window is None:
            err_window_is_none('show')
            return
        if WebUI is None:
            err_library_not_found('show')
            return
        # Show the window
        WebUI.webui_show(self.window, content.encode('utf-8'))


    def set_runtime(self, rt=runtime.deno):
        global WebUI
        if self.window is None:
            err_window_is_none('set_runtime')
            return
        if WebUI is None:
            err_library_not_found('set_runtime')
            return
        WebUI.webui_set_runtime(self.window, 
                        ctypes.c_uint(rt))


    def set_multi_access(self, status=False):
        global WebUI
        if self.window is None:
            err_window_is_none('set_multi_access')
            return
        if WebUI is None:
            err_library_not_found('set_multi_access')
            return
        WebUI.webui_set_multi_access(self.window, 
                        ctypes.c_bool(status))


    def close(self):
        global WebUI
        if WebUI is None:
            err_library_not_found('close')
            return
        WebUI.webui_close(self.window)


    def is_shown(self):
        global WebUI
        if WebUI is None:
            err_library_not_found('is_shown')
            return
        r = bool(WebUI.webui_is_shown(self.window))
        return r

    def script(self, script, timeout=0, response_size=(1024 * 8)) -> javascript:
        global WebUI
        if self.window is None:
            err_window_is_none('show')
            return
        if WebUI is None:
            err_library_not_found('show')
            return
        # Create Buffer
        buffer = ctypes.create_string_buffer(response_size)
        buffer.value = b""
        # Create a pointer to the buffer
        buffer_ptr = ctypes.pointer(buffer)
        # Run JavaScript
        status = bool(WebUI.webui_script(self.window, 
            ctypes.c_char_p(script.encode('utf-8')), 
            ctypes.c_uint(timeout), buffer_ptr,
            ctypes.c_uint(response_size)))
        # Initializing Result
        res = javascript()
        res.data = buffer.value.decode('utf-8')
        res.error = status
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
    r = bool(WebUI.webui_interface_is_app_running())
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
