# PERSIST example makefile

#    wmake -f watcom.mak dos32       (makes the DOS persist tutorial)
#    wmake -f watcom.mak windows     (makes the Windows persist tutorial)
#    wmake -f watcom.mak os2         (makes the OS/2 persist tutorial)
#    wmake -f watcom.mak dvx         (makes the DESQView/X persist tutorial)

# Be sure to set the LIB and INCLUDE environment variables for Zinc, e.g.:
#    set INCLUDE=.;C:\DVX\INCLUDE;C:\WATCOM\H;C:\ZINC\INCLUDE
#    set LIB=.;C:\DVX\LIB\C386;C:\WATCOM\LIB386\DOS;C:\WATCOM\LIB386;C:\ZINC\LIB\WCCPP950

.EXTENSIONS:
.EXTENSIONS: .o32 .obw .obn .obo .obm .cpp .c

# Compiler and linker: (Add -d2 to CPP_OPTS and DEBUG ALL to LINK_OPTS for debug.)

# ----- DOS extender compiler options ---------------------------------------
D32_C=wcc386
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
WIN_C=wcc
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
OS2_C=wcc386
OS2_CPP=wpp386
OS2_LINK=wlink
OS2_RC=rc
OS2_CPP_OPTS=/bt=OS2
OS2_LINK_OPTS=SYSTEM os2v2_pm OP ST=20000
OS2_RC_OPTS=
OS2_OBJS=
OS2_LIBS=os2_zil

# ----- DESQview/X MOTIF compiler options -----------------------------------
DVX_C=wcc386
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

.c.o32:
	$(D32_C) $(D32_CPP_OPTS) -fo=$[*.o32 $<

.c.obw:
	$(WIN_C) $(WIN_CPP_OPTS) -fo=$[*.obw $<

.c.obo:
	$(OS2_C) $(OS2_CPP_OPTS) -fo=$[*.obo $<

.c.obm:
	$(DVX_C) $(DVX_CPP_OPTS) -fo=$[*.obm $<

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
dos32: persist1.exe persist2.exe persist3.exe persist4.exe persist5.exe persist6.exe persist7.exe .SYMBOLIC

persist1.exe: persist1.o32
	$(D32_LINK) $(D32_LINK_OPTS) N persist1.exe F persist1.o32 $(D32_OBJS) L $(D32_LIBS)

persist2.exe: persist2.o32 draw.o32
	$(D32_LINK) $(D32_LINK_OPTS) N persist2.exe F persist2.o32,draw.o32 $(D32_OBJS) L $(D32_LIBS)

persist3.exe: persist3.o32 draw.o32
	$(D32_LINK) $(D32_LINK_OPTS) N persist3.exe F persist3.o32,draw.o32 $(D32_OBJS) L $(D32_LIBS)

persist4.exe: persist4.o32 draw.o32
	$(D32_LINK) $(D32_LINK_OPTS) N persist4.exe F persist4.o32,draw.o32 $(D32_OBJS) L $(D32_LIBS)

persist5.exe: persist5.o32 draw.o32
	$(D32_LINK) $(D32_LINK_OPTS) N persist5.exe F persist5.o32,draw.o32 $(D32_OBJS) L $(D32_LIBS)

persist6.exe: persist6.o32 draw.o32
	$(D32_LINK) $(D32_LINK_OPTS) N persist6.exe F persist6.o32,draw.o32 $(D32_OBJS) L $(D32_LIBS)

persist7.exe: persist7.o32 draw.o32
	$(D32_LINK) $(D32_LINK_OPTS) N persist7.exe F persist7.o32,draw.o32 $(D32_OBJS) L $(D32_LIBS)

# ----- Windows -------------------------------------------------------------
windows: wpersist.exe .SYMBOLIC

wpersist.exe: wpersist.obw wdraw.obw
	$(WIN_LINK) $(WIN_LINK_OPTS) N wpersist F wpersist.obw,wdraw.obw $(WIN_OBJS) L $(WIN_LIBS)
#	wrc $(WIN_RC_OPTS) $*.rc $*.exe


# ----- OS/2 ----------------------------------------------------------------
os2: opersist.exe .SYMBOLIC

opersist.exe: opersist.obo odraw.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) N opersist.exe F opersist.obo,odraw.obo $(OS2_OBJS) L $(OS2_LIBS)


# ----- DESQview/X Motif ----------------------------------------------------
dvx: mpersist.exe .SYMBOLIC

mpersist.exe: mpersist.obm mdraw.obm
	$(DVX_LINK) $(DVX_LINK_OPTS) N mpersist.exe F mpersist.obm,mdraw.obm $(DVX_OBJS) L $(DVX_LIBS)
