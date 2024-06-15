#include "webui.h"

const char *doc =
    "<html style=\"background: #654da9; color: #eee\">"
    "<head>"
    "  <script src=\"webui.js\"></script>"
    "</head>"
    "<body>"
    "  <script>"
    "    setTimeout(async () => {"
    "      await webui.call(\"close\");"
    "    }, 500)"
    "  </script>"
    "</body>"
    "</html>";

void close_window(webui_event_t *e) {
	webui_close(e->window);
}

int main() {
	size_t w = webui_new_window();

	webui_bind(w, "close", close_window);
	webui_show(w, doc);
	webui_wait();

	return 0;
}

