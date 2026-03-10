@echo off
choice Are you sure
if errorlevel 2 goto end
:DoIt
sweep del *.sym
sweep del *.obj
sweep del *.bak
sweep del *.lib
sweep del *.swp
sweep del *.exe
sweep del *.twd
:end

