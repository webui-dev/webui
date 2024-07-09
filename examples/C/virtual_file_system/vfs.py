import os
import sys

def generate_vfs_header(directory, output_header):
    files = []
    index_files = {}

    for root, _, filenames in os.walk(directory):
        for filename in filenames:
            filepath = os.path.join(root, filename)
            relative_path = os.path.relpath(filepath, directory)
            # Ensure path starts with a slash
            relative_path = '/' + relative_path.replace('\\', '/')
            files.append((relative_path, filepath))

            # Check for index files
            if filename.startswith("index."):
                dir_path = os.path.dirname(relative_path)
                if dir_path not in index_files:
                    index_files[dir_path] = relative_path

    with open(output_header, 'w') as header:
        header.write('#ifndef VIRTUAL_FILE_SYSTEM_H\n')
        header.write('#define VIRTUAL_FILE_SYSTEM_H\n\n')

        header.write('typedef struct {\n')
        header.write('    const char *path;\n')
        header.write('    const unsigned char *data;\n')
        header.write('    int length;\n')
        header.write('} VirtualFile;\n\n')

        header.write('static const VirtualFile virtual_files[] = {\n')

        for relative_path, filepath in files:
            with open(filepath, 'rb') as f:
                data = f.read()
                header.write('    {\n')
                header.write(f'        "{relative_path}",\n')
                header.write('        (const unsigned char[]){')
                header.write(','.join(f'0x{byte:02x}' for byte in data))
                header.write('},\n')
                header.write(f'        {len(data)}\n')
                header.write('    },\n')

        header.write('};\n\n')

        header.write('static const int virtual_files_count = sizeof(virtual_files) / sizeof(virtual_files[0]);\n\n')

        header.write('static const char* index_files[] = {\n')
        for dir_path, index_path in index_files.items():
            header.write(f'    "{dir_path}/", "{index_path}",\n')
        header.write('    NULL\n')
        header.write('};\n\n')

        header.write('bool virtual_file_system(const char* path, const unsigned char** file, int* length) {\n')
        header.write('    for (int i = 0; i < virtual_files_count; ++i) {\n')
        header.write('        if (strcmp(virtual_files[i].path, path) == 0) {\n')
        header.write('            *file = virtual_files[i].data;\n')
        header.write('            *length = virtual_files[i].length;\n')
        header.write('            return true;\n')
        header.write('        }\n')
        header.write('    }\n')
        header.write('    return false;\n')
        header.write('}\n\n')

        header.write('void* vfs(const char* path, int* length) {\n')
        header.write('    const unsigned char* file_data;\n')
        header.write('    int file_length;\n\n')

        header.write('    if (virtual_file_system(path, &file_data, &file_length)) {\n')
        header.write('        const char* content_type = webui_get_mime_type(path);\n')
        header.write('        const char* http_header_template = "HTTP/1.1 200 OK\\r\\n"\n')
        header.write('                                            "Content-Type: %s\\r\\n"\n')
        header.write('                                            "Content-Length: %d\\r\\n"\n')
        header.write('                                            "Cache-Control: no-cache\\r\\n\\r\\n";\n')
        header.write('        int header_length = snprintf(NULL, 0, http_header_template, content_type, file_length);\n')
        header.write('        *length = header_length + file_length;\n')
        header.write('        unsigned char* response = (unsigned char*) webui_malloc(*length);\n')
        header.write('        snprintf((char*) response, header_length + 1, http_header_template, content_type, file_length);\n')
        header.write('        memcpy(response + header_length, file_data, file_length);\n')
        header.write('        return response;\n')
        header.write('    } else {\n')
        header.write('        // Check for index file redirection\n')
        header.write('        char redirect_path[1024];\n')
        header.write('        snprintf(redirect_path, sizeof(redirect_path), "%s", path);\n')
        header.write('        size_t len = strlen(redirect_path);\n')
        header.write('        if (redirect_path[len - 1] != \'/\') {\n')
        header.write('            redirect_path[len] = \'/\';\n')
        header.write('            redirect_path[len + 1] = \'\\0\';\n')
        header.write('        }\n')
        header.write('        for (int i = 0; index_files[i] != NULL; i += 2) {\n')
        header.write('            if (strcmp(index_files[i], redirect_path) == 0) {\n')
        header.write('                const char* location_header = "HTTP/1.1 302 Found\\r\\n"\n')
        header.write('                                            "Location: %s\\r\\n"\n')
        header.write('                                            "Cache-Control: no-cache\\r\\n\\r\\n";\n')
        header.write('                int header_length = snprintf(NULL, 0, location_header, index_files[i + 1]);\n')
        header.write('                *length = header_length;\n')
        header.write('                unsigned char* response = (unsigned char*) webui_malloc(*length);\n')
        header.write('                snprintf((char*) response, header_length + 1, location_header, index_files[i + 1]);\n')
        header.write('                return response;\n')
        header.write('            }\n')
        header.write('        }\n')
        header.write('        return NULL;\n')
        header.write('    }\n')
        header.write('}\n\n')

        header.write('#endif // VIRTUAL_FILE_SYSTEM_H\n')

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(f'Usage: {sys.argv[0]} <directory> <output_header>')
        sys.exit(1)

    directory = sys.argv[1]
    output_header = sys.argv[2]
    generate_vfs_header(directory, output_header)
    print(f'Generated {output_header} from {directory}')
