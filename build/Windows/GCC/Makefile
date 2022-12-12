# WebUI Library
# Windows - GNU Compiler Collection

SHELL=CMD
SOURCE=../../../src
INCLUDE=../../../include

all: release

debug:
#	Static with Debug info
	@echo Build WebUI Library (Debug Static)...
	@gcc -g -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c" -Wno-stringop-overread
	@gcc -g -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c" -DWEBUI_LOG
	@ar rc libwebui-2-static-x64.a webui.o mongoose.o
	@ranlib libwebui-2-static-x64.a
#	Dynamic with Debug info
	@echo Build WebUI Library (Debug Dynamic)...
	@gcc -g -fPIC -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c" -Wno-stringop-overread
	@gcc -g -fPIC -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c" -DWEBUI_LOG
	@gcc -g -shared -o webui-2-x64.dll webui.o mongoose.o -lws2_32
#	Clean
	@- del *.o >nul 2>&1
	@echo Done.

release:
#	Static Release
	@echo Build WebUI Library (Release Static)...
	@gcc -Os -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c" -Wno-stringop-overread
	@gcc -Os -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c"
	@ar rc libwebui-2-static-x64.a webui.o mongoose.o
	@ranlib libwebui-2-static-x64.a
#	Dynamic Release
	@echo Build WebUI Library (Release Dynamic)...
	@gcc -O3 -fPIC -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c" -Wno-stringop-overread
	@gcc -O3 -fPIC -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c"
	@gcc -shared -o webui-2-x64.dll webui.o mongoose.o -lws2_32
#	Clean
	@- del *.o >nul 2>&1
	@echo Done.

clean:
	- del *.o >nul 2>&1
	- del *.dll >nul 2>&1
	- del *.a >nul 2>&1
