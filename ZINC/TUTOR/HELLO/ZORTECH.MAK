# HELLO tutorial makefile

#    make -fzortech.mak dos		      	(makes all the DOS hello tutorials)
#    make -fzortech.mak dos16	      	(makes all the DOS extender hello tutorials)
#    make -fzortech.mak dos32	      	(makes all the DOS extender hello tutorials)
#    make -fzortech.mak windows	      	(makes all the Windows hello tutorials)
#    make -fzortech.mak os2		      	(makes all the OS/2 hello tutorials)

# Be sure to set the LIB and INCLUDE environment variables for Zinc, e.g.:
#    set INCLUDE=.;C:\ZINC\INCLUDE;C:\ZTC\INCLUDE
#    set LIB=.;C:\ZINC\LIB\ZTCPP300;C:\ZTC\LIB

## Compiler and linker: (Add -g to CPP_OPTS and /CO to LINK_OPTS for debug.)

# ----- DOS compiler options ------------------------------------------------
DOS_CPP=ztc
DOS_LINK=blinkx

DOS_CPP_OPTS=-c -a1 -bx -ml
DOS_LINK_OPTS=/NOI
DOS_OBJS=
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
DOS_LIBS=dos_zil dos_gfx zt_lgfx
# --- Use the next line for UI_FG_DISPLAY ---
#DOS_LIBS=dos_zil dos_fg fg

# ----- Windows compiler options --------------------------------------------
WIN_CPP=ztc
WIN_LINK=blinkx
WIN_RC=rc

# --- Use the next 2 lines for Windows 3.0 compatibility ---
WIN_CPP_OPTS=-c -a1 -bx -ml -W2 -DWINVER=0x0300
WIN_LINK_OPTS=/NOI /PACKC:42000
WIN_RC_OPTS=-30
# --- Use the next 2 lines for Windows 3.1 compatibility ---
#WIN_CPP_OPTS=-c -a1 -bx -ml -W2
#WIN_RC_OPTS=
WIN_OBJS=
WIN_LIBS=win_zil libw

# ----- OS/2 compiler options -----------------------------------------------
OS2_CPP=ztc
OS2_LINK=blink
OS2_RC=rc

OS2_CPP_OPTS=-c -mf
OS2_LINK_OPTS=/BASE:0x10000 /PM:PM 
OS2_RC_OPTS=
OS2_OBJS=
OS2_LIBS=os2_zil

.c.obj:
	$(DOS_CPP) $(DOS_CPP_OPTS) $<

.cpp.obj:
	$(DOS_CPP) $(DOS_CPP_OPTS) $<

.c.obw:
	$(WIN_CPP) $(WIN_CPP_OPTS) -o$*.obw $<

.cpp.obw:
	$(WIN_CPP) $(WIN_CPP_OPTS) -o$*.obw $<

.c.obo:
	$(OS2_CPP) $(OS2_CPP_OPTS) -o$*.obo $<

.cpp.obo:
	$(OS2_CPP) $(OS2_CPP_OPTS) -o$*.obo $<

# ----- DOS extender compiler options -----------------------------------------
D16_CPP=ztc
D16_LINK=blinkx

D16_CPP_OPTS=-c -bx -mz -D$(FONT)
D16_LINK_OPTS=/NOI
D16_LIB_OPTS=/C /N /B

D16_OBJS=
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
D16_LIBS=zpm d16_zil d16_gfx zt_16gfx
# --- Use the next line for UI_FG_DISPLAY ---
#D16_LIBS=zpm d16_zil d16_fg fg

.cpp.o16:
	$(D16_CPP) $(D16_CPP_OPTS) -o$@ $<

# ----- DOS extender compiler options -----------------------------------------
D32_CPP=ztc
D32_LINK=blinkx

D32_CPP_OPTS=-c -bx -mx -D$(FONT)
D32_LINK_OPTS=/NOI
D32_LIB_OPTS=/C /N /B

D32_OBJS=cx
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
D32_LIBS=d32_zil d32_gfx zt_32gfx
# --- Use the next line for UI_FG_DISPLAY ---
#D32_LIBS=d32_zil d32_fg fgp

.cpp.o32:
	$(D32_CPP) $(D32_CPP_OPTS) -o$@ $<

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To make this Zinc tutorial for Zortech C++ type:
	@echo make -fzortech.mak dos
	@echo make -fzortech.mak dos16
	@echo make -fzortech.mak dos32
	@echo make -fzortech.mak os2
	@echo make -fzortech.mak windows
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean.bat

# ----- DOS -----------------------------------------------------------------
dos: hello1.exe hello2.exe hello3.exe hellog.exe

hello1.exe: hello1.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+hello1.obj,$*, ,$(DOS_LIBS),NUL

hello2.exe: hello2.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+hello2.obj,$*, ,$(DOS_LIBS),NUL

hello3.exe: hello3.obj hello.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+hello3.obj+hello.obj,$*, ,$(DOS_LIBS),NUL

hellog.exe: hellog.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+hellog.obj,$*, ,$(DOS_LIBS),NUL

# ----- Windows -------------------------------------------------------------
windows: whello1.exe whello2.exe whello3.exe whellog.exe

whello1.exe: hello1.obw
	$(WIN_LINK) $(WIN_LINK_OPTS) $(WIN_OBJS)+hello1.obw,$*, ,$(WIN_LIBS),whello.def
	$(WIN_RC) $(WIN_RC_OPTS) whello.rc $*.exe

whello2.exe: hello2.obw
	$(WIN_LINK) $(WIN_LINK_OPTS) $(WIN_OBJS)+hello2.obw,$*, ,$(WIN_LIBS),whello.def
	$(WIN_RC) $(WIN_RC_OPTS) whello.rc $*.exe

whello3.exe: hello3.obw hello.obw
	$(WIN_LINK) $(WIN_LINK_OPTS) $(WIN_OBJS)+hello3.obw+hello.obw,$*, ,$(WIN_LIBS),whello.def
	$(WIN_RC) $(WIN_RC_OPTS) whello.rc $*.exe

whellog.exe: hellog.obw
	$(WIN_LINK) $(WIN_LINK_OPTS) $(WIN_OBJS)+hellog.obw,$*, ,$(WIN_LIBS),whello.def
	$(WIN_RC) $(WIN_RC_OPTS) whello.rc $*.exe

# ----- OS/2 ----------------------------------------------------------------
os2: ohello1.exe ohello2.exe ohello3.exe ohellog.exe

ohello1.exe: hello1.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) $(OS2_OBJS)+hello1.obo,$*, ,$(OS2_LIBS),ohello.def
	$(OS2_RC) $(OS2_RC_OPTS) ohello.rc $*.exe

ohello2.exe: hello2.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) $(OS2_OBJS)+hello2.obo,$*, ,$(OS2_LIBS),ohello.def
	$(OS2_RC) $(OS2_RC_OPTS) ohello.rc $*.exe

ohello3.exe: hello3.obo hello.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) $(OS2_OBJS)+hello3.obo+hello.obo,$*, ,$(OS2_LIBS),ohello.def
	$(OS2_RC) $(OS2_RC_OPTS) ohello.rc $*.exe

ohellog.exe: hellog.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) $(OS2_OBJS)+hellog.obo,$*, ,$(OS2_LIBS),ohello.def
	$(OS2_RC) $(OS2_RC_OPTS) ohello.rc $*.exe

# ----- Dos extender --------------------------------------------------------
dos16: hello116.exe hello216.exe hello316.exe hellog16.exe

hello116.exe: hello1.o16
	$(D16_LINK) $(D16_LINK_OPTS) $(D16_OBJS)+hello1.o16,$*, ,$(D16_LIBS),z16.def

hello216.exe: hello2.o16
	$(D16_LINK) $(D16_LINK_OPTS) $(D16_OBJS)+hello2.o16,$*, ,$(D16_LIBS),z16.def

hello316.exe: hello3.o16 hello.o16
	$(D16_LINK) $(D16_LINK_OPTS) $(D16_OBJS)+hello3.o16+hello.o16,$*, ,$(D16_LIBS),z16.def

hellog16.exe: hellog.o16
	$(D16_LINK) $(D16_LINK_OPTS) $(D16_OBJS)+hellog.o16,$*, ,$(D16_LIBS),z16.def

# ----- Dos extender ----------------------------------------------------------
dos32: hello132.exe hello232.exe hello332.exe hellog32.exe

hello132.exe: hello1.o32
	$(D32_LINK) $(D32_LINK_OPTS) $(D32_OBJS)+hello1.o32,$*, ,$(D32_LIBS)

hello232.exe: hello2.o32
	$(D32_LINK) $(D32_LINK_OPTS) $(D32_OBJS)+hello2.o32,$*, ,$(D32_LIBS)

hello332.exe: hello3.o32 hello.o32
	$(D32_LINK) $(D32_LINK_OPTS) $(D32_OBJS)+hello3.o32+hello.o32,$*, ,$(D32_LIBS)

hellog32.exe: hellog.o32
	$(D32_LINK) $(D32_LINK_OPTS) $(D32_OBJS)+hellog.o32,$*, ,$(D32_LIBS)
