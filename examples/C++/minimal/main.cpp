#include "webui.hpp"
#include <iostream>

int main() {
    void* my_window = webui::new_window();
    webui::show(my_window, "<html>Hello World!</html>");
    webui::wait();
    return 0;
}

#ifdef _WIN32
    int WINAPI wWinMain(
        _In_ HINSTANCE hInstance,
        _In_opt_ HINSTANCE hPrevInstance,
        _In_ LPWSTR lpCmdLine,
        _In_ int nShowCmd
    ) {
        return main();
    }
#endif
