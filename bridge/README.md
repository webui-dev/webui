
# WebUI Bridge

The WebUI Bridge connects the UI (_Web Browser_) with the backend application through WebSocket. This bridge is written in TypeScript, and it needs to be transpiled to JavaScript using [ESBuild](https://esbuild.github.io/) to produce `webui_bridge.js`, then converted to C99 header using the Python script `js2c.py` to generate `webui_bridge.h`.

### Windows

- Install [Python](https://www.python.org/downloads/)
- Install [Node.js](https://nodejs.org/en/download)
- cd `webui\bridge`
- Run `npm install esbuild`
- Run `.\node_modules\.bin\esbuild --bundle --target="chrome90,firefox90,safari15" --format=esm --tree-shaking=false --outdir=.\ .\webui_bridge.ts`
- Run `python js2c.py`

### Linux

- Install [Python](https://www.python.org/downloads/)
- Install [Node.js](https://nodejs.org/en/download)
- cd `webui/bridge`
- Run `npm install esbuild`
- Run `./node_modules/.bin/esbuild --bundle --target="chrome90,firefox90,safari15" --format=esm --tree-shaking=false --outdir=./ ./webui_bridge.ts`
- Run `python js2c.py`

### macOS

- Install [Python](https://www.python.org/downloads/)
- Install [Node.js](https://nodejs.org/en/download)
- cd `webui/bridge`
- Run `npm install esbuild`
- Run `./node_modules/.bin/esbuild --bundle --target="chrome90,firefox90,safari15" --format=esm --tree-shaking=false --outdir=./ ./webui_bridge.ts`
- Run `python js2c.py`
