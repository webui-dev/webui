// Include the WebUI header
#include "webui.hpp"

// Include C++ STD
#include <iostream>

class ServeAFolderApp {
	public:
	ServeAFolderApp() {
		// Bind HTML element IDs with class methods
		my_window.bind("SwitchToSecondPage", this, &ServeAFolderApp::switch_to_second_page);
		my_window.bind("OpenNewWindow", this, &ServeAFolderApp::show_second_window);
		my_window.bind("Exit", this, &ServeAFolderApp::exit_app);
		my_second_window.bind("Exit", this, &ServeAFolderApp::exit_app);

		// Bind all events
		my_window.bind("", this, &ServeAFolderApp::events);
		my_second_window.bind("", this, &ServeAFolderApp::events);
	}

	void run() {
		// Print logs (debug build only)
		std::cout << "Starting..." << std::endl;

		// Show a new window
		my_window.show("index.html"); // my_window.show_browser("index.html", Chrome);

		// Wait until all windows get closed
		webui::wait();

		// Print logs (debug build only)
		std::cout << "Thank you." << std::endl;
	}

	private:
	webui::window my_window;
	webui::window my_second_window;

	const char* event_type_to_string(size_t type) const {
		switch (type) {
			case webui::DISCONNECTED: return "DISCONNECTED";
			case webui::CONNECTED: return "CONNECTED";
			case webui::MOUSE_CLICK: return "MOUSE_CLICK";
			case webui::NAVIGATION: return "NAVIGATION";
			case webui::CALLBACKS: return "CALLBACKS";
			default: return "UNKNOWN";
		}
	}

	// This method gets called every time the user clicks on "OpenNewWindow"
	void show_second_window(webui::window::event* e) {
		// Show a new window, and navigate to `/second.html`
		// if the window is already opened, then switch in the same window
		my_second_window.show("second.html");
	}

	// This method gets called every time the user clicks on "SwitchToSecondPage"
	void switch_to_second_page(webui::window::event* e) {
		// Switch to `/second.html` in the same opened window.
		e->get_window().show("second.html");
	}

	// This method receives all events because it's bind with an empty HTML ID.
	void events(webui::window::event* e) {
		std::cout << "[events] window=" << e->window
		          << " type=" << e->get_type()
		          << " (" << event_type_to_string(e->get_type()) << ")"
		          << " element='" << e->get_element()
		          << "' event=" << e->get_number()
		          << std::endl;

		if (e->event_type == webui::CONNECTED)
			std::cout << "Window Connected." << std::endl;
		else if (e->event_type == webui::DISCONNECTED)
			std::cout << "Window Disconnected." << std::endl;
		else if (e->event_type == webui::MOUSE_CLICK)
			std::cout << "Click on element: " << e->element << std::endl;
		else if (e->event_type == webui::NAVIGATION) {
			std::string url = e->get_string();
			std::cout << "Starting navigation to: " << url << std::endl;
			e->get_window().navigate(url);
		}
	}

	void exit_app(webui::window::event* e) {
		// Close all opened windows
		webui::exit();
	}
};

int main() {
	ServeAFolderApp app;
	app.run();
	return 0;
}

#ifdef _WIN32
// Release build
int WINAPI
    wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	return main();
}
#endif
