@echo off
REM MAKE.BAT - for Zinc Interface Library 3.0
REM This batch file can be used to compile all tutorial programs supplied.

if (%1)==() goto usage
goto spawn1

:usage
echo ...........
echo ...........
echo To make the Zinc examples for Borland C++ type:
echo make -fborland.mak dos
echo make -fborland.mak dos16
echo make -fborland.mak os2
echo make -fborland.mak windows
echo make -fborland.mak winnt
echo ...........
echo To make the Zinc examples for Microsoft C++ type:
echo nmake -fmicrosft.mak dos
echo nmake -fmicrosft.mak dos16
echo (Microsoft does not support OS/2)
echo nmake -fmicrosft.mak windows
echo nmake -f mscwnt.mak winnt
echo ...........
echo ...........
echo To make the Zinc examples for Watcom C++ type:
echo wmake -f watcom.mak dos
echo wmake -f watcom.mak dos16
echo wmake -f watcom.mak os2
echo wmake -f watcom.mak windows
echo ...........
echo To make the Zinc examples for Zortech C++ type:
echo make -fzortech.mak dos
echo make -fzortech.mak dos16
echo make -fzortech.mak dos32
echo make -fzortech.mak os2
echo make -fzortech.mak windows
echo ...........
echo ...........
goto done

:spawn1
if (%2)==(clean) goto spawn2
goto spawn3
:spawn2
@call z_clean.bat
:spawn3
cd analog
wmake %1 %2 %3 %4 %5
cd ..
cd bio
wmake %1 %2 %3 %4 %5
cd ..
cd calc
wmake %1 %2 %3 %4 %5
cd ..
cd calendar
wmake %1 %2 %3 %4 %5
cd ..
cd checkbox
wmake %1 %2 %3 %4 %5
cd ..
cd clock
wmake %1 %2 %3 %4 %5
cd ..
cd combobox
wmake %1 %2 %3 %4 %5
cd ..
cd display
wmake %1 %2 %3 %4 %5
cd ..
cd draw
wmake %1 %2 %3 %4 %5
cd ..
cd error
wmake %1 %2 %3 %4 %5
cd ..
cd fileedit
wmake %1 %2 %3 %4 %5
cd ..
cd freestor
wmake %1 %2 %3 %4 %5
cd ..
cd graph
wmake %1 %2 %3 %4 %5
cd ..
cd messages
wmake %1 %2 %3 %4 %5
cd ..
cd notepad
wmake %1 %2 %3 %4 %5
cd ..
cd periodic
wmake %1 %2 %3 %4 %5
cd ..
cd phonebk
wmake %1 %2 %3 %4 %5
cd ..
cd puzzle
wmake %1 %2 %3 %4 %5
cd ..
cd spy
wmake %1 %2 %3 %4 %5
cd ..
cd validate
wmake %1 %2 %3 %4 %5
cd ..

:done
