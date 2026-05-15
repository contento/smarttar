@echo off
if not exist bin\st.exe goto noexe
cd bin
st %1 %2 %3 %4
cd ..
goto end
:noexe
echo ERROR: bin\st.exe not found. Run from the st\ directory.
:end
