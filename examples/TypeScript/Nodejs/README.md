
# WebUI Examples - Nodejs

Unfortunately, Nodejs does not support FFI natively (only trough unofficial addon). We recommend using Deno instead.


The only way to use Nodejs with WebUI is to create a simple app in another language like C, C++, Nim, Go, Python, V, Zig, Rust... and use `webui_set_runtime()` to interpret the Nodejs files.
