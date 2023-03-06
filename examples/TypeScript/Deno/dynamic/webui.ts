// WebUI Library 2.0.6
//
// http://webui.me
// https://github.com/alifcommunity/webui
//
// Licensed under GNU General Public License v3.0.
// Copyright (C)2023 Hassan DRAGA <https://github.com/hassandraga>.

import { existsSync } from "https://deno.land/std/fs/mod.ts";
//import { readCString } from "https://deno.land/std/c/strings/mod.ts";

const version = '2.0.6';
const encoder = new TextEncoder();
const decoder = new TextDecoder();
let lib_loaded = false;
var webui_lib;
export const browser = {};
browser.any = 0;
browser.chrome = 1;
browser.firefox = 2;
browser.edge = 3;
const sleep = (ms: number) => new Promise((r) => setTimeout(r, ms));
var fun_list:number[][]
interface event {
    element_id: number;
    window_id: number;
    element_name_ptr: string;
    win: Deno.Pointer;
    data: string;
}

// Determine the library name based
// on the current OS
var lib_name;
if (Deno.build.os === 'windows')
    lib_name = 'webui-2-x64.dll';
else if (Deno.build.os === 'linux')
    lib_name = 'webui-2-x64.so';
else
    lib_name = 'webui-2-x64.dyn';

// Full path to the library name
var lib_path = './' + lib_name;

// Check if a file exist
function is_file_exist(path): boolean {
    return existsSync(path);
}

// Convert String to C-String
function string_to_uint8array(value: string) {
    return encoder.encode(value + '\0');
}

// Convert C-String to String
function uint8array_to_string(value: ArrayBuffer) {
    return decoder.decode(value);
}

// Load the library
function load_lib() {
    if(lib_loaded)
        return;
    // Check if the library file exist
    if(!is_file_exist(lib_path)) {
        console.log('WebUI Error: File not found (' + lib_path + ')');
        Deno.exit(1);
    }

    // Load the library
    // FFI
    webui_lib = Deno.dlopen(
        lib_path,
        {
            webui_wait: {
                parameters: [],
                result: 'void',
                nonblocking: false,
            },
            webui_new_window: {
                parameters: [],
                result: 'pointer',
                nonblocking: false,
            },
            webui_show: {
                parameters: ['pointer', 'buffer', 'u32'],
                result: 'i32',
                nonblocking: false,
            },
            webui_bind_interface: {
                parameters: ['pointer', 'buffer', 'function'],
                result: 'u32',
                nonblocking: false,
            },
            webui_is_app_running: {
                parameters: [],
                result: 'i32',
                nonblocking: false,
            },
            webui_exit: {
                parameters: [],
                result: 'void',
                nonblocking: false,
            },
        } as const,
    );

    // Make sure we don't load twice
    lib_loaded = true;
}

export function set_lib_path(path) {
	lib_path = path;
}

export function new_window() : number {
    load_lib();
	return webui_lib.symbols.webui_new_window();
}

export function show(win, html, browser) : number {
    load_lib();
    return webui_lib.symbols.webui_show(win, string_to_uint8array(html), browser);
}

export function exit() {
    load_lib();
    webui_lib.symbols.webui_exit();
}

export async function wait() {
    load_lib();
    while(true) {
        await sleep(250);
        if(!webui_lib.symbols.webui_is_app_running())
            break;
    }
}

export function bind(win, element, func) : number {
    load_lib();
    const callbackResource = new Deno.UnsafeCallback(
        {
            parameters: ['u32', 'u32', 'pointer', 'pointer', 'pointer', 'pointer'],
            result: 'void',
        } as const,
        (
            param_element_id: Deno.u32,
            param_window_id: Deno.u32,
            param_element_name_ptr: Deno.Pointer,
            param_win: Deno.Pointer,
            param_data: Deno.Pointer,
            param_response: Deno.Pointer,
        ) => {

            // Create elements
            const element_id = parseInt(param_element_id);
            const window_id = parseInt(param_window_id);
            const element_name = new Deno.UnsafePointerView(param_element_name_ptr).getCString();
            const win = param_win;
            const data = new Deno.UnsafePointerView(param_data).getCString();

            // Create struct
            const e: event = {
                element_id: element_id,
                window_id: window_id,
                element_name: element_name,
                win: win,
                data: data,
            };

            // Call callback
            const result = String(func(e));

            // TODO -----------------------------------------------------------------------
            //
            // `result`         : The callback result in string
            // `param_response` : It's a non initialized (null) `char**`
            //
            // Now, we should create a non-garbage colleted buffer, fill it with `result`
            // and set it to `param_response`.

            // Create A Buffer
            // This code is by ChatGPT (AI)
            const encoder = new TextEncoder(); // Create a TextEncoder instance
            const resultBytes = encoder.encode(result); // Convert the string to bytes
            const buf = new Uint8Array(resultBytes.length + 1); // Allocate a buffer to hold the string
            buf.set(resultBytes); // Copy the bytes of the string into the buffer
            buf[resultBytes.length] = 0; // Set the null terminator

            // Set our buffer `buf` to `char**`
            param_response = buf;
        },
    );

	webui_lib.symbols.webui_bind_interface(win, string_to_uint8array(element), callbackResource.pointer);
    return 0;
}
