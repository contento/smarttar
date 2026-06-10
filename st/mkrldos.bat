@echo off
REM real_dos build: DEACTIVATED in mini-smarttar.  Compiling the real
REM engine / hardware (real_dos\) trips a #error unless REAL_DOS_ENABLED
REM is defined -- so this build is EXPECTED to fail.  It exists to make the
REM deactivation explicit; a developer working on real hardware code must
REM opt in with -DREAL_DOS_ENABLED.  See MINI_SMARTTAR_PLAN.
if not exist MAKEFILE goto nodir
call make %1 %2 %3 %4 %5 %6 %7 %8 %9 -DREAL_DOS -DRUN
if errorlevel 1 goto failed
echo Build succeeded.
goto end
:nodir
echo ERROR: MAKEFILE not found. Run this from the st\ directory.
goto end
:failed
echo ERROR: Build failed.
:end
