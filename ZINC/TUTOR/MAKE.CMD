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
echo nmake -fmicrsft.mak dos
echo nmake -fmicrsft.mak windows
echo nmake -f mscwnt.mak winnt
echo (Microsoft does not support OS/2)
echo nmake -fmicrsft.mak dos16
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
make %1 %2 %3 %4 %5
cd ..
cd word
make %1 %2 %3 %4 %5
cd ..
cd zincapp
make %1 %2 %3 %4 %5
cd ..
cd macro
make %1 %2 %3 %4 %5
cd ..
cd helpbar
make %1 %2 %3 %4 %5
cd ..
cd vlist
make %1 %2 %3 %4 %5
cd ..
cd display
make %1 %2 %3 %4 %5
cd ..
cd persist
make %1 %2 %3 %4 %5
cd ..
cd currency
make %1 %2 %3 %4 %5
cd ..
cd language
make %1 %2 %3 %4 %5
cd ..

:done
