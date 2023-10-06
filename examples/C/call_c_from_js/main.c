// Call C from JavaScript Example

#include "webui.h"

void my_function_string(webui_event_t* e) {

    // JavaScript:
    // webui.call('MyID_One', 'Hello', 'World`);

    const char* str_1 = webui_get_string(e); // Or webui_get_string_at(e, 0);
    const char* str_2 = webui_get_string_at(e, 1);

    printf("my_function_string 1: %s\n", str_1); // Hello
    printf("my_function_string 2: %s\n", str_2); // World
}

void my_function_integer(webui_event_t* e) {

    // JavaScript:
    // webui.call('MyID_Two', 123, 456, 789);

    long long number_1 = webui_get_int(e); // Or webui_get_int_at(e, 0);
    long long number_2 = webui_get_int_at(e, 1);
    long long number_3 = webui_get_int_at(e, 2);

    printf("my_function_integer 1: %lld\n", number_1); // 123
    printf("my_function_integer 2: %lld\n", number_2); // 456
    printf("my_function_integer 3: %lld\n", number_3); // 789
}

void my_function_boolean(webui_event_t* e) {

    // JavaScript:
    // webui.call('MyID_Three', true, false);

    bool status_1 = webui_get_bool(e); // Or webui_get_bool_at(e, 0);
    bool status_2 = webui_get_bool_at(e, 1);

    printf("my_function_boolean 1: %s\n", (status_1 ? "True" : "False")); // True
    printf("my_function_boolean 2: %s\n", (status_2 ? "True" : "False")); // False
}

void my_function_raw_binary(webui_event_t* e) {

    // JavaScript:
    // webui.call('MyID_RawBinary', new Uint8Array([0x41]), new Uint8Array([0x42, 0x43]));

    const unsigned char* raw_1 = (const unsigned char*)webui_get_string(e); // Or webui_get_string_at(e, 0);
    const unsigned char* raw_2 = (const unsigned char*)webui_get_string_at(e, 1);

    size_t len_1 = webui_get_size(e); // Or webui_get_size_at(e, 0);
    size_t len_2 = webui_get_size_at(e, 1);

    // Print raw_1
    printf("my_function_raw_binary 1 (%zu bytes): ", len_1);
    for (size_t i = 0; i < len_1; i++)
        printf("0x%02x ", raw_1[i]);
    printf("\n");

    // Print raw_2
    printf("my_function_raw_binary 2 (%zu bytes): ", len_2);
    for (size_t i = 0; i < len_2; i++)
        printf("0x%02x ", raw_2[i]);
    printf("\n");
}

void my_function_with_response(webui_event_t* e) {

    // JavaScript:
    // webui.call('MyID_Four', number, 2).then(...)

    long long number = webui_get_int(e); // Or webui_get_int_at(e, 0);
    long long times = webui_get_int_at(e, 1);

    int res = number * times;
    printf("my_function_with_response: %lld * %lld = %lld\n", number, times, res);

    // Send back the response to JavaScript
    webui_return_int(e, res);
}

int main() {

    // HTML
    const char* my_html = "<!DOCTYPE html>"
    "<html>"
    "  <head>"
    "    <meta charset=\"UTF-8\">"
    "    <script src=\"webui.js\"></script>"
    "    <title>Call C from JavaScript Example</title>"
    "    <style>"
    "      body {"
    "        background: linear-gradient(to left, #36265a, #654da9);"
    "        color: AliceBlue;"
    "        font: 16px sans-serif;"
    "        text-align: center;"
    "        margin-top: 30px;"
    "      }"
    "      button {"
    "        margin: 5px 0 10px;"
    "      }"
    "    </style>"
    "  </head>"
    "  <body>"
    "    <h1>WebUI - Call C from JavaScript</h1>"
    "    <p>Call C functions with arguments (<em>See the logs in your terminal</em>)</p>"
    "    <button onclick=\"webui.call('MyID_One', 'Hello', 'World');\">Call my_function_string()</button>"
    "    <br>"
    "    <button onclick=\"webui.call('MyID_Two', 123, 456, 789);\">Call my_function_integer()</button>"
    "    <br>"
    "    <button onclick=\"webui.call('MyID_Three', true, false);\">Call my_function_boolean()</button>"
    "    <br>"
    "    <button onclick=\"webui.call('MyID_RawBinary', new Uint8Array([0x41]), new Uint8Array([0x42, 0x43]));\">Call my_function_raw_binary()</button>"
    "    <br>"
    "    <p>Call a C function that returns a response</p>"
    "    <button onclick=\"MyJS();\">Call my_function_with_response()</button>"
    "    <div>Double: <input type=\"text\" id=\"MyInputID\" value=\"2\"></div>"
    "    <script>"
    "      function MyJS() {"
    "        const MyInput = document.getElementById('MyInputID');"
    "        const number = MyInput.value;"
    "        webui.call('MyID_Four', number, 2).then((response) => {"
    "            MyInput.value = response;"
    "        });"
    "      }"
    "    </script>"
    "  </body>"
    "</html>";

    // Create a window
    size_t my_window = webui_new_window();

    // Bind HTML elements with C functions
    webui_bind(my_window, "MyID_One", my_function_string);
    webui_bind(my_window, "MyID_Two", my_function_integer);
    webui_bind(my_window, "MyID_Three", my_function_boolean);
    webui_bind(my_window, "MyID_Four", my_function_with_response);
    webui_bind(my_window, "MyID_RawBinary", my_function_raw_binary);

    // Show the window
    webui_show(my_window, my_html); // webui_show_browser(my_window, my_html, Chrome);

    // Wait until all windows get closed
    webui_wait();

    // Free all memory resources (Optional)
    webui_clean();

    return 0;
}

#if defined(_MSC_VER)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
        return main();
    }
#endif
