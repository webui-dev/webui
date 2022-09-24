# Build WebUI Dynamic & Static library
# Windows - MSVC - Microsoft Visual C

all: static

dynamic:
	cl /Fomongoose.obj /c /EHsc "../../../src/mongoose.c" /I "../../../include"
	cl /Fowebui.obj /c /EHsc "../../../src/webui.c" /I "../../../include"
	link /DLL /OUT:webui-2-x64.dll webui.obj mongoose.obj
	cl "../../example.c" /I "../../../include" /link /MACHINE:X64 /SUBSYSTEM:WINDOWS webui-2-x64.lib /OUT:example_dynamic.exe
	- del *.obj >nul 2>&1
	- del example_dynamic.exp
	- del example_dynamic.lib

dynamic-debug:
	cl /Zi /Fomongoose.obj /c /EHsc "../../../src/mongoose.c" /I "../../../include"
	cl /Zi /Fowebui.obj /c /EHsc "../../../src/webui.c" /I "../../../include"
	link /DLL /OUT:webui-2-x64.dll webui.obj mongoose.obj
	cl /Zi "../../example.c" /I "../../../include" /link /MACHINE:X64 /SUBSYSTEM:CONSOLE webui-2-x64.lib /OUT:example_dynamic_debug.exe
	- del *.obj >nul 2>&1

static:
	cl /Fomongoose.obj /c /EHsc "../../../src/mongoose.c" /I "../../../include"
	cl /Fowebui.obj /c /EHsc "../../../src/webui.c" /I "../../../include"
	lib /OUT:webui-2-x64.lib webui.obj mongoose.obj
	cl "../../example.c" /I "../../../include" /link /MACHINE:X64 /SUBSYSTEM:WINDOWS webui-2-x64.lib /OUT:example_static.exe
	- del *.obj >nul 2>&1
	- del example_static.exp
	- del example_static.lib

static-debug:
	cl /Zi /Fomongoose.obj /c /EHsc "../../../src/mongoose.c" /I "../../../include"
	cl /Zi /Fowebui.obj /c /EHsc "../../../src/webui.c" /I "../../../include"
	lib /OUT:webui-2-x64.lib webui.obj mongoose.obj
	cl /Zi "../../example.c" /I "../../../include" /link /MACHINE:X64 /SUBSYSTEM:CONSOLE webui-2-x64.lib /OUT:example_static_debug.exe
	- del *.obj >nul 2>&1

clean:
	- del *.obj >nul 2>&1
	- del *.dll >nul 2>&1
	- del *.exe >nul 2>&1
	- del *.ilk >nul 2>&1
	- del *.pdb >nul 2>&1
	- del *.lib >nul 2>&1
	- del *.exp >nul 2>&1
