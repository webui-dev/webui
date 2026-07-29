# WebUI C++ Starter Kit
# Linux / macOS / Windows (MinGW)
#
#   make          Build a single portable executable into bin/
#   make debug    Same, with a console window and debug symbols
#   make clean    Remove generated build files (keeps the lib/ snapshot)
#
# The first build must run inside the WebUI repository (examples/starter_kit_c++)
# so the WebUI static library can be copied into lib/. After that, this folder
# is fully self-contained and can be moved anywhere.

# == 1. VARIABLES =============================================================

# Your application name (the executable name in bin/)
APP_NAME = my_app

# Compiler: g++ or clang++ (macOS always uses clang++)
CC = g++

# WebUI repository paths (only used when building inside the repository)
WEBUI_DIST = ../../dist
WEBUI_INCLUDE = ../../include

# WebUI static library file name
LIB_FILE = libwebui-2-static.a

# Platform conditions
ifeq ($(OS),Windows_NT)
	# Windows (MinGW)
	PLATFORM := windows
	SHELL := CMD
	OUT := bin/$(APP_NAME).exe
	ICON_OBJ := icon.o
	LIBS := -L"lib" -lwebui-2-static -lws2_32 -lole32 -luser32 -lstdc++ -luuid -static
	GUI_APP := -Wl,-subsystem=windows
	CONSOLE_APP := -Wl,-subsystem=console
	STRIP_OPT := --strip-all
else
	OUT := bin/$(APP_NAME)
	ICON_OBJ :=
	ifeq ($(shell uname),Darwin)
		# macOS
		PLATFORM := macos
		CC = clang++
		LIBS := -L"lib" -lwebui-2-static -lpthread -lm -ldl -framework Cocoa -framework WebKit
	else
		# Linux
		PLATFORM := linux
		LIBS := -L"lib" -lwebui-2-static -lpthread -lm -ldl
		STRIP_OPT := --strip-all
		ifeq ($(CC),clang++)
			LLVM_OPT := llvm-
		endif
	endif
endif

# == 2. TARGETS ===============================================================

all: release

release: --prepare
	@echo "Build $(APP_NAME) (release)..."
	@$(CC) -Os -Wall -std=c++17 $(GUI_APP) src/main.cpp $(ICON_OBJ) -I"lib" $(LIBS) -o $(OUT)
	@$(LLVM_OPT)strip $(STRIP_OPT) $(OUT)
ifeq ($(PLATFORM),windows)
	@- del icon.o >nul 2>&1
endif
ifeq ($(PLATFORM),linux)
	@echo "Generate bin/$(APP_NAME).desktop..."
	@cp ui/icon.png bin/icon.png
	@printf '%s\n' \
		'[Desktop Entry]' \
		'Type=Application' \
		'Name=$(APP_NAME)' \
		'Exec="$(CURDIR)/bin/$(APP_NAME)"' \
		'Icon=$(CURDIR)/bin/icon.png' \
		'Terminal=false' \
		'Categories=Utility;' \
		'StartupWMClass=$(APP_NAME)' \
		> bin/$(APP_NAME).desktop
	@chmod +x bin/$(APP_NAME).desktop
endif
ifeq ($(PLATFORM),macos)
	@echo "Bundle bin/$(APP_NAME).app..."
	@rm -rf "bin/$(APP_NAME).app"
	@mkdir -p "bin/$(APP_NAME).app/Contents/MacOS" "bin/$(APP_NAME).app/Contents/Resources"
	@mv $(OUT) "bin/$(APP_NAME).app/Contents/MacOS/$(APP_NAME)"
	@sips -s format icns ui/icon.png --out "bin/$(APP_NAME).app/Contents/Resources/icon.icns" >/dev/null
	@printf '%s\n' \
		'<?xml version="1.0" encoding="UTF-8"?>' \
		'<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' \
		'<plist version="1.0">' \
		'<dict>' \
		'    <key>CFBundleExecutable</key>' \
		'    <string>$(APP_NAME)</string>' \
		'    <key>CFBundleIdentifier</key>' \
		'    <string>com.webui.$(APP_NAME)</string>' \
		'    <key>CFBundleName</key>' \
		'    <string>$(APP_NAME)</string>' \
		'    <key>CFBundleIconFile</key>' \
		'    <string>icon</string>' \
		'    <key>CFBundlePackageType</key>' \
		'    <string>APPL</string>' \
		'    <key>CFBundleInfoDictionaryVersion</key>' \
		'    <string>6.0</string>' \
		'    <key>NSHighResolutionCapable</key>' \
		'    <true/>' \
		'</dict>' \
		'</plist>' \
		> "bin/$(APP_NAME).app/Contents/Info.plist"
	@echo "Done: bin/$(APP_NAME).app"
else
	@echo "Done: $(OUT)"
endif

debug: --prepare
	@echo "Build $(APP_NAME) (debug)..."
	@$(CC) -g -Wall -std=c++17 $(CONSOLE_APP) src/main.cpp $(ICON_OBJ) -I"lib" $(LIBS) -o $(OUT)
ifeq ($(PLATFORM),windows)
	@- del icon.o >nul 2>&1
endif
	@echo "Done: $(OUT)"

clean:
ifeq ($(PLATFORM),windows)
	@- rmdir /S /Q "bin" >nul 2>&1
	@- del src\vfs.h >nul 2>&1
	@- del icon.o >nul 2>&1
	@- del tools\vfs.exe >nul 2>&1
else
	@- rm -rf bin
	@- rm -f src/vfs.h
	@- rm -f tools/vfs
endif
	@echo "Clean done (lib/ snapshot kept)."

# == 3. INTERNAL ==============================================================

# Prepare: snapshot the WebUI library into lib/, pack ui/ into src/vfs.h,
# and compile the Windows icon resource.
--prepare:
ifeq ($(PLATFORM),windows)
	@if not exist "bin" mkdir "bin"
	@if not exist "lib" mkdir "lib"
	@if exist "..\..\dist\$(LIB_FILE)" copy /Y "..\..\dist\$(LIB_FILE)" "lib" >nul
	@if exist "..\..\include\webui.h" copy /Y "..\..\include\webui.h" "lib" >nul
	@if exist "..\..\include\webui.hpp" copy /Y "..\..\include\webui.hpp" "lib" >nul
	@if not exist "lib\$(LIB_FILE)" (echo [Error] WebUI static library not found. Run 'mingw32-make' in the WebUI repository root first. & exit /b 1)
	@echo Pack ui/ into src/vfs.h...
	@$(CC) -x c++ tools/vfs.c -o tools/vfs.exe
	@tools\vfs.exe ui src\vfs.h
	@windres icon.rc -o icon.o
else
	@mkdir -p bin lib
	@if [ -f "$(WEBUI_DIST)/$(LIB_FILE)" ]; then cp -f "$(WEBUI_DIST)/$(LIB_FILE)" lib/; fi
	@if [ -f "$(WEBUI_INCLUDE)/webui.h" ]; then cp -f "$(WEBUI_INCLUDE)/webui.h" lib/; fi
	@if [ -f "$(WEBUI_INCLUDE)/webui.hpp" ]; then cp -f "$(WEBUI_INCLUDE)/webui.hpp" lib/; fi
	@if [ ! -f "lib/$(LIB_FILE)" ]; then echo "[Error] WebUI static library not found. Run 'make' in the WebUI repository root first."; exit 1; fi
	@echo "Pack ui/ into src/vfs.h..."
	@$(CC) -x c++ tools/vfs.c -o tools/vfs
	@./tools/vfs ui src/vfs.h
endif
