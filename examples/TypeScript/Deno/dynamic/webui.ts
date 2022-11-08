// WebUI Library 2.0.4
//
// http://webui.me
// https://github.com/alifcommunity/webui
//
// Licensed under GNU General Public License v3.0.
// Copyright (C)2022 Hassan DRAGA <https://github.com/hassandraga>.

import { existsSync } from "https://deno.land/std/fs/mod.ts";

const version = '2.0.4';
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
}

// Determine the library name based
// on the current OS
var lib_name;
if (Deno.build.os === 'windows')
    lib_name = 'webui-2-x64x.dll';
if (Deno.build.os === 'linux')
    lib_name = 'webui-2-x64x.so';
else
    lib_name = 'webui-2-x64x.dyn';

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
        await sleep(100);
        if(!webui_lib.symbols.webui_is_app_running())
            break;
    }
}

export function bind(win, element, func) : number {
    load_lib();
    const callbackResource = new Deno.UnsafeCallback(
        {
            parameters: ['u32', 'u32', 'pointer', 'pointer'],
            result: 'void',
        } as const,
        (
            element_id: Deno.u32,
            window_id: Deno.u32,
            element_name_ptr: Deno.PointerValue,
            win: Deno.Pointer,
        ) => {
            const element_name = new Deno.UnsafePointerView(BigInt(element_name_ptr)).getCString();
            const e: event = {
                element_id: element_id,
                window_id: window_id,
                element_name: element_name,
                win: win,
            };
            func(e);
        },
    );
	webui_lib.symbols.webui_bind_interface(win, string_to_uint8array(element), callbackResource.pointer);
    return 0;
}
