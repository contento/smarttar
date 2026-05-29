@echo off
REM Build PH_INFO.DAT from the .inf sources and place it in bin\.
REM Invoked from c:\st\ by build.sh / build.ps1 BEFORE make runs.
REM Not wired into MAKEFILE because Borland MAKE 3.6 mis-propagates
REM errorlevel from .bat-internal commands and deletes the
REM (just-created) target.  The bat works correctly when invoked
REM directly from DOSBox-X (i.e., the way build.sh does it).
set STTEST=1
if not exist util\inf2dat\inf2dat.exe goto noexe
util\inf2dat\inf2dat
if errorlevel 1 goto inf2dat_failed
if not exist util\inf2dat\PH_INFO.DAT goto missing_output
copy util\inf2dat\PH_INFO.DAT bin\PH_INFO.DAT > NUL
goto end
:noexe
echo MK_PH: util\inf2dat\inf2dat.exe not found -- build it first:
echo        cd util\inf2dat ^&^& make ^&^& cd ..\..
goto end
:inf2dat_failed
echo MK_PH: inf2dat exited with error
goto end
:missing_output
echo MK_PH: inf2dat ran but did not produce util\inf2dat\PH_INFO.DAT
:end
