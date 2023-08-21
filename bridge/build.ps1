$current_location = Get-Location
$project_root = git rev-parse --show-toplevel
Set-Location $project_root/bridge

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

if (!$silent) { Write-Host "Transpile and bundle TS sources to webui.js" }
esbuild --bundle --target="chrome90,firefox90,safari15" --format=esm --tree-shaking=false --outdir=. ./webui_bridge.ts $log_level

if (!$silent) { Write-Host "Convert JS source to C header" }
python3 js2c.py

if (!$silent) { Write-Host "Done." }
Set-Location $current_location
