// Web App Multi-Client Example

#include "webui.h"

// Arrays to hold permanent data
char privateInput_arr[256][1024] = {0}; // One for each user
char publicInput_arr[1024] = {0}; // One for all users
int users_count = 0;
int tab_count = 0;

void exit_app(webui_event_t* e) {
	// Close all opened windows
	webui_exit();
}

void save(webui_event_t* e) {
	// Get input value
	const char* privateInput = webui_get_string(e);
	// Save it in the array
	snprintf(privateInput_arr[e->client_id], 1024, "%s", privateInput);
}

void saveAll(webui_event_t* e) {
	// Get input value
	const char* publicInput = webui_get_string(e);
	// Save it in the array
	snprintf(publicInput_arr, 1024, "%s", publicInput);
	// Update all users
	char buffer[512] = {0};
	snprintf(buffer, sizeof(buffer), "document.getElementById(\"publicInput\").value = \"%s\";", publicInput);
	webui_run(e->window, buffer);
}

void events(webui_event_t* e) {

	// This function gets called every time
	// there is an event

	// Full web browser cookies
	const char* cookies = e->cookies;

	// Static client (Based on web browser cookies)
	size_t client_id = e->client_id;

	// Dynamic client connection ID (Changes on connect/disconnect events)
	size_t connection_id = e->connection_id;

	if (e->event_type == WEBUI_EVENT_CONNECTED) {
		// New connection
		if (users_count < (client_id + 1)) { // +1 because it start from 0
			users_count = (client_id + 1);
		}
		tab_count++;
	}
	else if (e->event_type == WEBUI_EVENT_DISCONNECTED) {
		// Disconnection
		if (tab_count > 0)
			tab_count--;
	}

	// Buffer
	char buffer[512] = {0};

	// Update this current user only

	// status
	webui_run_client(e, "document.getElementById(\"status\").innerText = \"Connected!\";");

	// userNumber
	snprintf(buffer, sizeof(buffer), "document.getElementById(\"userNumber\").innerText = \"%zu\";", client_id);
	webui_run_client(e, buffer);

	// connectionNumber
	snprintf(buffer, sizeof(buffer), "document.getElementById(\"connectionNumber\").innerText = \"%zu\";", connection_id);
	webui_run_client(e, buffer);

	// privateInput
	snprintf(buffer, sizeof(buffer), "document.getElementById(\"privateInput\").value = \"%s\";", privateInput_arr[client_id]);
	webui_run_client(e, buffer);

	// publicInput
	snprintf(buffer, sizeof(buffer), "document.getElementById(\"publicInput\").value = \"%s\";", publicInput_arr);
	webui_run_client(e, buffer);

	// Update all connected users

	// userCount
	snprintf(buffer, sizeof(buffer), "document.getElementById(\"userCount\").innerText = \"%d\";", users_count);
	webui_run(e->window, buffer);

	// tabCount
	snprintf(buffer, sizeof(buffer), "document.getElementById(\"tabCount\").innerText = \"%d\";", tab_count);
	webui_run(e->window, buffer);
}

int main() {

	// Allow multi-user connection
	webui_set_config(multi_client, true);

	// Allow cookies
	webui_set_config(use_cookies, true);

	// Create new window
	size_t win = webui_new_window();

	// Bind HTML with a C functions
	webui_bind(win, "save", save);
	webui_bind(win, "saveAll", saveAll);
	webui_bind(win, "exit_app", exit_app);

	// Bind all events
	webui_bind(win, "", events);

	// Start server only
	const char* url = webui_start_server(win, "index.html");

	// Open a new page in the default native web browser
	webui_open_url(url);

	// Wait until all windows get closed
	webui_wait();

	// Free all memory resources (Optional)
	webui_clean();

	return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
