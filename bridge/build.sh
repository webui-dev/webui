#!/bin/bash

# https://webui.me
# https://github.com/webui-dev/webui
# Copyright (c) 2020-2026 Hassan Draga.
# Licensed under MIT License.
# All rights reserved.
# Canada.
#
# Special Thanks to Turiiya (https://github.com/ttytm)

project_root=$(git rev-parse --show-toplevel)
cd $project_root/bridge

echo "Transpile and bundle TS sources to webui.js"
esbuild --bundle --target="chrome90,firefox90,safari15" --format=esm --tree-shaking=false --minify-syntax --minify-whitespace --outdir=. ./webui.ts

echo "Convert JS source to C header"
node js2c.js

echo "Done."
