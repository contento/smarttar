# GENHELP and ICO2DAT program makefile

#    wmake -f watcom.mak dos            (makes the DOS genhelp/ico2dat programs)
#    wmake -f watcom.mak windows        (makes the Windows genhelp/ico2dat program)
#    wmake -f watcom.mak os2            (makes the OS/2 genhelp/ico2dat program)

.EXTENSIONS: .o32 .obw .obn .obo .cpp

# ----- DOS compiler options ------------------------------------------------
D32_CPP_OPTS=/bt=dos -I..
D32_LINK_OPTS=SYSTEM dos4g OP stack=20000
D32_OBJS=
D32_LIBS=d32_zil,math3r,emu387,clib3r

D32_CPP=wpp386
D32_LINK=wlink

.cpp.o32:
	$(D32_CPP) $(D32_CPP_OPTS) -fo=$[*.o32 $<

# ----- OS/2 compiler options -----------------------------------------------
OS2_CPP=wpp386
OS2_LINK=wlink
OS2_RC=rc

OS2_CPP_OPTS=/bt=OS2
OS2_LINK_OPTS=SYSTEM os2v2_pm OP ST=20000
OS2_RC_OPTS=
OS2_OBJS=
OS2_LIBS=os2_zil

.cpp.obo:
	$(OS2_CPP) $(OS2_CPP_OPTS) -fo=$[*.obo $<

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To make the genhelp and ico2dat programs for Watcom C++ type:
	@echo wmake -f watcom.mak dos32
	@echo wmake -f watcom.mak os2
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean

# ----- DOS -----------------------------------------------------------------
dos32: genhelp.exe bmp2dat.exe ico2dat.exe
	copy genhelp.exe ..\..\bin
	copy bmp2dat.exe ..\..\bin
	copy ico2dat.exe ..\..\bin

genhelp.exe: genhelp.o32
	$(D32_LINK) $(D32_LINK_OPTS) N genhelp F genhelp.o32 $(D32_OBJS) L $(D32_LIBS)

bmp2dat.exe: bmp2dat.o32
	$(D32_LINK) $(D32_LINK_OPTS) N bmp2dat F bmp2dat.o32 $(D32_OBJS) L $(D32_LIBS)

ico2dat.exe: ico2dat.o32
	$(D32_LINK) $(D32_LINK_OPTS) N ico2dat F ico2dat.o32 $(D32_OBJS) L $(D32_LIBS)

# ----- Windows -------------------------------------------------------------
windows: genhelp.exe bmp2dat.exe ico2dat.exe
	copy genhelp.exe ..\..\bin
	copy bmp2dat.exe ..\..\bin
	copy ico2dat.exe ..\..\bin

# ----- OS/2 ----------------------------------------------------------------
os2: ogenhelp.exe obmp2dat.exe oico2dat.exe

ogenhelp.exe: genhelp.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) N ogenhelp.exe F genhelp.obo$(OS2_OBJS) L $(OS2_LIBS)

obmp2dat.exe: bmp2dat.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) N obmp2dat.exe F bmp2dat.obo$(OS2_OBJS) L $(OS2_LIBS)

oico2dat.exe: ico2dat.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) N oico2dat.exe F ico2dat.obo$(OS2_OBJS) L $(OS2_LIBS)


