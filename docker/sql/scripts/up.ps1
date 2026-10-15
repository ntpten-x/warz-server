# Start WarZ SQL Server container and seed databases
$ErrorActionPreference = "Stop"
$SqlDir = Split-Path $PSScriptRoot -Parent
Set-Location $SqlDir

Write-Host "==> docker compose up -d"
docker compose up -d

Write-Host "==> init databases"
bash ./scripts/init-db.sh

Write-Host ""
Write-Host "SQL ready: localhost,1433  (sa / see .env)"
Write-Host "Account:   local@warz.local / test123"
