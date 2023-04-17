// The C++ wrapper is coming soon.
// Please see the C example for now.

extern "C"{
    #include "webui.h"
}

#include <iostream>

int main() {

	std::cout << "The C++ wrapper is coming soon. Please see the C examples for now.";
    return 0;
}

#if defined(_MSC_VER)
	int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
		main();
	}
#endif
