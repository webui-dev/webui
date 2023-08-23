# WebUI Library

# == 1. VARIABLES =============================================================

MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR  := $(dir $(MAKEFILE_PATH))
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
LIB_STATIC_OUT := libwebui-2-static-x64.a

# Platform defaults and dynamic library outputs
ifeq ($(OS),Windows_NT)
	# Windows
	PLATFORM := windows
	VALID_COMPILERS := gcc tcc
	LIB_DYN_OUT := webui-2-x64.dll
	LWS2_OPT := -lws2_32
	ifeq ($(COMPILER),tcc)
		BUILD_TARGET := --tcc
	else
		CIVETWEB_DEFINE_FLAGS += -DMUST_IMPLEMENT_CLOCK_GETTIME
		BUILD_TARGET := --gcc-clang
		COMPILER = gcc
	endif
else
	BUILD_TARGET := --gcc-clang
	ifeq ($(shell uname),Darwin)
		# MacOS
		PLATFORM := macos
		VALID_COMPILERS := clang
		LIB_DYN_OUT := webui-2-x64.dylib
		ifeq ($(COMPILER),)
			COMPILER = clang
		endif
	else
		# Linux
		PLATFORM := linux
		VALID_COMPILERS := gcc clang
		LIB_DYN_OUT := webui-2-x64.so
		ifeq ($(COMPILER),)
			COMPILER = gcc
		else ifeq ($(COMPILER),clang)
			LLVM_OPT := llvm-
		endif
	endif
endif

# == 2.TARGETS ================================================================

all: release

release: --setup $(BUILD_TARGET)-release

debug: --setup $(BUILD_TARGET)-debug

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
# If a compiler is specified check if it's valid
ifneq ($(filter $(COMPILER),$(VALID_COMPILERS)),$(COMPILER))
	$(error Invalid compiler specified: `$(COMPILER)`)
endif
# Arch target is for CI cross-compilation
ifneq ($(ARCH_TARGET),)
	ifneq ($(PLATFORM),macos)
		$(error ARCH_TARGET is only available on macOS)
	else ifeq ($(ARCH_TARGET),arm64-apple-darwin)
		ARCH_TARGET := -target $(ARCH_TARGET)
	endif
endif

# == 2.1.1 GCC / CLANG ========================================================

--gcc-clang-debug:
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

--gcc-clang-release:
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

# == 2.1.2 TCC ================================================================

--tcc-debug:
#	Static with Debug info
	@cd "$(BUILD_DIR)" \
	&& echo "Build WebUI library ($(COMPILER) debug static)..." \
	&& $(COMPILER) $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS) -g \
	&& $(COMPILER) $(WEBUI_BUILD_FLAGS) -g -w -DWEBUI_LOG \
	&& $(LLVM_OPT)ar rc $(LIB_STATIC_OUT) webui.o civetweb.o
ifeq ($(PLATFORM),windows)
# Specify `powershell` to call the command to prevent an issue with switching to bash in CI
	@cd "$(BUILD_DIR)" \
	&& powershell -command "Remove-Item -Path *.o -Force -ErrorAction SilentlyContinue"
else
	@- rm -f $(BUILD_DIR)/*.o
endif
	@echo "Done."

--tcc-release:
#	Static Release
	@cd "$(BUILD_DIR)" \
	&& echo "Build WebUI library ($(COMPILER) release static)..." \
	&& $(COMPILER) $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS) \
	&& $(COMPILER) $(WEBUI_BUILD_FLAGS) -w \
	&& $(LLVM_OPT)ar rc $(LIB_STATIC_OUT) webui.o civetweb.o
#	Clean
ifeq ($(PLATFORM),windows)
	@cd "$(BUILD_DIR)" \
	&& powershell -command "Remove-Item -Path *.o -Force -ErrorAction SilentlyContinue"
else
	@- rm -f $(BUILD_DIR)/*.o
endif
	@echo "Done."

# == 2.1.3 Platfrom Clean Targets =============================================

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
