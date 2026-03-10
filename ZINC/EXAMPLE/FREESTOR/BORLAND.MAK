# FREESTOR example makefile

#    make -fborland.mak dos		      	(makes the DOS freestore program)
#    make -fborland.mak dos16	      	(makes the DOS extender freestore program)

# Be sure to update your TURBOC.CFG file to include the Zinc paths, e.g.:
#   -I.;C:\ZINC\INCLUDE;C:\BORLANDC\INCLUDE
#   -L.;C:\ZINC\LIB\BTCPP310;C:\BORLANDC\LIB
# and your TLINK.CFG file to include the Zinc paths, e.g.:
#   -L.;C:\ZINC\LIB\BTCPP310;C:\BORLANDC\LIB

## Compiler and linker: (Add -v to CPP_OPTS and /v to LINK_OPTS for debug.)

# ----- DOS compiler options ------------------------------------------------
DOS_CPP_OPTS=-c -ml -O -w
DOS_LINK_OPTS=/c /x
DOS_OBJS=c0l
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
DOS_LIBS=dos_zil dos_gfx bc_lgfx emu mathl cl
# --- Use the next line for UI_BGI_DISPLAY ---
#DOS_LIBS=dos_zil dos_bgi graphics emu mathl cl

CPP=bcc
LINK=tlink

.cpp.obj:
	$(CPP) $(DOS_CPP_OPTS) {$< }

# ----- DOS extender options ------------------------------------------------
D16_CPP=bcc286
D16_LINK=tlink

D16_CPP_OPTS=-c -ml -O1 -V -w
D16_LINK_OPTS=/c /x /C
D16_LIB_OPTS=/C

D16_OBJS=c0pl.obj
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
D16_LIBS=phapi d16_zil d16_gfx bc_16gfx emu286 emu mathl bcl286
# --- Use the next line for UI_BGI_DISPLAY ---
#D16_LIBS=phapi d16_zil d16_bgi graph286 emu286 mathl bcl286

.cpp.o16:
	$(D16_CPP) $(D16_CPP_OPTS) -o$*.o16 {$< }

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To make this Zinc example for Borland C++ type:
	@echo make -fborland.mak dos
	@echo make -fborland.mak dos16
	@echo (OS/2 environments do not support this example)
	@echo (Windows environments do not support this example)
	@echo (Windows NT environments do not support this example)
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean

# ----- DOS -----------------------------------------------------------------
dos: freestor.exe

freestor.exe: freestor.obj mem_err.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+freestor.obj+mem_err.obj
$*, ,$(DOS_LIBS)
!

# ----- DOS Extender ----------------------------------------------------------
dos16: freest16.exe

freest16.exe: freestor.o16 mem_err.o16
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+freestor.o16+mem_err.o16
$*, ,$(D16_LIBS), b16.def
!
