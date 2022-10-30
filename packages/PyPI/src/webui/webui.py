# WebUI Library 2.0.3
#
# http://webui.me
# https://github.com/alifcommunity/webui
#
# Licensed under GNU General Public License v3.0.
# Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.

import os
import platform
import sys
import ctypes
from ctypes import *
import shutil

WebUI = None
WebUI_Path = os.path.dirname(__file__)

# Event
class event:
	element_id = 0
	window_id = 0
	element_name = ""
	window = None

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
				print('Please download the latest WebUI dynamic library from https://webui.me')
				sys.exit(1)
			# Create new WebUI window
			webui_wrapper = None
			webui_wrapper = WebUI.webui_new_window
			webui_wrapper.restype = c_void_p
			self.window = c_void_p(webui_wrapper())
			# Initializing events() to be called from WebUI Library
			py_fun = ctypes.CFUNCTYPE(ctypes.c_void_p, ctypes.c_uint, ctypes.c_uint, ctypes.c_char_p, ctypes.c_void_p)
			self.c_events = py_fun(self.events)
		except OSError as e:
			print("WebUI Exception: %s" % e)
			sys.exit(1)
	
	def __del__(self):
		global WebUI
		if self.window is not None and WebUI is not None:
			WebUI.webui_close(self.window)
	
	def events(self, element_id, window_id, element_name, window):
		if self.cb_fun_list[int(element_id)] is None:
			print('WebUI Error: Callback is None.')
			return
		e = event()
		e.element_id = element_id
		e.window_id = window_id
		e.element_name = element_name
		e.window = window
		self.cb_fun_list[element_id](e)

	def bind(self, element, func):
		global WebUI
		if self.window is None:
			err_window_is_none('bind')
			return
		if WebUI is None:
			err_library_not_found('bind')
			return
		cb_index = int(WebUI.webui_bind_interface(self.window, element.encode('utf-8'), self.c_events))
		self.cb_fun_list.insert(cb_index, func)
	
	def show(self, html):
		global WebUI
		if self.window is None:
			err_window_is_none('show')
			return
		if WebUI is None:
			err_library_not_found('show')
			return
		WebUI.webui_show(self.window, html.encode('utf-8'), 0)
	
	def close(self):
		global WebUI
		if WebUI is None:
			err_library_not_found('close')
			return
		WebUI.webui_close(self.window)
	
	def run_js(self, script, timeout = 0) -> javascript:
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
		WebUI.webui_script_interface_struct(self.window, ctypes.byref(js))
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
		file = '\webui-2-x64.dll'
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
	if platform.system() == 'Darwin':
		WebUI = ctypes.CDLL(get_library_path())
		if WebUI is None:
			print("WebUI Error: Failed to load WebUI dynamic library.")
	elif platform.system() == 'Windows':
		if sys.version_info.major == 3 and sys.version_info.minor <= 8:
			os.chdir(os.getcwd())
			os.add_dll_directory(os.getcwd())
			WebUI = ctypes.CDLL(get_library_path())
		else:
			os.chdir(os.getcwd())
			os.add_dll_directory(os.getcwd())
			WebUI = cdll.LoadLibrary(get_library_path())
		if WebUI is None:
			print("WebUI Error: Failed to load WebUI dynamic library.")
	elif platform.system() == 'Linux':
		WebUI = ctypes.CDLL(get_library_path())
		if WebUI is None:
			print("WebUI Error: Failed to load WebUI dynamic library.")
	else:
		print("WebUI Error: Unsupported OS")

# Exit app
def exit():
	global WebUI
	if WebUI is None:
		err_library_not_found('exit')
		return
	WebUI.webui_exit()

# Wait until all windows get closed
def wait():
	global WebUI
	if WebUI is None:
		err_library_not_found('wait')
		return
	WebUI.webui_wait()
	try:
		shutil.rmtree(os.getcwd() + '/__intcache__/')
	except OSError:
		pass

def err_library_not_found(f):
	print('WebUI ' + f + '(): Library Not Found.')

def err_window_is_none(f):
	print('WebUI ' + f + '(): Window is None.')

def set_library_path(Path):
	global WebUI_Path
	WebUI_Path = Path
