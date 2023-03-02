
# WebUI Library 2.x
# Python Example
#
# http://webui.me
# https://github.com/alifcommunity/webui
#
# Licensed under GNU General Public License v3.0.
# Copyright (C)2023 Hassan DRAGA <https://github.com/hassandraga>.

# Install WebUI
# pip install --upgrade webui2

from webui import webui

MyWindow = webui.window()
MyWindow.show('<html>Hello World</html>')
webui.wait()
print('Bye.')
