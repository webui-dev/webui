const std = @import("std");
const Build = std.Build;
const OptimizeMode = std.builtin.OptimizeMode;
const Compile = Build.Step.Compile;
const Module = Build.Module;
const builtin = @import("builtin");

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
    const enable_webui_log = b.option(bool, "enable-webui-log", "Enable WebUI log output") orelse false;
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

    const webui = if (builtin.zig_version.minor == 14) (if (is_dynamic) b.addSharedLibrary(.{
        .name = lib_name,
        .target = target,
        .optimize = optimize,
        .pic = true,
    }) else b.addStaticLibrary(.{
        .name = lib_name,
        .target = target,
        .optimize = optimize,
    })) else b.addLibrary(.{
        .name = lib_name,
        .linkage = if (is_dynamic) .dynamic else .static,
        .root_module = b.createModule(.{
            // not, pic enabled, zig will not allow to build a static library
            // .pic = is_dynamic,
            .pic = if (is_dynamic) is_dynamic else true,
            .target = target,
            .optimize = optimize,
        }),
    });

    try addLinkerFlags(b, webui, enable_tls, debug_dependencies, enable_webui_log);

    b.installArtifact(webui);

    try build_examples(b, webui);
}

fn addLinkerFlags(
    b: *Build,
    webui: *Compile,
    enable_tls: bool,
    debug_dependencies: DebugDependencies,
    enable_webui_log: bool,
) !void {
    const webui_target = webui.rootModuleTarget();
    const is_windows = webui_target.os.tag == .windows;
    const is_darwin = webui_target.os.tag == .macos;
    // Zig 0.17 renamed `OptimizeMode` to `Optimize` with lowercase tags.
    const debug = webui.root_module.optimize.? == if (comptime builtin.zig_version.minor >= 17) .debug else .Debug;
    // In Zig 0.16, methods like addCSourceFile/linkLibC/addIncludePath/linkSystemLibrary/
    // linkFramework/addCMacro were removed from *Compile and live only on *Module.
    // Routing every call through `mod` keeps the build script compatible with 0.14/0.15/0.16.
    const mod = webui.root_module;

    // Prepare compiler flags.
    const no_tls_flags: []const []const u8 = &.{"-DNO_SSL"};
    const tls_flags: []const []const u8 = &.{ "-DWEBUI_TLS", "-DNO_SSL_DL", "-DOPENSSL_API_1_1" };
    const civetweb_flags: []const []const u8 = &.{
        "-DNO_CACHING",
        "-DNO_CGI",
        "-DUSE_WEBSOCKET",
        "-Wno-error=date-time",
    };

    if (debug and enable_webui_log) {
        mod.addCMacro("WEBUI_LOG", "");
    }
    mod.addCSourceFile(.{
        .file = b.path("src/webui.c"),
        .flags = if (enable_tls) tls_flags else no_tls_flags,
    });

    // Add Win32 WebView2 C++ support on Windows
    if (is_windows) {
        mod.addCSourceFile(.{
            .file = b.path("src/webview/win32_wv2.cpp"),
            .flags = if (enable_tls) tls_flags else no_tls_flags,
        });
        mod.link_libcpp = true;
    }

    const civetweb_debug = debug and debug_dependencies.contains(.civetweb);
    mod.addCSourceFile(.{
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
    mod.link_libc = true;
    mod.addIncludePath(b.path("include"));
    webui.installHeader(b.path("include/webui.h"), "webui.h");
    if (is_darwin) {
        mod.addCSourceFile(.{
            .file = b.path("src/webview/wkwebview.m"),
            .flags = &.{},
        });
        mod.linkFramework("Cocoa", .{});
        mod.linkFramework("WebKit", .{});
    } else if (is_windows) {
        mod.linkSystemLibrary("ws2_32", .{});
        mod.linkSystemLibrary("ole32", .{});
        if (webui_target.abi == .msvc) {
            mod.linkSystemLibrary("Advapi32", .{});
            mod.linkSystemLibrary("Shell32", .{});
            mod.linkSystemLibrary("user32", .{});
        }
        if (enable_tls) {
            mod.linkSystemLibrary("bcrypt", .{});
        }
    }
    if (enable_tls) {
        mod.linkSystemLibrary("ssl", .{});
        mod.linkSystemLibrary("crypto", .{});
    }

    for (mod.link_objects.items) |lo| {
        switch (lo) {
            .c_source_file => |csf| {
                log(.debug, .WebUI, "{s} linker flags:\n", .{
                    csf.file.src_path.sub_path,
                });
                for (csf.flags) |flag| {
                    log(.debug, .WebUI, "  {s}", .{flag});
                }
            },
            else => {},
        }
    }
}

fn build_examples(b: *Build, webui: *Compile) !void {
    const build_examples_step = b.step("examples", "builds the library and its examples");

    // Iterate examples/C. Zig 0.16 removed std.fs.cwd() and reworked Dir/Iterator
    // to require an Io instance, so split the open+iterate path by version while
    // sharing the per-example wiring below.
    if (comptime builtin.zig_version.minor >= 17) {
        const io = b.graph.io;
        var examples_dir = b.root.root_dir.handle.openDir(
            io,
            "examples/C",
            .{ .iterate = true },
        ) catch |e| switch (e) {
            error.FileNotFound => return,
            else => return e,
        };
        defer examples_dir.close(io);

        // Zig 0.17 caches the configure phase; re-run it when an example
        // directory is added or removed.
        b.dependOnDirectoryContents(b.path("examples/C"));

        var paths = examples_dir.iterate();
        while (try paths.next(io)) |val| {
            if (val.kind != .directory) continue;
            try add_example(b, webui, build_examples_step, val.name);
        }
    } else if (comptime builtin.zig_version.minor >= 16) {
        const io = b.graph.io;
        var examples_dir = b.build_root.handle.openDir(
            io,
            "examples/C",
            .{ .iterate = true },
        ) catch |e| switch (e) {
            error.FileNotFound => return,
            else => return e,
        };
        defer examples_dir.close(io);

        var paths = examples_dir.iterate();
        while (try paths.next(io)) |val| {
            if (val.kind != .directory) continue;
            try add_example(b, webui, build_examples_step, val.name);
        }
    } else {
        const examples_path = b.path("examples/C").getPath(b);
        var examples_dir = std.fs.cwd().openDir(
            examples_path,
            .{ .iterate = true },
        ) catch |e| switch (e) {
            error.FileNotFound => return,
            else => return e,
        };
        defer examples_dir.close();

        var paths = examples_dir.iterate();
        while (try paths.next()) |val| {
            if (val.kind != .directory) continue;
            try add_example(b, webui, build_examples_step, val.name);
        }
    }
}

fn add_example(
    b: *Build,
    webui: *Compile,
    build_examples_step: *Build.Step,
    example_name: []const u8,
) !void {
    const target = webui.root_module.resolved_target.?;
    const optimize = webui.root_module.optimize.?;

    const exe = b.addExecutable(if (builtin.zig_version.minor == 14) .{
        .name = example_name,
        .target = target,
        .optimize = optimize,
    } else .{
        .name = example_name,
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
            .pic = true,
        }),
    });
    const path = try std.fmt.allocPrint(b.allocator, "examples/C/{s}/main.c", .{example_name});
    defer b.allocator.free(path);

    // Route through root_module so Zig 0.16 (which removed these methods from *Compile) keeps working.
    exe.root_module.addCSourceFile(.{ .file = b.path(path), .flags = &.{} });
    exe.root_module.linkLibrary(webui);

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
