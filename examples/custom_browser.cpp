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
		<style>
			body{

				background: #ad5389;  /* fallback for old browsers */
				background: -webkit-linear-gradient(to right, #3c1053, #ad5389);  /* Chrome 10-25, Safari 5.1-6 */
				background: linear-gradient(to right, #3c1053, #ad5389); /* W3C, IE 10+/ Edge, Firefox 16+, Chrome 26+, Opera 12+, Safari 7+ */
				
				color:#fff; 
				font-family: Avant Garde,Avantgarde,Century Gothic,CenturyGothic,AppleGothic,sans-serif; 
				font-size: 18px;
				text-align: center;
			}
		</style>
	</head>
	<body>

		<h1>Welcome to WebUI!</h1>
		<h2>Custom Browser Example</h2>
		<br>
		<p>This example show how to use custom settings to run your favorite web browser.</p>
		<button id="MyButtonID1">Exit</button>

	</body>
</html>
)V0G0N";

webui::window my_window;

void close(webui::event e){

	std::cout << "Bye!" << std::endl;
	my_window.close();
}

int main(){

	// This example show how to make
	// WebUI use a custom browser of 
	// our choice.

	// Linux - Firefox
	webui::custom_browser_t Linux_Firefox {
		// Firefox on Linux
		// Command: 'firefox -private -url http://127.0.0.1:1234'
		.app = "firefox",
		.arg = "-private -url ",
		.link = true
	};

	// Linux - Chrome
	webui::custom_browser_t Windows_Firefox {
		// Firefox on Linux
		// Command: 'start firefox -private -url http://127.0.0.1:1234'
		.app = "start firefox",
		.arg = "-private -url ",
		.link = true
	};	

	// Linux - Chrome
	webui::custom_browser_t Linux_Chrome {
		// Firefox on Linux
		// Command: 'chrome -private --app=http://127.0.0.1:1234'
		.app = "chrome",
		.arg = "-private --app=",
		.link = true
	};

	// Windows - Chrome
	webui::custom_browser_t Windows_Chrome {
		// Firefox on Linux
		// Command: 'start chrome -private --app=http://127.0.0.1:1234'
		.app = "start chrome",
		.arg = "-private --app=",
		.link = true
	};

	my_window.bind("MyButtonID1", close);

	// Show window (Windows Firefox)
	if(!my_window.show(&my_html, &Windows_Firefox)){

		// Failed to start our custom web browser
		std::cout << "WebUI has failed to start the web browser\nPlease change settings." << std::endl;
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
