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

				background: #485563;  /* fallback for old browsers */
				background: -webkit-linear-gradient(to right, #29323c, #485563);  /* Chrome 10-25, Safari 5.1-6 */
				background: linear-gradient(to right, #29323c, #485563); /* W3C, IE 10+/ Edge, Firefox 16+, Chrome 26+, Opera 12+, Safari 7+ */

				color:#fff; 
				font-family: Avant Garde,Avantgarde,Century Gothic,CenturyGothic,AppleGothic,sans-serif; 
				font-size: 18px;
				text-align: center;
			}
		</style>
	</head>
	<body>

		<h1>Welcome to WebUI!</h1>
		<h2>Multi Access Example</h2>
		<br>
		<p>You can access this window from multiple browser using the same address.</p>
		<button id="MyButtonID1">execute welcome() function</button>

	</body>
</html>
)V0G0N";

webui::window my_window;

void welcome(webui::event e){

	std::cout << "Welcome!" << std::endl;
}

int main(){

	// This example show how to
	// get the link and make the
	// WebUI loop() wait for us.

	// Set WebUI to wait forever!
	// 0: infinit loop
	// n: wait for n seconds
	webui::set_timeout_sec(0);

	// Set WebUI to allow multi
	// window serving, so we can
	// access the same address
	// multiple times
	my_window.allow_multi_serving(true);

	// Link the HTML button
	my_window.bind("MyButtonID1", welcome);

	// Create a new server (HTTP & Websocket)
	// and get URL
	std::string url = my_window.new_server(&my_html);

	std::cout << "Please goto to this address: " << url << std::endl;
	std::cout << std::endl << "Wait forever!.." << std::endl;
	std::cout << "To stop just press Ctrl + C" << std::endl;

	// Loop
	std::thread ui(webui::loop);
	ui.join();
	
	return 0;
}

// Win32 entry point (if needed)
#ifdef _WIN32
	int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow){
		main();
		return 0;
	}
#endif
