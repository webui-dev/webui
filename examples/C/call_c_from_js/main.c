// Call C from JavaScript Example

#include "webui.h"

void my_function_string(webui_event_t* e) {

    // JavaScript:
    // webui.call('MyID_One', 'Hello');

    const char* str = webui_get_string(e);
    printf("my_function_string: %s\n", str); // Hello

    // Need Multiple Arguments?
    //
    // WebUI support only one argument. To get multiple arguments
    // you can send a JSON string from JavaScript then decode it.
    // Example:
    //
    // my_json = my_json_decoder(str);
    // foo = my_json[0];
    // bar = my_json[1];
}

void my_function_integer(webui_event_t* e) {

    // JavaScript:
    // webui.call('MyID_Two', 123456789);

    long long number = webui_get_int(e);
    printf("my_function_integer: %lld\n", number); // 123456789
}

void my_function_boolean(webui_event_t* e) {

    // JavaScript:
    // webui.call('MyID_Three', true);

    bool status = webui_get_bool(e); // True
    if(status)
        printf("my_function_boolean: True\n");
    else
        printf("my_function_boolean: False\n");
}

void my_function_raw_binary(webui_event_t* e) {

    // JavaScript:
    // webui.call('MyID_RawBinary', new Uint8Array([0x42, 0x43, 0x44]));

    const unsigned char* raw = (const unsigned char*)e->data;
    long long len = e->size;

    printf("my_function_raw_binary: %lld bytes\n", len);
    printf("my_function_raw_binary: ");
    for (size_t i = 0; i < len; i++)
        printf("0x%02x ", raw[i]);
    printf("\n");
}

void my_function_with_response(webui_event_t* e) {

    // JavaScript:
    // const result = webui.call('MyID_Four', number);

    long long number = webui_get_int(e);
    number = number * 2;
    printf("my_function_with_response: %lld\n", number);

    // Send back the response to JavaScript
    webui_return_int(e, number);
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
    "    <button onclick=\"webui.call('MyID_One', 'Hello');\">Call my_function_string()</button>"
    "    <br>"
    "    <button onclick=\"webui.call('MyID_Two', 123456789);\">Call my_function_integer()</button>"
    "    <br>"
    "    <button onclick=\"webui.call('MyID_Three', true);\">Call my_function_boolean()</button>"
    "    <br>"
    "    <button onclick=\"webui.call('MyID_RawBinary', new Uint8Array([0x41, 0x42, 0x43]));\">Call my_function_raw_binary()</button>"
    "    <br>"
    "    <p>Call a C function that returns a response</p>"
    "    <button onclick=\"MyJS();\">Call my_function_with_response()</button>"
    "    <div>Double: <input type=\"text\" id=\"MyInputID\" value=\"2\"></div>"
    "    <script>"
    "      function MyJS() {"
    "        const MyInput = document.getElementById('MyInputID');"
    "        const number = MyInput.value;"
    "        webui.call('MyID_Four', number).then((response) => {"
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
