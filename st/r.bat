@echo off
REM choice Are you sure
REM if errorlevel 2 goto end
REM :DoIt
REM rar u -r -s -ap ST.RAR @st.lst -x@xst.lst
"c:\Program Files\WinRAR\Rar.exe" u -r -s -ap ST.RAR @st.lst -x@xst.lst
:end

