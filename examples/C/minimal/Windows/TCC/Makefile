# WebUI Library 2.4.0
# C99 Example
# Windows - Tiny C Compiler

SHELL=CMD
LIB=../../../../../dist
INCLUDE=../../../../../include/
SOURCE=../../

all: release

debug:
#	Build Lib
	@cd "$(LIB)" && $(MAKE) debug
#	Static with Debug info
	@echo Build C99 Example (Static Debug)...
	@tcc -g -m64 -o main.exe "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" -lwebui-2-static -lws2_32 -lAdvapi32 -Wall -Wl,-subsystem=console -w -luser32
#	Dynamic with Debug info
	@echo Build C99 Example (Dynamic Debug)...
	@tcc -g -m64 -o main-dyn.exe "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" "$(LIB)/webui-2.def" -lws2_32 -lAdvapi32 -Wall -Wl,-subsystem=console -w -luser32
#	Clean
	@- del *.o >nul 2>&1
	@- del *.def >nul 2>&1
	@echo Done.

release:
#	Static Release
	@echo Build C99 Example (Static Release)...
	@tcc -m64 -o main.exe "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" -lwebui-2-static -lws2_32 -lAdvapi32 -Wall -Wl,-subsystem=windows -w -luser32
#	Dynamic Release
	@echo Build C99 Example (Dynamic Release)...
	@tcc -m64 -o main-dyn.exe "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" "$(LIB)/webui-2.def" -lws2_32 -lAdvapi32 -Wall -Wl,-subsystem=windows -w -luser32
#	Clean
	@- del *.o >nul 2>&1
	@- del *.def >nul 2>&1
	@echo Done.

clean:
	- del *.o >nul 2>&1
	- del *.def >nul 2>&1
	- del *.exe >nul 2>&1
