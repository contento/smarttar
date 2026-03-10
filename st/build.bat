@echo off
if "%1" == "QUIET" goto quiet
choice Est  seguro
if errorlevel 2 goto end
:DoIt
del bin\st.exe
del *.sym
del obj\*.obj
call make -B -DRUN %1 %2 %3 %4 %5 %6 %7
goto end
:quiet
del bin\st.exe
del *.sym
del obj\*.obj
call make -B -DRUN    %2 %3 %4 %5 %6 %7
:end

