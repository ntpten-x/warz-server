@echo off
cd /d C:\warzlocal\docker\sql
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0up.ps1"
pause
