const Builder = @import("std").build.Builder;
const Kind = @import("std").build.LibExeObjStep.SharedLibKind;

pub fn build(b: *Builder) void {
    const target = b.standardTargetOptions(.{});
    const mode = b.standardReleaseOptions();
    const buildDir = "build";

    // Build civetweb
    const civetweb = b.addObject("civetweb", null);
    civetweb.addIncludePath("include");
    civetweb.setTarget(target);
    civetweb.setBuildMode(mode);
    civetweb.linkLibC();
    civetweb.addCSourceFiles(&.{"src/civetweb/civetweb.c"}, &.{ "-DNDEBUG", "-DNO_CACHING", "-DNO_CGI", "-DNO_SSL", "-DUSE_WEBSOCKET", if (target.isWindows()) "-DMUST_IMPLEMENT_CLOCK_GETTIME" else "" });

    // Build WebUI
    const webui = b.addObject("webui", null);
    webui.addIncludePath("include");
    webui.setTarget(target);
    webui.setBuildMode(mode);
    webui.linkLibC();
    webui.addCSourceFiles(&.{"src/webui.c"}, &.{if (mode == .Debug) "-DWEBUI_LOG" else ""});

    // Create a static library
    const static = b.addStaticLibrary("libwebui-2-static", null);
    static.setTarget(target);
    static.setBuildMode(mode);
    static.addObject(civetweb);
    static.addObject(webui);
    static.setOutputDir(buildDir);
    static.install();

    // Create a shared library
    const shared = b.addSharedLibrary(
        "webui-2",
        null,
        Kind.unversioned,
    );
    shared.setTarget(target);
    shared.setBuildMode(mode);
    shared.addObject(civetweb);
    shared.addObject(webui);
    shared.setOutputDir(buildDir);
    shared.strip = true;
    webui.force_pic = true;
    civetweb.force_pic = true;
    if (target.isWindows()) {
        shared.linkSystemLibrary("ws2_32");
    }
    shared.install();
}
