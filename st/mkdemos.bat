@echo off
mkdir build > NUL
mkdir bin > NUL
mkdir lib > NUL
if not exist MAKEFILE goto nodir
make %1 %2 %3 %4 %5 %6 %7 %8 %9 -DDEMO_DOS -DRUN > C:\bld.log
if errorlevel 1 goto failed
echo Build succeeded. > C:\bld.log
goto end
:nodir
echo ERROR: MAKEFILE not found. > C:\bld.log
goto end
:failed
echo ERROR: Build failed. > C:\bld.log
