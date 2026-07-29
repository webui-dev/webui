# WebUI C++ Starter Kit

A ready-to-use project template for building a professional desktop app in
C++ with a web UI — compiled into a **single portable executable** with the
application icon and every UI file (HTML, CSS, JS, images) embedded inside it.

No runtime files, no installer, no dependencies beyond a C++17 compiler.

## Quick start

1. Clone WebUI: `git clone https://github.com/webui-dev/webui.git`
2. `cd webui`
3. Build the WebUI library:
   - Linux / macOS: `make`
   - Windows MinGW: `mingw32-make`
   - Windows MSVC: `nmake` (from a *Developer Command Prompt*)
4. `cd examples/starter_kit_c++`
5. Build the starter kit: `make` / `mingw32-make` / `nmake`
6. Done — your app is `bin/my_app` (`bin\my_app.exe` on Windows,
   `bin/my_app.app` on macOS), and the `starter_kit_c++` folder is now
   **self-contained**: copy it anywhere (outside the WebUI repository) and
   continue developing your own project from it.

The first build must happen inside the WebUI repository: it copies the WebUI
static library and headers into `lib/`. Every later build works from the
local `lib/` snapshot, wherever the folder lives.

## Build with CMake (optional)

The kit also ships `CMakeLists.txt` + `CMakePresets.json`, so it works out
of the box with CMake-based IDEs and tools (Visual Studio, CLion, VS Code,
Ninja, ...):

```sh
cmake -B build
cmake --build build --config Release
```

Or using presets: `cmake --preset default` then `cmake --build --preset
default` (`debug` presets included). The output matches the Makefile builds:
everything lands in `bin/`, and the same `lib/` snapshot rule applies — run
the first configure inside the WebUI repository.

## Folder structure

```
starter_kit_c++/
├── Makefile          # Windows MSVC (nmake)
├── GNUmakefile       # Linux, macOS, Windows MinGW (make / mingw32-make)
├── CMakeLists.txt    # CMake build (Visual Studio, CLion, VS Code, ...)
├── CMakePresets.json # CMake presets (Release / Debug)
├── icon.rc           # Windows resource: embeds ui/favicon.ico as the EXE icon
├── src/
│   ├── main.cpp      # Your C++ backend (HTTP handler, bindings, window)
│   └── vfs.h         # GENERATED — the ui/ folder packed as C arrays
├── tools/
│   └── vfs.c         # Build-time packer: ui/ -> src/vfs.h (auto-compiled)
├── ui/               # Your frontend — every file here gets embedded
│   ├── index.html
│   ├── style.css
│   ├── app.js
│   ├── favicon.ico   # App icon: Windows executable icon (via icon.rc)
│   └── icon.png      # App icon: HTML favicon + Linux/macOS app icon
├── lib/              # GENERATED — snapshot of webui.h + webui.hpp + static library
└── bin/              # GENERATED — my_app(.exe); Linux: + .desktop + icon.png; macOS: my_app.app
```

## Customize

- **App name** — edit `APP_NAME` at the top of `Makefile` and `GNUmakefile`.
- **Frontend** — edit or add any files under `ui/` (subfolders included).
  They are re-packed into the executable automatically on every build.
- **Backend** — add C++ functions in `src/main.cpp` and expose them with
  `win.bind()` (free functions or lambdas:
  `win.bind("name", [](webui::window::event* e) { ... });`). Every bound
  function becomes a global `async` JavaScript function:
  `const reply = await greet('World');`
- **HTTP** — every UI request goes through `http_handler()` in `src/main.cpp`
  (embedded files via `vfs()`, everything else 404) — add custom routes,
  headers, or dynamic responses there.
- **Icon** — two files in `ui/` (keep the names, swap the artwork):
  `ui/favicon.ico` — the Windows executable icon (resource ID 32512 /
  `IDI_APPLICATION`, used by Explorer, the taskbar, and the WebView window;
  use a real multi-size `.ico`); `ui/icon.png` — the HTML favicon, on
  Linux the window/taskbar icon and the `.desktop` launcher icon (the
  build copies it into `bin/`), and on macOS the app icon (converted to
  `icon.icns` inside the `.app` bundle).
- **Window** — `src/main.cpp` opens a frameless, transparent WebView window,
  and the UI draws its own title bar (`#titlebar` in `ui/index.html`, with
  drag regions and minimize/maximize/close buttons). For a normal OS window,
  remove the `win.set_frameless()` / `win.set_transparent()` lines; for
  browser mode, call `win.show()` instead of `win.show_wv()`.

## How it works

- `tools/vfs.c` is compiled with your same compiler and run at build time.
  It packs `ui/` into `src/vfs.h` as byte arrays plus a `vfs()` lookup
  that serves an embedded file as a full HTTP response (MIME type via
  `webui_get_mime_type()`), or returns `NULL` for unknown paths.
- Every HTTP request reaches `http_handler()` in `main.cpp`: embedded files
  are served through `vfs()`, anything else gets a 404 — tweak requests or
  add your own routes there (`webui.js` is always served by WebUI itself).
  Note: using a custom file handler disables WebUI's cookie-based client
  authentication (security becomes your responsibility).
- `icon.rc` embeds the icon at resource ID 32512 (`IDI_APPLICATION`), which
  WebUI's WebView window class picks up automatically on Windows.
- `main.cpp` calls `win.set_icon_file("icon.png")` before showing the
  window; WebUI resolves the file (working directory first, then next to
  the executable) and, on Linux, applies it to the WebView window, which
  gives the taskbar icon on X11 sessions.
- The app links against `lib/webui-2-static.lib` (MSVC) or
  `lib/libwebui-2-static.a` (MinGW / Linux / macOS) — fully static, one file.
- On Linux, the release build copies `ui/icon.png` into `bin/` and generates
  `bin/<app>.desktop` — a launcher pointing at the executable and the icon
  with absolute paths. Nothing is installed or written outside this folder;
  rebuild after moving the folder to refresh the paths.
- On macOS, the release build wraps the executable into `bin/<app>.app` —
  `Info.plist` plus `icon.icns` (converted from `ui/icon.png` with the
  built-in `sips` tool) — so Finder and the Dock show the app icon.

## Debugging

`make debug` / `nmake debug` builds with debug symbols and a console window,
so terminal output from your C++ code is visible. For WebUI's own internal
logs, build the library in debug mode at the repository root (`make debug`),
then copy the static library from `dist/debug/` over the one in `lib/`.

## Updating WebUI

Rebuild the library in the WebUI repository, then run one build of this kit
while it is inside the repository — `lib/` is refreshed automatically. (If you
moved the folder out, copy the new static library, `webui.h` and `webui.hpp`
into `lib/` manually, or rebuild from a fresh clone.)

## Platform notes

- **Windows** — the executable icon works everywhere (Explorer, taskbar, and
  the frameless WebView window). WebUI links the WebView2 loader statically and
  the WebView2 Runtime ships with Windows 10/11, so the single executable is
  all you need. If WebView2 is unavailable, the app falls back to a browser
  app-mode window (the embedded favicon shows as the tab icon).
- **Linux** — the frameless WebView window needs GTK + WebKitGTK installed
  (the app falls back to a browser app-mode window otherwise). The
  window/taskbar icon comes from `win.set_icon_file()` using `bin/icon.png`
  (copied next to the executable by the build) and works on X11 sessions
  with nothing installed; Wayland sessions only show icons for apps with an
  installed `.desktop` entry. ELF executables cannot embed icons, so the
  build also generates `bin/my_app.desktop`, a launcher pointing at the
  executable and `bin/icon.png` (absolute paths, regenerated on every
  build — rebuild after moving the folder). Everything stays inside this
  folder; the build never writes to the system. In GNOME Files, right-click
  the `.desktop` and choose "Allow Launching" once — it then shows your app
  icon and launches on double-click. Optional, your choice: copy
  `bin/my_app.desktop` into `~/.local/share/applications/` to add the app
  to the system menu (and the dock icon on Wayland) — the build itself
  never does this.
- **macOS** — the release build produces `bin/my_app.app` with the
  executable and icon embedded inside the bundle (`icon.icns` is generated
  from `ui/icon.png` using `sips`, which ships with macOS). Launch it from
  Finder or with `open bin/my_app.app`; the Dock and Finder show the app
  icon. The bundle is unsigned — fine for local use; distributing to other
  Macs needs `codesign`/notarization. `make debug` still produces a plain
  `bin/my_app` binary for terminal debugging.
