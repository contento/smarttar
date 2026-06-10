@echo off
REM demo_dos build: the buildable mini-smarttar variant.
REM DEMO + NODONGLE + full Pharlap bind step (RUN).  Add -DDEBUG (via
REM build.sh --debug) for symbols.  real_dos is excluded from the link.
if not exist MAKEFILE goto nodir
call make %1 %2 %3 %4 %5 %6 %7 %8 %9 -DDEMO -DRUN -DNODONGLE
if errorlevel 1 goto failed
echo Build succeeded.
goto end
:nodir
echo ERROR: MAKEFILE not found. Run this from the st\ directory.
goto end
:failed
echo ERROR: Build failed.
:end
