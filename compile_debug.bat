@echo off
setlocal

echo ========================================
echo    Compiling HUGO in DEBUG via script
echo ========================================

cd /d "%~dp0HUGO"

set LOG_DIR=build\logs
if not exist "%LOG_DIR%" mkdir "%LOG_DIR%"

cmake --preset VS_17_2022_x64 > "%LOG_DIR%\configure.log" 2>&1
if errorlevel 1 (
    echo Error in configuration step
    exit /b 1
)

cmake --build --preset DEBUG_VS_17_2022_x64 > "%LOG_DIR%\build.log" 2>&1
if errorlevel 1 (
    echo Error in compiling step
    exit /b 1
)

echo ======================================
echo     Compiling finished succesfully
echo ======================================

endlocal
pause