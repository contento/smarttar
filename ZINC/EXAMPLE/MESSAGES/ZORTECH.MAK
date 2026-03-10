# MESSAGES example makefile

#    make -fzortech.mak dos		      	(makes the DOS messages program)
#    make -fzortech.mak dos16	      	(makes the DOS extender messages program)
#    make -fzortech.mak dos32	      	(makes the DOS extender messages program)
#    make -fzortech.mak windows	      	(makes the Windows messages program)
#    make -fzortech.mak os2	      	 	(makes the OS/2 messages program)

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
WIN_RC_OPTS=-30
# --- Use the next 2 lines for Windows 3.1 compatibility ---
#WIN_CPP_OPTS=-c -a1 -bx -ml -W2
#WIN_RC_OPTS=
WIN_LINK_OPTS=/NOI /PACKC:42000
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
	@echo To make this Zinc example for Zortech C++ type:
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
dos: messages.exe

messages.exe: messages.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+messages.obj,$*, ,$(DOS_LIBS),NUL

# ----- Windows -------------------------------------------------------------
windows: wmessage.exe

wmessage.exe: messages.obw
	$(WIN_LINK) $(WIN_LINK_OPTS) $(WIN_OBJS)+messages.obw,$*, ,$(WIN_LIBS),wmessage.def
	$(WIN_RC) $(WIN_RC_OPTS) wmessage.rc $*.exe

# ----- OS/2 ----------------------------------------------------------------
os2: omessage.exe

omessage.exe: messages.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) $(OS2_OBJS)+messages.obo,$*, ,$(OS2_LIBS),omessage.def
	$(OS2_RC) $(OS2_RC_OPTS) omessage.rc $*.exe

# ----- Dos extender ----------------------------------------------------------
dos16: messag16.exe

messag16.exe: messages.o16
	$(D16_LINK) $(D16_LINK_OPTS) $(D16_OBJS)+messages.o16,$*, ,$(D16_LIBS),z16.def


# ----- Dos extender ----------------------------------------------------------
dos32: messag32.exe

messag32.exe: messages.o32
	$(D32_LINK) $(D32_LINK_OPTS) $(D32_OBJS)+messages.o32,$*, ,$(D32_LIBS)
