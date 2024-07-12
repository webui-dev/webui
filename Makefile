# WebUI Library
# Windows - Microsoft Visual C

# == 1. VARIABLES =============================================================

WEBUI_OUT_LIB_NAME = webui-2
TLS_CFLAG = /DNO_SSL

# TLS Enabled
!IF "$(WEBUI_USE_TLS)" == "1"

WEBUI_OUT_LIB_NAME = webui-2-secure
TLS_CFLAG = /DWEBUI_TLS /DNO_SSL_DL /DOPENSSL_API_1_1
TLS_LDFLAG_DYNAMIC = libssl.lib libcrypto.lib

!IF "$(WEBUI_TLS_INCLUDE)" != ""
TLS_CFLAG = $(TLS_CFLAG) /I"$(WEBUI_TLS_INCLUDE)"
!ELSE
TLS_CFLAG = $(TLS_CFLAG) /I"."
!ENDIF

!IF "$(WEBUI_TLS_LIB)" != ""
TLS_LDFLAG_DYNAMIC = $(TLS_LDFLAG_DYNAMIC) /LIBPATH:"$(WEBUI_TLS_LIB)"
!ELSE
TLS_LDFLAG_DYNAMIC += $(TLS_LDFLAG_DYNAMIC) /LIBPATH:"."
!ENDIF

!ENDIF

# Build Flags
CIVETWEB_BUILD_FLAGS = /Fo"civetweb.obj" /c /EHsc "$(MAKEDIR)/src/civetweb/civetweb.c" /I"$(MAKEDIR)/src/civetweb/" $(TLS_CFLAG)
CIVETWEB_DEFINE_FLAGS = /D NDEBUG /D NO_CACHING /D NO_CGI /D USE_WEBSOCKET
WEBUI_BUILD_FLAGS = /Fo"webui.obj" /c /EHsc "$(MAKEDIR)/src/webui.c" /I"$(MAKEDIR)/include" /I"$(WEBUI_TLS_INCLUDE)" $(TLS_CFLAG)
WARNING_RELEASE = /w
WARNING_LOG = /W4

# Output Commands
LIB_STATIC_OUT = /OUT:"$(WEBUI_OUT_LIB_NAME)-static.lib" "webui.obj" "civetweb.obj"
LIB_DYN_OUT = /DLL /OUT:"$(WEBUI_OUT_LIB_NAME).dll" "webui.obj" "civetweb.obj" user32.lib Advapi32.lib Shell32.lib Ole32.lib $(TLS_LDFLAG_DYNAMIC)

# == 2.TARGETS ================================================================

all: release

debug:
	@- mkdir dist\debug >nul 2>&1
#	Static with Debug info
	@- cd $(MAKEDIR)/dist/debug
	@echo Build WebUI Library (MSVC Debug Static)...
	@cl /Zl /Zi $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS)
	@cl /Zl /Zi $(WEBUI_BUILD_FLAGS) $(WARNING_LOG) /D WEBUI_LOG
	@lib $(LIB_STATIC_OUT)
#	Dynamic with Debug info
	@echo Build WebUI Library (MSVC Debug Dynamic)...
	@cl /Zi $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS)
	@cl /Zi $(WEBUI_BUILD_FLAGS) $(WARNING_LOG) /D WEBUI_LOG /D WEBUI_DYNAMIC
	@link $(LIB_DYN_OUT)
#	Clean
	@- del *.obj >nul 2>&1
	@- del *.ilk >nul 2>&1
	@- del *.exp >nul 2>&1
	@echo Done.

release:
	@- mkdir dist >nul 2>&1
	@- cd $(MAKEDIR)/dist
#	Static Release
	@echo Build WebUI Library (MSVC Release Static)...
	@cl /Zl $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS)
	@cl /Zl $(WEBUI_BUILD_FLAGS) $(WARNING_RELEASE)
	@lib $(LIB_STATIC_OUT)
#	Dynamic Release
	@echo Build WebUI Library (MSVC Release Dynamic)...
	@cl $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS)
	@cl $(WEBUI_BUILD_FLAGS) $(WARNING_RELEASE) /D WEBUI_DYNAMIC
	@link $(LIB_DYN_OUT)
#	Clean
	@- del *.pdb >nul 2>&1
	@- del *.obj >nul 2>&1
	@- del *.ilk >nul 2>&1
	@- del *.exp >nul 2>&1
	@- echo Done.

clean:
	@- cd $(BUILD_DIR)
	@- del *.pdb >nul 2>&1
	@- del *.obj >nul 2>&1
	@- del *.ilk >nul 2>&1
	@- del *.exp >nul 2>&1
	@- del *.dll >nul 2>&1
	@- del *.lib >nul 2>&1
	@- echo Done.
