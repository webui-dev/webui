# WebUI Library
# macOS - Clang

SOURCE=../../../src
INCLUDE=../../../include

all: release

debug:
	#	Static with Debug info
	@echo "Build WebUI Library (Debug Static)..."
	@clang -g -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@clang -DWEBUI_LOG -g -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c"
	@ar rc libwebui-2-static-x64.a webui.o mongoose.o
	@ranlib libwebui-2-static-x64.a
	#	Dynamic with Debug info
	@echo "Build WebUI Library (Debug Dynamic)..."
	@clang -g -fPIC -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@clang -DWEBUI_LOG -g -fPIC -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c"
	@clang -g -shared -o webui-2-x64.dyn webui.o mongoose.o
	#	Clean
	@- rm -f *.o
	@echo "Done."


release:
	#	Static Release
	@echo "Build WebUI Library (Release Static)..."
	@clang -Os -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@clang -Os -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c"
	@ar rc libwebui-2-static-x64.a webui.o mongoose.o
	@ranlib libwebui-2-static-x64.a
	#	Dynamic Release
	@echo "Build WebUI Library (Release Dynamic)..."
	@clang -O3 -fPIC -m64 -o mongoose.o -I "$(INCLUDE)" -c "$(SOURCE)/mongoose.c"
	@clang -O3 -fPIC -m64 -o webui.o -I "$(INCLUDE)" -c "$(SOURCE)/webui.c"
	@clang -shared -o webui-2-x64.dyn webui.o mongoose.o
	#	Clean
	@- rm -f *.o
	@echo "Done."

clean:
	- rm -f *.o
	- rm -f *.dylib
	- rm -f *.a
