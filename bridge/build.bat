@echo off
SETLOCAL EnableDelayedExpansion

REM Get current location and project root
cd %~dp0
FOR /F "tokens=*" %%i IN ('git rev-parse --show-toplevel') DO SET project_root=%%i
cd %project_root%/bridge

REM Check which python command is available
    SET python_cmd=python
    @REM python3 --version > NUL 2>&1
    @REM IF %errorlevel%==0 (
    @REM     SET python_cmd=python3
    @REM ) ELSE (
    @REM     python --version > NUL 2>&1
    @REM     IF %errorlevel%==0 (
    @REM         SET python_cmd=python
    @REM     ) ELSE (
    @REM         echo Error: Please install Python.
    @REM         cd %cd%
    @REM         exit /b
    @REM     )
    @REM )

REM Check if node_modules\esbuild exists, if not, install using npm
IF NOT EXIST "%project_root%\bridge\node_modules\esbuild\" (
    where npm > NUL 2>&1
    IF %errorlevel%==0 (
        echo Installing esbuild...
        npm install esbuild
    ) ELSE (
        echo Error: Please install NPM.
        cd %cd%
        exit /b
    )
)

REM Transpile WebUI-Bridge (TS to JS) & Convert WebUI-Bridge (JS to C)
echo Transpile and bundle WebUI-Bridge from TypeScript to JavaScript...
.\node_modules\.bin\esbuild --bundle --target="chrome90,firefox90,safari15" --format=esm --tree-shaking=false --minify-syntax --minify-whitespace --outdir=. ./webui_bridge.ts & %python_cmd% js2c.py

echo Done.
cd %cd%

ENDLOCAL
