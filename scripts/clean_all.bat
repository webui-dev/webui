@echo off

Set RootPath=%CD%\..\
cd "%RootPath%"

echo.
echo Clean all...

DEL /Q /F /S "*.exe" >nul 2>&1
DEL /Q /F /S "*.dll" >nul 2>&1
DEL /Q /F /S "*.lib" >nul 2>&1
DEL /Q /F /S "*.o" >nul 2>&1
DEL /Q /F /S "*.a" >nul 2>&1
DEL /Q /F /S "*.def" >nul 2>&1
DEL /Q /F /S "*.exp" >nul 2>&1
DEL /Q /F /S "*.pdb" >nul 2>&1
DEL /Q /F /S "*.ilk" >nul 2>&1
DEL /Q /F /S "*.obj" >nul 2>&1
DEL /Q /F /S "*.res" >nul 2>&1
DEL /Q /F /S "*.bak" >nul 2>&1
