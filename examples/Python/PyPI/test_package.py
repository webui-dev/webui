# This script is for debugging & the development of the WebUI Python wrapper.
# The wrapper source code is located at 'webui/examples/Python/PyPI/Package/src/webui/webui.py'

# [!] Make sure to remove the WebUI package
# pip uninstall webui2

# Import the WebUI local module
import sys
sys.path.append('Package/src/webui')
import webui

# Use the local WebUI Dynamic lib
webui.set_library_path('../../../build/Windows/MSVC')

# HTML
html = """
<!DOCTYPE html>
<html>
	<head>
		<title>WebUI 2 - Python Wrapper Test</title>
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
		<h2>Python Wrapper Test</h2>
		<br>
		<input type="text" id="MyInput" OnKeyUp="document.getElementById('err').innerHTML='&nbsp;';" autocomplete="off" value=\"2\">
		<br>
		<h3 id="err" style="color: #dbdd52">&nbsp;</h3>
		<br>
		<button id="P2JS">Test Python-To-JS</button>
		<button OnClick="MyJS();">Test JS-To-Python</button>
		<button id="ExitID">Exit</button>
		<script>
			function MyJS() {
				const number = document.getElementById('MyInput').value;
				var result = webui_fn('JS2P', number);
				document.getElementById('MyInput').value = result;
			}
		</script>
    </body></html>
"""

def all_events(e : webui.event):
	print('Function: all_events()')
	print('Element: ' + e.Element)
	print('EventType: ' + e.EventType)
	print('Data: ' + e.Data)

def python_to_js(e : webui.event):
	print('Function: python_to_js()')
	print('Element: ' + e.Element)
	print('EventType: ' + e.EventType)
	print('Data: ' + e.Data)
	# Run JavaScript to get the password
	res = e.window.script("return document.getElementById('MyInput').value;")
	# Check for any error
	if res.error is True:
		print("JavaScript Error -> Output: [" + res.data + "]")
	else:
		print("JavaScript OK -> Output: [" + res.data + "]")

def js_to_python(e : webui.event):
	print('Function: js_to_python()')
	print('Element: ' + e.Element)
	print('EventType: ' + e.EventType)
	print('Data: ' + e.Data)
	v = int(e.data)
	v = v * 2
	return v

def exit(e : webui.event):
	print('Function: exit()')
	print('Element: ' + e.Element)
	print('EventType: ' + e.EventType)
	print('Data: ' + e.Data)
	webui.exit()

def main():

	# Create a window object
	MyWindow = webui.window()

	# Bind am HTML element ID with a python function
	MyWindow.bind('', all_events)
	MyWindow.bind('P2JS', python_to_js)
	MyWindow.bind('JS2P', js_to_python)
	MyWindow.bind('Exit', exit)

	# Show the window
	MyWindow.show(html)

	# Wait until all windows are closed
	webui.wait()

	print('Done.')

if __name__ == "__main__":
	main()
