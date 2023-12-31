const std = @import("std");
const builtin = @import("builtin");

const Build = std.Build;
const OptimizeMode = std.builtin.OptimizeMode;
const CrossTarget = std.zig.CrossTarget;
const Compile = Build.Step.Compile;
const Module = Build.Module;

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

    const webui = build_webui(b, optimize, target, isStatic, enableTLS);
    webui.linkLibrary(build_civetweb(b, optimize, target, isStatic, enableTLS));

    webui.installHeader("include/webui.h", "webui.h");

    b.installArtifact(webui);
}

fn build_webui(b: *Build, optimize: OptimizeMode, target: CrossTarget, is_static: bool, enable_tls: bool) *Compile {
    const name = "webui";
    const webui = if (is_static) b.addStaticLibrary(.{ .name = name, .target = target, .optimize = optimize }) else b.addSharedLibrary(.{ .name = name, .target = target, .optimize = optimize });

    webui.addCSourceFile(.{
        .file = .{ .path = "src/webui.c" },
        .flags = if (enable_tls)
            &[_][]const u8{ "-DNO_SSL", "-DWEBUI_TLS", "-DNO_SSL_DL", "-DOPENSSL_API_1_1" }
        else
            &[_][]const u8{"-DNO_SSL"},
    });

    webui.linkLibC();

    webui.addIncludePath(.{ .path = "include" });

    return webui;
}

fn build_civetweb(b: *Build, optimize: OptimizeMode, target: CrossTarget, is_static: bool, enable_tls: bool) *Compile {
    const name = "civetweb";
    const civetweb = if (is_static) b.addStaticLibrary(.{ .name = name, .target = target, .optimize = optimize }) else b.addSharedLibrary(.{ .name = name, .target = target, .optimize = optimize });

    civetweb.addIncludePath(.{ .path = "include" });

    const extra_flags = if (target.os_tag == .windows) "-DMUST_IMPLEMENT_CLOCK_GETTIME" else "";

    const cflags = if (enable_tls) [_][]const u8{ "-DNDEBUG", "-DNO_CACHING", "-DNO_CGI", "-DUSE_WEBSOCKET", "-DWEBUI_TLS", "-DNO_SSL_DL", "-DOPENSSL_API_1_1", extra_flags } else [_][]const u8{ "-DNDEBUG", "-DNO_CACHING", "-DNO_CGI", "-DUSE_WEBSOCKET", "-DNO_SSL", extra_flags, "", "" };

    civetweb.addCSourceFile(.{
        .file = .{ .path = "src/civetweb/civetweb.c" },
        .flags = &cflags,
    });

    civetweb.linkLibC();

    if (target.os_tag == .windows) {
        civetweb.linkSystemLibrary("ws2_32");
        if (enable_tls) {
            civetweb.linkSystemLibrary("bcrypt");
        }
    }
    if (enable_tls) {
        civetweb.linkSystemLibrary("ssl");
        civetweb.linkSystemLibrary("crypto");
    }

    return civetweb;
}
