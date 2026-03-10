# ZINCAPP example makefile

#    wmake -f watcom.mak dos32       (makes the DOS zincapp tutorial)
#    wmake -f watcom.mak windows     (makes the Windows zincapp tutorial)
#    wmake -f watcom.mak os2         (makes the OS/2 zincapp tutorial)
#    wmake -f watcom.mak dvx         (makes the DESQView/X zincapp tutorial)

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
dos32: zincapp.exe .SYMBOLIC

zincapp.exe: zincapp.o32 control.o32 display.o32 window.o32 event.o32 help.o32 support.o32
	%create d32_zil.rsp
	@%append d32_zil.rsp $(D32_LINK_OPTS) 
	@%append d32_zil.rsp N zincapp 
	@%append d32_zil.rsp F zincapp.o32,control.o32,display.o32,window.o32,event.o32,help.o32,support.o32 $(D32_OBJS)
	@%append d32_zil.rsp L $(D32_LIBS)
	$(D32_LINK) @d32_zil.rsp
	del d32_zil.rsp

# ----- Windows -------------------------------------------------------------
windows: wzincapp.exe .SYMBOLIC

wzincapp.exe: zincapp.obw control.obw display.obw window.obw event.obw help.obw support.obw
	%create win_zil.rsp
	@%append win_zil.rsp $(WIN_LINK_OPTS) 
	@%append win_zil.rsp N wzincapp 
	@%append win_zil.rsp F zincapp.obw,control.obw,display.obw,window.obw,event.obw,help.obw,support.obw $(WIN_OBJS)
	@%append win_zil.rsp L $(WIN_LIBS)
	$(WIN_LINK) @win_zil.rsp
	del win_zil.rsp
#	wrc $(WIN_RC_OPTS) $*.rc $*.exe


# ----- OS/2 ----------------------------------------------------------------
os2: ozincapp.exe .SYMBOLIC

ozincapp.exe: zincapp.obo control.obo display.obo window.obo event.obo help.obo support.obo
	%create os2_zil.rsp
	@%append os2_zil.rsp $(OS2_LINK_OPTS) 
	@%append os2_zil.rsp N ozincapp 
	@%append os2_zil.rsp F zincapp.obo,control.obo,display.obo,window.obo,event.obo,help.obo,support.obo $(OS2_OBJS)
	@%append os2_zil.rsp L $(OS2_LIBS)
	$(OS2_LINK) @os2_zil.rsp
	del os2_zil.rsp


# ----- DESQview/X Motif ----------------------------------------------------
dvx: mzincapp.exe .SYMBOLIC

mzincapp.exe: zincapp.obm control.obm display.obm window.obm event.obm help.obm support.obm
	%create dvx_zil.rsp
	@%append dvx_zil.rsp $(DVX_LINK_OPTS) 
	@%append dvx_zil.rsp N mzincapp 
	@%append dvx_zil.rsp F zincapp.obm,control.obm,display.obm,window.obm,event.obm,help.obm,support.obm $(DVX_OBJS)
	@%append dvx_zil.rsp L $(DVX_LIBS)
	$(DVX_LINK) @dvx_zil.rsp
	del dvx_zil.rsp
