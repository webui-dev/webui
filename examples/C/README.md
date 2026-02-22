# WebUI C Examples

Examples of how to create a WebUI application in C.

## C Examples

The only requirement to build the examples is a C compiler.

- `minimal`: Creates a minimal WebUI application.
- `call_c_from_js`: Calls C from JavaScript.
- `call_js_from_c`: Calls JavaScript from C.
- `serve_a_folder`: Use WebUI to serve a folder that contains `.html`, `.css`, `.js`, or `.ts` files.
- `custom_web_server`: Use your preferred web server like NGINX/Apache... with WebUI to serve a folder.
- `chatgpt_api`: Calls ChatGPT API from C.
- `frameless`: Frameless window example.
- `public_network_access`: Allows network access to external devices.
- `react`: React + WebUI example.
- `virtual_file_system`: Embeds files using a virtual file system.
- `web_app_multi_client`: Multiple clients example.

To build an example, cd into its directory and run the make command.

- **Windows**

  ```sh
  # GCC
  mingw32-make

  # MSVC
  nmake
  ```

- **Linux**

  ```sh
  # GCC
  make

  # Clang
  make CC=clang
  ```

- **macOS**
  ```sh
  make
  ```

## Additional Examples

- [`text-editor`](https://github.com/webui-dev/webui/tree/main/examples/C/text-editor): A lightweight and portable text editor written in C using WebUI as the GUI library.
