// Public Network Access Example

#include "webui.h"

// Windows
size_t private_window = 0;
size_t public_window = 0;

void app_exit(webui_event_t* e) {
	webui_exit();
}

void public_window_events(webui_event_t* e) {
	if (e->event_type == WEBUI_EVENT_CONNECTED) {
		// New connection
		webui_run(private_window, "document.getElementById(\"Logs\").value += \"New connection.\\n\";");
	}
	else if (e->event_type == WEBUI_EVENT_DISCONNECTED) {
		// Disconnection
		webui_run(private_window, "document.getElementById(\"Logs\").value += \"Disconnected.\\n\";");
	}
}

void private_window_events(webui_event_t* e) {
	if (e->event_type == WEBUI_EVENT_CONNECTED) {
		
		// Get port of public window
		size_t port = webui_get_port(public_window);

		// Get URL of public window
		const char* url = webui_get_url(public_window);

		// JavaScript
		char javascript[1024];
		sprintf(javascript, "document.getElementById('urlSpan1').innerHTML = 'http://localhost:%zu';", port);
		webui_run(private_window, javascript);
		sprintf(javascript, "document.getElementById('urlSpan2').innerHTML = '%s';", url);
		webui_run(private_window, javascript);
	}
}

int main() {

	// Main Private Window HTML
	const char* private_html = "<!DOCTYPE html>"
		"<html>"
		"  <head>"
		"    <meta charset=\"UTF-8\">"
		"    <script src=\"webui.js\"></script>"
		"    <title>Public Network Access Example</title>"
		"    <style>"
		"       body {"
		"            font-family: 'Arial', sans-serif;"
		"            color: white;"
		"            background: linear-gradient(to right, #507d91, #1c596f, #022737);"
		"            text-align: center;"
		"            font-size: 18px;"
		"        }"
		"        button, input {"
		"            padding: 10px;"
		"            margin: 10px;"
		"            border-radius: 3px;"
		"            border: 1px solid #ccc;"
		"            box-shadow: 0 3px 5px rgba(0,0,0,0.1);"
		"            transition: 0.2s;"
		"        }"
		"        button {"
		"            background: #3498db;"
		"            color: #fff; "
		"            cursor: pointer;"
		"            font-size: 16px;"
		"        }"
		"        h1 { text-shadow: -7px 10px 7px rgb(67 57 57 / 76%); }"
		"        button:hover { background: #c9913d; }"
		"        input:focus { outline: none; border-color: #3498db; }"
		"    </style>"
		"  </head>"
		"  <body>"
		"    <h1>WebUI - Public Network Access Example</h1>"
		"    <br>"
		"    The public window is configured to be accessible from <br>"
		"    any device in the network. <br>"
		"    <br>"
		"    Public window links: <br>"
		"    <h1 id=\"urlSpan1\" style=\"color:#c9913d\">...</h1>"
		"    <h1 id=\"urlSpan2\" style=\"color:#c9913d\">...</h1>"
		"    Public window events: <br>"
		"    <textarea id=\"Logs\" rows=\"4\" cols=\"50\" style=\"width:60%\"></textarea>"
		"    <br>"
		"    <button id=\"Exit\">Exit</button>"
		"  </body>"
		"</html>";

	// Public Window HTML
	const char* public_html = "<!DOCTYPE html>"
		"<html>"
		"  <head>"
		"    <meta charset=\"UTF-8\">"
		"    <script src=\"webui.js\"></script>"
		"    <title>Welcome to Public UI</title>"
		"  </head>"
		"  <body>"
		"    <h1>Welcome to Public UI!</h1>"
		"  </body>"
		"</html>";

	// Create windows
	private_window = webui_new_window();
	public_window = webui_new_window();

	// App
	webui_set_timeout(0); // Wait forever (never timeout)

	// Public Window
	webui_set_public(public_window, true); // Make URL accessible from public networks
	webui_bind(public_window, "", public_window_events); // Bind all events
	webui_set_port(public_window, 9000); // Custom port
	webui_show_browser(public_window, public_html, NoBrowser); // Set public window HTML

	// Private Window
	webui_bind(private_window, "", private_window_events); // Run JS
	webui_bind(private_window, "Exit", app_exit); // Bind exit button
	webui_show(private_window, private_html); // Show the window

	// Wait until all windows get closed
	webui_wait();

	// Free all memory resources (Optional)
	webui_clean();

	return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
