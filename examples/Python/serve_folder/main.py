


# Install WebUI
# pip install --upgrade webui2

from webui import webui

def switch_to_second_page(e : webui.event):
	# This function get called every time 
	# the user click on "SwitchToSecondPage" button
	e.window.open("second.html", webui.browser.any)

def close_the_application(e : webui.event):
	webui.exit()

def main():

	# Create a window object
	MyWindow = webui.window()

	# Bind am HTML element ID with a python function
	MyWindow.bind('SwitchToSecondPage', switch_to_second_page)
	MyWindow.bind('Exit', close_the_application)

	# The root path. Leave it empty to let the WebUI 
	# automatically select the current working folder
	root_path = ""

	# Create a new web server using WebUI
	url = MyWindow.new_server(root_path)

	# Show a window using the generated URL
	MyWindow.open(url, webui.browser.chrome)

	# Wait until all windows are closed
	webui.wait()

	# --[ Note ]-----------------
	# Add this script to all your .html files:
	# <script src="webui.js"></script>
	# ---------------------------

	print('Thank you.')

if __name__ == "__main__":
	main()
