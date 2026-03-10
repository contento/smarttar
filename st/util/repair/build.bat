@echo off
bc /b 
tdstrip repair.exe
copy repair.exe ..\..\bin
REM !!! No se puede usar PKLITE ya que el GEN no funcionaria !!!

