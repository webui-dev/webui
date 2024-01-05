const std = @import("std");
const builtin = @import("builtin");

const Build = std.Build;
const OptimizeMode = std.builtin.OptimizeMode;
const CrossTarget = std.zig.CrossTarget;
const Compile = Build.Step.Compile;
const Module = Build.Module;

const log = std.log.scoped(.WebUI);

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
    if (current_zig.minor == 11) {
        build_11(b);
    } else if (current_zig.minor == 12) {
        build_12(b);
    }
}

fn build_examples_12(b: *Build, optimize: OptimizeMode, target: Build.ResolvedTarget, webui_lib: *Compile) void {
    var lazy_path = Build.LazyPath{
        .path = "examples/C",
    };

    const build_all_step = b.step("build_all", "build all examples");

    const examples_path = lazy_path.getPath(b);

    var iter_dir = if (comptime current_zig.minor == 11)
        std.fs.openIterableDirAbsolute(examples_path, .{}) catch |err| {
            log.err("open examples_path failed, err is {}", .{err});
            std.os.exit(1);
        }
    else
        std.fs.openDirAbsolute(examples_path, .{ .iterate = true }) catch |err| {
            log.err("open examples_path failed, err is {}", .{err});
            std.os.exit(1);
        };
    defer iter_dir.close();

    var itera = iter_dir.iterate();

    while (itera.next()) |val| {
        if (val) |entry| {
            if (entry.kind == .directory) {
                const example_name = entry.name;
                const path = std.fmt.allocPrint(b.allocator, "examples/C/{s}/main.c", .{example_name}) catch |err| {
                    log.err("fmt path for examples failed, err is {}", .{err});
                    std.os.exit(1);
                };

                const exe = b.addExecutable(.{
                    .name = example_name,
                    .target = target,
                    .optimize = optimize,
                });

                exe.addCSourceFile(.{
                    .file = .{
                        .path = path,
                    },
                    .flags = &.{},
                });

                exe.linkLibrary(webui_lib);

                const exe_install = b.addInstallArtifact(exe, .{});

                build_all_step.dependOn(&exe_install.step);

                const exe_run = b.addRunArtifact(exe);
                exe_run.step.dependOn(&exe_install.step);

                if (comptime (current_zig.minor > 11)) {
                    const cwd = std.fmt.allocPrint(b.allocator, "examples/C/{s}", .{example_name}) catch |err| {
                        log.err("fmt path for examples failed, err is {}", .{err});
                        std.os.exit(1);
                    };
                    exe_run.setCwd(.{
                        .path = cwd,
                    });
                } else {
                    const cwd = std.fmt.allocPrint(b.allocator, "{s}/{s}", .{ examples_path, example_name }) catch |err| {
                        log.err("fmt path for examples failed, err is {}", .{err});
                        std.os.exit(1);
                    };
                    exe_run.cwd = cwd;
                }

                const step_name = std.fmt.allocPrint(b.allocator, "run_{s}", .{example_name}) catch |err| {
                    log.err("fmt step_name for examples failed, err is {}", .{err});
                    std.os.exit(1);
                };

                const step_desc = std.fmt.allocPrint(b.allocator, "run example {s}", .{example_name}) catch |err| {
                    log.err("fmt step_desc for examples failed, err is {}", .{err});
                    std.os.exit(1);
                };

                const exe_run_step = b.step(step_name, step_desc);
                exe_run_step.dependOn(&exe_run.step);
            }
        } else {
            break;
        }
    } else |err| {
        log.err("iterate examples_path failed, err is {}", .{err});
        std.os.exit(1);
    }
}

fn build_examples_11(b: *Build, optimize: OptimizeMode, target: CrossTarget, webui_lib: *Compile) void {
    var lazy_path = Build.LazyPath{
        .path = "examples/C",
    };

    const build_all_step = b.step("build_all", "build all examples");

    const examples_path = lazy_path.getPath(b);

    var iter_dir =
        std.fs.openIterableDirAbsolute(examples_path, .{}) catch |err| {
        log.err("open examples_path failed, err is {}", .{err});
        std.os.exit(1);
    };
    defer iter_dir.close();

    var itera = iter_dir.iterate();

    while (itera.next()) |val| {
        if (val) |entry| {
            if (entry.kind == .directory) {
                const example_name = entry.name;
                const path = std.fmt.allocPrint(b.allocator, "examples/C/{s}/main.c", .{example_name}) catch |err| {
                    log.err("fmt path for examples failed, err is {}", .{err});
                    std.os.exit(1);
                };

                const exe = b.addExecutable(.{
                    .name = example_name,
                    .target = target,
                    .optimize = optimize,
                });

                exe.addCSourceFile(.{
                    .file = .{
                        .path = path,
                    },
                    .flags = &.{},
                });

                exe.linkLibrary(webui_lib);

                const exe_install = b.addInstallArtifact(exe, .{});

                build_all_step.dependOn(&exe_install.step);

                const exe_run = b.addRunArtifact(exe);
                exe_run.step.dependOn(&exe_install.step);

                if (comptime (current_zig.minor > 11)) {
                    const cwd = std.fmt.allocPrint(b.allocator, "examples/C/{s}", .{example_name}) catch |err| {
                        log.err("fmt path for examples failed, err is {}", .{err});
                        std.os.exit(1);
                    };
                    exe_run.setCwd(.{
                        .path = cwd,
                    });
                } else {
                    const cwd = std.fmt.allocPrint(b.allocator, "{s}/{s}", .{ examples_path, example_name }) catch |err| {
                        log.err("fmt path for examples failed, err is {}", .{err});
                        std.os.exit(1);
                    };
                    exe_run.cwd = cwd;
                }

                const step_name = std.fmt.allocPrint(b.allocator, "run_{s}", .{example_name}) catch |err| {
                    log.err("fmt step_name for examples failed, err is {}", .{err});
                    std.os.exit(1);
                };

                const step_desc = std.fmt.allocPrint(b.allocator, "run example {s}", .{example_name}) catch |err| {
                    log.err("fmt step_desc for examples failed, err is {}", .{err});
                    std.os.exit(1);
                };

                const exe_run_step = b.step(step_name, step_desc);
                exe_run_step.dependOn(&exe_run.step);
            }
        } else {
            break;
        }
    } else |err| {
        log.err("iterate examples_path failed, err is {}", .{err});
        std.os.exit(1);
    }
}

fn build_webui_12(b: *Build, optimize: OptimizeMode, target: Build.ResolvedTarget, is_static: bool, enable_tls: bool) *Compile {
    const name = "webui";
    const webui = if (is_static) b.addStaticLibrary(.{ .name = name, .target = target, .optimize = optimize }) else b.addSharedLibrary(.{ .name = name, .target = target, .optimize = optimize });

    const extra_flags = if (target.query.os_tag == .windows or (target.query.os_tag == null and builtin.os.tag == .windows))
        "-DMUST_IMPLEMENT_CLOCK_GETTIME"
    else
        "";

    const cflags = if (enable_tls)
        [_][]const u8{ "-DNDEBUG", "-DNO_CACHING", "-DNO_CGI", "-DUSE_WEBSOCKET", "-DWEBUI_TLS", "-DNO_SSL_DL", "-DOPENSSL_API_1_1", extra_flags }
    else
        [_][]const u8{ "-DNDEBUG", "-DNO_CACHING", "-DNO_CGI", "-DUSE_WEBSOCKET", "-DNO_SSL", extra_flags, "", "" };

    webui.addCSourceFile(.{
        .file = .{ .path = "src/webui.c" },
        .flags = if (enable_tls)
            &[_][]const u8{ "-DNO_SSL", "-DWEBUI_TLS", "-DNO_SSL_DL", "-DOPENSSL_API_1_1" }
        else
            &[_][]const u8{"-DNO_SSL"},
    });

    webui.addCSourceFile(.{
        .file = .{ .path = "src/civetweb/civetweb.c" },
        .flags = &cflags,
    });

    webui.linkLibC();

    webui.addIncludePath(.{ .path = "include" });

    if (target.query.os_tag == .windows or (target.query.os_tag == null and builtin.os.tag == .windows)) {
        webui.linkSystemLibrary("ws2_32");
        if (enable_tls) {
            webui.linkSystemLibrary("bcrypt");
        }
    }
    if (enable_tls) {
        webui.linkSystemLibrary("ssl");
        webui.linkSystemLibrary("crypto");
    }

    return webui;
}

fn build_webui_11(b: *Build, optimize: OptimizeMode, target: CrossTarget, is_static: bool, enable_tls: bool) *Compile {
    const name = "webui";
    const webui = if (is_static) b.addStaticLibrary(.{ .name = name, .target = target, .optimize = optimize }) else b.addSharedLibrary(.{ .name = name, .target = target, .optimize = optimize });

    const extra_flags = if (target.os_tag == .windows or (target.os_tag == null and builtin.os.tag == .windows))
        "-DMUST_IMPLEMENT_CLOCK_GETTIME"
    else
        "";

    const cflags = if (enable_tls)
        [_][]const u8{ "-DNDEBUG", "-DNO_CACHING", "-DNO_CGI", "-DUSE_WEBSOCKET", "-DWEBUI_TLS", "-DNO_SSL_DL", "-DOPENSSL_API_1_1", extra_flags }
    else
        [_][]const u8{ "-DNDEBUG", "-DNO_CACHING", "-DNO_CGI", "-DUSE_WEBSOCKET", "-DNO_SSL", extra_flags, "", "" };

    webui.addCSourceFile(.{
        .file = .{ .path = "src/webui.c" },
        .flags = if (enable_tls)
            &[_][]const u8{ "-DNO_SSL", "-DWEBUI_TLS", "-DNO_SSL_DL", "-DOPENSSL_API_1_1" }
        else
            &[_][]const u8{"-DNO_SSL"},
    });

    webui.addCSourceFile(.{
        .file = .{ .path = "src/civetweb/civetweb.c" },
        .flags = &cflags,
    });

    webui.linkLibC();

    webui.addIncludePath(.{ .path = "include" });

    if (target.os_tag == .windows or (target.os_tag == null and builtin.os.tag == .windows)) {
        webui.linkSystemLibrary("ws2_32");
        if (enable_tls) {
            webui.linkSystemLibrary("bcrypt");
        }
    }
    if (enable_tls) {
        webui.linkSystemLibrary("ssl");
        webui.linkSystemLibrary("crypto");
    }

    return webui;
}

fn build_11(b: *Build) void {
    const isStatic = b.option(bool, "is_static", "whether lib is static") orelse default_isStatic;
    const enableTLS = b.option(bool, "enable_tls", "whether lib enable tls") orelse default_enableTLS;
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    if (enableTLS) {
        std.log.info("enable TLS support", .{});
        if (!target.isNative()) {
            std.log.info("when enable tls, not support cross compile", .{});
            std.os.exit(1);
        }
    }

    const webui = build_webui_11(b, optimize, target, isStatic, enableTLS);

    webui.installHeader("include/webui.h", "webui.h");

    build_examples_11(b, optimize, target, webui);

    b.installArtifact(webui);
}

fn build_12(b: *Build) void {
    const isStatic = b.option(bool, "is_static", "whether lib is static") orelse default_isStatic;
    const enableTLS = b.option(bool, "enable_tls", "whether lib enable tls") orelse default_enableTLS;
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    if (enableTLS) {
        std.log.info("enable TLS support", .{});
        if (!target.query.isNative()) {
            std.log.info("when enable tls, not support cross compile", .{});
            std.os.exit(1);
        }
    }

    const webui = build_webui_12(b, optimize, target, isStatic, enableTLS);

    webui.installHeader("include/webui.h", "webui.h");

    build_examples_12(b, optimize, target, webui);

    b.installArtifact(webui);
}
