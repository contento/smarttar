rem This example program can only be used Microsoft C 5.1 and 6.0
nmake -f shrseg.mak
if errorlevel 1 goto exit
nmake -f shrtest.mak
:exit

