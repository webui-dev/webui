# WebUI Library
# Linux - GCC

SOURCE=../../../src
INCLUDE=../../../include
INCLUDE_CIVETWEB=../../../src/civetweb

all: release

debug:
#	Static with Debug info
	@echo "Build WebUI Library (GCC Debug Static)..."
	@gcc -g -m64 -o civetweb.o -I "$(INCLUDE)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET
	@gcc -g -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c" -DWEBUI_LOG
	@ar rc libwebui-2-static-x64.a webui.o civetweb.o
	@ranlib libwebui-2-static-x64.a
#	Dynamic with Debug info
	@echo "Build WebUI Library (GCC Debug Dynamic)..."
	@gcc -g -fPIC -m64 -o civetweb.o -I "$(INCLUDE)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET
	@gcc -g -fPIC -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c" -DWEBUI_LOG
	@gcc -g -shared -o webui-2-x64.so webui.o civetweb.o
#	Clean
	@- rm -f *.o
	@echo "Done."

release:
#	Static Release
	@echo "Build WebUI Library (GCC Release Static)..."
	@gcc -Os -m64 -o civetweb.o -I "$(INCLUDE)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET
	@gcc -Os -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c"
	@ar rc libwebui-2-static-x64.a webui.o civetweb.o
	@ranlib libwebui-2-static-x64.a
#	Dynamic Release
	@echo "Build WebUI Library (GCC Release Dynamic)..."
	@gcc -O3 -fPIC -m64 -o civetweb.o -I "$(INCLUDE)" -c "$(SOURCE)/civetweb/civetweb.c" -DNDEBUG -DNO_CACHING -DNO_CGI -DNO_SSL -DUSE_WEBSOCKET
	@gcc -O3 -fPIC -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c"
	@gcc -shared -o webui-2-x64.so webui.o civetweb.o
#	Clean
	@- rm -f *.o
	@echo "Done."

clean:
	- rm -f *.o
	- rm -f *.so
	- rm -f *.a
