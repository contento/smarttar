# ----- DOS compiler options ------------------------------------------------
DOS_CPP_OPTS=-c -a1 -bx -ml -I..
DOS_LINK_OPTS=/NOI
DOS_OBJS=
DOS_LIBS=dos_zil

DOS_CPP=ztc
DOS_LINK=blinkx

.cpp.obj:
	$(DOS_CPP) $(DOS_CPP_OPTS) $<

# ----- OS/2 compiler options -----------------------------------------------
OS2_CPP_OPTS=-c -mf -I..
OS2_LINK_OPTS=/PM:WINDOWCOMPAT
OS2_OBJS=
OS2_LIBS=os2_zil

OS2_CPP=ztc
OS2_LINK=blinkx

.cpp.obo:
	$(OS2_CPP) $(OS2_CPP_OPTS) -o$*.obo $<

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To generate the storage utilities type:
	@echo make -fzortech.mak dos
	@echo make -fzortech.mak os2
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean.bat

# ----- DOS -----------------------------------------------------------------
dos: browse.exe zdump.exe rrmdir.exe cppath.exe chkfile.exe gc.exe

chkfile.exe: chkfile.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+chkfile.obj,$*, ,$(DOS_LIBS),NUL

gc.obj:
	$(DOS_CPP) $(DOS_CPP_OPTS) -DDOGC -o$*.obj cppath.cpp

gc.exe: gc.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+gc.obj,$*, ,$(DOS_LIBS),NUL

cppath.exe: cppath.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+cppath.obj,$*, ,$(DOS_LIBS),NUL

rrmdir.exe: rrmdir.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+rrmdir.obj,$*, ,$(DOS_LIBS),NUL

zdump.exe: zdump.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+zdump.obj,$*, ,$(DOS_LIBS),NUL

browse.exe: browse.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+browse.obj,$*, ,$(DOS_LIBS),NUL


# ----- OS/2 ----------------------------------------------------------------
os2: obrowse.exe ozdump.exe orrmdir.exe ocppath.exe ochkfile.exe ogc.exe

ochkfile.exe: chkfile.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) $(OS2_OBJS)+chkfile.obo,$*, ,$(OS2_LIBS),

gc.obo:
	$(OS2_CPP) $(OS2_CPP_OPTS) -DDOGC -o$*.obo cppath.cpp

ogc.exe: gc.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) $(OS2_OBJS)+gc.obo,$*, ,$(OS2_LIBS),

ocppath.exe: cppath.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) $(OS2_OBJS)+cppath.obo,$*, ,$(OS2_LIBS),

orrmdir.exe: rrmdir.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) $(OS2_OBJS)+rrmdir.obo,$*, ,$(OS2_LIBS),

ozdump.exe: zdump.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) $(OS2_OBJS)+zdump.obo,$*, ,$(OS2_LIBS),

obrowse.exe: browse.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) $(OS2_OBJS)+browse.obo,$*, ,$(OS2_LIBS),


