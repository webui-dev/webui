# Build WebUI Dynamic & Static library
# Windows - GCC - GNU Compiler Collection

all: static

dynamic:
	gcc -O3 -fPIC -m64 -o mongoose.o -I "../../../include" -c "../../../src/mongoose.c"
	gcc -O3 -fPIC -m64 -o webui.o -I "../../../include" -c "../../../src/webui.c"
	gcc -shared -o webui-2-x64.dll webui.o mongoose.o -lws2_32
	gcc -m64 -o example_dynamic.exe "../../example.c" -I "../../../include" -L. webui-2-x64.dll -lws2_32 -Wall -Wl,-subsystem=windows -luser32
	- del webui.o
	- del mongoose.o

dynamic-debug:
	gcc -g -fPIC -m64 -o mongoose.o -I "../../../include" -c "../../../src/mongoose.c"
	gcc -g -fPIC -m64 -o webui.o -I "../../../include" -c "../../../src/webui.c"
	gcc -g -shared -o webui-2-x64.dll webui.o mongoose.o -lws2_32
	gcc -g -m64 -o example_dynamic_debug.exe "../../example.c" -I "../../../include" -L. webui-2-x64.dll -lws2_32
	- del webui.o
	- del mongoose.o

static:
	gcc -Os -m64 -o mongoose.o -I "../../../include" -c "../../../src/mongoose.c"
	gcc -Os -m64 -o webui.o -I "../../../include" -c "../../../src/webui.c"
	ar rc libwebui-2-x64.a webui.o mongoose.o
	ranlib libwebui-2-x64.a
	gcc -static -Os -m64 -o example_static.exe "../../example.c" -I "../../../include" -L. -lwebui-2-x64 -lws2_32 -Wall -Wl,-subsystem=windows -luser32
	strip --strip-all example_static.exe
	- del webui.o
	- del mongoose.o

static-debug:
	gcc -g -m64 -o mongoose.o -I "../../../include" -c "../../../src/mongoose.c"
	gcc -g -m64 -o webui.o -I "../../../include" -c "../../../src/webui.c"
	ar rc libwebui-2-x64.a webui.o mongoose.o
	ranlib libwebui-2-x64.a
	gcc -g -m64 -o example_static_debug.exe "../../example.c" -I "../../../include" -L. -lwebui-2-x64 -lws2_32
	- del webui.o
	- del mongoose.o

clean:
	- del *.o >nul 2>&1
	- del *.dll >nul 2>&1
	- del *.a >nul 2>&1
	- del *.exe >nul 2>&1
