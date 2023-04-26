# WebUI Library
# Windows - Microsoft Visual C

all: release

debug:
#	Static with Debug info
	@echo Build WebUI Library (MSVC Debug Static)...
	@cl /Zl /Zi /Fomongoose.obj /c /EHsc "../../../src/mongoose.c" /I "../../../include" 1>NUL 2>&1
	@cl /Zl /Zi /Fowebui.obj /c /EHsc /DWEBUI_LOG "../../../src/webui.c" /I "../../../include" 1>NUL 2>&1
	@lib /OUT:webui-2-static-x64.lib webui.obj mongoose.obj 1>NUL 2>&1
#	Dynamic with Debug info
	@echo Build WebUI Library (MSVC Debug Dynamic)...
	@cl /Zi /Fomongoose.obj /c /EHsc "../../../src/mongoose.c" /I "../../../include" 1>NUL 2>&1
	@cl /Zi /Fowebui.obj /c /EHsc /DWEBUI_LOG "../../../src/webui.c" /I "../../../include" 1>NUL 2>&1
	@link /DLL /OUT:webui-2-x64.dll webui.obj mongoose.obj user32.lib Advapi32.lib 1>NUL 2>&1
#	Clean
	@- del *.obj >nul 2>&1
	@echo Done.

release:
#	Static Release
	@echo Build WebUI Library (MSVC Release Static)...
	@cl /Zl /Fomongoose.obj /c /EHsc "../../../src/mongoose.c" /I "../../../include" 1>NUL 2>&1
	@cl /Zl /Fowebui.obj /c /EHsc "../../../src/webui.c" /I "../../../include" 1>NUL 2>&1
	@lib /OUT:webui-2-static-x64.lib webui.obj mongoose.obj 1>NUL 2>&1
#	Dynamic Release
	@echo Build WebUI Library (MSVC Release Dynamic)...
	@cl /Fomongoose.obj /c /EHsc "../../../src/mongoose.c" /I "../../../include" 1>NUL 2>&1
	@cl /Fowebui.obj /c /EHsc "../../../src/webui.c" /I "../../../include" 1>NUL 2>&1
	@link /DLL /OUT:webui-2-x64.dll webui.obj mongoose.obj user32.lib Advapi32.lib 1>NUL 2>&1
#	Clean
	@- del *.obj >nul 2>&1
	@- del *.ilk >nul 2>&1
	@- del *.pdb >nul 2>&1
	@- del *.exp >nul 2>&1
	@echo Done.

clean:
	- del *.obj >nul 2>&1
	- del *.dll >nul 2>&1
	- del *.ilk >nul 2>&1
	- del *.pdb >nul 2>&1
	- del *.lib >nul 2>&1
	- del *.exp >nul 2>&1
