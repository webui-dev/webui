# WebUI

Use any web browser as GUI, with your preferred language in the backend, and HTML/JS/TS/CSS in the frontend.

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

![ScreenShot](https://raw.githubusercontent.com/alifcommunity/webui/main/screenshot.png)
