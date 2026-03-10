all: sig.exe ctrl_c.exe criterr.exe int33p.exe timeloop.exe int33p.exe

sig.exe: sig.c
	cl -Lp sig.c
	bind286 sig

ctrl_c.exe: ctrl_c.c
	cl -Lp -Gs ctrl_c.c 
	bind286 ctrl_c

criterr.exe: criterr.c
	cl -Lp -DDOSX286 -Gs criterr.c 
	bind286 criterr

timeloop.exe: timeloop.c
	cl -Lp -Ox timeloop.c
	bind286 timeloop

int33p.exe: int33p.c
	cl -Lp -Ox int33p.c 
	bind286 int33p

