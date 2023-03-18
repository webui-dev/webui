const c = @import("c.zig");
const std = @import("std");

pub fn main() !void {
    var my_window: *c.webui_window_t = c.webui_new_window();
    if (!c.webui_show(my_window, "<html>Hello</html>", c.webui.browser.any)) {
        std.log.err("Unable to show webui", .{});
        return error.ShowWebui;
    }
    c.webui_wait();
}
