$ErrorActionPreference = 'Stop'

if (-not (Get-Command wsl -ErrorAction SilentlyContinue)) {
    throw 'WSL is not available. Install WSL2 (Ubuntu) or run `make run` in a Linux environment.'
}

Write-Host 'Running via WSL (QEMU in WSL)...'
wsl --cd "$(Resolve-Path .)" -- make run
