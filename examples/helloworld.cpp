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

				background: #E44D26;  /* fallback for old browsers */
				background: -webkit-linear-gradient(to right, #F16529, #E44D26);  /* Chrome 10-25, Safari 5.1-6 */
				background: linear-gradient(to right, #F16529, #E44D26); /* W3C, IE 10+/ Edge, Firefox 16+, Chrome 26+, Opera 12+, Safari 7+ */

				color:#fff; 
				font-family: Avant Garde,Avantgarde,Century Gothic,CenturyGothic,AppleGothic,sans-serif; 
				font-size: 18px;
				text-align: center;
			}
		</style>
	</head>
	<body>

		<h1>Welcome to WebUI!</h1>
		<h2>Hello World Example</h2>
		<br>
		<p>You can access the dahboard in the same window or in a new one.</p>
		<button id="MyButtonID1">Switch to dashboard</button> | <button id="MyButtonID2">Open dashboard in a new window</button>

	</body>
</html>
)V0G0N";

const std::string dashboard = R"V0G0N(
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

		<h1>Dashboard</h1>
		<br>
		<button id="MyDashButton1">Run JS from C++ app</button> | <button id="MyDashButton2">Close this window</button> | <button id="MyDashButton3">Close app</button>

	</body>
</html>
)V0G0N";

webui::window my_window;
webui::window dashboard_window;

void run_javascript(webui::event e){

    // Run a JavaScript code
	if(dashboard_window.is_show())
		dashboard_window.run_js("alert('A message from your C++ application!');");
	else
		my_window.run_js("alert('It\\'s seem you chose to open the dashboard in the same window.');");
}

void switch_to_dashboard(webui::event e){

	// Switch same window to the dashboard HTML script
	my_window.show(&dashboard);
}

void open_dashboard(webui::event e){

	// Open dashboard in a new window
	dashboard_window.show(&dashboard);
}

void close_dashboard(webui::event e){

	// Close dashboard window
	if(dashboard_window.is_show())
		dashboard_window.close();
	else
		my_window.close();
}

void close_app(webui::event e){

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
