allocseg 10240
coddat
codedata
criterr \bin\ls a:
ctrl_c
dump fe00:0008
enummod
enumproc viocalls
except
gpfault
REM hexcalc uses reverse notation, e.g., - 1 2 ===> FFFF; q to quit
hexcalc
huge
REM keys: under Windows, can't use Ctrl-Esc to quit: hit ^C
keys
lastdrv
protvec 21
realseg
realvec 21
segdesc
vec
verify
linphys
gdt
run286 -loaddll memtrace enummod
exitlist
REM run commands from within TSHELL
tshell
realavl
