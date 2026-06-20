@echo off
REM Build MiniDB test: DEMO + NODONGLE
if not exist C:\VENDOR\BC\BIN\MAKE.EXE goto nomake
call make bin\test_mdb.exe %1 %2 %3 %4 %5 %6 %7 %8 %9 -DDEMO -DRUN -DNODONGLE
if errorlevel 1 goto failed
echo test_mdb.exe built.
goto end
:nomake
echo ERROR: MAKE.EXE not found in C:\VENDOR\BC\BIN\. Run: ./setup-vendor.sh
goto failed
:failed
echo ERROR: Build failed.
:end
