# WebUI C++ Example
# Windows - Microsoft Visual C++

SHELL = CMD
LIB_DIR = ../../../dist
INCLUDE_DIR = ../../../include

# Build the WebUI library if running `nmake BUILD_LIB=true`
BUILD_LIB =

all: release

debug:
!IF "$(BUILD_LIB)" == "true"
	@cd "$(LIB_DIR)" && cd .. && $(MAKE) debug
!ENDIF
#	Static with Debug info
	@echo Build C++ Example (Debug Static)...
	@cl /Zi /EHsc /std:c++17 main.cpp /I"$(INCLUDE_DIR)" /link /LIBPATH:"$(LIB_DIR)/debug" /MACHINE:X64 /SUBSYSTEM:CONSOLE webui-2-static.lib user32.lib Advapi32.lib Shell32.lib /OUT:main.exe 1>NUL 2>&1
#	Dynamic with Debug info
	@echo Build C++ Example (Debug Dynamic)...
	@cl /Zi /EHsc /std:c++17 main.cpp /I"$(INCLUDE_DIR)" /link /LIBPATH:"$(LIB_DIR)/debug" /MACHINE:X64 /SUBSYSTEM:CONSOLE webui-2.lib user32.lib Advapi32.lib Shell32.lib /OUT:main-dyn.exe 1>NUL 2>&1
#	Clean
	@- del *.exp >nul 2>&1
	@- del *.ilk >nul 2>&1
	@- del *.lib >nul 2>&1
	@- del *.obj >nul 2>&1
	@- del *.pdb >nul 2>&1
	@echo Done.

release:
!IF "$(BUILD_LIB)" == "true"
	@cd "$(LIB_DIR)" && cd .. && $(MAKE)
!ENDIF
#	Static Release
	@echo Build C++ Example (Release Static)...
	@cl /EHsc /std:c++17 main.cpp /I"$(INCLUDE_DIR)" /link /LIBPATH:"$(LIB_DIR)" /MACHINE:X64 /SUBSYSTEM:WINDOWS webui-2-static.lib user32.lib Advapi32.lib Shell32.lib /OUT:main.exe 1>NUL 2>&1
#	Dynamic Release
	@echo Build C++ Example (Release Dynamic)...
	@cl /EHsc /std:c++17 main.cpp /I"$(INCLUDE_DIR)" /link /LIBPATH:"$(LIB_DIR)" /MACHINE:X64 /SUBSYSTEM:WINDOWS webui-2.lib user32.lib Advapi32.lib Shell32.lib /OUT:main-dyn.exe 1>NUL 2>&1
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
