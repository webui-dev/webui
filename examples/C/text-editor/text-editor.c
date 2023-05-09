// Text Editor in C using WebUI

#include "webui.h"
#include "cross_platform_open_file.h"

#include <stdio.h>
#include <stdlib.h>

void Close(webui_event_t* e) {
    // Close all opened windows
    printf("Exit.\n");
    webui_exit();
}

void Save(webui_event_t* e) {

    // Save data received from the UI

    FILE *file = fopen(FILE_PATH, "w");
    int results = fputs(e->data, file);
    if(results == EOF) {
        // Failed to write
    }
    fclose(file);
}

void Open(webui_event_t* e) {

    // Open a new file

    // Open file and save the path to FILE_PATH
    if(show_file_open_dialog() == NULL)
        return;

    // Read the full content to FULL_FILE_BUFFER
    size_t bytes_read = read_file_into_buffer(FILE_PATH, FULL_FILE_BUFFER, FULL_FILE_BUFFER_SIZE);
    if(bytes_read < 1)
        return;

    // Send file content
    // Encode the full content to base64
    char* file_encoded = webui_encode(FULL_FILE_BUFFER);
    if(file_encoded == NULL)
        return;   
    sprintf(JAVASCRIPT_BUFFER, "addText('%s')", file_encoded);    
    webui_run(e->window, JAVASCRIPT_BUFFER);

    // Send file name
    // Encode the file path to base64
    char* path_encoded = webui_encode(FILE_PATH);
    if(path_encoded == NULL)
        return;
    sprintf(JAVASCRIPT_BUFFER, "SetFile('%s')", path_encoded);    
    webui_run(e->window, JAVASCRIPT_BUFFER);

    // Clean
    webui_free(file_encoded);
    webui_free(path_encoded);

    /*
        // Add line by line example:

        FILE *file = fopen(FILE_PATH, "r");
        if(file == NULL)
            return;

        char line[1024] = {0};
        while (fgets(line, 1024, file) != NULL) {

            // Line
            char* line_encoded = webui_encode(line);

            if(line_encoded != NULL) {

                char js[1024] = {0};

                // JS
                sprintf(js, "addLine('%s')", line_encoded);

                // Send
                webui_run(e->window, js);

                // Clean
                webui_free(line_encoded);
            }
        }

        fclose(file);
    */
}

int main() {

    // Create new windows
    int MainWindow = webui_new_window();

    // Bind HTML element IDs with a C functions
    webui_bind(MainWindow, "Open", Open);
    webui_bind(MainWindow, "Save", Save);
    webui_bind(MainWindow, "Close", Close);

    // Show a new window
    webui_show(MainWindow, "ui/MainWindow.html");

    // Wait until all windows get closed
    webui_wait();

    return 0;
}

#if defined(_MSC_VER)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
        return main();
    }
#endif
