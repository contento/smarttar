@echo off
mkdir build > NUL
mkdir bin > NUL
mkdir lib > NUL
if not exist MAKEFILE goto nodir
call make %1 %2 %3 %4 %5 %6 %7 %8 %9 -DDEMO_DOS -DRUN
if errorlevel 1 goto failed
echo Build succeeded. > C:\build.log
goto end
:nodir
echo ERROR: MAKEFILE not found. Run this from the st\ directory. > C:\build.log
goto end
:failed
echo ERROR: Build failed. > C:\build.log
