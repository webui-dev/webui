# WebUI Library
# Linux - Clang

SOURCE=../../../src
INCLUDE=../../../include

all: release

debug:
#	Static with Debug info
	@echo "Build WebUI Library (Clang Debug Static)..."
	@clang -g -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@clang -g -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c" -DWEBUI_LOG
	@llvm-ar rc libwebui-2-static-x64.a webui.o mongoose.o
	@llvm-ranlib libwebui-2-static-x64.a
#	Dynamic with Debug info
	@echo "Build WebUI Library (Clang Debug Dynamic)..."
	@clang -g -fPIC -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@clang -g -fPIC -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c" -DWEBUI_LOG
	@clang -g -shared -o webui-2-x64.so webui.o mongoose.o
#	Clean
	@- rm -f *.o
	@echo "Done."

release:
#	Static Release
	@echo "Build WebUI Library (Clang Release Static)..."
	@clang -Os -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@clang -Os -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c"
	@llvm-ar rc libwebui-2-static-x64.a webui.o mongoose.o
	@llvm-ranlib libwebui-2-static-x64.a
#	Dynamic Release
	@echo "Build WebUI Library (Clang Release Dynamic)..."
	@clang -O3 -fPIC -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@clang -O3 -fPIC -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c"
	@clang -shared -o webui-2-x64.so webui.o mongoose.o
#	Clean
	@- rm -f *.o
	@echo "Done."

clean:
	- rm -f *.o
	- rm -f *.so
	- rm -f *.a
