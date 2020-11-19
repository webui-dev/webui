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
const std::string html = R"V0G0N(
<!DOCTYPE html>
<html>
	<head>
		<title>My first WebUI app</title>
	</head>
	<body style="background-color:#515C6B; color:#fff; font-family:"Lucida Console", Courier, monospace">
		<h1>Welcome to WebUI!</h1>
		<button id="MyButtonID">Click on me!</button>
	</body>
</html>
)V0G0N";

webui::window my_window(&html);

void my_handler(){

    my_window.run_js("alert('A message from someone!');");
}

int main(){

	// Execute my_handler() when user click on 
	// an HTML DOM element with id 'MyButtonID'.
	my_window.bind("MyButtonID", my_handler);

	// Show window
	//if(!my_window.show(webui::browser::firefox))    // If Firefox not installed
		my_window.show();                           // try other web browsers.

	// Loop
	std::thread ui(webui::loop);
	ui.join();

	// All windows are closed.
	return 0;
}

// Win32 entry point (if needed)
#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow){
   main();
   return 0;
}
#endif