@echo off
if not exist MAKEFILE goto nodir
make %1 %2 %3 %4 %5 %6 %7 %8 %9 -DDEMO_DOS -DRUN
if errorlevel 1 goto failed
echo Build succeeded.
goto end
:nodir
echo ERROR: MAKEFILE not found. Run this from the st\ directory.
goto end
:failed
echo ERROR: Build failed.
:end
