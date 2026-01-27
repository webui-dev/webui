# WebUI Library
# https://webui.me
# https://github.com/webui-dev/webui
# Copyright (c) 2020-2026 Hassan Draga.
# Licensed under MIT License.
# All rights reserved.
# Canada.
# 
# WebUI Virtual File System Generator
# v1.2.0

import os
import sys

def generate_vfs_header(directory, output_header):
    files = []

    # Walk through the directory and collect files
    for root, _, filenames in os.walk(directory):
        for filename in filenames:
            filepath = os.path.join(root, filename)
            relative_path = os.path.relpath(filepath, directory)
            # Ensure path starts with a slash
            relative_path = '/' + relative_path.replace('\\', '/')
            files.append((relative_path, filepath))

    # Generate the C header file
    with open(output_header, 'w') as header:
        header.write('#ifndef VIRTUAL_FILE_SYSTEM_H\n')
        header.write('#define VIRTUAL_FILE_SYSTEM_H\n\n')
        header.write('typedef struct {\n')
        header.write('    const char *path;\n')
        header.write('    const unsigned char *data;\n')
        header.write('    int length;\n')
        header.write('} VirtualFile;\n\n')

        for i, (relative_path, filepath) in enumerate(files):
            with open(filepath, 'rb') as f:
                data = f.read()
                header.write(f'static const unsigned char FILE_{i}[] = {{')
                header.write(','.join(f'0x{byte:02x}' for byte in data))
                header.write('};\n\n')

        header.write('\nstatic const VirtualFile virtual_files[] = {\n')

        for i, (relative_path, filepath) in enumerate(files):
            with open(filepath, 'rb') as f:
                data = f.read()
                header.write('    {\n')
                header.write(f'        "{relative_path}",\n')
                header.write(f'        FILE_{i},\n')
                header.write(f'        {len(data)}\n')
                header.write('    },\n')

        header.write('};\n\n')

        header.write('static const int virtual_files_count = sizeof(virtual_files) / sizeof(virtual_files[0]);\n\n')

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

        header.write('const void* vfs(const char* path, int* length) {\n')
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
        header.write('    }\n')
        header.write('    return NULL;\n')
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
