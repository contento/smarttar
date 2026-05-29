@echo off
set STTEST=1
if not exist util\ini2cfg\ini2cfg.exe goto noexe
util\ini2cfg\ini2cfg
goto end
:noexe
echo MK_CFG: util\ini2cfg\ini2cfg.exe not found -- build it first:
echo        cd util\ini2cfg ^&^& make ^&^& cd ..\..
:end
