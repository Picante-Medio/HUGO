@echo off
setlocal

set "BUILD_DIR=%~dp0HUGO\build"
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"

set "BIN_DIR=%~dp0HUGO\bin"
if exist "%BIN_DIR%" rmdir /s /q "%BIN_DIR%"

echo .
echo .
echo .

echo Cleanup completed
endlocal
exit /b 0