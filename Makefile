# WebUI Library
# Windows - Microsoft Visual C

# == 1. VARIABLES =============================================================

# Build Flags
CIVETWEB_BUILD_FLAGS = /Fo"civetweb.obj" /c /EHsc "$(MAKEDIR)/src/civetweb/civetweb.c" /I"$(MAKEDIR)/src/civetweb/"
CIVETWEB_DEFINE_FLAGS = -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET
WEBUI_BUILD_FLAGS = /Fo"webui.obj" /c /EHsc "$(MAKEDIR)/src/webui.c" /I"$(MAKEDIR)/include"

# Output Commands
LIB_STATIC_OUT = /OUT:"webui-2-static.lib" "webui.obj" "civetweb.obj"
LIB_DYN_OUT = /DLL /OUT:"webui-2.dll" "webui.obj" "civetweb.obj" user32.lib Advapi32.lib Shell32.lib

# == 2.TARGETS ================================================================

all: release

debug:
	@- mkdir dist\debug >nul 2>&1
#	Static with Debug info
	@- cd $(MAKEDIR)/dist/debug
	@echo Build WebUI Library (MSVC Debug Static)...
	@cl /Zl /Zi $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS)
	@cl /Zl /Zi $(WEBUI_BUILD_FLAGS) /DWEBUI_LOG
	@lib $(LIB_STATIC_OUT)
#	Dynamic with Debug info
	@echo Build WebUI Library (MSVC Debug Dynamic)...
	@cl /Zi $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS)
	@cl /Zi $(WEBUI_BUILD_FLAGS) /DWEBUI_LOG
	@link $(LIB_DYN_OUT)
#	Move into `debug/` dir
	@- move webui-2-static.lib "debug\"
	@- move webui-2.dll "debug\"
#	Clean
	@- del *.pdb >nul 2>&1
	@- del *.obj >nul 2>&1
	@- del *.ilk >nul 2>&1
	@- del *.pdb >nul 2>&1
	@- del *.exp >nul 2>&1
	@echo Done.

release:
	@- mkdir dist >nul 2>&1
	@- cd $(MAKEDIR)/dist
#	Static Release
	@echo Build WebUI Library (MSVC Release Static)...
	@cl /Zl $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS)
	@cl /Zl $(WEBUI_BUILD_FLAGS)
	@lib $(LIB_STATIC_OUT)
#	Dynamic Release
	@echo Build WebUI Library (MSVC Release Dynamic)...
	@cl $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS)
	@cl $(WEBUI_BUILD_FLAGS)
	@link $(LIB_DYN_OUT)
#	Clean
	@- del *.pdb >nul 2>&1
	@- del *.obj >nul 2>&1
	@- del *.ilk >nul 2>&1
	@- del *.pdb >nul 2>&1
	@- del *.exp >nul 2>&1
	@- echo Done.

clean:
	@- cd $(BUILD_DIR)
	@- del *.pdb >nul 2>&1
	@- del *.obj >nul 2>&1
	@- del *.ilk >nul 2>&1
	@- del *.pdb >nul 2>&1
	@- del *.exp >nul 2>&1
	@- del *.dll >nul 2>&1
	@- del *.lib >nul 2>&1
	@- echo Done.
