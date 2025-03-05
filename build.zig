const std = @import("std");
const builtin = @import("builtin");

const Build = std.Build;
const OptimizeMode = std.builtin.OptimizeMode;
const Compile = Build.Step.Compile;
const Module = Build.Module;

const lib_name = "webui";
var global_log_level: std.log.Level = .warn;

/// Vendored dependencies of webui.
pub const Dependency = enum {
    civetweb,
    // TODO: Check and add all vendored dependencies, e.g. "webview"
};

const DebugDependencies = std.EnumSet(Dependency);

pub fn build(b: *Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const is_dynamic = b.option(bool, "dynamic", "build the dynamic library") orelse false;
    const enable_tls = b.option(bool, "enable-tls", "enable TLS support") orelse false;
    const verbose = b.option(std.log.Level, "verbose", "set verbose output") orelse .warn;
    global_log_level = verbose;
    // TODO: Support list of dependencies once support is limited to >0.13.0
    const debug = b.option(Dependency, "debug", "enable dependency debug output");
    const debug_dependencies = DebugDependencies.initMany(if (debug) |d| &.{d} else &.{});

    if (enable_tls and !target.query.isNative()) {
        log(.err, .WebUI, "cross compilation is not supported with TLS enabled", .{});
        return error.InvalidBuildConfiguration;
    }

    log(.info, .WebUI, "Building {s} WebUI library{s}...", .{
        if (is_dynamic) "dynamic" else "static",
        if (enable_tls) " with TLS support" else "",
    });
    defer {
        log(.info, .WebUI, "Done.", .{});
    }

    const webui = if (is_dynamic) b.addSharedLibrary(.{
        .name = lib_name,
        .target = target,
        .optimize = optimize,
        .pic = true,
    }) else b.addStaticLibrary(.{
        .name = lib_name,
        .target = target,
        .optimize = optimize,
    });
    try addLinkerFlags(b, webui, enable_tls, debug_dependencies);

    b.installArtifact(webui);

    try build_examples(b, webui);
}

fn addLinkerFlags(
    b: *Build,
    webui: *Compile,
    enable_tls: bool,
    debug_dependencies: DebugDependencies,
) !void {
    const webui_target = webui.rootModuleTarget();
    const is_windows = webui_target.os.tag == .windows;
    const is_darwin = webui_target.os.tag == .macos;
    const debug = webui.root_module.optimize.? == .Debug;

    // Prepare compiler flags.
    const no_tls_flags: []const []const u8 = &.{"-DNO_SSL"};
    const tls_flags: []const []const u8 = &.{ "-DWEBUI_TLS", "-DNO_SSL_DL", "-DOPENSSL_API_1_1" };
    const civetweb_flags: []const []const u8 = &.{
        "-DNO_CACHING",
        "-DNO_CGI",
        "-DUSE_WEBSOCKET",
        "-Wno-error=date-time",
    };

    if (debug) {
        webui.root_module.addCMacro("WEBUI_LOG", "");
    }
    webui.addCSourceFile(.{
        .file = b.path("src/webui.c"),
        .flags = if (enable_tls) tls_flags else no_tls_flags,
    });

    const civetweb_debug = debug and debug_dependencies.contains(.civetweb);
    webui.addCSourceFile(.{
        .file = b.path("src/civetweb/civetweb.c"),
        .flags = if (enable_tls and !civetweb_debug)
            civetweb_flags ++ tls_flags ++ .{"-DNDEBUG"}
        else if (enable_tls and civetweb_debug)
            civetweb_flags ++ tls_flags
        else if (!enable_tls and !civetweb_debug)
            civetweb_flags ++ .{"-DUSE_WEBSOCKET"} ++ no_tls_flags ++ .{"-DNDEBUG"}
        else
            civetweb_flags ++ .{"-DUSE_WEBSOCKET"} ++ no_tls_flags,
    });
    webui.linkLibC();
    webui.addIncludePath(b.path("include"));
    webui.installHeader(b.path("include/webui.h"), "webui.h");
    if (is_darwin) {
        webui.addCSourceFile(.{
            .file = b.path("src/webview/wkwebview.m"),
            .flags = &.{},
        });
        webui.linkFramework("Cocoa");
        webui.linkFramework("WebKit");
    } else if (is_windows) {
        webui.linkSystemLibrary("ws2_32");
        webui.linkSystemLibrary("ole32");
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

    for (webui.root_module.link_objects.items) |lo| {
        switch (lo) {
            .c_source_file => |csf| {
                log(.debug, .WebUI, "{s} linker flags: {s}", .{
                    csf.file.src_path.sub_path,
                    csf.flags,
                });
            },
            else => {},
        }
    }
}

fn build_examples(b: *Build, webui: *Compile) !void {
    const build_examples_step = b.step("examples", "builds the library and its examples");
    const target = webui.root_module.resolved_target.?;
    const optimize = webui.root_module.optimize.?;

    const examples_path = b.path("examples/C").getPath(b);
    var examples_dir = std.fs.cwd().openDir(
        examples_path,
        .{ .iterate = true },
    ) catch |e| switch (e) {
        // Do not attempt building examples if directory does not exist.
        error.FileNotFound => return,
        else => return e,
    };
    defer examples_dir.close();

    var paths = examples_dir.iterate();
    while (try paths.next()) |val| {
        if (val.kind != .directory) {
            continue;
        }
        const example_name = val.name;

        const exe = b.addExecutable(.{
            .name = example_name,
            .target = target,
            .optimize = optimize,
        });
        const path = try std.fmt.allocPrint(b.allocator, "examples/C/{s}/main.c", .{example_name});
        defer b.allocator.free(path);

        exe.addCSourceFile(.{ .file = b.path(path), .flags = &.{} });
        exe.linkLibrary(webui);

        const exe_install = b.addInstallArtifact(exe, .{});
        const exe_run = b.addRunArtifact(exe);
        const step_name = try std.fmt.allocPrint(b.allocator, "run_{s}", .{example_name});
        defer b.allocator.free(step_name);
        const step_desc = try std.fmt.allocPrint(b.allocator, "run example {s}", .{example_name});
        defer b.allocator.free(step_desc);

        const cwd = try std.fmt.allocPrint(b.allocator, "src/examples/{s}", .{example_name});
        defer b.allocator.free(cwd);
        exe_run.setCwd(b.path(cwd));

        exe_run.step.dependOn(&exe_install.step);
        build_examples_step.dependOn(&exe_install.step);
        b.step(step_name, step_desc).dependOn(&exe_run.step);
    }
}

/// Function to runtime-scope log levels based on build flag, for all scopes.
fn log(
    comptime level: std.log.Level,
    comptime scope: @TypeOf(.EnumLiteral),
    comptime format: []const u8,
    args: anytype,
) void {
    const should_print: bool = @intFromEnum(global_log_level) >= @intFromEnum(level);
    if (should_print) {
        switch (comptime level) {
            .err => std.log.scoped(scope).err(format, args),
            .warn => std.log.scoped(scope).warn(format, args),
            .info => std.log.scoped(scope).info(format, args),
            .debug => std.log.scoped(scope).debug(format, args),
        }
    }
}
