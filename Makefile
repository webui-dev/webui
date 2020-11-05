# --- Paths --------------------------------------------

# Microsoft build tools
MSVC_INCLUDE=E:/Hassan/Code/Projects/webui/thirdparty/msvc/include
MSVC_LIB=E:/Hassan/Code/Projects/webui/thirdparty/msvc/lib/x86

# Microsoft SDK
SDK_INCLUDE=E:/Hassan/Code/Projects/webui/thirdparty/sdk/include
SDK_LIB=E:/Hassan/Code/Projects/webui/thirdparty/sdk/lib

# ------------------------------------------------------

ifeq ($(OS),Windows_NT) 
    detected_OS := Windows
else
    detected_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

BOOST_INCLUDE=./thirdparty/boost/include
BOOST_LIB=./thirdparty/boost/lib
WEBUI_INCLUDE=./include
WEBUI_LIB=./lib/webui

$(info   )

ifeq ($(detected_OS),Windows)
#$(info [!] Detected OS Windows )
CC=cl.exe
LD=lib.exe
SubSys=WINDOWS
LibName=webui.lib
TestAppName=test.exe
CC_FLAGSFULL=/c /std:c++17 /EHsc /MT /errorReport:none /nologo /D WIN32 /D NDEBUG /D _WINDOWS /D _UNICODE /D UNICODE ./src/webui.cpp /I $(MSVC_INCLUDE) /I $(SDK_INCLUDE) /I $(BOOST_INCLUDE) /I $(WEBUI_INCLUDE) /link /LIBPATH:$(MSVC_LIB) /LIBPATH:$(SDK_LIB) /LIBPATH:$(BOOST_LIB)
LD_FLAGSFULL=webui.obj /nologo /OUT:$(WEBUI_LIB)/$(LibName)
CCFLAGS=/std:c++17 /EHsc /MT /TP /analyze- /errorReport:none /nologo /D WIN32 /D NDEBUG /D _WINDOWS /D _UNICODE /D UNICODE /I $(MSVC_INCLUDE) /I $(SDK_INCLUDE) /I $(BOOST_INCLUDE) /I $(WEBUI_INCLUDE) /link /LIBPATH:$(MSVC_LIB) /LIBPATH:$(SDK_LIB) /LIBPATH:$(BOOST_LIB) /LIBPATH:$(WEBUI_LIB) user32.lib kernel32.lib Shell32.lib Advapi32.lib Comdlg32.lib webui.lib ./resources/app.res /SUBSYSTEM:$(SubSys) /OPT:REF 
CLEAN=del *.obj
RESOURCES=rc ./resources/app.rc
OUTPUT=/OUT:
APP_EXTENTION=.exe
endif

ifeq ($(detected_OS),Linux)
$(info [!] Detected OS Linux )
CC=gcc
LD=ld
LibName=webui.a
TestAppName=
endif

ifeq ($(detected_OS),Darwin)
$(info [!] Detected OS macOS )
CC=clang
LD=ld
LibName=webui.a
TestAppName=test
endif

ifeq ($(detected_OS),Unknown)
$(info [!] Detected OS 'Unknown' )
$(info Detecting OS failed. )
exit 1
endif

all: webui.lib

webui.lib: webui.obj
	$(info   )
	@$(LD) $(LD_FLAGSFULL)
	$(info   WebUI Library: $(WEBUI_LIB)/$(LibName))

webui.obj:
	$(info   )
	$(info Compiling library.. )
	$(info   )
	$(CC) $(CC_FLAGSFULL)
	
test: webui.lib icon
	$(info   )
	$(info Compiling test program.. )
	$(info   )
	$(CC) ./test/test.cpp $(CCFLAGS) $(OUTPUT)./test/test$(APP_EXTENTION)
	./test/test$(APP_EXTENTION)
	
examples: webui.lib icon
	$(info   )
	$(info Compiling examples.. )
	$(info   )
	$(CC) ./examples/helloworld/helloworld.cpp $(CCFLAGS) $(OUTPUT)./examples/helloworld/helloworld$(APP_EXTENTION)

icon:
	$(info   )
	$(info Compiling icon resources.. )
	$(info   )
	$(RESOURCES)

clean:
	$(info Cleaning... )
	@$(CLEAN)
