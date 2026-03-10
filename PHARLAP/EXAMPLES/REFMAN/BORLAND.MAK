
all: allocseg.exe calldll.exe testdll.dll coddat.exe codedata.exe dump.exe \
     enummod.exe enumproc.exe except.exe hexcalc.exe keys.exe \
     realdll.dll realdll.lib testrdll.exe \
     lastdrv.exe protvec.exe realseg.exe realvec.exe memtrace.dll \
     segdesc.exe vec.exe verify.exe linphys.exe gdt.exe exitlist.exe \
     tshell.exe trace.exe realavl.exe criterr.exe ctrl_c.exe gpfault.exe \
     huge.exe machmode.exe memavail.exe

allocseg.exe: allocseg.c
	bcc286 allocseg.c 

calldll.exe: calldll.c
	bcc286 calldll.c

testdll.dll: testdll.c
	bcc286 -dll testdll.c testdll.def 

coddat.exe: coddat.c
	bcc286 coddat.c

codedata.exe: codedata.c
	bcc286 codedata.c

criterr.exe: criterr.c
	bcc286 criterr.c

ctrl_c.exe: ctrl_c.c
	bcc286 ctrl_c.c

dump.exe: dump.c
	bcc286 dump.c

enummod.exe: enummod.c
	bcc286 enummod.c

enumproc.exe: enumproc.c
	bcc286 enumproc.c

except.exe: except.c
	bcc286 except.c

gpfault.exe: gpfault.c
	bcc286 gpfault.c

hexcalc.exe: hexcalc.c
	bcc286 -v hexcalc.c

huge.exe: huge.c
	bcc286 huge.c
	
keys.exe: keys.c
	bcc286 keys.c

lastdrv.exe: lastdrv.c
	bcc286 lastdrv.c

machmode.exe: machmode.c
	bcc286 machmode.c

protvec.exe: protvec.c
	bcc286 protvec.c

realdll.dll: realdll.c
	bcc286 -realdll realdll.c realdll.def

testrdll.exe: testrdll.c realdll.lib
	bcc286 testrdll.c realdll.lib

realseg.exe: realseg.c
	bcc286 realseg.c

realvec.exe: realvec.c
	bcc286 realvec.c

segdesc.exe: segdesc.c
	bcc286 segdesc.c

vec.exe: vec.c
	bcc286 vec.c

verify.exe: verify.c
	bcc286 verify.c

linphys.exe: linphys.c
	bcc286 linphys.c

gdt.exe: gdt.c
	bcc286 gdt.c

exitlist.exe: exitlist.c
	bcc286 exitlist.c

tshell.exe: tshell.c
	bcc286 tshell.c	

trace.exe: trace.c
	bcc286 trace.c 

realavl.exe: realavl.c
	bcc286 realavl.c

memtrace.dll: memtrace.c
	bcc286 -r- -dll memtrace.c memtrace.def -start=START

memavail.exe: memavail.c
	bcc286 memavail.c 


