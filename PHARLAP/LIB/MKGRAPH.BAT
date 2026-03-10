@echo off
rem $Id: mkgraph.bat 1.2 1993/12/08 10:16:33 clark Exp $

if "%1"=="" goto usage

copy dll30.def graphics.def
copy dll30.lib dll.lib

set PLLPTH=%1

:do_bgi
shift
set BGINAME=%1
if "%BGINAME%"=="" goto do_dll
if not exist %BGINAME% goto error4
shift
set BGIPUB=%1
if "%BGIPUB%"=="" goto error5

echo *
echo * Adding "%BGINAME%" to DLL.LIB
echo *
tlib dll.lib +%BGINAME%
echo *
echo * Adding "%BGIPUB%" to GRAPHICS.DEF
echo *
echo %BGIPUB% >> graphics.def
goto do_bgi

:do_dll
echo *
echo * Creating GRAPHICS.DLL
echo *
if "%PLLPTH%" == "" goto no_path
goto path_supplied
:no_path
set PLLPTH=.
:path_supplied
if exist %PLLPTH%\graphics.lib goto lib_exists
goto error1
:lib_exists
if not exist graphics.dll goto dll_no_exist
echo *
echo * Warning: File "graphics.dll" already exists and will be overwritten.
echo *          Press Ctrl-C to Abort ...
echo *
pause
:dll_no_exist

echo c0dll.obj+ > graphics.lnk
echo %PLLPTH%\graphics.lib+ >> graphics.lnk
echo dll.lib >> graphics.lnk
echo graphics.dll /Twd >> graphics.lnk
echo graphics /m >> graphics.lnk
echo /n %PLLPTH%\cl.lib phapi.lib  >> graphics.lnk
echo graphics.def >> graphics.lnk

echo * linking ...
tlink @graphics.lnk
if errorlevel 1 goto error2
echo * marking ...
bcc286 -markreal graphics.dll
if errorlevel 1 goto error2
echo * copying ...
copy graphics.dll ..\..\bin
del dll.lib
del graphics.lnk

echo *
echo * Creating GRAPH286.LIB
echo *
echo * creating import lib ...
implib graph286.lib graphics.def
if errorlevel 1 goto error3
echo * adding modules ...
tlib graph286.lib +grph30.lib
if errorlevel 1 goto error3

echo *
echo * GRAPH286.LIB and GRAPHICS.DLL were successfully created.
echo *
echo * GRAPHICS.DLL has been copied to the RUN286 master BIN
echo * directory (..\..\BIN).
echo *
goto end

:usage
echo Usage: MKGRAPH [dir path] [bgi1.obj bg1_pubsym] [bgi2.obj bgi2_pubsym] [...]
echo This batch file produces a real-mode DLL named GRAPHICS.DLL and
echo a protected-mode library named GRAPH286.LIB (both get placed in
echo the current directory).
echo The first parameter is the path to the directory that contains
echo the BORLAND C++ 3.0 library files. Other parameters are optional.
echo Parameters two and three are the names of a BGI driver (or font)
echo object file produced by BGIOBJ and the public symbol used to reference
echo that BGI driver, respectively. Note: the public symbol must be entered
echo as the linker will see it (in other words, prepend an underscore if it
echo uses C-Calling convention).  For more information, refer to the manual:
echo "Borland C++ User's Guide to 286|DOS-Extender".
echo Sample Usage(s):
echo MKGRAPH c:\borlandc\lib 
echo MKGRAPH c:\borlandc\lib cga.obj _CGA_driver 
goto end

:error1
echo *
echo *  ERROR: Library "%PLLPTH%\graphics.lib" Not Found.
echo *
goto end

:error2
echo *
echo *	ERROR: Errors encountered in build of GRAPHICS.DLL
echo *
goto end

:error3
echo *
echo *	ERROR: Errors encountered in the creation of GRAPH286.LIB
echo *
goto end

:error4
echo *
echo *	ERROR: BGI driver (or font) "%BGINAME%" not found.
echo *
goto end

:error5
echo *
echo *	ERROR: Public symbol for "%BGINAME%" not specified.
echo *
goto end

:end
set PLLPTH=
set BGINAME=
set BGIPUB=
