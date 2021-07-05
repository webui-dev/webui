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

webui::window my_window;

void welcome(webui::event e){

	std::cout << "Welcome! You clicked on the watch!" << std::endl;
}

int main(){

	// This example show how to
	// make the WebUI server files
	// in a specific folder.

	// Path to server
	// Note: if it's empty the WebUI 
	// automaticaly server the current folder.
	my_window.serve_folder("");

	// Set WebUI to wait for first request
	// 0: infinit wait
	// n: wait for n seconds
	webui::set_timeout_sec(0);

	// Set WebUI to allow multi
	// window serving, this is
	// because each file is a new
	// request
	my_window.allow_multi_serving(true);

	// Link the HTML button
	my_window.bind("MyDiv", welcome);

	// Create a new server (HTTP & Websocket)
	// and get URL
	std::string url = my_window.new_server();

	std::cout << "Starting WebUI server.." << std::endl;
	std::cout << "Please goto to this address: " << url << std::endl;
	std::cout << std::endl << "Infinite wait.." << std::endl;
	std::cout << "To stop just press Ctrl + C" << std::endl;

	// Open a window!
	std::string full_link = webui::get_current_path() + "index.html?webui=" + url + "/webuicore.js";
	my_window.open_window(full_link, webui::browser::chrome);

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
