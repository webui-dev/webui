const c = @import("c.zig");
const std = @import("std");

pub fn main() void {
    const my_html =
        \\<!DOCTYPE html>
        \\<html>
        \\    <head>
        \\        <title>WebUI 2 - Zig Example</title>
        \\        <style>
        \\            body {
        \\                 color: white;
        \\                 background: #0F2027;
        \\                 background: -webkit-linear-gradient(to right, #2C5364, #203A43, #0F2027);
        \\                 background: linear-gradient(to right, #2C5364, #203A43, #0F2027);
        \\                 text-align:center; font-size: 18px; font-family: sans-serif;
        \\            }
        \\        </style>
        \\    </head>
        \\    <body>
        \\        <h2>WebUI 2 - C99 Example</h2>
        \\        <p>Call function with arguments (See log in the Windows console)</p><br>
        \\        <button OnClick="webui_fn('One', 'Hello');"">Call function one</button><br><br>
        \\        <button OnClick="webui_fn('Two', 2022);"">Call function two</button><br><br>
        \\        <button OnClick="webui_fn('Three', true);"">Call function three</button><br><br>
        \\        <p>Call function four, and wait for the result</p><br>
        \\        <button OnClick="MyJS();"">Call function four</button><br><br>
        \\        <input type="text" id="MyInput" value="2">
        \\        <script>
        \\            function MyJS() {
        \\                const number = document.getElementById('MyInput').value;
        \\                var result = webui_fn('Four', number);
        \\                document.getElementById('MyInput').value = result;
        \\            }
        \\        </script>
        \\    </body>
        \\</html>
    ;

    // Create a window
    var my_window: *c.webui_window_t = c.webui_new_window();

    // Bind HTML elements with functions
    _ = c.webui_bind(my_window, "One", function_one);
    _ = c.webui_bind(my_window, "Two", function_two);
    _ = c.webui_bind(my_window, "Three", function_three);
    _ = c.webui_bind(my_window, "Four", function_four);

    // Show the window
    c.webui_show(my_window, my_html);

    // Wait until all windows get closed
    c.webui_wait();
}

export fn function_one(e: ?*c.webui_event_t) callconv(.C) void {
    // JavaScript: webui_fn('One', 'Hello');
    const str = c.webui_get_string(e);
    std.debug.print("function_one: {s}\n", .{str});
}

export fn function_two(e: ?*c.webui_event_t) callconv(.C) void {
    // JavaScript: webui_fn('Two', 2022);
    const number = c.webui_get_int(e);
    std.debug.print("function_two: {}\n", .{number});
}

export fn function_three(e: ?*c.webui_event_t) callconv(.C) void {
    // JavaScript: webui_fn('Three', true);
    const status = c.webui_get_bool(e);
    if (status) {
        std.debug.print("function_three: True\n", .{});
    } else {
        std.debug.print("function_three: False\n", .{});
    }
}

export fn function_four(e: ?*c.webui_event_t) callconv(.C) void {
    // JavaScript: webui_fn('Four', 2);
    var number = c.webui_get_int(e);
    number = number * 2;
    std.debug.print("function_four: {}\n", .{number});

    c.webui_return_int(e, number);
}
