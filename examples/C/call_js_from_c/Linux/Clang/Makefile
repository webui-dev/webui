# WebUI Library 2.1.1
# C99 Example
# Linux - Clang

LIB=../../../../../build/Linux/Clang
INCLUDE=../../../../../include
SOURCE=../..

all: release

debug:
#	Build Lib
	@cd "$(LIB)" && $(MAKE) debug
#	Static with Debug info
	@echo "Build C99 Example (Static Debug)..."
	@clang -g -static -m64 -o main "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" -lwebui-2-static-x64 -lpthread -lm
#	Dynamic with Debug info
	@echo "Build C99 Example (Dynamic Debug)..."
	@clang -g -m64 -o main-dyn "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" "$(LIB)/webui-2-x64.so" -lpthread -lm
#	Clean
	@- rm -f *.o
	@echo "Done."

release:
#	Build Lib
	@cd "$(LIB)" && $(MAKE)
#	Static Release
	@echo "Build C99 Example (Static Release)..."
	@clang -static -Os -m64 -o main "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" -lwebui-2-static-x64 -lpthread -lm
	@llvm-strip --strip-all main
#	Dynamic Release
	@echo "Build C99 Example (Dynamic Release)..."
	@clang -m64 -o main-dyn "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" "$(LIB)/webui-2-x64.so" -lpthread -lm
	@llvm-strip --strip-all main-dyn
#	Clean
	@- rm -f *.o
	@echo "Done."

clean:
	- rm -f *.o
	- rm -f *.so
	- rm -f *.a
