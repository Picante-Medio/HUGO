@echo off
set "DIRECTORY=%~dp0HUGO\build"

del /f /s /q "%DIRECTORY%\*.*"
for /d %%p in ("%DIRECTORY%\*") do rmdir "%%p" /s /q

echo .
echo .
echo .

echo Cleanup completed
pause