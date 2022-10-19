# WebUI Library 2.x
# C99 Example
# Windows - Microsoft Visual C

SHELL=CMD
_LIB=../../../../build/Windows/MSVC/
_INCLUDE=../../../../include/
_SOURCE=../../

all: release

debug:
#	Build Lib
	@cd "$(_LIB)" && $(MAKE) debug
#	Static with Debug info
	@echo Build C99 Example (Debug Static)...
	@rc win.rc
	@cl /Zi "$(_SOURCE)/main.c" /I "$(_INCLUDE)" /link /LIBPATH:"$(_LIB)" /MACHINE:X64 /SUBSYSTEM:CONSOLE win.res webui-2-static-x64.lib /OUT:main.exe
#	Dynamic with Debug info
	@echo Build C99 Example (Debug Dynamic)...
	@rc win.rc
	@cl /Zi "$(_SOURCE)/main.c" /I "$(_INCLUDE)" /link /LIBPATH:"$(_LIB)" /MACHINE:X64 /SUBSYSTEM:CONSOLE win.res webui-2-x64.lib /OUT:main-dyn.exe

release:
#	Build Lib
	@cd "$(_LIB)" && $(MAKE)
#	Static Release
	@echo Build C99 Example (Release Static)...
	@rc win.rc
	@cl "$(_SOURCE)/main.c" /I "$(_INCLUDE)" /link /LIBPATH:"$(_LIB)" /MACHINE:X64 /SUBSYSTEM:WINDOWS win.res webui-2-static-x64.lib /OUT:main.exe
#	Dynamic Release
	@echo Build C99 Example (Release Dynamic)...
	@rc win.rc
	@cl "$(_SOURCE)/main.c" /I "$(_INCLUDE)" /link /LIBPATH:"$(_LIB)" /MACHINE:X64 /SUBSYSTEM:WINDOWS win.res webui-2-x64.lib /OUT:main-dyn.exe
#	Clean
	@- del *.res >nul 2>&1
	@- del *.obj >nul 2>&1
	@- del *.exp >nul 2>&1
	@- del *.lib >nul 2>&1
	@echo Done.

clean:
	- del *.obj >nul 2>&1
	- del *.ilk >nul 2>&1
	- del *.pdb >nul 2>&1
	- del *.exp >nul 2>&1
	- del *.exe >nul 2>&1
	- del *.lib >nul 2>&1
	- del *.res >nul 2>&1
