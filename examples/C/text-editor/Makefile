# WebUI Text Editor Example

# == 1. VARIABLES =============================================================

PROJECT_DIR := $(shell git rev-parse --show-toplevel)
LIB_DIR := $(PROJECT_DIR)/dist

# ARGS
# Set a compiler when running on Linux via `make CC=gcc` / `make CC=clang`
CC = gcc
# Build the WebUI library if running via `make BUILD_LIB=true`
BUILD_LIB ?=

# BUILD FLAGS
CFLAGS = -m64 text-editor.c -I"$(PROJECT_DIR)/include"
LDFLAGS = -L"$(LIB_DIR)"

# Platform conditions
ifeq ($(OS),Windows_NT)
	# Windows
	SHELL=CMD
	CFLAGS += -Wl,-subsystem=windows
	LDFLAGS += webui-2.dll -lcomdlg32
	OUT := text-editor.exe
	STRIP_OPT := --strip-all
else
	LDFLAGS += -lpthread -lm -lwebui-2-static
	OUT := text-editor
	ifeq ($(shell uname),Darwin)
		# MacOS
		CC = clang
		CFLAGS += -ObjC -framework Cocoa
		LDFLAGS += -stdlib=libc++ -lstdc++
	else
		# Linux
		LDFLAGS += $(shell pkg-config --cflags --libs gtk+-3.0)
		STRIP_OPT := --strip-all
		ifeq ($(CC),clang)
			LLVM_OPT := llvm-
		endif
	endif
endif

# == 2.TARGETS ================================================================

all: release

debug: --setup
	@echo "Build Text Editor Example ($@)..."
	@$(CC) -g -Wall $(LDFLAGS) -o $(OUT)
	@echo "Done."

release: --setup
	@echo "Build Text Editor Example ($@)..."
	@$(CC) -Os $(CFLAGS) $(LDFLAGS) -o $(OUT)
	@$(LLVM_OPT)strip $(STRIP_OPT) $(OUT)
	@echo "Done."

# INTERNAL TARGETS

--setup:
ifneq ($(filter $(CC),gcc clang),$(CC))
	$(error Invalid compiler specified: `$(CC)`)
endif
ifeq ($(BUILD_LIB),true)
	@cd "$(LIB_DIR)" && cd .. && $(MAKE)
endif
ifeq ($(OS),Windows_NT)
	@copy "$(LIB_DIR)\webui-2.dll" ".\webui-2.dll" >nul 2>&1
endif
