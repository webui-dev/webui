#!/bin/bash

# https://webui.me
# https://github.com/webui-dev/webui
# Copyright (c) 2020-2025 Hassan Draga.
# Licensed under MIT License.
# All rights reserved.
# Canada.
#
# Special Thanks to Turiiya (https://github.com/ttytm)

project_root=$(git rev-parse --show-toplevel)
cd $project_root/bridge

# Loop through all arguments
for opt in "$@"; do
	case $opt in
		--silent)
			silent=true
			;;
		*)
			echo "Invalid option: $opt"
			exit 0
			;;
	esac
	shift  # Move to the next argument
done

if [ "$silent" = true ]; then log_level=--log-level=warning; fi

if [ "$silent" != true ]; then echo "Transpile and bundle TS sources to webui.js"; fi
esbuild --bundle --target="chrome90,firefox90,safari15" --format=esm --tree-shaking=false --minify-syntax --minify-whitespace --outdir=. ./webui.ts $log_level

if [ "$silent" != true ]; then echo "Convert JS source to C header"; fi
python3 js2c.py

if [ "$silent" != true ]; then echo "Done."; fi
