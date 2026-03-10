all: div.exe b800.exe math.exe  

div.exe: div.c
	bcc286 div.c

b800.exe: b800.c
	bcc286 b800.c

math.exe: math.c
	bcc286 math.c



