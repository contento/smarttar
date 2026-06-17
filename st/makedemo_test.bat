@echo off
REM Build MiniDB test: DEMO + NODONGLE
call make bin\test_mdb.exe %1 %2 %3 %4 %5 %6 %7 %8 %9 -DDEMO -DRUN -DNODONGLE
if errorlevel 1 goto failed
echo test_mdb.exe built.
goto end
:nodir
echo ERROR: MAKEFILE not found. Run this from the st\ directory.
goto end
:failed
echo ERROR: Build failed.
:end
