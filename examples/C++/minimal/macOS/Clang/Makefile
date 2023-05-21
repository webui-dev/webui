# WebUI Library 2.3.0
# C++ Example
# macOS - Clang

LIB=../../../../../build/macOS/Clang
INCLUDE=../../../../../include
SOURCE=../..

all: release

debug:
    # Build Lib
	@cd "$(LIB)" && $(MAKE) debug
    # Static with Debug info
	@echo "Build C++ Example (Static Debug)..."
	@clang -std=c++11 -g -m64 -o main "$(SOURCE)/main.cpp" -I "$(INCLUDE)" -L "$(LIB)" -lwebui-2-static-x64 -lpthread -lm
    # Dynamic with Debug info
	@echo "Build C++ Example (Dynamic Debug)..."
	@clang -std=c++11 -g -m64 -o main-dyn "$(SOURCE)/main.cpp" -I "$(INCLUDE)" -L "$(LIB)" "$(LIB)/webui-2-x64.dyn" -lpthread -lm
    # Clean
	@- rm -f *.o
	@- rm -rf *.dSYM
	@echo "Done."

release:
    # Build Lib
	@cd "$(LIB)" && $(MAKE)
    # Static Release
	@echo "Build C++ Example (Static Release)..."
	@clang -std=c++11 -lstdc++ -Os -m64 -o main "$(SOURCE)/main.cpp" -I "$(INCLUDE)" -L "$(LIB)" -lwebui-2-static-x64 -lpthread -lm
    # Dynamic Release
	@echo "Build C++ Example (Dynamic Release)..."
	@clang -std=c++11 -lstdc++ -m64 -o main-dyn "$(SOURCE)/main.cpp" -I "$(INCLUDE)" -L "$(LIB)" "$(LIB)/webui-2-x64.dyn" -lpthread -lm
    # Clean
	@- rm -f *.o
	@- rm -rf *.dSYM
	@echo "Done."

clean:
	- rm -f *.o
	- rm -f *.dyn
	- rm -f *.a
	- rm -rf *.dSYM

