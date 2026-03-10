@echo off
REM
REM Move res.dat source files.
REM See ..\source\res_fix.cpp to fix a problem with res.cpp and W_PHONE class
REM You have to do it by hand.
REM                           
@move res.hpp ..\include
@move res.cpp ..\source
echo See ..\source\res_fix.cpp to fix a problem with res.cpp and W_PHONE class


