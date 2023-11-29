# WebUI C Example

# == 1. VARIABLES =============================================================

MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
PROJECT_DIR := $(dir $(MAKEFILE_PATH))/../../../
TARGET := $(firstword $(MAKECMDGOALS))
LIB_DIR := $(PROJECT_DIR)/dist
ifeq ($(TARGET), debug)
LIB_DIR := $(LIB_DIR)/debug
endif
INCLUDE_DIR := $(PROJECT_DIR)/include
WEBUI_LIB_NAME = webui-2
ifeq ($(WEBUI_USE_TLS), 1)
WEBUI_LIB_NAME = webui-2-secure
endif

# ARGS
# Set a compiler when running on Linux via `make CC=gcc` / `make CC=clang`
CC = gcc
# Build the WebUI library if running via `make BUILD_LIB=true`
BUILD_LIB ?=

# BUILD FLAGS
STATIC_BUILD_FLAGS = main.c -I"$(INCLUDE_DIR)" -L"$(LIB_DIR)"
DYN_BUILD_FLAGS = main.c -I"$(INCLUDE_DIR)" -L"$(LIB_DIR)"

# Platform conditions
ifeq ($(OS),Windows_NT)
	# Windows
	PLATFORM := windows
	SHELL := CMD
	STATIC_BUILD_FLAGS += -l$(WEBUI_LIB_NAME)-static -lws2_32 -Wall -luser32 -static
	COPY_LIB_CMD := @copy "$(LIB_DIR)\$(WEBUI_LIB_NAME).dll" "$(WEBUI_LIB_NAME).dll"
	DYN_BUILD_FLAGS += "$(WEBUI_LIB_NAME).dll" -lws2_32 -Wall -luser32
	STATIC_OUT := main.exe
	DYN_OUT := main-dyn.exe
	LWS2_OPT := -lws2_32
	STRIP_OPT := --strip-all
	CONSOLE_APP := -Wl,-subsystem=console
	GUI_APP := -Wl,-subsystem=windows
else
	STATIC_BUILD_FLAGS += -lpthread -lm -l$(WEBUI_LIB_NAME)-static
	DYN_BUILD_FLAGS += -lpthread -lm
	STATIC_OUT := main
	DYN_OUT := main-dyn
	CONSOLE_APP := 
	GUI_APP := 
	ifeq ($(shell uname),Darwin)
		# MacOS
		PLATFORM := macos
		CC = clang
		COPY_LIB_CMD := @cp "$(LIB_DIR)/$(WEBUI_LIB_NAME).dylib" "$(WEBUI_LIB_NAME).dylib"
		DYN_BUILD_FLAGS += "./$(WEBUI_LIB_NAME).dylib"
	else
		# Linux
		PLATFORM := linux
		COPY_LIB_CMD := @cp "$(LIB_DIR)/$(WEBUI_LIB_NAME).so" "$(WEBUI_LIB_NAME).so"
		DYN_BUILD_FLAGS += "./$(WEBUI_LIB_NAME).so"
		STRIP_OPT := --strip-all
		ifeq ($(CC),clang)
			LLVM_OPT := llvm-
		endif
	endif
endif

# == 2.TARGETS ================================================================

all: release

debug: --validate-args
ifeq ($(BUILD_LIB),true)
	@cd "$(PROJECT_DIR)" && $(MAKE) debug
endif
#	Static with Debug info
ifneq ($(WEBUI_USE_TLS), 1)
	@echo "Build C Example ($(CC) debug static)..."
	@$(CC) -g $(CONSOLE_APP) $(STATIC_BUILD_FLAGS) $(LWS2_OPT) -o $(STATIC_OUT)
endif
#	Dynamic with Debug info
	@echo "Build C Example ($(CC) debug dynamic)..."
	$(COPY_LIB_CMD)
	@$(CC) -g $(CONSOLE_APP) $(DYN_BUILD_FLAGS) $(LWS2_OPT) -o $(DYN_OUT)
#	Clean
ifeq ($(PLATFORM),windows)
	@- del *.o >nul 2>&1
else
	@- rm -f *.o
	@- rm -rf *.dSYM # macOS
endif
	@echo "Done."

release: --validate-args
ifeq ($(BUILD_LIB),true)
	@cd "$(PROJECT_DIR)" && $(MAKE)
endif
#	Static Release
ifneq ($(WEBUI_USE_TLS), 1)
	@echo "Build C Example ($(CC) release static)..."
	@$(CC) -Os $(GUI_APP) $(STATIC_BUILD_FLAGS) $(LWS2_OPT) -o $(STATIC_OUT)
	@$(LLVM_OPT)strip $(STRIP_OPT) $(STATIC_OUT)
endif
#	Dynamic Release
	@echo "Build C Example ($(CC) release dynamic)..."
	$(COPY_LIB_CMD)
	@$(CC) $(GUI_APP) $(DYN_BUILD_FLAGS) $(LWS2_OPT) -o $(DYN_OUT)
	@$(LLVM_OPT)strip $(STRIP_OPT) $(DYN_OUT)
#	Clean
ifeq ($(PLATFORM),windows)
	@- del *.o >nul 2>&1
else
	@- rm -f *.o
	@- rm -rf *.dSYM # macOS
endif
	@echo "Done."

clean: --clean-$(PLATFORM)

# INTERNAL TARGETS

--validate-args:
ifneq ($(filter $(CC),gcc clang aarch64-linux-gnu-gcc arm-linux-gnueabihf-gcc musl-gcc),$(CC))
$(error Invalid compiler specified: `$(CC)`)
endif

--clean-linux: --clean-unix

--clean-macos: --clean-unix

--clean-unix:
	- rm -f *.o
	- rm -f *.a
	- rm -f *.so
	- rm -f *.dylib
	- rm -rf *.dSYM

--clean-windows:
	- del *.o >nul 2>&1
	- del *.dll >nul 2>&1
	- del *.a >nul 2>&1
	- del *.exe >nul 2>&1
