# WebUI Library
# Windows - Tiny C Compiler

SHELL=CMD
SOURCE=../../../src
INCLUDE=../../../include

all: release

debug:
#	Static with Debug info
	@echo Build WebUI Library (TCC Debug Static)...
	@tcc -g -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c" -w
	@tcc -g -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c" -DWEBUI_LOG -DWEBUI_NO_TLHELPER32 -w
	@tcc -m64 -ar rcs libwebui-2-static-x64.a webui.o mongoose.o
#	Dynamic with Debug info
	@echo Build WebUI Library (TCC Debug Dynamic)...
	@tcc -g -fPIC -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c" -w
	@tcc -g -fPIC -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c" -DWEBUI_LOG -DWEBUI_NO_TLHELPER32 -w
	@tcc -g -shared -o webui-2-x64.dll webui.o mongoose.o -lws2_32 -lAdvapi32
#	Clean
	@- del *.o >nul 2>&1
	@echo Done.

release:
#	Static Release
	@echo Build WebUI Library (TCC Release Static)...
	@tcc -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c" -w
	@tcc -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c" -DWEBUI_NO_TLHELPER32 -w
	@tcc -m64 -ar rcs libwebui-2-static-x64.a webui.o mongoose.o
#	Dynamic Release
	@echo Build WebUI Library (TCC Release Dynamic)...
	@tcc -fPIC -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c" -w
	@tcc -fPIC -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c" -DWEBUI_NO_TLHELPER32 -w
	@tcc -shared -o webui-2-x64.dll webui.o mongoose.o -lws2_32 -lAdvapi32
#	Clean
	@- del *.o >nul 2>&1
	@echo Done.

clean:
	- del *.o >nul 2>&1
	- del *.dll >nul 2>&1
	- del *.a >nul 2>&1
	- del *.def >nul 2>&1
