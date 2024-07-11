# WebUI Bridge

The WebUI Bridge connects the UI (_Web Browser_) with the backend application through WebSocket. This bridge is written in TypeScript, and it needs to be transpiled to JavaScript using [ESBuild](https://esbuild.github.io/) to produce `webui.js`, then converted to C header using the Python script `js2c.py` to generate `webui_bridge.h`.

### Windows

- Install [Python](https://www.python.org/downloads/)
- Install [Node.js](https://nodejs.org/en/download)
- cd `webui\bridge`
- `npm install esbuild`
- `.\node_modules\.bin\esbuild --bundle --target="chrome90,firefox90,safari15" --format=esm --tree-shaking=false --outdir=.\ .\webui.ts`
- `python js2c.py`

### Windows PowerShell

- cd `webui\bridge`
- `./build.ps1`
- If you get _running scripts is disabled on this
  system_ error. Then run `Set-ExecutionPolicy RemoteSigned` to enable script execution. After done, you can roll back by running `Set-ExecutionPolicy Restricted`

### Linux

- Install [Python](https://www.python.org/downloads/)
- Install [Node.js](https://nodejs.org/en/download)
- cd `webui/bridge`
- `npm install esbuild`
- `./node_modules/.bin/esbuild --bundle --target="chrome90,firefox90,safari15" --format=esm --tree-shaking=false --outdir=./ ./webui.ts`
- `python js2c.py`

### Linux Bash

- cd `webui\bridge`
- sh `./build.sh`

### macOS

- Install [Python](https://www.python.org/downloads/)
- Install [Node.js](https://nodejs.org/en/download)
- cd `webui/bridge`
- `npm install esbuild`
- `./node_modules/.bin/esbuild --bundle --target="chrome90,firefox90,safari15" --format=esm --tree-shaking=false --outdir=./ ./webui.ts`
- `python js2c.py`

### macOS Bash

- cd `webui\bridge`
- sh `./build.sh`
