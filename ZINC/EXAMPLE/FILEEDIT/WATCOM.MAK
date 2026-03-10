# FILEEDIT example makefile

#    wmake -f watcom.mak dos32       (makes the DOS fedit tutorial)
#    wmake -f watcom.mak windows     (makes the Windows fedit tutorial)
#    wmake -f watcom.mak os2         (makes the OS/2 fedit tutorial)
#    wmake -f watcom.mak dvx         (makes the DESQView/X fedit tutorial)

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
dos32: fedit.exe .SYMBOLIC

fedit.exe: fedit.o32 direct.o32 dir_win.o32
	$(D32_LINK) $(D32_LINK_OPTS) N fedit.exe F fedit.o32,direct.o32,dir_win.o32 $(D32_OBJS) L $(D32_LIBS)

# ----- Windows -------------------------------------------------------------
windows: wfedit.exe .SYMBOLIC

wfedit.exe: fedit.obw direct.obw dir_win.obw
	$(WIN_LINK) $(WIN_LINK_OPTS) N wfedit F fedit.obw,direct.obo,dir_win.obo $(WIN_OBJS) L $(WIN_LIBS)
#	wrc $(WIN_RC_OPTS) $*.rc $*.exe


# ----- OS/2 ----------------------------------------------------------------
os2: ofedit.exe .SYMBOLIC

ofedit.exe: fedit.obo direct.obo dir_win.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) N ofedit.exe F fedit.obo,direct.obo,dir_win.obo $(OS2_OBJS) L $(OS2_LIBS)


# ----- DESQview/X Motif ----------------------------------------------------
dvx: mfedit.exe .SYMBOLIC

mfedit.exe: fedit.obm direct.obm dir_win.obm
	$(DVX_LINK) $(DVX_LINK_OPTS) N mfedit.exe F fedit.obm,direct.obm,dir_win.obm $(DVX_OBJS) L $(DVX_LIBS)
