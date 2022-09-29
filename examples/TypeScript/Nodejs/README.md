
# WebUI Examples - TypeScript / JavaScript

WebUI can use [Deno](https://deno.land/) or [Node.js](https://nodejs.org/) runtimes to parse any `JavaScript` or `TypeScript` files.

## Deno

 1. Download and [Install Deno](https://github.com/denoland/deno/releases) (*Or just copy deno binary file into this folder*)
 2. Build WebUI Library (*[instructions](https://github.com/alifcommunity/webui/tree/main/build)*)
 3. Build `example.c` using any C compiler (*[instructions](https://github.com/alifcommunity/webui/tree/main/examples/C)*)

Folder structure example (*Windows*)

    [My Folder]
	    * Deno.exe (if not installed)
	    * Example.exe
	    * index.html
	    * style.css
	    * file1.ts
	    * file2.js
	    * ...

## Node.js

 1. Download and [Install Node.js](https://nodejs.org/en/download/)
 2. Build WebUI Library (*[instructions](https://github.com/alifcommunity/webui/tree/main/build)*)
 3. Edit `example.c` and **change** `webui.runtime.deno` to `webui.runtime.nodejs`
 4. Build `example.c` using any C compiler (*[instructions](https://github.com/alifcommunity/webui/tree/main/examples/C)*)

Folder structure example (*Windows*)

    [My Folder]
	    * Example.exe
	    * index.html
	    * style.css
	    * file1.js
	    * file2.js
	    * ...
