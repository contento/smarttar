!if $d(DEBUG)
CC_OPTS= -v
!else
CC_OPTS=
!endif

test.exe: test.cpp pr_fx.dll
  bcc286 $(CC_OPTS) test.cpp

pr_fx.dll: pr_fx.cpp
  bcc286 $(CC_OPTS) -dll pr_fx.cpp

