@echo off
set STTEST=1
if not exist util\inf2dat\inf2dat.exe goto noexe
util\inf2dat\inf2dat
goto end
:noexe
echo MK_PH: util\inf2dat\inf2dat.exe not found -- build it first:
echo        cd util\inf2dat ^&^& make ^&^& cd ..\..
:end
