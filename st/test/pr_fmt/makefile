!if $d(DEBUG)
CC_OPTS= +test.cfg -v
!else
CC_OPTS= +test.cfg
!endif

SOURCE=..\..\source

test.exe: test.cpp pr_sr80.dll prn_fmt.obj
  bcc286 $(CC_OPTS) test.cpp prn_fmt.obj

prn_fmt.obj: $(SOURCE)\prn_fmt.cpp
  bcc286 $(CC_OPTS) -c $(SOURCE)\prn_fmt.cpp

pr_sr80.dll: $(SOURCE)\pr_sr80.c
  bcc286 $(CC_OPTS) -dll $(SOURCE)\pr_sr80.c

