#include "webui.h"

const char *doc =
    "<html style=\"background: #654da9; color: #eee\">"
    "<head>"
    "  <script src=\"webui.js\"></script>"
    "</head>"
    "<body>"
    "  <script>"
    "    setTimeout(async () => {"
    "      await webui.call(\"fail\");"
    "    }, 500)"
    "  </script>"
    "</body>"
    "</html>";

void fail(webui_event_t *e) {
	// This test should help to ensure that test failures can be detected from CI.
	exit(EXIT_FAILURE);
}

int main() {
	size_t w = webui_new_window();

	webui_bind(w, "fail", fail);
	webui_show(w, doc);
	webui_wait();

	return 0;
}
