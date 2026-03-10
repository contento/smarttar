@echo off
rem $Id: mklib.bat 1.10 1993/12/23 12:03:20 rob Exp $
set PLLPTH=%2
rem if "%1" == "s" goto valid_model
rem if "%1" == "m" goto valid_model
if "%1" == "l" goto valid_model
if "%1" == "h" goto valid_model
if "%1" == "L" goto valid_model
if "%1" == "H" goto valid_model
rem if "%1" == "c" goto valid_model
goto usage
:valid_model
set m=%1
if "%PLLPTH%" == "" goto no_path
goto path_supplied
:no_path
set PLLPTH=.
:path_supplied
if exist %PLLPTH%\c%m%.lib goto lib_exists
goto error1
:lib_exists
if not exist bc%m%286.lib goto targ_no_exist
echo * Warning: File "bc%m%286.lib" already exists and will be overwritten.
echo *          Press Ctrl-C to Abort ...
pause
del bc%m%286.lib
:targ_no_exist
echo * Creating empty library "bc%m%286.lib"...
tlib bc%m%286 > nul
if not exist bc%m%286.lib goto error4
echo * Copying c%m%.lib to bc%m%286.lib
copy %PLLPTH%\c%m%.lib bc%m%286.lib
rem
rem Extract OBJs from Phar Lap libraries
rem
echo * Extracting object files from tmp%m%.lib
tlib tmp%m%.lib @extmods.lbc > nul
if errorlevel 1 goto error2
rem
rem Add modules into new library.
rem
echo * Replacing objects in bc%m%286.lib.  Please be patient...
tlib bc%m%286.lib @insmods.lbc > nul
if errorlevel 1 goto error3
rem
rem Delete intermediate object files
rem
echo * Cleaning up.
call remobjs.bat
del bc%m%286.bak
echo *
echo *	Library "bc%m%286.lib" created successfully.
echo *
goto end
:usage
echo Usage: MKLIB [L or H]  [dir path]
echo The first parameter is a letter that determines what model
echo library is to be built:
echo	  l : Large Model 
echo	  h : Huge Model 
echo The second parameter is the path to the DIRECTORY that
echo contains the Borland C++ library files. The new library
echo will be created in the current directory.
echo Sample Usage:
echo MKLIB l c:\bc\lib      (creates library BCL286.LIB)
goto end
:error1
echo *
echo *  ERROR: Library "%PLLPTH%\c%m%.lib" Not Found.
echo *
goto end
:error2
echo *
echo *	ERROR: Could Not Extract Modules from library "tmp%m%.lib".
echo *
goto end
:error3
echo *
echo *	ERROR: Could Not Create New Library "bc%m%286.lib".
echo *
goto end
:error4
echo *
echo *	ERROR: Could Not Invoke Librarian "tlib.exe".
echo *
goto end
:end
set PLLPTH=
rem Borland C++ is a trademark of Borland International, Inc.
rem Borland is a registered trademark of Borland International, Inc.
