all: twinclip.exe pdev.exe r_api.exe ptestapi.exe 

twinclip.exe: twinclip.c pwinclip.c 
	bcc286 twinclip.c pwinclip.c

pdev.exe: pdev.c
	bcc286 pdev.c

r_api.exe: r_api.c
	bcc -I\bc\include -L\bc\lib r_api.c

ptestapi.exe: ptestapi.c
	bcc286 ptestapi.c

