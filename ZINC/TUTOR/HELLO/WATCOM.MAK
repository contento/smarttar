# HELLO example makefile

#    wmake -f watcom.mak dos32       (makes the DOS hello tutorial)
#    wmake -f watcom.mak windows     (makes the Windows hello tutorial)
#    wmake -f watcom.mak os2         (makes the OS/2 hello tutorial)
#    wmake -f watcom.mak dvx         (makes the DESQView/X hello tutorial)

# Be sure to set the LIB and INCLUDE environment variables for Zinc, e.g.:
#    set INCLUDE=.;C:\DVX\INCLUDE;C:\WATCOM\H;C:\ZINC\INCLUDE
#    set LIB=.;C:\DVX\LIB\C386;C:\WATCOM\LIB386\DOS;C:\WATCOM\LIB386;C:\ZINC\LIB\WCCPP950

.EXTENSIONS:
.EXTENSIONS: .o32 .obw .obn .obo .obm .cpp

# Compiler and linker: (Add -d2 to CPP_OPTS and DEBUG ALL to LINK_OPTS for debug.)

# ----- DOS extender compiler options ---------------------------------------
D32_CPP=wpp386
D32_LINK=wlink
D32_CPP_OPTS=/bt=dos
D32_LINK_OPTS=SYSTEM dos4g OP stack=20000

# --- Use the next line for UI_GRAPHICS_DISPLAY ---
D32_OBJS=
D32_LIBS=d32_zil,d32_gfx,wc_32gfx,math3r,emu387,clib3r
# --- Use the next line for UI_WCC_DISPLAY ---
#D32_OBJS=
#D32_LIBS=d32_zil,d32_wcc,math3r,emu387,clib3r

# ----- Windows compiler options --------------------------------------------
WIN_CPP=wpp
WIN_LINK=wlink
WIN_RC=wrc
WIN_CPP_OPTS= -zW -oaxt -w4 -ml -DWINVER=0x0300
WIN_LINK_OPTS=SYS windows OP heapsize=16k OP stack=16k OP version=3.0
WIN_RC_OPTS=/30 /bt=windows
#WIN_CPP_OPTS= -zW -oaxt -w4 -ml
#WIN_LINK_OPTS=SYS windows OP heapsize=16k OP stack=16k
#WIN_RC_OPTS=/bt=windows
WIN_OBJS=
WIN_LIBS=windows,win_zil

# ----- OS/2 compiler options -----------------------------------------------
OS2_CPP=wpp386
OS2_LINK=wlink
OS2_RC=rc
OS2_CPP_OPTS=/bt=OS2
OS2_LINK_OPTS=SYSTEM os2v2_pm OP ST=20000
OS2_RC_OPTS=
OS2_OBJS=
OS2_LIBS=os2_zil

# ----- DESQview/X MOTIF compiler options -----------------------------------
DVX_CPP=wpp386
DVX_LINK=wlink
DVX_CPP_OPTS=/bt=dos /d__DVX__ /dXTFUNCPROTO -w0
DVX_LINK_OPTS=SYSTEM dos4g OP stack=20000
DVX_OBJS=
DVX_LIBS=dvx_zil,math3r,emu387,clib3r,Xm,Xt,Xlib,sys

.cpp.o32:
	$(D32_CPP) $(D32_CPP_OPTS) -fo=$[*.o32 $<

.cpp.obw:
	$(WIN_CPP) $(WIN_CPP_OPTS) -fo=$[*.obw $<

.cpp.obo:
	$(OS2_CPP) $(OS2_CPP_OPTS) -fo=$[*.obo $<

.cpp.obm:
	$(DVX_CPP) $(DVX_CPP_OPTS) -fo=$[*.obm $<

# ----- Usage --------------------------------------------------------------
usage: .SYMBOLIC
	@echo ...........
	@echo ...........
	@echo To make this Zinc example for Watcom C++ type:
	@echo wmake -f watcom.mak dos32
	@echo wmake -f watcom.mak os2
	@echo wmake -f watcom.mak windows
	@echo wmake -f watcom.mak dvx
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean: .SYMBOLIC
	z_clean

# ----- DOS extender --------------------------------------------------------
dos32: hello1.exe hello2.exe hello3.exe hellog.exe .SYMBOLIC

hello1.exe: hello1.o32
	$(D32_LINK) $(D32_LINK_OPTS) N hello1.exe F hello1.o32 $(D32_OBJS) L $(D32_LIBS)

hello2.exe: hello2.o32
	$(D32_LINK) $(D32_LINK_OPTS) N hello2.exe F hello2.o32 $(D32_OBJS) L $(D32_LIBS)

hello3.exe: hello3.o32 hello.o32
	$(D32_LINK) $(D32_LINK_OPTS) N hello3.exe F hello3.o32,hello.o32 $(D32_OBJS) L $(D32_LIBS)

hellog.exe: hellog.o32
	$(D32_LINK) $(D32_LINK_OPTS) N hellog.exe F hellog.o32 $(D32_OBJS) L $(D32_LIBS)

# ----- Windows -------------------------------------------------------------
windows: whello1.exe whello2.exe whello3.exe whellog.exe .SYMBOLIC

whello1.exe: hello1.obw 
	$(WIN_LINK) $(WIN_LINK_OPTS) N whello1 F hello1.obw $(WIN_OBJS) L $(WIN_LIBS)
#	wrc $(WIN_RC_OPTS) $*.rc $*.exe

whello2.exe: hello2.obw 
	$(WIN_LINK) $(WIN_LINK_OPTS) N whello2 F hello2.obw $(WIN_OBJS) L $(WIN_LIBS)
#	wrc $(WIN_RC_OPTS) $*.rc $*.exe

whello3.exe: hello3.obw hello.obw
	$(WIN_LINK) $(WIN_LINK_OPTS) N whello3 F hello3.obw,hello.obw $(WIN_OBJS) L $(WIN_LIBS)
#	wrc $(WIN_RC_OPTS) $*.rc $*.exe

whellog.exe: hellog.obw 
	$(WIN_LINK) $(WIN_LINK_OPTS) N whellog F hellog.obw $(WIN_OBJS) L $(WIN_LIBS)
#	wrc $(WIN_RC_OPTS) $*.rc $*.exe


# ----- OS/2 ----------------------------------------------------------------
os2: ohello1.exe ohello2.exe ohello3.exe ohellog.exe .SYMBOLIC

ohello1.exe: hello1.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) N ohello1.exe F hello1.obo $(OS2_OBJS) L $(OS2_LIBS)

ohello2.exe: hello2.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) N ohello2.exe F hello2.obo $(OS2_OBJS) L $(OS2_LIBS)

ohello3.exe: hello3.obo hello.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) N ohello3.exe F hello3.obo,hello.obo $(OS2_OBJS) L $(OS2_LIBS)

ohellog.exe: hellog.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) N ohellog.exe F hellog.obo $(OS2_OBJS) L $(OS2_LIBS)


# ----- DESQview/X Motif ----------------------------------------------------
dvx: mhello1.exe mhello2.exe mhello3.exe mhellog.exe .SYMBOLIC

mhello1.exe: hello1.obm
	$(DVX_LINK) $(DVX_LINK_OPTS) N mhello1.exe F hello1.obm $(DVX_OBJS) L $(DVX_LIBS)

mhello2.exe: hello2.obm
	$(DVX_LINK) $(DVX_LINK_OPTS) N mhello2.exe F hello2.obm $(DVX_OBJS) L $(DVX_LIBS)

mhello3.exe: hello3.obm hello.obm
	$(DVX_LINK) $(DVX_LINK_OPTS) N mhello3.exe F hello3.obm,hello.obm $(DVX_OBJS) L $(DVX_LIBS)

mhellog.exe: hellog.obm
	$(DVX_LINK) $(DVX_LINK_OPTS) N mhellog.exe F hellog.obm $(DVX_OBJS) L $(DVX_LIBS)
