//! Note: This file is just for 0.11 zig!
//! For 0.12 and later, please see zig-webui!
const std = @import("std");
const builtin = @import("builtin");

const Build = std.Build;
const OptimizeMode = std.builtin.OptimizeMode;
const Compile = Build.Step.Compile;
const Module = Build.Module;

const log = std.log.scoped(.WebUI);
const lib_name = "webui";
const zig_ver = builtin.zig_version.minor;

pub fn build(b: *Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const is_dynamic = b.option(bool, "dynamic", "build the dynamic library") orelse false;
    const enable_tls = b.option(bool, "enable-tls", "enable TLS support") orelse false;
    const verbose = b.option(bool, "verbose", "enable verbose output") orelse false;

    switch (zig_ver) {
        11 => {
            if (enable_tls and !target.isNative()) {
                log.err("cross compilation is not supported with TLS enabled", .{});
                std.os.exit(1);
            }
        },
        12, 13 => {
            if (enable_tls and !target.query.isNative()) {
                log.err("cross compilation is not supported with TLS enabled", .{});
                std.process.exit(1);
            }
        },
        else => @compileError("unsupported Zig version!"),
    }

    if (verbose) {
        std.debug.print("Building {s} WebUI library{s}...\n", .{
            if (is_dynamic) "dynamic" else "static",
            if (enable_tls) " with TLS support" else "",
        });
        defer std.debug.print("Done.\n", .{});
    }

    const webui = if (is_dynamic) b.addSharedLibrary(.{
        .name = lib_name,
        .target = target,
        .optimize = optimize,
    }) else b.addStaticLibrary(.{
        .name = lib_name,
        .target = target,
        .optimize = optimize,
    });
    try addLinkerFlags(b, webui, enable_tls);

    b.installArtifact(webui);

    build_examples(b, webui) catch |err| {
        log.err("failed to build examples: {}", .{err});
        if (zig_ver < 12) std.os.exit(1) else std.process.exit(1);
    };
}

fn addLinkerFlags(b: *Build, webui: *Compile, enable_tls: bool) !void {
    const webui_target = if (zig_ver < 12) webui.target else webui.rootModuleTarget();
    const is_windows = if (zig_ver < 12) webui_target.isWindows() else webui_target.os.tag == .windows;

    // Prepare compiler flags.
    const tls_flags = &[_][]const u8{ "-DWEBUI_TLS", "-DNO_SSL_DL", "-DOPENSSL_API_1_1" };
    var civetweb_flags = std.ArrayList([]const u8).init(std.heap.page_allocator);
    defer civetweb_flags.deinit();
    try civetweb_flags.appendSlice(&[_][]const u8{ "-DNDEBUG", "-DNO_CACHING", "-DNO_CGI", "-DUSE_WEBSOCKET" });
    try civetweb_flags.appendSlice(if (enable_tls) tls_flags else &[_][]const u8{ "-DUSE_WEBSOCKET", "-DNO_SSL" });
    if (is_windows) try civetweb_flags.append("-DMUST_IMPLEMENT_CLOCK_GETTIME");

    webui.addCSourceFile(.{
        .file = if (zig_ver < 12) .{ .path = "src/webui.c" } else b.path("src/webui.c"),
        .flags = if (enable_tls) tls_flags else &[_][]const u8{"-DNO_SSL"},
    });
    webui.addCSourceFile(.{
        .file = if (zig_ver < 12) .{ .path = "src/civetweb/civetweb.c" } else b.path("src/civetweb/civetweb.c"),
        .flags = civetweb_flags.items,
    });
    webui.linkLibC();
    webui.addIncludePath(if (zig_ver < 12) .{ .path = "include" } else b.path("include"));
    if (zig_ver < 12) {
        webui.installHeader("include/webui.h", "webui.h");
    } else {
        webui.installHeader(b.path("include/webui.h"), "webui.h");
    }
    if (webui_target.isDarwin()) {
        webui.addCSourceFile(.{
            .file = if (zig_ver < 12) .{ .path = "src/webview/wkwebview.m" } else b.path("src/webview/wkwebview.m"),
            .flags = &.{},
        });
        webui.linkFramework("Cocoa");
        webui.linkFramework("WebKit");
    } else if (is_windows) {
        webui.linkSystemLibrary("ws2_32");
        webui.linkSystemLibrary("Ole32");
        if (webui_target.abi == .msvc) {
            webui.linkSystemLibrary("Advapi32");
            webui.linkSystemLibrary("Shell32");
            webui.linkSystemLibrary("user32");
        }
        if (enable_tls) {
            webui.linkSystemLibrary("bcrypt");
        }
    }
    if (enable_tls) {
        webui.linkSystemLibrary("ssl");
        webui.linkSystemLibrary("crypto");
    }
}

fn build_examples(b: *Build, webui: *Compile) !void {
    const build_examples_step = b.step("examples", "builds the library and its examples");
    const target = if (zig_ver < 12) webui.target else webui.root_module.resolved_target.?;
    const optimize = if (zig_ver < 12) webui.optimize else webui.root_module.optimize.?;

    const examples_path = (if (zig_ver < 12) (Build.LazyPath{ .path = "examples/C" }) else b.path("examples/C")).getPath(b);
    var examples_dir = if (zig_ver < 12)
        try std.fs.openIterableDirAbsolute(examples_path, .{})
    else
        try std.fs.openDirAbsolute(examples_path, .{ .iterate = true });
    defer examples_dir.close();

    var paths = examples_dir.iterate();
    while (try paths.next()) |val| {
        if (val.kind != .directory) {
            continue;
        }

        const example_name = val.name;
        const exe = b.addExecutable(.{ .name = example_name, .target = target, .optimize = optimize });
        const path = try std.fmt.allocPrint(b.allocator, "examples/C/{s}/main.c", .{example_name});

        exe.addCSourceFile(.{ .file = if (zig_ver < 12) .{ .path = path } else b.path(path), .flags = &.{} });
        exe.linkLibrary(webui);

        const exe_install = b.addInstallArtifact(exe, .{});
        const exe_run = b.addRunArtifact(exe);
        const step_name = try std.fmt.allocPrint(b.allocator, "run_{s}", .{example_name});
        const step_desc = try std.fmt.allocPrint(b.allocator, "run example {s}", .{example_name});

        const cwd = try std.fmt.allocPrint(b.allocator, "src/examples/{s}", .{example_name});
        if (zig_ver < 12) exe_run.cwd = cwd else exe_run.setCwd(b.path(cwd));

        exe_run.step.dependOn(&exe_install.step);
        build_examples_step.dependOn(&exe_install.step);
        b.step(step_name, step_desc).dependOn(&exe_run.step);
    }
}
