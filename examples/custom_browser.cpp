#ifndef UNICODE
#define UNICODE
#endif

// WebUI header
#include <webui/webui.hpp>

// C++ headers
#include <iostream>

// Win32 headers
#ifdef _WIN32
	#include <windows.h>
#endif

// HTML Code
const std::string my_html = R"V0G0N(
<!DOCTYPE html>
<html>
	<head>
		<title>Custom Browser - WebUI</title>
	</head>
	<body style="background-color:#515C6B; color:#fff; font-family:"Lucida Console", Courier, monospace">

		<h1>Welcome to WebUI!</h1>
		<p>This window is using custom web browser settings.</p>
		<button id="MyButtonID1">Close This Window</button>

	</body>
</html>
)V0G0N";

webui::window my_window;

void close(webui::event e){

	std::cout << "You just clicked on a button!" << std::endl;
	my_window.close();
}

int main(){

	// This example show how to set
	// WebUI to use a custom browser

	webui::custom_browser_t MyBrowser {

		// Firefox on Linux
		// Command: 'firefox -private -url http://127.0.0.1:1234'

		.app = "firefox",
		.arg = "-private -url ",
		.link = true
	};

	my_window.bind("MyButtonID1", close);

	// Show window
	if(!my_window.show(&my_html, &MyBrowser)){

		// Failed to start our custom web browser
		std::cout << "WebUI has failed to start the web browser" << std::endl;
	}

	// Loop
	std::thread ui(webui::loop);
	ui.join();

	// All windows are closed.
	printf("Good! All WebUI windows are closed now.\n");
	
	return 0;
}

// Win32 entry point (if needed)
#ifdef _WIN32
	int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow){
		main();
		return 0;
	}
#endif
