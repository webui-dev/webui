
# -------------------------------------------------------------------------------
# cWebUI
# - - - -
# http://webui.me
# https://github.com/alifcommunity/webui
# Licensed under GNU General Public License v3.0.
# Copyright (C)2020 Hassan DRAGA <https://github.com/hassandraga>.
# -------------------------------------------------------------------------------

# ---[ WebUI Class ]--------------------------------------------------------------
import os
import platform
import sys
import ctypes
from ctypes import cdll, c_void_p, CFUNCTYPE, POINTER

class WebUI:
    webui_lib = None
    window = None
    cb_fun = None
    cb_fun_list = []
    def __init__(self): 
        webui_wrapper = None
        try:
            if platform.system() == 'Darwin':
                self.webui_lib = ctypes.CDLL('cwebui.dylib')
            elif platform.system() == 'Windows':
                if sys.version_info.major == 3 and sys.version_info.minor == 8:
                    os.chdir(os.getcwd())
                    os.add_dll_directory(os.getcwd())
                    self.webui_lib = ctypes.CDLL('cwebui.dll')
                    #self.webui_lib = cdll.LoadLibrary('cwebui.dll')
            elif platform.system() == 'Linux':
                os.chdir(os.getcwd())
                self.webui_lib = ctypes.CDLL(os.getcwd() + '/libcwebui.so')
            webui_wrapper = self.webui_lib.c_create_window
            webui_wrapper.restype = c_void_p
            self.window = c_void_p(webui_wrapper())
            self.webui_lib.c_ini()
        except OSError as e:
            print("WebUI Err: %s" % e)
            sys.exit(1)
    def __del__(self):
        if self.window is not None and self.webui_lib is not None:
            self.webui_lib.c_destroy_window(self.window)
    def bind(self, element, func_ref): 
        if self.window is None or self.webui_lib is None:
            return
        cb_fun_type = None
        prototype = None
        fun = None
        cb_fun_type = ctypes.CFUNCTYPE(ctypes.c_void_p) # define C pointer to a function type
        self.cb_fun = cb_fun_type(func_ref)             # define a C function equivalent to the python function
        self.cb_fun_list.append(self.cb_fun)
        prototype = ctypes.PYFUNCTYPE(
            ctypes.c_void_p,    # fun return
            ctypes.c_void_p,    # arg 1
            ctypes.c_char_p,    # arg 2
            ctypes.py_object,   # arg 3
            ctypes.c_void_p     # arg 4
        )
        fun = prototype(('c_bind_element', self.webui_lib))
        fun(self.window, element.encode('utf-8'), func_ref, self.cb_fun_list[-1])
    def show(self, html):
        if self.window is not None and self.webui_lib is not None:
            self.webui_lib.c_show_window(self.window, html.encode('utf-8'))
    def loop(self): 
        if self.webui_lib is not None:
            self.webui_lib.c_loop()
# -------------------------------------------------------------------------------

# HTML
my_html = """
<html>
    <head>
        <title>My first WebUI Python script</title>
    </head>
    <body style="background-color:#515C6B; color:#fff; font-family:"Lucida Console", Courier, monospace">
        <h1>Welcome to WebUI Python!</h1>
        <button id="MyButtonID1">Click on me!</button> | <button id="MyButtonID2">Exit!</button>
    </body>
</html>
"""

# a sample function to be called
# when a specific button has clicked
def my_function():
    print('You clicked on the first button!')

def my_function_two():
    print('You clicked on the second button!')

# Create a window object
MyWindow = WebUI()

# Bind am HTML element ID with a python function
MyWindow.bind('MyButtonID1', my_function)
MyWindow.bind('MyButtonID2', my_function_two)

# Show the window
MyWindow.show(my_html)

# Wait unitil all windows are closed
MyWindow.loop()

print('Good! All windows are closed now.')
sys.exit(0)
