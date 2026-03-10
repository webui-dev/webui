#ifndef VIRTUAL_FILE_SYSTEM_H
#define VIRTUAL_FILE_SYSTEM_H

typedef struct {
    const char *path;
    const unsigned char *data;
    int length;
} VirtualFile;


static const VirtualFile virtual_files[] = {
};

static const int virtual_files_count = sizeof(virtual_files) / sizeof(virtual_files[0]);

bool virtual_file_system(const char* path, const unsigned char** file, int* length) {
    for (int i = 0; i < virtual_files_count; ++i) {
        if (strcmp(virtual_files[i].path, path) == 0) {
            *file = virtual_files[i].data;
            *length = virtual_files[i].length;
            return true;
        }
    }
    return false;
}

const void* vfs(const char* path, int* length) {
    const unsigned char* file_data;
    int file_length;

    if (virtual_file_system(path, &file_data, &file_length)) {
        const char* content_type = webui_get_mime_type(path);
        const char* http_header_template = "HTTP/1.1 200 OK\r\n"
                                            "Content-Type: %s\r\n"
                                            "Content-Length: %d\r\n"
                                            "Cache-Control: no-cache\r\n\r\n";
        int header_length = snprintf(NULL, 0, http_header_template, content_type, file_length);
        *length = header_length + file_length;
        unsigned char* response = (unsigned char*) webui_malloc(*length);
        snprintf((char*) response, header_length + 1, http_header_template, content_type, file_length);
        memcpy(response + header_length, file_data, file_length);
        return response;
    }
    return NULL;
}

#endif // VIRTUAL_FILE_SYSTEM_H
