@echo off
setlocal

cd /d "%~dp0"

if "%CONFIG%"=="" set "CONFIG=Release"

where pwsh >nul 2>nul
if errorlevel 1 (
    set "POWERSHELL_EXE=powershell"
) else (
    set "POWERSHELL_EXE=pwsh"
)

"%POWERSHELL_EXE%" -NoProfile -ExecutionPolicy Bypass -File tools\build.ps1 -Config "%CONFIG%" %*
if errorlevel 1 exit /b 1

endlocal
