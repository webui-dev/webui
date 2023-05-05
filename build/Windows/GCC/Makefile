# WebUI Library
# Windows - GNU Compiler Collection

SHELL=CMD
SOURCE=../../../src
INCLUDE=../../../include
INCLUDE_CIVETWEB=../../../src/civetweb

all: release

debug:
#	Static with Debug info
	@echo Build WebUI Library (GCC Debug Static)...
	@gcc -g -m64 -o civetweb.o -I "$(INCLUDE_CIVETWEB)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET -DMUST_IMPLEMENT_CLOCK_GETTIME
	@gcc -g -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c" -DWEBUI_LOG
	@ar rc libwebui-2-static-x64.a webui.o civetweb.o
	@ranlib libwebui-2-static-x64.a
#	Dynamic with Debug info
	@echo Build WebUI Library (GCC Debug Dynamic)...
	@gcc -g -fPIC -m64 -o civetweb.o -I "$(INCLUDE_CIVETWEB)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET -DMUST_IMPLEMENT_CLOCK_GETTIME
	@gcc -g -fPIC -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c" -DWEBUI_LOG
	@gcc -g -shared -o webui-2-x64.dll webui.o civetweb.o -lws2_32
#	Clean
	@- del *.o >nul 2>&1
	@echo Done.

release:
#	Static Release
	@echo Build WebUI Library (GCC Release Static)...
	@gcc -Os -m64 -o civetweb.o -I "$(INCLUDE_CIVETWEB)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET -DMUST_IMPLEMENT_CLOCK_GETTIME
	@gcc -Os -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c"
	@ar rc libwebui-2-static-x64.a webui.o civetweb.o
	@ranlib libwebui-2-static-x64.a
#	Dynamic Release
	@echo Build WebUI Library (GCC Release Dynamic)...
	@gcc -Os -fPIC -m64 -o civetweb.o -I "$(INCLUDE_CIVETWEB)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET -DMUST_IMPLEMENT_CLOCK_GETTIME
	@gcc -O3 -fPIC -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c"
	@gcc -shared -o webui-2-x64.dll webui.o civetweb.o -lws2_32
	@strip --strip-unneeded webui-2-x64.dll
#	Clean
	@- del *.o >nul 2>&1
	@echo Done.

clean:
	- del *.o >nul 2>&1
	- del *.dll >nul 2>&1
	- del *.a >nul 2>&1
