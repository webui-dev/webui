#include "webui.hpp"
#include <iostream>

int main() {
	webui::window my_window;
	my_window.show("<html><head><script src=\"webui.js\"></script></head> C++ Hello World ! </html>");
	webui::wait();
	return 0;
}

#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
