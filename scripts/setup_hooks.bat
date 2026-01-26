@echo off

set "HOOKS_DIR=%~dp0..\.git\hooks"

if not exist "%HOOKS_DIR%" (
    echo CREATING hooks directory at %HOOKS_DIR%...
    mkdir "%HOOKS_DIR%"
)

echo INSTALLING Windows Git Hooks...

(
echo #!/bin/sh
echo powershell.exe -ExecutionPolicy Bypass -File "./scripts/update_badge.ps1"
) > "%HOOKS_DIR%\pre-commit"

(
echo #!/bin/sh
echo powershell.exe -ExecutionPolicy Bypass -File "./scripts/update_badge.ps1"
) > "%HOOKS_DIR%\post-merge"

echo INSTALLED hooks successfully.
