# WebUI Library 2.2.0
# C++ Example
# Windows - Microsoft Visual C

SHELL=CMD
_LIB=../../../../../build/Windows/MSVC/
_INCLUDE=../../../../../include/
_SOURCE=../../

all: release

debug:
#	Build Lib
	@cd "$(_LIB)" && $(MAKE) debug
#	Static with Debug info
	@echo Build C++ Example (Debug Static)...
	@rc win.rc 1>NUL 2>&1
	@cl /Zi "$(_SOURCE)/main.cpp" /I "$(_INCLUDE)" /link /LIBPATH:"$(_LIB)" /MACHINE:X64 /SUBSYSTEM:CONSOLE win.res webui-2-static-x64.lib user32.lib Advapi32.lib /OUT:main.exe 1>NUL 2>&1
#	Dynamic with Debug info
	@echo Build C++ Example (Debug Dynamic)...
	@rc win.rc 1>NUL 2>&1
	@cl /Zi "$(_SOURCE)/main.cpp" /I "$(_INCLUDE)" /link /LIBPATH:"$(_LIB)" /MACHINE:X64 /SUBSYSTEM:CONSOLE win.res webui-2-x64.lib user32.lib Advapi32.lib /OUT:main-dyn.exe 1>NUL 2>&1

release:
#	Build Lib
	@cd "$(_LIB)" && $(MAKE)
#	Static Release
	@echo Build C++ Example (Release Static)...
	@rc win.rc 1>NUL 2>&1
	@cl "$(_SOURCE)/main.cpp" /I "$(_INCLUDE)" /link /LIBPATH:"$(_LIB)" /MACHINE:X64 /SUBSYSTEM:WINDOWS win.res webui-2-static-x64.lib user32.lib Advapi32.lib /OUT:main.exe 1>NUL 2>&1
#	Dynamic Release
	@echo Build C++ Example (Release Dynamic)...
	@rc win.rc 1>NUL 2>&1
	@cl "$(_SOURCE)/main.cpp" /I "$(_INCLUDE)" /link /LIBPATH:"$(_LIB)" /MACHINE:X64 /SUBSYSTEM:WINDOWS win.res webui-2-x64.lib user32.lib Advapi32.lib /OUT:main-dyn.exe 1>NUL 2>&1
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
