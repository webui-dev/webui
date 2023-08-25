# WebUI Library 2.4.0
# C99 Example
# Linux - Clang

LIB=../../../../../dist
INCLUDE=../../../../../include
SOURCE=../..

all: release

debug:
#	Build Lib
	@cd "$(LIB)" && $(MAKE) debug
#	Static with Debug info
	@echo "Build C99 Example (Static Debug)..."
	@clang -g -m64 -o main "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" -lwebui-2-static -lpthread -lm
#	Dynamic with Debug info
	@echo "Build C99 Example (Dynamic Debug)..."
	@clang -g -m64 -o main-dyn "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" "$(LIB)/webui-2.so" -lpthread -lm
#	Clean
	@- rm -f *.o
	@echo "Done."

release:
#	Static Release
	@echo "Build C99 Example (Static Release)..."
	@clang -lstdc++ -Os -m64 -o main "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" -lwebui-2-static -lpthread -lm
	@llvm-strip --strip-all main
#	Dynamic Release
	@echo "Build C99 Example (Dynamic Release)..."
	@clang -lstdc++ -m64 -o main-dyn "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" "$(LIB)/webui-2.so" -lpthread -lm
	@llvm-strip --strip-all main-dyn
#	Clean
	@- rm -f *.o
	@echo "Done."

clean:
	- rm -f *.o
	- rm -f *.so
	- rm -f *.a
