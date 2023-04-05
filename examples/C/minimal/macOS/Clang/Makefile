# WebUI Library 2.1.1
# C99 Example
# macOS - Clang

LIB=../../../../../build/macOS/Clang
INCLUDE=../../../../../include
SOURCE=../..

all: release

debug:
    # Build Lib
	@cd "$(LIB)" && $(MAKE) debug
    # Static with Debug info
	@echo "Build C99 Example (Static Debug)..."
	@clang -g -m64 -o main "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" -lwebui-2-static-x64 -lpthread -lm
    # Dynamic with Debug info
	@echo "Build C99 Example (Dynamic Debug)..."
	@clang -g -m64 -o main-dyn "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" "$(LIB)/webui-2-x64.dyn" -lpthread -lm
    # Clean
	@- rm -f *.o
	@- rm -rf *.dSYM
	@echo "Done."

release:
    # Build Lib
	@cd "$(LIB)" && $(MAKE)
    # Static Release
	@echo "Build C99 Example (Static Release)..."
	@clang -Os -m64 -o main "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" -lwebui-2-static-x64 -lpthread -lm
    # Dynamic Release
	@echo "Build C99 Example (Dynamic Release)..."
	@clang -m64 -o main-dyn "$(SOURCE)/main.c" -I "$(INCLUDE)" -L "$(LIB)" "$(LIB)/webui-2-x64.dyn" -lpthread -lm
    # Clean
	@- rm -f *.o
	@- rm -rf *.dSYM
	@echo "Done."

clean:
	- rm -f *.o
	- rm -f *.dyn
	- rm -f *.a
	- rm -rf *.dSYM

