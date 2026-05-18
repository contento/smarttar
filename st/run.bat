@echo off
REM cd into bin\ so st.exe finds st.cfg / res.dat / RX.DAT relative to cwd.
if not exist bin\nul goto nobin
if not exist bin\st.exe goto noexe
cd bin
st %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..
goto end
:nobin
echo ERROR: bin\ not found. Run this from the st\ directory.
goto end
:noexe
echo ERROR: bin\st.exe not found. Build it first (e.g., make RUN=1).
:end
