# WebUI Library 2.1.0
# C99 Example
# Linux - GCC

LIB=../../../../../build/Linux/GCC
INCLUDE=../../../../../include
SOURCE=../..

all: release

debug:
#	Build Lib
	@cd "$(LIB)" && $(MAKE) debug
#	Static with Debug info
	@echo "Build C99 Example (Static Debug)..."
	@gcc -g -static -m64 -o main "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" -lwebui-2-static-x64 -lpthread -lm
#	Dynamic with Debug info
	@echo "Build C99 Example (Dynamic Debug)..."
	@gcc -g -m64 -o main-dyn "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" "$(LIB)/webui-2-x64.so" -lpthread -lm
#	Clean
	@- rm -f *.o
	@echo "Done."

release:
#	Build Lib
	@cd "$(LIB)" && $(MAKE)
#	Static Release
	@echo "Build C99 Example (Static Release)..."
	@gcc -static -Os -m64 -o main "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" -lwebui-2-static-x64 -lpthread -lm
	@strip --strip-all main
#	Dynamic Release
	@echo "Build C99 Example (Dynamic Release)..."
	@gcc -m64 -o main-dyn "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" "$(LIB)/webui-2-x64.so" -lpthread -lm
	@strip --strip-all main-dyn
#	Clean
	@- rm -f *.o
	@echo "Done."

clean:
	- rm -f *.o
	- rm -f *.so
	- rm -f *.a
