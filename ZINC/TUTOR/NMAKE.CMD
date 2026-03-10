@echo off
REM MAKE.BAT - for Zinc Interface Library 3.0
REM This batch file can be used to compile all tutorial programs supplied.

if (%1)==() goto usage
goto spawn1

:usage
echo ...........
echo ...........
echo To make the Zinc tutorials for Borland C++ type:
echo make -fborland.mak dos
echo make -fborland.mak windows
echo make -fborland.mak os2
echo make -fborland.mak dos16
echo ...........
echo To make the Zinc tutorials for Microsoft C++ type:
echo nmake -fmicrosoft.mak dos
echo nmake -fmicrosoft.mak windows
echo nmake -f mscwnt.mak winnt
echo (Microsoft does not support OS/2)
echo nmake -fmicrosoft.mak dos16
echo ...........
echo To make the Zinc tutorials for Zortech C++ type:
echo make -fzortech.mak dos
echo make -fzortech.mak windows
echo make -fzortech.mak os2
echo make -fzortech.mak dos16
echo make -fzortech.mak dos32
echo ...........
echo ...........
goto done

:spawn1
if (%2)==(clean) goto spawn2
goto spawn3
:spawn2
@call z_clean.cmd
:spawn3
cd hello
nmake %1 %2 %3 %4 %5
cd ..
cd word
nmake %1 %2 %3 %4 %5
cd ..
cd zincapp
nmake %1 %2 %3 %4 %5
cd ..
cd macro
nmake %1 %2 %3 %4 %5
cd ..
cd helpbar
nmake %1 %2 %3 %4 %5
cd ..
cd vlist
nmake %1 %2 %3 %4 %5
cd ..
cd display
nmake %1 %2 %3 %4 %5
cd ..
cd persist
nmake %1 %2 %3 %4 %5
cd ..
cd currency
nmake %1 %2 %3 %4 %5
cd ..
cd language
nmake %1 %2 %3 %4 %5
cd ..

:done
