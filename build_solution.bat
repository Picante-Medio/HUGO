@echo off
setlocal

echo ========================================
echo    Building HUGO sln
echo ========================================

cd /d "%~dp0HUGO"

set LOG_DIR=build\logs
if not exist "%LOG_DIR%" mkdir "%LOG_DIR%"

powershell -NoProfile -Command ^
  "cmake --preset VS_17_2022_x64 2>&1 | Tee-Object '%LOG_DIR%\configure.log'; exit $LASTEXITCODE"

if errorlevel 1 (
    echo Error building solution. Check "%LOG_DIR%\configure.log" for more information
    exit /b 1
)

echo(
echo ======================================
echo     Solution built succesfully
echo ======================================

endlocal
exit /b 0