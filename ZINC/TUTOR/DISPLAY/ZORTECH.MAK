# DISPLAY tutorial makefile

#	make -fzortech.mak dos		      	(makes the DOS display program)
#	make -fzortech.mak dos16		    (makes the DOS extender display program)
#	make -fzortech.mak dos32		    (makes the DOS extender display program)

# Be sure to set the LIB and INCLUDE environment variables for Zinc, e.g.:
#    set INCLUDE=.;C:\ZINC\INCLUDE;C:\ZTC\INCLUDE
#    set LIB=.;C:\ZINC\LIB\ZTCPP300;C:\ZTC\LIB

## Compiler and linker: (Add -g to CPP_OPTS and /CO to LINK_OPTS for debug.)

# ----- DOS compiler options ------------------------------------------------
DOS_CPP=ztc
DOS_LINK=blinkx

# --- FG display options ---
DOS_CPP_OPTS=-c -a1 -bx -ml
DOS_LINK_OPTS=/NOI
DOS_OBJS=
DOS_LIBS=dos_zil dos_fg fg

# --- GFX display options ---
#DOS_CPP_OPTS=-c -a1 -bx -ml -DGFX_DISPLAY
#DOS_LINK_OPTS=/NOI
#DOS_OBJS=
#DOS_LIBS=dos_zil dos_gfx zt_lgfx

.c.obj:
	$(DOS_CPP) $(DOS_CPP_OPTS) $<

.cpp.obj:
	$(DOS_CPP) $(DOS_CPP_OPTS) $<

# ----- DOS extender compiler options -----------------------------------------
D16_CPP=ztc
D16_LINK=blinkx

# --- FG display options ---
D16_CPP_OPTS=-c -bx -mz -D$(FONT)
D16_LINK_OPTS=/NOI
D16_LIB_OPTS=/C /N /B
D16_OBJS=
D16_LIBS=zpm d16_zil d16_fg fg

# --- GFX display options ---
#D16_CPP_OPTS=-c -bx -mz -D$(FONT) -DGFX_DISPLAY
#D16_LINK_OPTS=/NOI
#D16_LIB_OPTS=/C /N /B
#D16_OBJS=
#D16_LIBS=zpm d16_zil d16_gfx zt_16gfx

.cpp.o16:
	$(D16_CPP) $(D16_CPP_OPTS) -o$@ $<

# ----- DOS extender compiler options -----------------------------------------
D32_CPP=ztc
D32_LINK=blinkx

# --- FG display options ---
D32_CPP_OPTS=-c -bx -mx -D$(FONT)
D32_LINK_OPTS=/NOI
D32_LIB_OPTS=/C /N /B
D32_OBJS=cx
D32_LIBS=d32_zil d32_fg fgp

# --- GFX display options ---
#D32_CPP_OPTS=-c -bx -mx -D$(FONT) -DGFX_DISPLAY
#D32_LINK_OPTS=/NOI
#D32_LIB_OPTS=/C /N /B
#D32_OBJS=cx
#D32_LIBS=d32_zil d32_gfx zt_32gfx

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
	@echo (This tutorial is not supported in OS/2)
	@echo (This tutorial is not supported in Windows)
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean.bat

# ----- DOS -----------------------------------------------------------------
dos: display.exe

display.exe: display.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+display.obj,$*, ,$(DOS_LIBS),NUL

windows:
	@echo (This tutorial is not supported in Windows or OS/2)

os2:
	@echo (This tutorial is not supported in Windows or OS/2)

# ----- Dos extender ----------------------------------------------------------
dos16: disp16.exe

disp16.exe: display.o16
	$(D16_LINK) $(D16_LINK_OPTS) $(D16_OBJS)+display.o16,$*, ,$(D16_LIBS),z16.def

# ----- Dos extender ----------------------------------------------------------
dos32: disp32.exe

disp32.exe: display.o32
	$(D32_LINK) $(D32_LINK_OPTS) $(D32_OBJS)+display.o32,$*, ,$(D32_LIBS)
