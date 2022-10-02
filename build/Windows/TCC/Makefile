# WebUI Library
# Windows - Tiny C Compiler

SHELL=CMD
SOURCE=../../../src
INCLUDE=../../../include
EXAMPLES=../../../examples

all: release

debug:
	@echo Build WebUI (Debug Static)...
	@tcc -g -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@tcc -g -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c"
	@tcc -m64 -ar rcs libwebui-2-static-x64.a webui.o mongoose.o
	@echo Build WebUI (Debug Dynamic)...
	@tcc -g -fPIC -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@tcc -g -fPIC -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c"
	@tcc -g -shared -o webui-2-x64.dll webui.o mongoose.o -lws2_32
	@echo Done.

release:
	@echo Build WebUI (Release Static)...
	@tcc -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@tcc -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c"
	@tcc -m64 -ar rcs libwebui-2-static-x64.a webui.o mongoose.o
	@echo Build WebUI (Release Dynamic)...
	@tcc -fPIC -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@tcc -fPIC -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c"
	@tcc -shared -o webui-2-x64.dll webui.o mongoose.o -lws2_32
	@echo Done.

clean:
	- del *.o >nul 2>&1
	- del *.dll >nul 2>&1
	- del *.a >nul 2>&1
	- del *.def >nul 2>&1
