@echo off
bc /b 
tdstrip viewlog.exe
copy viewlog.exe ..\..\bin
REM !!! No se puede usar PKLITE ya que el GEN no funcionaria !!!

