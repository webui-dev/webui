# http://webui.me
# https://github.com/webui-dev/webui
# Copyright (c) 2020-2024 Hassan Draga.
# Licensed under MIT License.
# All rights reserved.
#
# Special Thanks to Turiiya (https://github.com/ttytm)

Set-StrictMode -version latest
$ErrorActionPreference="Stop"

$current_location = Get-Location
$project_root = git rev-parse --show-toplevel
Set-Location $project_root/bridge
$silent=$false
$log_level=$null

# Arguments
foreach ($opt in $args) {
    switch ($opt) {
        "--silent" { $silent = $true }
        default {
            Write-Host "Invalid option: $opt"
            exit
        }
    }
}
if ($silent) { $log_level = "--log-level=warning" }

$ErrorActionPreference="SilentlyContinue"
# Check which python command is available
$commandResult = python3 --version 2>&1 > $null
if (!$?) {
    $commandResult = python --version 2>&1 > $null
    if (!$?) {
        Write-Host "Error: Please install Python."
        Set-Location $current_location
        exit
    }
    else {
        $python_cmd = "python"
    }
} else {
    $python_cmd = "python3"
}
$ErrorActionPreference="Stop"

# Check if node_modules\esbuild exists, if not, install using npm
if (-not (Test-Path "$project_root\bridge\node_modules\esbuild")) {
    if (Get-Command npm -ErrorAction SilentlyContinue) {
        if (!$silent) { Write-Host "Installing esbuild..." }
        npm install esbuild
    } else {
        Write-Host "Error: Please install NPM."
        Set-Location $current_location
        exit
    }
}

# Transpile WebUI-Bridge (TS to JS)
if (!$silent) { Write-Host "Transpile and bundle WebUI-Bridge from TypeScript to JavaScript..." }
.\node_modules\.bin\esbuild --bundle --target="chrome90,firefox90,safari15" --format=esm --tree-shaking=false --minify-syntax --minify-whitespace --outdir=. ./webui_bridge.ts $log_level

# Convert WebUI-Bridge (JS to C)
if (!$silent) { Write-Host "Convert WebUI-Bridge JavaScript to C Header..." }
& $python_cmd js2c.py

# Done
if (!$silent) { Write-Host "Done." }
Set-Location $current_location
