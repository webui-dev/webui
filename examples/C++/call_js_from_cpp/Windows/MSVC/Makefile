# WebUI Library 2.4.0
# C++ Example
# Windows - Microsoft Visual C++

SHELL=CMD
_LIB=../../../../../dist
_INCLUDE=../../../../../include/
_SOURCE=../../

all: release

debug:
#	Static with Debug info
	@echo Build C++ Example (Debug Static)...
	@rc win.rc 1>NUL 2>&1
	@cl /Zi /EHsc /std:c++17 "$(_SOURCE)/main.cpp" /I "$(_INCLUDE)" /link /LIBPATH:"$(_LIB)" /MACHINE:X64 /SUBSYSTEM:CONSOLE win.res webui-2-static.lib user32.lib Advapi32.lib /OUT:main.exe 1>NUL 2>&1
#	Dynamic with Debug info
	@echo Build C++ Example (Debug Dynamic)...
	@rc win.rc 1>NUL 2>&1
	@cl /Zi /EHsc /std:c++17 "$(_SOURCE)/main.cpp" /I "$(_INCLUDE)" /link /LIBPATH:"$(_LIB)" /MACHINE:X64 /SUBSYSTEM:CONSOLE win.res webui-2.lib user32.lib Advapi32.lib /OUT:main-dyn.exe 1>NUL 2>&1

release:
#	Static Release
	@echo Build C++ Example (Release Static)...
	@rc win.rc 1>NUL 2>&1
	@cl /EHsc /std:c++17 "$(_SOURCE)/main.cpp" /I "$(_INCLUDE)" /link /LIBPATH:"$(_LIB)" /MACHINE:X64 /SUBSYSTEM:WINDOWS win.res webui-2-static.lib user32.lib Advapi32.lib /OUT:main.exe 1>NUL 2>&1
#	Dynamic Release
	@echo Build C++ Example (Release Dynamic)...
	@rc win.rc 1>NUL 2>&1
	@cl /EHsc /std:c++17 "$(_SOURCE)/main.cpp" /I "$(_INCLUDE)" /link /LIBPATH:"$(_LIB)" /MACHINE:X64 /SUBSYSTEM:WINDOWS win.res webui-2.lib user32.lib Advapi32.lib /OUT:main-dyn.exe 1>NUL 2>&1
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
