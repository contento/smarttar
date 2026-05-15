@echo off
if not exist MAKEFILE goto nodir
call make %1 %2 -DDEMO -DRUN -DNODONGLE
if errorlevel 1 goto failed
echo Build succeeded.
goto end
:nodir
echo ERROR: MAKEFILE not found. Run from the st\ directory.
goto end
:failed
echo ERROR: Build failed.
:end
