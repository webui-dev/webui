# WebUI Library
# Windows - Microsoft Visual C

all: release

debug:
	@echo Build WebUI (Debug Static)...
	@cl /Zi /Fomongoose.obj /c /EHsc "../../../src/mongoose.c" /I "../../../include" 1>NUL 2>&1
	@cl /Zi /Fowebui.obj /c /EHsc "../../../src/webui.c" /I "../../../include" 1>NUL 2>&1
	@lib /OUT:webui-2-static-x64.lib webui.obj mongoose.obj 1>NUL 2>&1
	@echo Build WebUI (Debug Dynamic)...
	@cl /Zi /Fomongoose.obj /c /EHsc "../../../src/mongoose.c" /I "../../../include" 1>NUL 2>&1
	@cl /Zi /Fowebui.obj /c /EHsc "../../../src/webui.c" /I "../../../include" 1>NUL 2>&1
	@link /DLL /OUT:webui-2-x64.dll webui.obj mongoose.obj 1>NUL 2>&1
	@echo Done.

release:
	@echo Build WebUI (Release Static)...
	@cl /Fomongoose.obj /c /EHsc "../../../src/mongoose.c" /I "../../../include" 1>NUL 2>&1
	@cl /Fowebui.obj /c /EHsc "../../../src/webui.c" /I "../../../include" 1>NUL 2>&1
	@lib /OUT:webui-2-static-x64.lib webui.obj mongoose.obj 1>NUL 2>&1
	@echo Build WebUI (Release Dynamic)...
	@cl /Fomongoose.obj /c /EHsc "../../../src/mongoose.c" /I "../../../include" 1>NUL 2>&1
	@cl /Fowebui.obj /c /EHsc "../../../src/webui.c" /I "../../../include" 1>NUL 2>&1
	@link /DLL /OUT:webui-2-x64.dll webui.obj mongoose.obj 1>NUL 2>&1
	@echo Done.

clean:
	- del *.obj >nul 2>&1
	- del *.dll >nul 2>&1
	- del *.ilk >nul 2>&1
	- del *.pdb >nul 2>&1
	- del *.lib >nul 2>&1
	- del *.exp >nul 2>&1
