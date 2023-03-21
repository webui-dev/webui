/*
    WebUI Library 2.0.7
    C99 Example

    http://webui.me
    https://github.com/alifcommunity/webui

    Licensed under GNU General Public License v3.0.
    Copyright (C)2023 Hassan DRAGA <https://github.com/hassandraga> - Canada.
*/

#include "webui.h"

int main() {

    webui_window_t* my_window = webui_new_window();
    webui_show(my_window, "<html>Hello</html>", webui.browser.any);
    webui_wait();
    return 0;
}
