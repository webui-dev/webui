// WebUI C - ChatGPT HTTPS API Example

#include "webui.h"

// ChatGPT Configuration
#define USER_KEY "sk-proj-xxx-xxxxxxxxxxxxxxxxxxxxxxx_xxx"
#define USER_MODEL "gpt-4o"
#define USER_ASSISTANT "You are an assistant, answer with very short messages."

#define BUF_SIZE (1024)
size_t hiddenWindow = 0;

// HTML
const char* html = "<!DOCTYPE html>"
"<html>"
"  <head>"
"    <script src=\"webui.js\"></script>"
"  </head>"
"  <body>"
"    <script>"
"      function run_gpt_api(userKey, userModel, userAssistant, userContent) {"
"        const xhr = new XMLHttpRequest();"
"        xhr.open(\"POST\", \"https://api.openai.com/v1/chat/completions\", false);"
"        xhr.setRequestHeader(\"Content-Type\", \"application/json\");"
"        xhr.setRequestHeader(\"Authorization\", \"Bearer \" + userKey);"
"        xhr.send(JSON.stringify({"
"          model: userModel,"
"          messages: ["
"            { role: \"developer\", content: userAssistant },"
"            { role: \"user\", content: userContent }"
"          ]"
"        }));"
"        const responseJson = JSON.parse(xhr.responseText);"
"        if (responseJson.error) {"
"          return 'Error: ' + responseJson.error.message;"
"        }"
"        return (responseJson.choices[0].message.content).trim();"
"      }"
"    </script>"
"  </body>"
"</html>";

bool run_ai_query(const char* user_query, char* ai_response) {

    char js[BUF_SIZE];
    memset(js, 0, BUF_SIZE);

    // Generate JavaScript
    sprintf(js,
        "return run_gpt_api('%s', '%s', '%s', '%s');",
        USER_KEY, USER_MODEL, USER_ASSISTANT, user_query
    );

    // Run HTTPS API
    if (webui_script(hiddenWindow, js, 30, ai_response, BUF_SIZE)) {
        return true;
    }

    return false;
}

int main(int argc, char *argv[]) {
    
    if (argc < 2) {
        printf("Please provide a query.\nExample: %s What is the capital of Canada?\n", argv[0]);
        return 0;
    }

    // Initializing
    char ai_response[BUF_SIZE];
    char user_query[BUF_SIZE];
    memset(ai_response, 0, BUF_SIZE);
    memset(user_query, 0, BUF_SIZE);

    // Get user query
    for (int i = 1; i < argc; i++) {
        strcat(user_query, argv[i]);
        strcat(user_query, " ");
    }

    // Start WebUI server
    hiddenWindow = webui_new_window();
    webui_set_hide(hiddenWindow, true);
    webui_show_browser(hiddenWindow, html, ChromiumBased);

    // Run HTTPS API
    if (run_ai_query(user_query, ai_response)) {
        printf("AI Response: %s\n", ai_response);
    } else {
        printf("Error:\n%s\n", ai_response);
    }

    // Exit
    webui_exit();
    webui_clean();

    return 0;
}
