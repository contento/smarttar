# ----- OS/2 compiler options -----------------------------------------------
OS2_CPP=icc
OS2_LINK=link386
OS2_LIBRARIAN=lib
OS2_RC=rc

OS2_CPP_OPTS=-c -D__OS2__
OS2_LINK_OPTS=/NOE /PM:VIO
OS2_LIB_OPTS=
OS2_RC_OPTS=

OS2_OBJS=
OS2_LIBS=os2_zil.lib

.SUFFIXES : .cpp

.cpp.obj:
	$(OS2_CPP) $(OS2_CPP_OPTS) $<

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To generate the storage utilities type:
	@echo nmake -f ibm.mak os2
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean.cmd

# ----- DOS Libraries and Programs ------------------------------------------
dos:
	@echo IBM does not support DOS programs.

# ----- Windows Libraries and Programs --------------------------------------
windows:
	@echo IBM does not support Windows programs.

# ----- OS/2 Libraries and Programs -----------------------------------------
os2: obrowse.exe ozdump.exe orrmdir.exe ocppath.exe ochkfile.exe ogc.exe

ochkfile.exe: chkfile.obj
	$(OS2_LINK) $(OS2_LINK_OPTS) @<<zil.rsp
$(OS2_OBJS)+chkfile.obj
$*,NUL,$(OS2_LIBS),NUL
<<

gc.obj:
	ren cppath.obj *.sav
	$(OS2_CPP) $(OS2_CPP_OPTS) cppath.cpp
	ren cppath.obj gc.obj
	ren cppath.sav *.obj

ogc.exe: gc.obj
	$(OS2_LINK) $(OS2_LINK_OPTS) @<<zil.rsp
$(OS2_OBJS)+gc.obj
$*,NUL,$(OS2_LIBS),NUL
<<

ocppath.exe: cppath.obj
	$(OS2_LINK) $(OS2_LINK_OPTS) @<<zil.rsp
$(OS2_OBJS)+cppath.obj
$*,NUL,$(OS2_LIBS),NUL
<<

orrmdir.exe: rrmdir.obj
	$(OS2_LINK) $(OS2_LINK_OPTS) @<<zil.rsp
$(OS2_OBJS)+rrmdir.obj
$*,NUL,$(OS2_LIBS),NUL
<<

ozdump.exe: zdump.obj
	$(OS2_LINK) $(OS2_LINK_OPTS) @<<zil.rsp
$(OS2_OBJS)+zdump.obj
$*,NUL,$(OS2_LIBS),NUL
<<

obrowse.exe: browse.obj
	$(OS2_LINK) $(OS2_LINK_OPTS) @<<zil.rsp
$(OS2_OBJS)+browse.obj
$*,NUL,$(OS2_LIBS),NUL
<<

