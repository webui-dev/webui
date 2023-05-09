#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
    #include <commdlg.h>
#elif __APPLE__
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC
        #include <CoreFoundation/CoreFoundation.h>
        #include <Cocoa/Cocoa.h>
    #endif
#elif __linux__
    #include <gtk/gtk.h>
#endif

#define FULL_FILE_BUFFER_SIZE (2 * 1024 * 1024) // 2 MB
char FULL_FILE_BUFFER[FULL_FILE_BUFFER_SIZE];
char JAVASCRIPT_BUFFER[FULL_FILE_BUFFER_SIZE];
char FILE_PATH[1024];

size_t read_file_into_buffer(const char *filename, char *buffer, size_t buffer_size) {

    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file %s.\n", filename);
        return -1;
    }

    size_t bytes_read = fread(buffer, 1, buffer_size, file);
    buffer[bytes_read] = '\0';

    if (ferror(file)) {
        fprintf(stderr, "Error: Could not read file %s.\n", filename);
        fclose(file);
        return -1;
    }

    fclose(file);
    return bytes_read;
}

#ifdef _WIN32
    UINT_PTR OpenDialogHooks(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
        BringWindowToTop(hdlg);
        return 0;
    }
#endif

char* show_file_open_dialog() {

    #ifdef _WIN32

        OPENFILENAMEA ofn;
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = FILE_PATH;
        ofn.nMaxFile = sizeof(FILE_PATH);
        ofn.lpstrFilter = "All Files\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.lpfnHook = OpenDialogHooks;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        if (GetOpenFileNameA(&ofn)) {
            return FILE_PATH;
        }

    #elif __APPLE__

        @autoreleasepool {
            NSOpenPanel *openPanel = [NSOpenPanel openPanel];
            [openPanel setCanChooseFiles:YES];
            [openPanel setCanChooseDirectories:NO];
            [openPanel setAllowsMultipleSelection:NO];
            [openPanel setAllowedFileTypes:nil];

            if ([openPanel runModal] == NSModalResponseOK) {
                NSURL *fileURL = [[openPanel URLs] objectAtIndex:0];
                const char *fileSystemRepresentation = [[fileURL fileSystemRepresentation] UTF8String];
                strncpy(FILE_PATH, fileSystemRepresentation, sizeof(FILE_PATH));
                FILE_PATH[sizeof(FILE_PATH) - 1] = '\0';
                return FILE_PATH;
            }
        }

    #else

        if (!gtk_init_check(NULL, NULL)) {
            fprintf(stderr, "Error: Failed to initialize GTK.\n");
            return NULL;
        }

        GtkWidget *dialog = gtk_file_chooser_dialog_new("Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        gtk_file_chooser_set_select_multiple(chooser, FALSE);
        gtk_file_chooser_set_current_folder(chooser, g_get_home_dir());

        gint response = gtk_dialog_run(GTK_DIALOG(dialog));
        if (response == GTK_RESPONSE_ACCEPT) {
            char *filename = gtk_file_chooser_get_filename(chooser);
            strncpy(FILE_PATH, filename, sizeof(FILE_PATH));
            FILE_PATH[sizeof(FILE_PATH) - 1] = '\0';
            g_free(filename);
            gtk_widget_destroy(dialog);
            while (g_main_context_iteration(NULL, FALSE));
            return FILE_PATH;
        }

        gtk_widget_destroy(dialog);
        while (g_main_context_iteration(NULL, FALSE));

    #endif

    return NULL;
}
