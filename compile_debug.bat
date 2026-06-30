@echo off
setlocal

echo ========================================
echo    Compiling HUGO in DEBUG via script
echo ========================================

cd /d "%~dp0HUGO"

set LOG_DIR=build\logs
if not exist "%LOG_DIR%" mkdir "%LOG_DIR%"

echo(
echo ----------------------------------
echo    CMake configure step...
echo ----------------------------------
echo(

powershell -NoProfile -Command ^
  "cmake --preset VS_17_2022_x64 2>&1 | Tee-Object '%LOG_DIR%\configure.log'; exit $LASTEXITCODE"

if errorlevel 1 (
    echo Error in configuration step. Check "%LOG_DIR%\configure.log" for more information
    exit /b 1
)

echo(
echo ----------------------------------
echo    CMake compiling step...
echo ----------------------------------
echo(

powershell -NoProfile -Command ^
  "cmake --build --preset DEBUG_VS_17_2022_x64 2>&1 | Tee-Object '%LOG_DIR%\build.log'; exit $LASTEXITCODE"

if errorlevel 1 (
    echo Error in compiling step. Check "%LOG_DIR%\build.log" for more information
    exit /b 1
)

echo(
echo ======================================
echo     Compiling finished succesfully
echo ======================================

endlocal
exit /b 0