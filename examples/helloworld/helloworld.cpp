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
		<title>My first WebUI app</title>
	</head>
	<body style="background-color:#515C6B; color:#fff; font-family:"Lucida Console", Courier, monospace">
		<h1>Welcome to WebUI!</h1>
		<button id="MyButtonID1">Switch to dashboard</button> | <button id="MyButtonID2">Open dashboard in a new window</button>
	</body>
</html>
)V0G0N";

const std::string dashboard = R"V0G0N(
<!DOCTYPE html>
<html>
	<head>
		<title>My Second window!</title>
	</head>
	<body style="background-color:#515C6B; color:#fff; font-family:"Lucida Console", Courier, monospace">
		<h1>Dashboard</h1>
		<button id="MyDashButton1">Run JS from C++ app</button> | <button id="MyDashButton2">Close dashboard</button> | <button id="MyDashButton3">Close app</button>
	</body>
</html>
)V0G0N";

webui::window my_window;
webui::window dashboard_window;

void run_javascript(){

    // Run a JavaScript code
	if(dashboard_window.is_show())
		dashboard_window.run_js("alert('A message from your C++ application!');");
	else
		my_window.run_js("alert('It\\'s seem you chose to open the dashboard in the same window.');");
}

void switch_to_dashboard(){

	// Switch same window to the dashboard HTML script
	my_window.show(&dashboard);
}

void open_dashboard(){

	// Open dashboard in a new window
	dashboard_window.show(&dashboard);
}

void close_dashboard(){

	// Close dashboard window
	if(dashboard_window.is_show())
		dashboard_window.close();
	else
		my_window.close();
}

void close_app(){

	// Close all opened windows
	// this make webui::loop() to finish
	webui::exit();
}

int main(){

	// Bind 'MyButtonID' with my_handler().
	// Mean: Execute my_handler() when the user click 
	// on an HTML DOM element with id 'MyButtonID'.

	// Bind two first buttons
	my_window.bind("MyButtonID1", switch_to_dashboard);
	my_window.bind("MyButtonID2", open_dashboard);

	// Bind dashboard buttons with first window (for switch)
	my_window.bind("MyDashButton1", run_javascript);
	my_window.bind("MyDashButton2", close_dashboard);
	my_window.bind("MyDashButton3", close_app);

	// Bind dashboard buttons
	dashboard_window.bind("MyDashButton1", run_javascript);
	dashboard_window.bind("MyDashButton2", close_dashboard);
	dashboard_window.bind("MyDashButton3", close_app);

	// Show window
	if(!my_window.show(&my_html, webui::browser::chrome))	// If Google Chrome not installed
		my_window.show(&my_html);							// try other web browsers.

	// Loop
	std::thread ui(webui::loop);
	ui.join();

	// All windows are closed.
	printf("Good! All windows are closed now.\n");
	
	return 0;
}

// Win32 entry point (if needed)
#ifdef _WIN32
	int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow){
		main();
		return 0;
	}
#endif
