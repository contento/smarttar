@echo off
choice /T:Y,2 Are you sure
if errorlevel 2 goto end
:DoIt
genhelp source\help.txt help.dat
move help.dat bin
move help.hpp include
:end
