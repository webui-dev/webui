// WebUI C - Frameless Example

#include "webui.h"

const char* html =
"<html>"
"  <head>"
"    <meta charset='UTF-8'>"
"    <script src=\"webui.js\"></script>"
"    <style>"
"      * { margin: 0; padding: 0; box-sizing: border-box; }"
"      html, body { height: 100%; width: 100%; overflow: hidden; background: transparent; }"
"      #titlebar {"
"        height: 40px;"
"        background: linear-gradient(to right, #2c3e50, #34495e);"
"        color: white;"
"        display: flex;"
"        align-items: center;"
"        justify-content: space-between;"
"        padding: 0 15px;"
"        box-shadow: 0 2px 10px rgba(0, 0, 0, 0.3);"
"        -webkit-app-region: drag;"
"        font-family: Arial, sans-serif;"
"      }"
"      #title { font-size: 16px; font-weight: bold; }"
"      #buttons { -webkit-app-region: no-drag; }"
"      .button {"
"        display: inline-block;"
"        width: 24px;"
"        height: 24px;"
"        margin-left: 8px;"
"        border-radius: 50%;"
"        text-align: center;"
"        line-height: 24px;"
"        cursor: pointer;"
"        transition: all 0.2s;"
"      }"
"      .minimize { background: #f1c40f; }"
"      .maximize { background: #2ecc71; }"
"      .close { background: #e74c3c; }"
"      .button:hover { filter: brightness(120%); }"
"      #content {"
"        height: calc(100% - 40px);"
"        background: rgba(0, 0, 0, 0.5);"
"        display: flex;"
"        align-items: center;"
"        justify-content: center;"
"      }"
"      #message {"
"        color: white;"
"        font-size: 32px;"
"        font-family: Arial, sans-serif;"
"        text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.3);"
"      }"
"    </style>"
"  </head>"
"  <body>"
"    <div id='titlebar'>"
"      <span id='title'>WebUI Frameless Window</span>"
"      <div id='buttons'>"
"        <span class='button minimize' onclick='minimize()'>–</span>"
// "     <span class='button maximize' onclick='maximize()'>□</span>"
"        <span class='button close' onclick='close_win()'>✕</span>"
"      </div>"
"    </div>"
"    <div id='content'>"
"      <span id='message'>This is a WebUI frameless example</span>"
"    </div>"
"  </body>"
"</html>";

void minimize(webui_event_t* e) {
    webui_minimize(e->window);
}

void maximize(webui_event_t* e) {
    webui_maximize(e->window);
}

void close_win(webui_event_t* e) {
    webui_close(e->window);
}

int main() {

	size_t my_window = webui_new_window();

	webui_bind(my_window, "minimize", minimize);
	webui_bind(my_window, "maximize", maximize);
	webui_bind(my_window, "close_win", close_win);

	webui_set_frameless(my_window, true);
	webui_set_transparent(my_window, true);
	webui_set_resizable(my_window, true);
	webui_set_center(my_window);
	
	webui_show_wv(my_window, html);
	webui_wait();
	return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
