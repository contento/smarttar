# DISPLAY tutorial makefile

#	make -fborland.mak dos		      	(makes the DOS display program)
#	make -fborland.mak dos16	      	(makes the DOS extender display program)

# Be sure to update your TURBOC.CFG file to include the Zinc paths, e.g.:
#   -I.;C:\ZINC\INCLUDE;C:\BORLANDC\INCLUDE
#   -L.;C:\ZINC\LIB\BTCPP310;C:\BORLANDC\LIB
# and your TLINK.CFG file to include the Zinc paths, e.g.:
#   -L.;C:\ZINC\LIB\BTCPP310;C:\BORLANDC\LIB

## Compiler and linker: (Add -v to CPP_OPTS and /v to LINK_OPTS for debug.)

# ----- DOS compiler options ------------------------------------------------
# --- BGI display options ---
DOS_CPP_OPTS=-c -ml -O -w
DOS_LINK_OPTS=/c /x
DOS_OBJS=c0l
DOS_LIBS=dos_zil dos_bgi graphics emu mathl cl

# --- GFX display options ---
#DOS_CPP_OPTS=-c -ml -O -w -DGFX_DISPLAY
#DOS_LINK_OPTS=/c /x
#DOS_OBJS=c0l
#DOS_LIBS=dos_zil dos_gfx bc_lgfx emu mathl cl

CPP=bcc
LINK=tlink

.cpp.obj:
	$(CPP) $(DOS_CPP_OPTS) {$< }

# --- Use the next line for PharLap Extended programs. ---
D16_CPP=bcc286
D16_LINK=tlink

# --- BGI display options ---
D16_CPP_OPTS=-c -ml -O1 -V -w
D16_LINK_OPTS=/c /x /C
D16_OBJS=c0pl.obj
D16_LIBS=phapi d16_zil d16_bgi graph286 emu286 mathl bcl286

# --- GFX display options ---
#D16_CPP_OPTS=-c -ml -O1 -V -w -DGFX_DISPLAY
#D16_LINK_OPTS=/c /x /C
#D16_OBJS=c0pl.obj
#D16_LIBS=phapi d16_zil d16_gfx bc_16gfx emu286 emu mathl bcl286

.cpp.o16:
	$(D16_CPP) $(D16_CPP_OPTS) -o$*.o16 {$< }

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To make this Zinc tutorial for Borland C++ type:
	@echo make -fborland.mak dos
	@echo make -fborland.mak dos16
	@echo (This tutorial is not supported in OS/2)
	@echo (This tutorial is not supported in Windows)
	@echo (This tutorial is not supported in Winnt)
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean

# ----- DOS -----------------------------------------------------------------
dos: display.exe

display.exe: display.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+display.obj
$*, ,$(DOS_LIBS)
!

windows:
	@echo (This tutorial is not supported in Windows or OS/2)

winnt:
	@echo (This tutorial is not supported in Windows NT)

os2:
	@echo (This tutorial is not supported in Windows or OS/2)

# ----- DOS Extender ----------------------------------------------------------
dos16: disp16.exe

disp16.exe: display.o16
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+display.o16
$*, ,$(D16_LIBS), b16.def
!
