
# WebUI Examples - Python

To use WebUI in your Python script, you will need to install it using pip, `pip install --upgrade webui2`. The source code of the Python WebUI module is [here](https://github.com/alifcommunity/webui/tree/main/packages/PyPI/src/webui)

```sh
pip install --upgrade webui2
```

```python
from webui import webui
MyWindow = webui.window()
MyWindow.bind('MyID', my_function)
MyWindow.show("MyHTML")
webui.loop()
```

```sh
python test.py
```
