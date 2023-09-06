# WebUI Library

# == 1. VARIABLES =============================================================

MAKEFILE_DIR  := $(shell git rev-parse --show-toplevel)
BUILD_DIR  := $(MAKEFILE_DIR)/dist

# Args
# Allow to add arch-target for macOS CI cross compilation
ARCH_TARGET ?=
COMPILER ?=

# Build flags
CIVETWEB_BUILD_FLAGS := -m64 -o civetweb.o -I "$(MAKEFILE_DIR)/include/" -c "$(MAKEFILE_DIR)/src/civetweb/civetweb.c"
CIVETWEB_DEFINE_FLAGS = -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET
WEBUI_BUILD_FLAGS := -m64 -o webui.o -I "$(MAKEFILE_DIR)/include/" -c "$(MAKEFILE_DIR)/src/webui.c"

# Output files
# The static output is the same for all platforms
# The dynamic output is platform dependent
LIB_STATIC_OUT := libwebui-2-static.a

# Platform defaults and dynamic library outputs
ifeq ($(OS),Windows_NT)
	# Windows
	PLATFORM := windows
	LIB_DYN_OUT := webui-2.dll
	LWS2_OPT := -lws2_32
	CIVETWEB_DEFINE_FLAGS += -DMUST_IMPLEMENT_CLOCK_GETTIME
	COMPILER = gcc
else
	ifeq ($(shell uname),Darwin)
		# MacOS
		PLATFORM := macos
		LIB_DYN_OUT := webui-2.dylib
		COMPILER = clang
	else
		# Linux
		PLATFORM := linux
		LIB_DYN_OUT := webui-2.so
		ifeq ($(COMPILER),)
			COMPILER = gcc
		else ifeq ($(COMPILER),clang)
			LLVM_OPT := llvm-
		endif
	endif
endif

# == 2.TARGETS ================================================================

all: release

release: --setup --release

debug: --setup --debug

clean: --clean-$(PLATFORM)

# == 2.1 Internal Targets =====================================================

# Prepare build dir and evaluate input arguments
--setup:
# Create build directory
ifeq ($(PLATFORM),windows)
	@mkdir "$(BUILD_DIR)" >nul 2>&1 ||:
else
	@mkdir -p "$(BUILD_DIR)"
endif
# Linux can set a COMPILER
ifeq ($(PLATFORM),linux)
ifneq ($(filter $(COMPILER),gcc clang),$(COMPILER))
	$(error Invalid compiler specified: `$(COMPILER)`)
endif
endif
# macOS can set an ARCH_TARGET - for CI cross-compilation
ifneq ($(ARCH_TARGET),)
ifneq ($(PLATFORM),macos)
	$(error ARCH_TARGET is only available on macOS)
endif
# WARN: Wrong input is not covered yet due to difficulties with differing behavior on Mac
# ARCH_TARGET is intented for CI use. Valid input is `ARCH_TARGET="-target arm64-apple-darwin"`.
endif

--debug:
#	Static with Debug info
	@cd "$(BUILD_DIR)" \
	&& echo "Build WebUI library ($(COMPILER) debug static)..." \
	&& $(COMPILER) $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS) -g \
	&& $(COMPILER) $(WEBUI_BUILD_FLAGS) -g -DWEBUI_LOG \
	&& $(LLVM_OPT)ar rc $(LIB_STATIC_OUT) webui.o civetweb.o \
	&& $(LLVM_OPT)ranlib $(LIB_STATIC_OUT)
#	Dynamic with Debug info
	@cd "$(BUILD_DIR)" \
	&& echo "Build WebUI library ($(COMPILER) debug dynamic)..." \
	&& $(COMPILER) $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS) -g -fPIC \
	&& $(COMPILER) $(WEBUI_BUILD_FLAGS) -g -fPIC -DWEBUI_LOG \
	&& $(COMPILER) -shared -o $(LIB_DYN_OUT) webui.o civetweb.o -g $(LWS2_OPT)
ifeq ($(PLATFORM),windows)
	@strip --strip-unneeded "$(BUILD_DIR)/$(LIB_DYN_OUT)"
	@cd "$(BUILD_DIR)" \
	&& powershell -command "Remove-Item -Path *.o -Force -ErrorAction SilentlyContinue"
else
	@- rm -f $(BUILD_DIR)/*.o
endif
	@echo "Done."

--release:
#	Static Release
	@cd "$(BUILD_DIR)" \
	&& echo "Build WebUI library ($(COMPILER) release static)..." \
	&& $(COMPILER) $(ARCH_TARGET) $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS) -Os \
	&& $(COMPILER) $(ARCH_TARGET) $(WEBUI_BUILD_FLAGS) -Os \
	&& $(LLVM_OPT)ar rc $(LIB_STATIC_OUT) webui.o civetweb.o \
	&& $(LLVM_OPT)ranlib $(LIB_STATIC_OUT)
#	Dynamic Release
	@cd "$(BUILD_DIR)" \
	&& echo "Build WebUI library ($(COMPILER) release dynamic)..." \
	&& $(COMPILER) $(ARCH_TARGET) $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS) -Os -fPIC \
	&& $(COMPILER) $(ARCH_TARGET) $(WEBUI_BUILD_FLAGS) -O3 -fPIC \
	&& $(COMPILER) $(ARCH_TARGET) -shared -o $(LIB_DYN_OUT) webui.o civetweb.o $(LWS2_OPT)
#	Clean
ifeq ($(PLATFORM),windows)
	@strip --strip-unneeded $(BUILD_DIR)/$(LIB_DYN_OUT)
	@cd "$(BUILD_DIR)" \
	&& powershell -command "Remove-Item -Path *.o -Force -ErrorAction SilentlyContinue"
else
	@- rm -f $(BUILD_DIR)/*.o
endif
	@echo "Done."

# Platfrom Clean Targets

--clean-linux: --clean-unix

--clean-macos: --clean-unix

--clean-unix:
	@- cd "$(BUILD_DIR)" \
	&& rm -f *.o \
	&& rm -f *.so \
	&& rm -f *.dylib \
	&& rm -f *.a

--clean-windows:
	@- cd "$(BUILD_DIR)" \
	&& del *.a >nul 2>&1 \
	&& del *.o >nul 2>&1 \
	&& del *.dll >nul 2>&1 \
	&& del *.obj >nul 2>&1 \
	&& del *.ilk >nul 2>&1 \
	&& del *.pdb >nul 2>&1 \
	&& del *.lib >nul 2>&1 \
	&& del *.exp >nul 2>&1
