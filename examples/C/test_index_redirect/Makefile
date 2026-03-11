# WebUI C Example
# Windows - Microsoft Visual C

SHELL = CMD
LIB_DIR = ../../../dist
INCLUDE_DIR = ../../../include
WEBUI_LIB_NAME = webui-2
!IF "$(WEBUI_USE_TLS)" == "1"
WEBUI_LIB_NAME = webui-2-secure
!ENDIF

# Build the WebUI library if running `nmake BUILD_LIB=true`
BUILD_LIB =

all: release

debug:
!IF "$(BUILD_LIB)" == "true"
	@cd "$(LIB_DIR)" && cd .. && $(MAKE) debug
!ENDIF
#	Static with Debug info
!IF "$(WEBUI_USE_TLS)" != "1"
	@echo Build C Example (Static Debug)...
	@cl /Zi main.c /I"$(INCLUDE_DIR)" /link /LIBPATH:"$(LIB_DIR)\debug" /SUBSYSTEM:CONSOLE $(WEBUI_LIB_NAME)-static.lib user32.lib Advapi32.lib Shell32.lib Ole32.lib /OUT:main.exe 1>NUL 2>&1
!ENDIF
#	Dynamic with Debug info
	@echo Build C Example (Dynamic Debug)...
	@copy "$(LIB_DIR)\debug\$(WEBUI_LIB_NAME).dll" "$(WEBUI_LIB_NAME).dll"
	@cl /Zi main.c /I"$(INCLUDE_DIR)" /link /LIBPATH:"$(LIB_DIR)\debug" /SUBSYSTEM:CONSOLE $(WEBUI_LIB_NAME).lib user32.lib Advapi32.lib Shell32.lib Ole32.lib /OUT:main-dyn.exe 1>NUL 2>&1
#	Clean
	@- del *.exp >nul 2>&1
	@- del *.ilk >nul 2>&1
	@- del *.lib >nul 2>&1
	@- del *.obj >nul 2>&1
	@echo Done.

release:
!IF "$(BUILD_LIB)" == "true"
	@cd "$(LIB_DIR)" && cd .. && $(MAKE)
!ENDIF
#	Static Release
!IF "$(WEBUI_USE_TLS)" != "1"
	@echo Build C Example (Static Release)...
	@cl main.c /I"$(INCLUDE_DIR)" /link /LIBPATH:"$(LIB_DIR)" /SUBSYSTEM:WINDOWS $(WEBUI_LIB_NAME)-static.lib user32.lib Advapi32.lib Shell32.lib Ole32.lib /OUT:main.exe 1>NUL 2>&1
!ENDIF
#	Dynamic Release
	@echo Build C Example (Dynamic Release)...
	@copy "$(LIB_DIR)\$(WEBUI_LIB_NAME).dll" "$(WEBUI_LIB_NAME).dll"
	@cl main.c /I"$(INCLUDE_DIR)" /link /LIBPATH:"$(LIB_DIR)" /SUBSYSTEM:WINDOWS $(WEBUI_LIB_NAME).lib user32.lib Advapi32.lib Shell32.lib Ole32.lib /OUT:main-dyn.exe 1>NUL 2>&1
#	Clean
	@- del *.exp >nul 2>&1
	@- del *.ilk >nul 2>&1
	@- del *.lib >nul 2>&1
	@- del *.obj >nul 2>&1
	@- del *.pdb >nul 2>&1
	@echo Done.

clean:
	- del *.obj >nul 2>&1
	- del *.ilk >nul 2>&1
	- del *.pdb >nul 2>&1
	- del *.exp >nul 2>&1
	- del *.exe >nul 2>&1
	- del *.lib >nul 2>&1
