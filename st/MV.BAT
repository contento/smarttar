@echo off
setlocal

:: Check if sufficient arguments are provided
if "%~1p"=="p" goto :Usage
if "%~2p"=="p" goto :Usage

:: Check if source file exists
if not exist "%~1" (
    echo Error: Source file or directory "%~1" not found.
    goto :End
)

:: Check if destination is a directory
if exist "%~2\" (
    set "dest=%~2\%~nx1"
) else (
    set "dest=%~2"
)

:: Perform the copy operation
copy /Y "%~1" "%dest%"

:: Check the status of the copy operation
if errorlevel 1 (
    echo Error: Failed to move "%~1" to "%~2"
    goto :End
)

:: Remove the source file
erase /F "%~1"

:Usage
echo Usage: mv source destination

:End
endlocal
