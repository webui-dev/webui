# WebUI C Starter Kit
# Windows - Microsoft Visual C (nmake, from a Developer Command Prompt)
#
#   nmake          Build a single portable executable into bin\
#   nmake debug    Same, with a console window and debug symbols
#   nmake clean    Remove generated build files (keeps the lib\ snapshot)
#
# The first build must run inside the WebUI repository (examples\starter_kit_c)
# so the WebUI static library can be copied into lib\. After that, this folder
# is fully self-contained and can be moved anywhere.

SHELL = CMD

# Your application name (the executable name in bin\)
APP_NAME = my_app

# WebUI static library file name
LIB_FILE = webui-2-static.lib

# WebUI repository paths (only used when building inside the repository)
WEBUI_DIST = ..\..\dist
WEBUI_INCLUDE = ..\..\include

all: release

release: prepare
	@echo Build $(APP_NAME) (Release)...
	@cl /nologo /O2 src\main.c icon.res /I"lib" /link /LIBPATH:"lib" /SUBSYSTEM:WINDOWS $(LIB_FILE) user32.lib Advapi32.lib Shell32.lib Ole32.lib /OUT:bin\$(APP_NAME).exe
	@- del main.obj >nul 2>&1
	@- del icon.res >nul 2>&1
	@echo Done: bin\$(APP_NAME).exe

debug: prepare
	@echo Build $(APP_NAME) (Debug)...
	@cl /nologo /Zi src\main.c icon.res /I"lib" /link /LIBPATH:"lib" /SUBSYSTEM:CONSOLE $(LIB_FILE) user32.lib Advapi32.lib Shell32.lib Ole32.lib /OUT:bin\$(APP_NAME).exe
	@- del main.obj >nul 2>&1
	@- del icon.res >nul 2>&1
	@echo Done: bin\$(APP_NAME).exe

# Prepare: snapshot the WebUI library into lib\, pack ui\ into src\vfs.h,
# and compile the icon resource.
prepare:
	@if not exist "bin" mkdir "bin"
	@if not exist "lib" mkdir "lib"
	@if exist "$(WEBUI_DIST)\$(LIB_FILE)" copy /Y "$(WEBUI_DIST)\$(LIB_FILE)" "lib" >nul
	@if exist "$(WEBUI_INCLUDE)\webui.h" copy /Y "$(WEBUI_INCLUDE)\webui.h" "lib" >nul
	@if not exist "lib\$(LIB_FILE)" (echo [Error] WebUI static library not found. Run 'nmake' in the WebUI repository root first. & exit /b 1)
	@echo Pack ui\ into src\vfs.h...
	@cl /nologo tools\vfs.c /Fetools\vfs.exe /Fotools\vfs.obj
	@tools\vfs.exe ui src\vfs.h
	@- del tools\vfs.obj >nul 2>&1
	@echo Build icon resource...
	@rc /nologo /fo icon.res icon.rc

clean:
	- rmdir /S /Q bin >nul 2>&1
	- del src\vfs.h >nul 2>&1
	- del icon.res >nul 2>&1
	- del tools\vfs.exe >nul 2>&1
	- del tools\vfs.obj >nul 2>&1
	- del *.obj >nul 2>&1
	- del *.pdb >nul 2>&1
	- del *.ilk >nul 2>&1
	@echo Clean done (lib\ snapshot kept).
