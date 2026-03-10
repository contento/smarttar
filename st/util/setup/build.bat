@echo off
bc /b 
tdstrip setup.exe
copy setup.exe ..\..\bin
REM !!! No se puede usar PKLITE ya que el GEN no funcionaria !!!

