// WebUI Library 2.2.0
// Serve a Folder Example

pub fn main() void {
    // Create a window
    var my_window: *c.webui_window_t = c.webui_new_window();

    // Bind HTML element ID to c function
    _ = c.webui_bind(my_window, "SwitchToSecondPage", switch_to_second_page);
    _ = c.webui_bind(my_window, "Exit", exit_app);

    // The root path. Leave it empty to let WebUI
    // automatically select the current working folder
    const root_path = "";

    const url = c.webui_new_server(my_window, root_path);

    // Show the window using the generated url
    c.webui_show(my_window, url);

    // Wait until all windows get closed
    c.webui_wait();
}

// Called every time the user clicks on the SwitchToSecondPage button
export fn switch_to_second_page(e_opt: ?*c.webui_event_t) callconv(.C) void {
    const e = e_opt orelse {
        std.log.err("switch_to_second_page called with null event", .{});
        return;
    };
    _ = c.webui_open(e.window, "second.html", c.webui.browser.any);
}

export fn exit_app(e: ?*c.webui_event_t) callconv(.C) void {
    _ = e;
    c.webui_exit();
}

const std = @import("std");
const c = @import("c.zig");
