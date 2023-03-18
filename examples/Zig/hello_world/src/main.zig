const c = @import("c.zig");
const std = @import("std");

pub fn main() void {
    const my_html =
        \\ <!DOCTYPE html>
        \\ <html>
        \\     <head>
        \\         <title>WebUI 2 - C99 Example</title>
        \\         <style>
        \\         body{
        \\             color: white; background: #0F2027;
        \\             background: -webkit-linear-gradient(to right, #2C5364, #203A43, #0F2027);
        \\             background: linear-gradient(to right, #2C5364, #203A43, #0F2027);
        \\             text-align:center;
        \\             font-size: 18px;
        \\             font-family: sans-serif;
        \\         }
        \\         </style>
        \\     </head>
        \\     <body>
        \\         <h1>WebUI 2 - C99 Example</h1><br>
        \\         <input type="password" id="MyInput"><br><br>
        \\         <button id="MyButton1">Check Password</button> - <button id="MyButton2">Exit</button>
        \\     </body>
        \\ </html>;
    ;

    std.log.debug("Hello", .{});

    // Create a window
    var my_window: *c.webui_window_t = c.webui_new_window();

    // Bind HTML elements with functions
    _ = c.webui_bind(my_window, "MyButton1", check_the_password);
    _ = c.webui_bind(my_window, "MyButton2", close_the_application);

    // Show the window
    if (!c.webui_show(my_window, my_html, c.webui.browser.chrome)) {
        _ = c.webui_show(my_window, my_html, c.webui.browser.any);
    }

    // Wait until all windows get closed
    c.webui_wait();
}

export fn close_the_application(e: ?*c.webui_event_t) callconv(.C) void {
    _ = e;

    // Close all the opened windows
    c.webui_exit();
}

// check the password function
export fn check_the_password(e_opt: ?*c.webui_event_t) callconv(.C) void {
    const e = e_opt orelse {
        std.log.err("Passed an empty event: {}", .{@src()});
        return;
    };

    // This function gets called every time the user clicks on "MyButton1"
    var js = c.webui_script_t{
        .script = " return document.getElementById(\"MyInput\").value; ",
        .timeout = 3,
        .result = undefined,
    };

    // Run the JavaScript in the UI (Web Browser)
    c.webui_script(e.window, &js);

    // Check if there is a JavaScript error
    if (js.result.@"error") {
        std.log.err("JavaScript Error: {s}\n", .{js.result.data});
        return;
    }

    // Get the password
    const password = js.result.data;
    std.log.info("Password: {s}", .{password});

    // Check the password
    if (std.mem.eql(u8, password, "123456")) {
        // Correct password
        js.script = "alert('Good, password is correct.')";
        c.webui_script(e.window, &js);
    } else {
        // Wrong password
        js.script = "alert('Sorry, wrong password.')";
        c.webui_script(e.window, &js);
    }
}
