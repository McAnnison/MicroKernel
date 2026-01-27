$ErrorActionPreference = 'Stop'

if (-not (Get-Command wsl -ErrorAction SilentlyContinue)) {
    throw 'WSL is not available. Install WSL2 (Ubuntu) or run `make` in a Linux environment.'
}

Write-Host 'Building via WSL...'
wsl --cd "$(Resolve-Path .)" -- make
