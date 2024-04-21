const std = @import("std");
const builtin = @import("builtin");

const Build = std.Build;
const OptimizeMode = std.builtin.OptimizeMode;
const CrossTarget = std.zig.CrossTarget;
const Compile = Build.Step.Compile;
const Module = Build.Module;

const log = std.log.scoped(.WebUI);

const build_11 = @import("build_11.zig").build_11;
const build_12 = @import("build_12.zig").build_12;
const build_13 = @import("build_13.zig").build_13;

const min_zig_string = "0.11.0";

const default_isStatic = true;
const default_enableTLS = false;

const current_zig = builtin.zig_version;

comptime {
    const min_zig = std.SemanticVersion.parse(min_zig_string) catch unreachable;
    if (current_zig.order(min_zig) == .lt) {
        @compileError(std.fmt.comptimePrint("Your Zig version v{} does not meet the minimum build requirement of v{}", .{ current_zig, min_zig }));
    }
}

pub fn build(b: *std.Build) void {
    switch (current_zig.minor) {
        11 => build_11(b),
        12 => build_12(b),
        13 => build_13(b),
        else => @compileError("uknown zig version!"),
    }
}
