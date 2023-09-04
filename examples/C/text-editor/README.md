# WebUI C - Text Editor

This [text editor example](https://github.com/webui-dev/webui/tree/main/examples/C/text-editor) is written in C using WebUI as the GUI library. The final executable is portable and has less than _1 MB_ in size (_+html and css files_).

![webui_c_example](https://github.com/ttytm/webui/assets/34311583/7c3e1f1f-e9a3-4ad9-988a-c0baa3df0a00)

- **Windows**

```sh
# MinGW
git clone https://github.com/webui-dev/webui.git
cd webui\examples\C\text-editor
gcc -o text-editor.exe text-editor.c webui-2.dll -Wl,-subsystem=windows -lcomdlg32
```
