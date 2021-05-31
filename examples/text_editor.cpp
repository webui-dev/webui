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
		<title>Text Editor - WebUI</title>
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
		<h2>Text Editor (Beta)</h2>
		<br>
		<p>In future, this example will show how to use WebUI for an text editor app.</p>
		<button id="MyButtonID1">Click on me!</button>
		<button id="MyButtonID2">Click on me!</button>

	</body>
</html>
)V0G0N";

// It's recommended to remove all blank spaces.
const std::string my_favicon = "<?xml version=\"1.0\" ?><svg data-name=\"Layer 1\" id=\"Layer_1\" viewBox=\"0 0 64 64\" xmlns=\"http://www.w3.org/2000/svg\"><defs><style>.cls-1{fill:#4f4c5f;}.cls-2{fill:#484559;}.cls-3{fill:#403d4f;}.cls-4{fill:#fbb03b;}.cls-5{fill:#292733;}</style></defs><path class=\"cls-1\" d=\"M63.866,20.823A14.407,14.407,0,0,1,53.9,34.515a5.845,5.845,0,0,1-2.981-5.592,8.219,8.219,0,0,0,6.764-8.686A8.439,8.439,0,0,0,49.156,12.6H40.223a6.176,6.176,0,0,1-6.167-6.167A6.172,6.172,0,0,1,40.223.264h7.2A3.09,3.09,0,0,1,50.5,3.348a3.04,3.04,0,0,1-.74,1.994,3.035,3.035,0,0,1-2.344,1.09h2.056A14.407,14.407,0,0,1,63.866,20.823Z\"/><path class=\"cls-1\" d=\"M27.888,10.5V21.691c-3.5-.433-7.195-2.686-8.439-5.51l6.682-6.373A1.038,1.038,0,0,1,27.888,10.5Z\"/><path class=\"cls-1\" d=\"M4.976,9.808l6.681,6.373a11.131,11.131,0,0,1-8.439,5.51V10.5A1.038,1.038,0,0,1,4.976,9.808Z\"/><path class=\"cls-1\" d=\"M54.61,39.33V60.92a3.028,3.028,0,0,1-.02.31,2.941,2.941,0,0,1-.12.59,3.281,3.281,0,0,1-2.32,2.12,3.179,3.179,0,0,1-.62.06,3.308,3.308,0,0,1-.55-.05c-.15-.04-.29-.08-.44-.13a3.2,3.2,0,0,1-.93-.57c-.01-.01-.03-.02-.04-.03a3.391,3.391,0,0,1-1.12-2.58v-10a3.11,3.11,0,0,0-3.83-2.99,3.257,3.257,0,0,0-2.34,3.21V60.92a3.107,3.107,0,0,1-3.82,2.99,3.263,3.263,0,0,1-2.35-3.2V51.66a4.13,4.13,0,0,0-5.61-3.82,4.278,4.278,0,0,0-2.61,4.02v9.06a3.112,3.112,0,0,1-3.82,2.99,3.263,3.263,0,0,1-2.35-3.2V50.64a3.1,3.1,0,0,0-3.82-2.99,3.263,3.263,0,0,0-2.35,3.2V60.92a3.112,3.112,0,0,1-3.82,2.99A3.275,3.275,0,0,1,9.39,60.7V42.56A19.684,19.684,0,0,1,28.92,22.88h9.25a16.419,16.419,0,0,1,12.74,6.05,16.495,16.495,0,0,1,3.7,10.4Z\"/><path class=\"cls-2\" d=\"M54.61,39.33V60.92a3.028,3.028,0,0,1-.02.31,2.941,2.941,0,0,1-.12.59,2.978,2.978,0,0,1-.76,1.28,3.215,3.215,0,0,1-1.56.84,3.179,3.179,0,0,1-.62.06,3.308,3.308,0,0,1-.55-.05c-.15-.04-.29-.08-.44-.13a3.2,3.2,0,0,1-.93-.57,1.428,1.428,0,0,0,.18-.15,3.077,3.077,0,0,0,.91-2.18V39.33A16.495,16.495,0,0,0,47,28.93a16.439,16.439,0,0,0-12.75-6.05h3.92a16.419,16.419,0,0,1,12.74,6.05,16.495,16.495,0,0,1,3.7,10.4Z\"/><path class=\"cls-3\" d=\"M28.813,22.817c-.287-.442-.606-.864-.925-1.264a16.172,16.172,0,0,0-8.439-5.51,15.641,15.641,0,0,0-7.792,0,16.176,16.176,0,0,0-8.439,5.51A17.193,17.193,0,0,0,.134,27.371,17.471,17.471,0,0,0,2.344,32a16.54,16.54,0,0,0,4.092,4.235c.349.257.709.494,1.079.72a17.642,17.642,0,0,0,1.963,1,15.5,15.5,0,0,0,14.114-1c.37-.226.729-.463,1.079-.72A16.537,16.537,0,0,0,28.762,32a17.44,17.44,0,0,0,2.21-4.625A17.157,17.157,0,0,0,28.813,22.817Z\"/><path class=\"cls-4\" d=\"M23.777,24.934a3.076,3.076,0,1,1-2.056-2.909A3.074,3.074,0,0,1,23.777,24.934Z\"/><path class=\"cls-4\" d=\"M13.5,24.934a3.075,3.075,0,1,1-2.055-2.909A3.073,3.073,0,0,1,13.5,24.934Z\"/><path class=\"cls-5\" d=\"M16.818,29.965H14.289a1.026,1.026,0,0,0-.884,1.553l.39.647a2.022,2.022,0,0,0,.73.716v1.31a1.028,1.028,0,1,0,2.056,0v-1.31a2.022,2.022,0,0,0,.73-.716l.391-.647A1.027,1.027,0,0,0,16.818,29.965Z\"/><path class=\"cls-5\" d=\"M29.944,33.163H22.749a1.029,1.029,0,0,1,0-2.057h7.2a1.029,1.029,0,0,1,0,2.057Z\"/><path class=\"cls-5\" d=\"M8.358,33.163h-7.2a1.029,1.029,0,0,1,0-2.057h7.2a1.029,1.029,0,0,1,0,2.057Z\"/><path class=\"cls-5\" d=\"M6.3,39.33a1.028,1.028,0,0,1-.727-1.754l3.084-3.084a1.027,1.027,0,0,1,1.453,1.453L7.029,39.029A1.024,1.024,0,0,1,6.3,39.33Z\"/><path class=\"cls-5\" d=\"M24.805,39.33a1.024,1.024,0,0,1-.727-.3l-3.084-3.084a1.028,1.028,0,0,1,1.454-1.453l3.083,3.084a1.028,1.028,0,0,1-.726,1.754Z\"/><path class=\"cls-5\" d=\"M21.721,22.025v5.818a3.107,3.107,0,0,1-2.056,0V22.025a3.124,3.124,0,0,1,2.056,0Z\"/><path class=\"cls-5\" d=\"M11.442,22.025v5.818a3.107,3.107,0,0,1-2.056,0V22.025a3.124,3.124,0,0,1,2.056,0Z\"/><path class=\"cls-2\" d=\"M49.763,5.342a3.035,3.035,0,0,1-2.344,1.09H38.054A3.754,3.754,0,0,1,40.7,5.342Z\"/></svg>";

webui::window my_window;

void my_all_events(webui::event e){

	std::cout << "Something just happen!" << std::endl;
	std::cout << "Window ID: " << e.window_id << std::endl;
	std::cout << "Element ID: " << e.element_id << std::endl;
	std::cout << "Element Name: " << e.element_name << std::endl << std::endl;
}

int main(){

	// This example show how to
	// use a custom favicon.

	my_window.set_favicon(
		&my_favicon, 
		"image/svg+xml" // image/avif, image/webp, image/apng, image/svg+xml and more..!
	);

	// Bind all HTML elements
	my_window.bind_all(my_all_events);

	// Show the window
	my_window.show(&my_html);

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
