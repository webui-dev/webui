# WebUI Library 2.x
# C99 Example
# Windows - GCC

SHELL=CMD
LIB=../../../../build/Windows/GCC
INCLUDE=../../../../include
SOURCE=../..

all: release

debug:
#	Build Lib
	@cd "$(LIB)" && $(MAKE) debug
#	Static with Debug info
	@echo Build C99 Example (Debug Static)...
	@windres win.rc -O coff -o win.res
	@gcc -g -static -m64 -o main.exe "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" win.res -lwebui-2-static-x64 -lws2_32 -Wall -Wl,-subsystem=console -luser32
#	Dynamic with Debug info
	@echo Build C99 Example (Debug Dynamic)...
	@windres win.rc -O coff -o win.res
	@gcc -g -m64 -o main-dyn.exe "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" win.res "$(LIB)/webui-2-x64.dll" -lws2_32 -Wall -Wl,-subsystem=console -luser32
#	Clean
	@- del *.o >nul 2>&1
	@- del *.res >nul 2>&1
	@echo Done.

release:
#	Build Lib
	@cd "$(LIB)" && $(MAKE)
#	Static Release
	@echo Build C99 Example (Release Static)...
	@windres win.rc -O coff -o win.res
	@gcc -static -Os -m64 -o main.exe "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" win.res -lwebui-2-static-x64 -lws2_32 -Wall -Wl,-subsystem=windows -luser32
	@strip --strip-all main.exe
#	Dynamic Release
	@echo Build C99 Example (Release Dynamic)...
	@windres win.rc -O coff -o win.res
	@gcc -m64 -o main-dyn.exe "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" win.res "$(LIB)/webui-2-x64.dll" -lws2_32 -Wall -Wl,-subsystem=windows -luser32
	@strip --strip-all main-dyn.exe
#	Clean
	@- del *.o >nul 2>&1
	@- del *.res >nul 2>&1
	@echo Done.

clean:
	- del *.o >nul 2>&1
	- del *.dll >nul 2>&1
	- del *.a >nul 2>&1
	- del *.exe >nul 2>&1
	- del *.res >nul 2>&1
