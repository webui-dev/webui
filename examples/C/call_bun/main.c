// WebUI C - Minimal Example

#include "webui.h"

int main() {
	size_t my_window = webui_new_window();
	// Make Bun as the `.ts` and `.js` interpreter
	webui_set_runtime(my_window, Bun);
	webui_show_browser(my_window, "index.html", Chrome);
	webui_wait();
	webui_clean();
	return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
