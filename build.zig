//! Note: This file is just for 0.11 zig!
//! For 0.12 and later, please see zig-webui!
const std = @import("std");
const builtin = @import("builtin");

const Build = std.Build;
const OptimizeMode = std.builtin.OptimizeMode;
const CrossTarget = std.zig.CrossTarget;
const Compile = Build.Step.Compile;
const Module = Build.Module;

const log = std.log.scoped(.WebUI);
const lib_name = "webui";

pub fn build(b: *Build) void {
    if (builtin.zig_version.minor > 11) {
        log.err("unsupported Zig version. For building with 0.12 and later, plaese see `github.com/webui-dev/zig-webui`", .{});
        std.process.exit(1);
    }

    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const is_dynamic = b.option(bool, "dynamic", "build the dynamic library") orelse false;
    const enable_tls = b.option(bool, "enable-tls", "enable TLS support") orelse false;

    if (enable_tls and !target.isNative()) {
        log.err("cross compilation is not supported with TLS enabled", .{});
        std.os.exit(1);
    }

    std.debug.print("Building {s} WebUI library{s}...\n", .{
        if (is_dynamic) "dynamic" else "static",
        if (enable_tls) " with TLS support" else "",
    });

    const lib = build_lib(b, optimize, target, is_dynamic, enable_tls) catch |err| {
        log.err("failed to build webui library: {}", .{err});
        std.os.exit(1);
    };
    build_examples(b, optimize, target, lib) catch |err| {
        log.err("failed to build examples: {}", .{err});
        std.os.exit(1);
    };
}

fn build_lib(b: *Build, optimize: OptimizeMode, target: CrossTarget, is_dynamic: bool, enable_tls: bool) !*Compile {
    // Prepare compiler flags.
    const tls_flags = &[_][]const u8{ "-DWEBUI_TLS", "-DNO_SSL_DL", "-DOPENSSL_API_1_1" };

    var civetweb_flags = std.ArrayList([]const u8).init(std.heap.page_allocator);
    defer civetweb_flags.deinit();
    try civetweb_flags.appendSlice(&[_][]const u8{ "-DNDEBUG", "-DNO_CACHING", "-DNO_CGI", "-DUSE_WEBSOCKET" });
    try civetweb_flags.appendSlice(if (enable_tls) tls_flags else &.{ "-DUSE_WEBSOCKET", "-DNO_SSL" });
    if (target.isWindows()) try civetweb_flags.append("-DMUST_IMPLEMENT_CLOCK_GETTIME");

    const webui = if (is_dynamic) b.addSharedLibrary(.{
        .name = lib_name,
        .target = target,
        .optimize = optimize,
    }) else b.addStaticLibrary(.{
        .name = lib_name,
        .target = target,
        .optimize = optimize,
    });

    webui.addCSourceFile(.{
        .file = .{ .path = "src/webui.c" },
        .flags = if (enable_tls) tls_flags else &[_][]const u8{"-DNO_SSL"},
    });
    webui.addCSourceFile(.{
        .file = .{ .path = "src/civetweb/civetweb.c" },
        .flags = civetweb_flags.items,
    });
    webui.linkLibC();
    webui.addIncludePath(.{ .path = "include" });
    webui.installHeader("include/webui.h", "webui.h");

    if (target.isDarwin()) {
        webui.addCSourceFile(.{
            .file = .{ .path = "src/webview/wkwebview.m" },
            .flags = &.{},
        });
        webui.linkFramework("Cocoa");
        webui.linkFramework("WebKit");
    } else if (target.isWindows()) {
        webui.linkSystemLibrary("ws2_32");
        webui.linkSystemLibrary("Ole32");
        if (target.abi == .msvc) {
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

    b.installArtifact(webui);
    return webui;
}

fn build_examples(b: *Build, optimize: OptimizeMode, target: CrossTarget, webui_lib: *Compile) !void {
    const build_all_step = b.step("examples", "builds the library and its examples");

    const examples_path = (Build.LazyPath{ .path = "examples/C" }).getPath(b);
    var examples_dir = try std.fs.openIterableDirAbsolute(examples_path, .{});
    defer examples_dir.close();

    var paths = examples_dir.iterate();
    while (try paths.next()) |val| {
        if (val.kind != .directory) {
            continue;
        }

        const example_name = val.name;
        const exe = b.addExecutable(.{ .name = example_name, .target = target, .optimize = optimize });
        const path = try std.fmt.allocPrint(b.allocator, "examples/C/{s}/main.c", .{example_name});

        exe.addCSourceFile(.{ .file = .{ .path = path }, .flags = &.{} });
        exe.linkLibrary(webui_lib);

        const exe_install = b.addInstallArtifact(exe, .{});
        const exe_run = b.addRunArtifact(exe);

        build_all_step.dependOn(&exe_install.step);
        exe_run.step.dependOn(&exe_install.step);
        exe_run.cwd = try std.fmt.allocPrint(b.allocator, "{s}/{s}", .{ examples_path, example_name });

        const step_name = try std.fmt.allocPrint(b.allocator, "run_{s}", .{example_name});
        const step_desc = try std.fmt.allocPrint(b.allocator, "run example {s}", .{example_name});
        b.step(step_name, step_desc).dependOn(&exe_run.step);
    }
}
