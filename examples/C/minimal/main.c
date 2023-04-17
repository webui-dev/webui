// WebUI C - Minimal Example

#include "webui.h"

int main() {

    void* my_window = webui_new_window();
    webui_show(my_window, "<html>Hello</html>");
    webui_wait();
    return 0;
}
