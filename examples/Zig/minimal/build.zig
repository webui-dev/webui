const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const webui = b.dependency("webui", .{});

    const exe = b.addExecutable(.{
        .name = "minimal",
        .root_source_file = .{ .path = "minimal.zig" },
        .target = target,
        .optimize = optimize,
    });

    exe.linkLibrary(webui.artifact("webui"));
    exe.install();

    const run = exe.run();
    run.step.dependOn(b.getInstallStep());

    const run_step = b.step("run", "Runs the minimal zig webui example");
    run_step.dependOn(&run.step);
}
