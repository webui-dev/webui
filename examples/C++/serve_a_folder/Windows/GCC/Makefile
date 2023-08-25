# WebUI Library 2.4.0
# C++ Example
# Windows - GCC

SHELL=CMD
LIB=../../../../../dist
INCLUDE=../../../../../include
SOURCE=../..

all: release

debug:
#	Build Lib
	@cd "$(LIB)" && $(MAKE) debug
#	Static with Debug info
	@echo Build C++ Example (Debug Static)...
	@windres win.rc -O coff -o win.res
	@g++ -std=c++17 -g -static -m64 -o main.exe "$(SOURCE)/main.cpp" -I "$(INCLUDE)" -L "$(LIB)" win.res -lwebui-2-static -lws2_32 -Wall -Wl,-subsystem=console -luser32
#	Dynamic with Debug info
	@echo Build C++ Example (Debug Dynamic)...
	@windres win.rc -O coff -o win.res
	@g++ -std=c++17 -g -m64 -o main-dyn.exe "$(SOURCE)/main.cpp" -I "$(INCLUDE)" -L "$(LIB)" win.res "$(LIB)/webui-2.dll" -lws2_32 -Wall -Wl,-subsystem=console -luser32
#	Clean
	@- del *.o >nul 2>&1
	@- del *.res >nul 2>&1
	@echo Done.

release:
#	Static Release
	@echo Build C++ Example (Release Static)...
	@windres win.rc -O coff -o win.res
	@g++ -std=c++17 -static -Os -m64 -o main.exe "$(SOURCE)/main.cpp" -I "$(INCLUDE)" -L "$(LIB)" win.res -lwebui-2-static -lws2_32 -Wall -Wl,-subsystem=windows -luser32
	@strip --strip-all main.exe
#	Dynamic Release
	@echo Build C++ Example (Release Dynamic)...
	@windres win.rc -O coff -o win.res
	@g++ -std=c++17 -m64 -o main-dyn.exe "$(SOURCE)/main.cpp" -I "$(INCLUDE)" -L "$(LIB)" win.res "$(LIB)/webui-2.dll" -lws2_32 -Wall -Wl,-subsystem=windows -luser32
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
