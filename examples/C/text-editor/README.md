
# WebUI C - Text Editor (Not Complete)

A text editor application in C using WebUI.

### Windows

- **MinGW**
```sh
git clone https://github.com/alifcommunity/webui.git
cd webui\examples\C\text-editor
gcc -o text-editor.exe text-editor.c webui-2-x64.dll -Wl,-subsystem=windows -lcomdlg32
```
