!if $d(DEBUG)
CC_OPTS= -v
!else
CC_OPTS=
!endif

test.exe: test.cpp pr_fx.dll pr_tm.dll
  bcc286 $(CC_OPTS) test.cpp

pr_fx.dll: pr_fx.c
  bcc286 $(CC_OPTS) -dll pr_fx.c

pr_tm.dll: pr_tm.c
  bcc286 $(CC_OPTS) -dll pr_tm.c

