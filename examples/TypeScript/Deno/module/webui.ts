

import { existsSync } from "https://deno.land/std/fs/mod.ts";
// import { readCString } from "https://deno.land/std/c/strings/mod.ts";

const version = '2.1.1';
const encoder = new TextEncoder();
const decoder = new TextDecoder();
let lib_loaded = false;
let webui_lib;
let fun_list:number[][];

export const browser = {};
browser.any = 0;
browser.chrome = 1;
browser.firefox = 2;
browser.edge = 3;
browser.safari = 4;
browser.chromium = 5;

const sleep = (ms: number) => new Promise((r) => setTimeout(r, ms));

export interface event {
  element_id: number;
  window_id: number;
  element_name_ptr: string;
  win: Deno.Pointer;
  data: string;
}

export interface js {
  Timeout: number;
	Script:  string;
	Error:   boolean;
	Length:  number;
	Data:  string;
}

// Determine the library name based
// on the current operating system
let lib_name : string;
let os_sep : string;
if (Deno.build.os === 'windows') {
  lib_name = 'webui-2-x64.dll';
  os_sep = '\\';
}
else if (Deno.build.os === 'linux') {
  lib_name = 'webui-2-x64.so';
  os_sep = '/';
}
else {
  lib_name = 'webui-2-x64.dyn';
  os_sep = '/';
}

// Full path to the library name
let lib_path = './' + lib_name;

// Check if a file exist
function is_file_exist(path : string): boolean {
  // TODO: existsSync() is deprecated
  return existsSync(path);
}

// Convert String to C-String
function string_to_uint8array(value: string) : Uint8Array {
  return encoder.encode(value + '\0');
}

// Get current folder path
function get_current_module_path() : string {
  const __dirname = new URL('.', import.meta.url).pathname;
  let directory = String(__dirname);
  if (Deno.build.os === 'windows') {
    // Remove '/'
    let buf = directory.substring(1);
    directory = buf;
    // Replace '/' by '\'
    buf = directory.replaceAll('/', os_sep);
    directory = buf;
  }
  return directory;
}

// Convert C-String to String
function uint8array_to_string(value: ArrayBuffer) : string {
  return decoder.decode(value);
}

// Load the library
function load_lib() {
  if(lib_loaded)
    return;
  
  // Check if the library file exist
  if(!is_file_exist(lib_path)) {
    let lib_path_cwd = get_current_module_path() + lib_name;
    if(!is_file_exist(lib_path_cwd)) {
      console.log('WebUI Error: File not found (' + lib_path + ') or (' + lib_path_cwd + ')');
      Deno.exit(1);
    }
    lib_path = lib_path_cwd;
  }

  console.log('Load the library = [' + lib_path + ']');

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
      webui_open: {
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
      webui_script_interface: {
        parameters: ['pointer', 'buffer', 'u32', 'buffer', 'buffer', 'buffer'],
        result: 'void',
        nonblocking: false,
      },
      webui_clean_mem: {
        parameters: ['pointer'],
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

export function open(win, url, browser) : number {
  load_lib();
  return webui_lib.symbols.webui_open(win, string_to_uint8array(url), browser);
}

export function exit() {
  load_lib();
  webui_lib.symbols.webui_exit();
  Deno.exit(0);
}

export async function wait() {
  load_lib();
  while(true) {
    await sleep(250);
    if(!webui_lib.symbols.webui_is_app_running())
      break;
  }
}

export function run(win : number, javascript : js) {
  load_lib();

  // Buffers
  const bool_buffer = new Uint8Array(1); // 1 byte bool
  const integer_buffer = new Uint8Array(4); // 4 bytes integer
  const pointer_buffer = new Uint8Array(8); // 8 bytes pointer (x64)

  // Execute the script
  webui_lib.symbols.webui_script_interface(win, string_to_uint8array(javascript.Script), javascript.Timeout, bool_buffer, integer_buffer, pointer_buffer);

  let error : Boolean = false;
  let length : Number = 0;
  let data : String = "";

  // Resolve boolean
  if(bool_buffer[0] === 0)
    error = false;
  else
    error = true;

  // Resolve number
  length = Number(new Uint32Array(integer_buffer.buffer)[0]);

  // Resolve string
  const charPointer = Deno.UnsafePointer.create(BigInt(new BigUint64Array(pointer_buffer.buffer)[0]));
  data = String(Deno.UnsafePointerView.getCString(charPointer, 0));

  // Update
  javascript.Error = error;
  javascript.Length = length;
  javascript.Data = data;

  // Clean memory
  webui_lib.symbols.webui_clean_mem(charPointer);
}

export function bind(win : number, element : string, func : Function) : number {
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
      param_response: Deno.PointerValue,
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

      // Call the user callback
      const result = String(func(e));

      // -- This code is by ChatGPT (AI) --------
      const resultBytes = encoder.encode(result); // Convert the string to bytes
      const buf = new Uint8Array(resultBytes.length + 1); // Allocate a buffer to hold the string
      buf.set(resultBytes); // Copy the bytes of the string into the buffer
      buf[resultBytes.length] = 0; // Set the null terminator
      // ----------------------------------------

      // 8 bytes ie. pointer size on a 64 bit machine, use BigUin64Array to get a writable and suitable view into it.
      const response_writable = new BigUint64Array(Deno.UnsafePointerView.getArrayBuffer(param_response, 8));

      // Write our string response buffer address into the `param_response` memory slot.
      response_writable[0] = BigInt(Deno.UnsafePointer.value(Deno.UnsafePointer.of(buf)));
    },
  );

	webui_lib.symbols.webui_bind_interface(win, string_to_uint8array(element), callbackResource.pointer);
  return 0;
}
