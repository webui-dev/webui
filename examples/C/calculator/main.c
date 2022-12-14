/*
    WebUI Library 2.x
    C99 Example

    http://webui.me
    https://github.com/alifcommunity/webui

    Licensed under GNU General Public License v3.0.
    Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.

    this program is very simple calculator
    programmed by Kemari Mohammed
*/

#include "webui.h"

char op = ' ';                              // operation ( + , - , * , / )
int a, b, c ;
bool Is_First_Op[2] = { true , true } ;     // just for calculate one operation ( The two elements because there are two events)
bool begin_second_number = false ;          // for begin writing second number and clear first number
bool end_calculate = false ;                // stop calculate when click "="

void close_the_application(webui_event_t* e){
    // Close all opened windows
    webui_exit();
}

void Onclick_numbers(webui_event_t* e){
    webui_script_t js;
    if(!end_calculate)
    {
        if(begin_second_number)
        {
            js.script = "Calculator.Input.value = '" ;
            js.timeout = 3;
            begin_second_number = false;
        }
        else
        {
            js.script = "Calculator.Input.value += '" ;
            js.timeout = 3;
        }
        const char* str = webui_get_string(e);
        asprintf(&js.script, "%s%s%s", js.script, str , "'" );

        // Run the JavaScript on the UI (Web Browser)
        webui_script(e->window, &js);
    }
}

void operation(webui_event_t* e){
    if(Is_First_Op[0] )
    {
        begin_second_number = true;
        a = webui_get_int(e);
        Is_First_Op[0] = false;
    }
}

void operation2(webui_event_t* e){
    if(Is_First_Op[1])
    {
        op = * webui_get_string(e);
        Is_First_Op[1] = false;
    }
}

void Result(webui_event_t* e){
    if(!end_calculate)
    {
        webui_script_t js = {
            .script = "Calculator.Input.value = '" ,
            .timeout = 3
        };
        b = webui_get_int(e);
        switch(op)
            {
            case '+':
                c = a + b;
                break;
            case '-':
                c = a - b;
                break;
            case '*':
                c = a * b;
                break;
            case '/':
                c = a / b;
                break;
            };
        asprintf(&js.script, "%s%d%s", js.script, c , "'" );
        webui_script(e->window, &js);

        begin_second_number  = false ;
        end_calculate        = true  ;
    }
}

void clear(webui_event_t* e){
        webui_script_t js = {
        .script = "Calculator.Input.value = ''" ,
        .timeout = 3
    };
    webui_script(e->window, &js);
    op = ' ';
    a = b = c = 0;

    Is_First_Op[0]       = true  ;
    Is_First_Op[1]       = true  ;
    begin_second_number  = false ;
    end_calculate        = false ;
}

int main() {
    // Create a window
    webui_set_timeout(20);
    webui_window_t* my_window = webui_new_window();

    // Bind HTML elements with functions
    webui_bind( my_window , "num"   , Onclick_numbers      );
    webui_bind( my_window , "ope"   , operation            );
    webui_bind( my_window , "ope2"  , operation2           );
    webui_bind( my_window , "calc"  , Result               );
    webui_bind( my_window , "clear" , clear                );
    webui_bind( my_window , "Exit"  , close_the_application);

    // HTML server
    const char* url = webui_new_server(my_window, "calc");

    // Show the window
    if(!webui_show(my_window, url, webui.browser.chrome))   // Run the window on Chrome
        webui_show(my_window, url, webui.browser.any);      // If not, run on any other installed web browser

    // Wait until all windows get closed
    webui_wait();

    return 0;
}

#if defined(_MSC_VER)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
        main();
    }
#endif
