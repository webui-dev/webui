# WebUI Library 2.4.0
# C++ Example
# Linux - GCC

LIB=../../../../../dist
INCLUDE=../../../../../include
SOURCE=../..

all: release

debug:
#	Static with Debug info
	@echo "Build C++ Example (Static Debug)..."
	@g++ -std=c++17 -g -m64 -o main "$(SOURCE)/main.cpp" -I "$(INCLUDE)" -L "$(LIB)" -lwebui-2-static -lpthread -lm
#	Dynamic with Debug info
	@echo "Build C++ Example (Dynamic Debug)..."
	@g++ -std=c++17 -g -m64 -o main-dyn "$(SOURCE)/main.cpp" -I "$(INCLUDE)" -L "$(LIB)" "$(LIB)/webui-2.so" -lpthread -lm
#	Clean
	@- rm -f *.o
	@echo "Done."

release:
#	Static Release
	@echo "Build C++ Example (Static Release)..."
	@g++ -std=c++17 -Os -m64 -o main "$(SOURCE)/main.cpp" -I "$(INCLUDE)" -L "$(LIB)" -lwebui-2-static -lpthread -lm
	@strip --strip-all main
#	Dynamic Release
	@echo "Build C++ Example (Dynamic Release)..."
	@g++ -std=c++17 -m64 -o main-dyn "$(SOURCE)/main.cpp" -I "$(INCLUDE)" -L "$(LIB)" "$(LIB)/webui-2.so" -lpthread -lm
	@strip --strip-all main-dyn
#	Clean
	@- rm -f *.o
	@echo "Done."

clean:
	- rm -f *.o
	- rm -f *.so
	- rm -f *.a
