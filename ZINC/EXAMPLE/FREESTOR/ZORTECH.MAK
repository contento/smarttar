# FREESTOR example makefile

#    make -fzortech.mak dos		      	(makes the DOS freestore program)
#    make -fzortech.mak dos16	      	(makes the DOS extender freestore program)
#    make -fzortech.mak dos32	      	(makes the DOS extender freestore program)

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

.cpp.obj:
	$(DOS_CPP) $(DOS_CPP_OPTS) $<

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
	@echo (OS/2 environment does not support this example)
	@echo (Windows environments do not support this example)
	@echo (Windows NT environments do not support this example)
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean.bat

# ----- DOS -----------------------------------------------------------------
dos: freestor.exe

freestor.exe: freestor.obj mem_err.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+freestor.obj+mem_err.obj,$*, ,$(DOS_LIBS),NUL

# ----- Dos extender ----------------------------------------------------------
dos16: freest16.exe

freest16.exe: freestor.o16 mem_err.o16
	$(D16_LINK) $(D16_LINK_OPTS) $(D16_OBJS)+freestor.o16+mem_err.o16,$*, ,$(D16_LIBS),z16.def

# ----- Dos extender ----------------------------------------------------------
dos32: freest32.exe

freest32.exe: freestor.o32 mem_err.o32
	$(D32_LINK) $(D32_LINK_OPTS) $(D32_OBJS)+freestor.o32+mem_err.o32,$*, ,$(D32_LIBS)
