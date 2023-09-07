# WebUI Text Editor Example

# == 1. VARIABLES =============================================================

PROJECT_DIR := $(shell git rev-parse --show-toplevel)
LIB_DIR := $(PROJECT_DIR)/dist

# ARGS
# Build the WebUI library if running via `make BUILD_LIB=true`
BUILD_LIB ?=

# BUILD FLAGS
SHELL = CMD
CC = gcc
CFLAGS := -m64 text-editor.c -I"$(PROJECT_DIR)/include" -Wl,-subsystem=windows
LDFLAGS := webui-2.dll -lcomdlg32
OUT := text-editor.exe
STRIP_OPT := --strip-all

# == 2.TARGETS ================================================================

all: release

debug: --setup
	@echo "Build Text Editor Example ($(CC) $@)..."
	@$(CC) -g -Wall $(LDFLAGS) -o $(OUT)
	@echo "Done."

release: --setup
	@echo "Build Text Editor Example ($(CC) $@)..."
	@$(CC) -Os $(CFLAGS) $(LDFLAGS) -o $(OUT)
	@$(LLVM_OPT)strip $(STRIP_OPT) $(OUT)
	@echo "Done."

# INTERNAL TARGETS

--setup:
ifeq ($(BUILD_LIB),true)
	@cd "$(LIB_DIR)" && cd .. && $(MAKE)
endif
	@copy "$(LIB_DIR)\webui-2.dll" ".\webui-2.dll"
