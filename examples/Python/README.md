
# WebUI Examples - Python

[![PyPI version](https://img.shields.io/pypi/v/webui2?style=for-the-badge)](https://pypi.org/project/webui2/)

* **Minimal**: The minimal code to use WebUI
* **Hello World**: An example of how  to use WebUI & JavaScript
* **Dev**: A test script to use the local WebUI module instead of the installed one. It's for debugging & development of the WebUI purpose only.

```sh
pip install webui2
```

```python
from webui import webui
MyWindow = webui.window()
MyWindow.bind('MyID', my_function)
MyWindow.show("MyHTML")
webui.wait()
```

```sh
python test.py
```
