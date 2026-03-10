@echo off
REM
REM TELECOM
REM
bc stl.prj /b
tdstrip stl
pklite stl.exe
copy stl.exe ..\..\bin
REM
REM EDA
REM
bc stleda.prj /b
tdstrip stleda
pklite stleda.exe
copy stleda.exe ..\..\bin

