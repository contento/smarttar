@echo off
bc /b 
tdstrip update.exe
copy update.exe ..\..\bin
REM !!! No se puede usar PKLITE ya que el GEN no funcionaria !!!

