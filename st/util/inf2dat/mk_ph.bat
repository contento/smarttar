@echo off
REM ---------------------------------------------------------------------------
REM mk_ph.bat -- compile cfg\ telephony .inf sources into bin\PH_INFO.BIN
REM
REM Invoked from c:\st\ by build.sh / build.ps1.  NOT wired into the MAKEFILE:
REM Borland MAKE 3.6 mis-propagates errorlevel from .bat-internal commands and
REM would delete the just-created target.  Works correctly when invoked
REM directly from DOSBox-X (the way the build runners do it).
REM
REM PH_INFO.BIN is a runtime data file, not a link input: if this step fails
REM the app self-heals by parsing the bin\*.inf fallback, so it never breaks
REM the build.
REM ---------------------------------------------------------------------------
set STTEST=1

REM -- stage canonical .inf next to inf2dat (cfg\ is the source of truth) --
copy cfg\local.inf util\inf2dat\local.inf > NUL
copy cfg\ddn.inf   util\inf2dat\ddn.inf   > NUL
copy cfg\ddi.inf   util\inf2dat\ddi.inf   > NUL

REM -- always run make: it is incremental and relinks inf2dat.exe when any
REM -- source/module changed, so a stale exe can never run.
if not exist util\inf2dat\obj mkdir util\inf2dat\obj
cd util\inf2dat
make -DRUN
cd ..\..
if not exist util\inf2dat\inf2dat.exe goto nobuild

REM -- run it (reads util\inf2dat\*.inf, writes util\inf2dat\PH_INFO.BIN) --
util\inf2dat\inf2dat
if errorlevel 1 goto failed
if not exist util\inf2dat\PH_INFO.BIN goto missing
copy util\inf2dat\PH_INFO.BIN bin\PH_INFO.BIN > NUL
echo MK_PH: bin\PH_INFO.BIN generated.
goto end
:nobuild
echo MK_PH: ERROR -- failed to build inf2dat.exe (app will use .inf fallback)
goto end
:failed
echo MK_PH: ERROR -- inf2dat exited with error
goto end
:missing
echo MK_PH: ERROR -- inf2dat ran but produced no PH_INFO.BIN
:end
