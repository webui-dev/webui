
# WebUI Library 2.x
# Python Example
#
# http://webui.me
# https://github.com/alifcommunity/webui
#
# Licensed under GNU General Public License v3.0.
# Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.

import webui

# Set the WebUI dynamic library location (Optional)
# Default is the same folder, otherwise use this option
webui.set_library_path("../../../build/Windows/MSVC")

# Create a global window object
MyWindow = webui.window()

# HTML
login_html = """
<!DOCTYPE html>
<html>
  <head>
    <title>WebUI 2 - Python Example</title>
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
    <h1>WebUI 2 - Python Example</h1>
    <br>
    <input type="password" id="MyInput" OnKeyUp="document.getElementById('err').innerHTML='&nbsp;';" autocomplete="off">
    <br>
	<h3 id="err" style="color: #dbdd52">&nbsp;</h3>
    <br>
	<button id="CheckPassword">Check Password</button> - <button id="Exit">Exit</button>
  </body>
</html>
"""

dashboard_html = """
<!DOCTYPE html>
<html>
  <head>
    <title>Dashboard</title>
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
    <h1>Welcome !</h1>
    <br>
    <br>
	<button id="Exit">Exit</button>
  </body>
</html>
"""

# This function get called every time the user click on "MyButton1"
def check_the_password(e : webui.event):

	# Print some info (optional)
	# print('Element_id: ' + str(e.element_id))
	# print('Window_id: ' + str(e.window_id))
	# print('Element_name: ' + e.element_name.decode('utf-8'))

	# Run JavaScript to get the password
	res = MyWindow.run_js("return document.getElementById(\"MyInput\").value;")

	# Check for any error
	if res.error is True:
		print("JavaScript Error: " + res.data)
		return

	# Check the password
	if res.data == "123456":
		print("Password is correct.")
		MyWindow.show(dashboard_html)
	else:
		print("Wrong password: " + res.data)
		MyWindow.run_js(" document.getElementById('err').innerHTML = 'Sorry. Wrong password'; ")

def close_the_application(e : webui.event):
	webui.exit()

def main():

	# Bind am HTML element ID with a python function
	MyWindow.bind('CheckPassword', check_the_password)
	MyWindow.bind('Exit', close_the_application)

	# Show the window
	MyWindow.show(login_html)

	# Wait until all windows are closed
	webui.loop()

	print('Bye.')

if __name__ == "__main__":
    main()
