all: int33t.exe

int33t.exe: int33t.c
	cl -AL -Lp -Gs -G2 int33t.c
	bind286 int33t
