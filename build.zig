const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const lib = b.addStaticLibrary(.{
        .name = "webui",
        .target = target,
        .optimize = optimize,
    });

    lib.addCSourceFiles(&.{
        "src/webui.c",
        "src/mongoose.c",
    }, &.{""});
    lib.linkLibC();
    lib.addIncludePath("include");
    lib.installHeadersDirectory("include", "");
    lib.install();

    if (target.isWindows()) {
        lib.linkSystemLibrary("Ws2_32");
    }
}
