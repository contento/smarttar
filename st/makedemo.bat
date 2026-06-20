@echo off
REM Demo build: DEMO + NODONGLE with full Pharlap bind step (RUN).
if not exist MAKEFILE goto nodir
if not exist C:\VENDOR\BC\BIN\MAKE.EXE goto nomake
call make %1 %2 %3 %4 %5 %6 %7 %8 %9 -DDEMO -DRUN -DNODONGLE
if errorlevel 1 goto failed
echo Build succeeded.
goto end
:nomake
echo ERROR: MAKE.EXE not found in C:\VENDOR\BC\BIN\. Run: ./setup-vendor.sh
goto failed
:nodir
echo ERROR: MAKEFILE not found. Run this from the st\ directory.
goto end
:failed
echo ERROR: Build failed.
:end
