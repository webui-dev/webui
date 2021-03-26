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

	// This example show how to
	// get the link and make the
	// WebUI loop() wait for us.

	// Set WebUI to wait forever!
	// 0: infinit loop
	// n: wait for n seconds
	webui::set_timeout_sec(0);

	// Link the HTML button
	my_window.bind("MyButtonID1", close);

	// Get URL
	std::string url = my_window.get_address(&my_html);

	std::cout << "Please goto to this address: " << url << std::endl;
	std::cout << std::endl << "Wait forever!.." << std::endl;
	std::cout << "To stop just hit Ctrl + C" << std::endl;

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
