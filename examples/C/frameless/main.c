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
"      #ui-container {"
"        height: 100%;"
"        width: 100%;"
"        background: rgba(30, 30, 30, 0.95);"
"        color: #f5f5f5;"
"        font-family: 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;"
"        display: flex;"
"        flex-direction: column;"
"        border-radius: 10px;"
"        backdrop-filter: blur(24px);"
"        -webkit-backdrop-filter: blur(24px);"
"        border: 1px solid rgba(255, 255, 255, 0.12);"
"        overflow: hidden;"
"      }"
"      #titlebar {"
"        height: 48px;"
"        background: rgba(0, 0, 0, 0.25);"
"        -webkit-app-region: drag;"
"        --webui-app-region: drag;"
"        display: flex;"
"        align-items: center;"
"        justify-content: space-between;"
"        padding: 0 18px;"
"        flex-shrink: 0;"
"      }"
"      #title { font-size: 15px; font-weight: 500; }"
"      #buttons {"
"        -webkit-app-region: no-drag;"
"        display: flex;"
"        gap: 12px;"
"      }"
"      #buttons span {"
"        width: 14px;"
"        height: 14px;"
"        border-radius: 50%;"
"        cursor: pointer;"
"        transition: all 0.15s ease-out;"
"      }"
"      #buttons span:hover { transform: scale(1.1); filter: brightness(1.15); }"
"      .buttons span:active { transform: scale(0.9); filter: brightness(0.9); }"
"      .close { background: #ff5f57; }"
"      .minimize { background: #ffbd2e; }"
"      /* .maximize { background: #28c940; } REMOVED */"
"      #content {"
"        flex-grow: 1;"
"        display: flex;"
"        flex-direction: column;"
"        align-items: center;"
"        justify-content: center;"
"        padding: 20px;"
"        text-align: center;"
"        overflow: auto;"
"      }"
"      #message {"
"        font-size: 38px;"
"        font-weight: 200;"
"        letter-spacing: 0.5px;"
"        text-shadow: 0 2px 4px rgba(0, 0, 0, 0.3);"
"      }"
"      #sub-message {"
"        font-size: 16px;"
"        font-weight: 300;"
"        color: rgba(240, 240, 240, 0.7);"
"        margin-top: 12px;"
"      }"
"    </style>"
"  </head>"
"  <body>"
"    <div id='ui-container'>"
"      <div id='titlebar'>"
"        <span id='title'>WebUI Frameless WebView Window</span>"
"        <div id='buttons'>"
"          <span class='button minimize' onclick='minimize()'></span>"
"          <span class='button close' onclick='close_win()'></span>"
"        </div>"
"      </div>"
"      <div id='content'>"
"        <span id='message'>Welcome to Your WebUI App</span>"
"        <span id='sub-message'>This is a stylish, frameless WebUI WebView window.</span>"
"      </div>"
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

	webui_set_size(my_window, 800, 600);
	webui_set_frameless(my_window, true);
	webui_set_transparent(my_window, true);
	webui_set_resizable(my_window, false);
	webui_set_center(my_window);

	webui_show_wv(my_window, html);
	webui_wait();
	return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(); }
#endif
