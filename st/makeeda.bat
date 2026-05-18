@echo off
REM EDA-operator build with full Pharlap bind step (RUN).
if not exist MAKEFILE goto nodir
call make %1 %2 %3 %4 %5 %6 %7 %8 %9 -DEDA -DRUN
if errorlevel 1 goto failed
echo Build succeeded.
goto end
:nodir
echo ERROR: MAKEFILE not found. Run this from the st\ directory.
goto end
:failed
echo ERROR: Build failed.
:end
