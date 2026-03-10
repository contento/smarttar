# PHONEBK example makefile

#    nmake -f ibm.mak os2	(makes all the OS/2 phonebk examples)

# ----- OS/2 compiler options -----------------------------------------------
OS2_CPP=icc
OS2_LINK=link386
OS2_RC=rc

OS2_CPP_OPTS=-c -D__OS2__
OS2_LINK_OPTS=/NOE /BASE:0x10000 /PM:PM
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
	@echo To make this Zinc example for IBM C++ type:
	@echo nmake -f ibm.mak os2
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean.cmd

# ----- OS/2 ----------------------------------------------------------------
os2: ophonebk.exe

ophonebk.exe: phonebk.obj phon_win.obj
	$(OS2_LINK) $(OS2_LINK_OPTS) @<<zil.rsp
$(OS2_OBJS)+phonebk.obj+phon_win.obj
$*,NUL,$(OS2_LIBS),NUL
<<
	$(OS2_RC) $(OS2_RC_OPTS) $*.rc $*.exe
