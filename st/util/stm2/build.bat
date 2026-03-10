@echo off
bc dump.prj /b
tdstrip dump.exe
REM
bc fill.prj /b
tdstrip fill.exe
REM
bc fdump.prj /b
tdstrip fdump.exe

copy dump.exe ..\..\bin
REM !!! No se puede usar PKLITE ya que el GEN no funcionaria !!!

