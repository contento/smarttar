# GENHELP and ICO2DAT program makefile

#    nmake -f ibm.mak os2	(makes all the OS/2 genhelp/ico2dat programs)

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
	@echo To make the genhelp, ico2dat and bmp2dat programs for IBM C++ type:
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
os2: ogenhelp.exe obmp2dat.exe oico2dat.exe
	copy ogenhelp.exe ..\..\bin
	copy obmp2dat.exe ..\..\bin
	copy oico2dat.exe ..\..\bin

ogenhelp.exe: genhelp.obj
	$(OS2_LINK) $(OS2_LINK_OPTS) @<<zil.rsp
$(OS2_OBJS)+genhelp.obj
$*,NUL,$(OS2_LIBS),NUL
<<

obmp2dat.exe: bmp2dat.obj
	$(OS2_LINK) $(OS2_LINK_OPTS) @<<zil.rsp
$(OS2_OBJS)+bmp2dat.obj
$*,NUL,$(OS2_LIBS),NUL
<<

oico2dat.exe: ico2dat.obj
	$(OS2_LINK) $(OS2_LINK_OPTS) @<<zil.rsp
$(OS2_OBJS)+ico2dat.obj
$*,NUL,$(OS2_LIBS),NUL
<<

