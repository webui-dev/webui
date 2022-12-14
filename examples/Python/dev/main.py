
# WebUI Library 2.x
# Python Example
#
# http://webui.me
# https://github.com/alifcommunity/webui
#
# Licensed under GNU General Public License v3.0.
# Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.

# This script is for debugging & development of the WebUI Python wrapper
# The source code is located at 'webui/packages/PyPI/src/webui/webui.py'

# [!] Make sure to remove the WebUI package
# pip uninstall webui2

# Import the WebUI local module
import sys
sys.path.append('../../../packages/PyPI/src/webui')
import webui

# Use the local WebUI Dynamic lib
# For instructions on compiling it please visit:
# https://github.com/alifcommunity/webui/tree/main/build
webui.set_library_path('../../../build/Windows/MSVC')

# HTML
html = """
<!DOCTYPE html>
<html>
	<head>
		<title>WebUI 2 - Python Debug & Development</title>
		<style>
			body {
				color: white;
				background: #0F2027;
				background: -webkit-linear-gradient(to right, #4e99bb, #2c91b5, #07587a);
				background: linear-gradient(to right, #4e99bb, #2c91b5, #07587a);
				text-align: center;
				font-size: 18px;
				font-family: sans-serif;
			}
		</style>
	</head>
	<body>
		<h2>Python Debug & Development</h2>
		<br>
		<input type="text" id="MyInput" OnKeyUp="document.getElementById('err').innerHTML='&nbsp;';" autocomplete="off" value=\"2\">
		<br>
		<h3 id="err" style="color: #dbdd52">&nbsp;</h3>
		<br>
		<button id="TestID">Test Python-To-JS</button>
		<button OnClick="MyJS();">Test JS-To-Python</button>
		<button id="ExitID">Exit</button>
		<script>
			function MyJS() {
				const number = document.getElementById('MyInput').value;
				var result = webui_fn('Test2', number);
				document.getElementById('MyInput').value = result;
			}
		</script>
    </body></html>
"""

def test(e : webui.event):
	print('Element_id: ' + str(e.element_id))
	print('Window_id: ' + str(e.window_id))
	print('Element_name: ' + e.element_name)

	# Run JavaScript to get the password
	res = e.window.run_js("return document.getElementById('MyInput').value;")

	# Check for any error
	if res.error is True:
		print("JavaScript Error -> Output: [" + res.data + "]")
	else:
		print("JavaScript OK -> Output: [" + res.data + "]")

def test2(e : webui.event):
	print('Element_id: ' + str(e.element_id))
	print('Window_id: ' + str(e.window_id))
	print('Element_name: ' + e.element_name)
	print('Data: ' + e.data)
	v = int(e.data)
	v = v * 2
	return v

def close(e : webui.event):
	print('Element_id: ' + str(e.element_id))
	print('Window_id: ' + str(e.window_id))
	print('Element_name: ' + e.element_name)
	webui.exit()

def main():

	# Create a window object
	MyWindow = webui.window()

	# Bind am HTML element ID with a python function
	MyWindow.bind_all(test)
	MyWindow.bind('Test2', test2)
	MyWindow.bind('ExitID', close)

	# Show the window
	MyWindow.show(html, webui.browser.any)

	# Wait until all windows are closed
	webui.wait()

	print('Done.')

if __name__ == "__main__":
	main()
