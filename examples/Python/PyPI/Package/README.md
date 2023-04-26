# WebUI

Use any web browser as GUI, with Python in the backend and HTML5 in the frontend, all in a lightweight portable lib.

* [Online Documentation](https://webui.me/docs/#/python_api)

```sh
pip install webui2
```

```python
from webui import webui

def my_function(e : webui.event)
    print("Hi!, You clicked on " + e.element + " element")

MyWindow = webui.window()
MyWindow.bind("MyID", my_function)
MyWindow.show("<html>Hello World</html>")
webui.wait()
```

```sh
python test.py
```

![ScreenShot](https://raw.githubusercontent.com/alifcommunity/webui/main/screenshot.png)
