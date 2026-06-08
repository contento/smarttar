@echo off
set STTEST=1
if not exist util\ini2cfg\ini2cfg.exe goto build
:run
util\ini2cfg\ini2cfg
goto end
:build
echo MK_CFG: util\ini2cfg\ini2cfg.exe not found -- building it...
cd util\ini2cfg
make -DRUN
cd ..\..
if exist util\ini2cfg\ini2cfg.exe goto run
echo MK_CFG: failed to build util\ini2cfg\ini2cfg.exe
:end
