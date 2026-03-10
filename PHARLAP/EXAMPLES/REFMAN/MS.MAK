all: allocseg.exe calldll.exe coddat.exe codedata.exe criterr.exe \
ctrl_c.exe dump.exe enummod.exe enumproc.exe except.exe \
gpfault.exe hexcalc.exe huge.exe keys.exe lastdrv.exe \
protvec.exe realseg.exe \
realvec.exe segdesc.exe testdll.dll vec.exe verify.exe \
linphys.exe gdt.exe memtrace.dll exitlist.exe \
tshell.exe trace.exe realavl.exe memavail.exe

allocseg.exe: allocseg.c
	cl -Lp allocseg.c 
	bind286 allocseg

calldll.exe: calldll.c
	cl -Lp calldll.c
	bind286 calldll

coddat.exe: coddat.c
	cl -Lp coddat.c
	bind286 coddat

codedata.exe: codedata.c
	cl -Lp -DDOSX286 codedata.c
	bind286	codedata

criterr.exe: criterr.c
	cl -Lp -DDOSX286 -Gs criterr.c
	bind286	criterr

ctrl_c.exe: ctrl_c.c
	cl -Lp -Gs ctrl_c.c
	bind286 ctrl_c

dump.exe: dump.c
	cl -Lp dump.c
	bind286 dump

enummod.exe: enummod.c
	cl -Lp enummod.c
	bind286	enummod

enumproc.exe: enumproc.c
	cl -Lp enumproc.c
	bind286	enumproc

except.exe: except.c
	cl -Lp except.c
	bind286	except

gpfault.exe: gpfault.c
	cl -Lp -Gs gpfault.c
	bind286	gpfault

hexcalc.exe: hexcalc.c
	cl -Lp -DDOSX286 hexcalc.c
	bind286	hexcalc

huge.exe: huge.c
	cl -Lp huge.c
	bind286	huge

keys.exe: keys.c
	cl -Lp -DDOSX286 keys.c
	bind286 keys

lastdrv.exe: lastdrv.c
	cl -Lp lastdrv.c
	bind286	lastdrv

protvec.exe: protvec.c
	cl -Lp protvec.c 
	bind286	protvec

#realdll.dll: realdll.c realdll.def
#	cl -ALw -FPa -Gs2 -c realdll.c 
#	link /nod realdll, realdll.dll,,,realdll.def
#	markreal realdll.dll

#realdll.lib: realdll.def
#	implib realdll.lib realdll.def

#testrdll.exe: testrdll.c realdll.lib
#	cl -Lp testrdll.c -link realdll.lib
#	bind286	testrdll

realseg.exe: realseg.c
	cl -Lp realseg.c
	bind286	realseg

realvec.exe: realvec.c
	cl -Lp realvec.c
	bind286	realvec

segdesc.exe: segdesc.c
	cl -Lp segdesc.c
	bind286	segdesc

testdll.dll: testdll.c testdll.def
	cl -ALu -FPa -Gs2 -Lp -c testdll.c 
	link /nod/noi testdll, testdll.dll,,,testdll.def

vec.exe: vec.c
	cl -Lp vec.c
	bind286	vec

verify.exe: verify.c
	cl -Lp verify.c
	bind286 verify

linphys.exe: linphys.c
	cl -Lp linphys.c
	bind286	linphys

gdt.exe: gdt.c
	cl -Lp -G2 gdt.c
	bind286	gdt

memtrace.dll: memtrace.c start.asm memtrace.def
	cl -ALu -Lp -Gs -G2 -c memtrace.c
	ml -c start.asm
	link memtrace start,memtrace.dll,,/nod:llibce llibcep phapi,memtrace.def

exitlist.exe: exitlist.c
	cl -Lp exitlist.c
	bind286	exitlist

tshell.exe: tshell.c
	cl -Lp tshell.c
	bind286	tshell

trace.exe: trace.c
	cl -Lp trace.c
	bind286	trace

realavl.exe: realavl.c
	cl -Lp realavl.c
	bind286	realavl

memavail.exe: memavail.c
	cl -Lp memavail.c
	bind286	memavail
