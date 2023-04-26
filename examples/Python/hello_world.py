# Install WebUI
# pip install --upgrade webui2

from webui import webui

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

	# Run JavaScript to get the password
	res = e.window.script("return document.getElementById(\"MyInput\").value;")

	# Check for any error
	if res.error is True:
		print("JavaScript Error: " + res.data)
		return

	# Check the password
	if res.data == "123456":
		print("Password is correct.")
		e.window.show(dashboard_html)
	else:
		print("Wrong password: " + res.data)
		e.window.script(" document.getElementById('err').innerHTML = 'Sorry. Wrong password'; ")

def close_the_application(e : webui.event):
	webui.exit()

def main():

	# Create a window object
	MyWindow = webui.window()

	# Bind am HTML element ID with a python function
	MyWindow.bind('CheckPassword', check_the_password)
	MyWindow.bind('Exit', close_the_application)

	# Show the window
	MyWindow.show(login_html)

	# Wait until all windows are closed
	webui.wait()

	print('Thank you.')

if __name__ == "__main__":
	main()
