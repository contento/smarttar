# CALC example makefile

#    make -fborland.mak dos		      	(makes the DOS calc program)
#    make -fborland.mak dos16	      	(makes the DOS extender calc program)
#    make -fborland.mak windows	      	(makes the Windows calc program)
#    make -fborland.mak os2	      	 	(makes the OS/2 calc program)

# Be sure to update your TURBOC.CFG file to include the Zinc paths, e.g.:
#   -I.;C:\ZINC\INCLUDE;C:\BORLANDC\INCLUDE
#   -L.;C:\ZINC\LIB\BTCPP310;C:\BORLANDC\LIB
# and your TLINK.CFG file to include the Zinc paths, e.g.:
#   -L.;C:\ZINC\LIB\BTCPP310;C:\BORLANDC\LIB
# and and add the Pharlap directories if you are using the dos16 stuff.

## Compiler and linker: (Add -v to CPP_OPTS and /v to LINK_OPTS for debug.)

# ----- DOS compiler options ------------------------------------------------
DOS_CPP_OPTS=-c -ml -O -w
DOS_LINK_OPTS=/c /x
DOS_OBJS=c0l
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
DOS_LIBS=dos_zil dos_gfx bc_lgfx emu mathl cl
# --- Use the next line for UI_BGI_DISPLAY ---
#DOS_LIBS=dos_zil dos_bgi graphics emu mathl cl

# ----- Windows compiler options --------------------------------------------

# --- Use the next 2 lines for Windows 3.0 compatibility ---
WIN_CPP_OPTS=-c -ml -O -WE -w -DWINVER=0x0300
WIN_RC_OPTS=-k -30
# --- Use the next 2 lines for Windows 3.1 compatibility ---
#WIN_CPP_OPTS=-c -ml -O -WE -w
#WIN_RC_OPTS=-k
WIN_LINK_OPTS=/c /C /Twe /x
WIN_OBJS=c0wl
WIN_LIBS=win_zil mathwl import cwl

# ----- Windows NT compiler options -----------------------------------------
WNT_CPP=bcc32
WNT_LINK=tlink32
WNT_RC=brc32

WNT_CPP_OPTS=-c -W
WNT_LINK_OPTS=-Tpe -aa -x -c
WNT_RC_OPTS=-w32
WNT_LIB_OPTS=/C

WNT_OBJS=c0w32
WNT_LIBS=wnt_zil imprtw32 import32 cw32

# ----- OS/2 compiler options -----------------------------------------------
OS2_CPP_OPTS=-c
OS2_LINK_OPTS=/c /B:0x10000 /aa
OS2_OBJS=c02.obj
OS2_LIBS=os2_zil.lib c2.lib os2.lib

CPP=bcc
LINK=tlink
RC=rc

.cpp.obj:
	$(CPP) $(DOS_CPP_OPTS) {$< }

.cpp.obw:
	$(CPP) $(WIN_CPP_OPTS) -o$*.obw {$< }

.cpp.obn:
	$(WNT_CPP) $(WNT_CPP_OPTS) -o$*.obn {$< }

.cpp.obo:
	$(CPP) $(OS2_CPP_OPTS) -o$*.obo {$< }

# --- Use the next line for PharLap Extended programs. ---
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
	@echo make -fborland.mak os2
	@echo make -fborland.mak windows
	@echo make -fborland.mak winnt
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean

# ----- DOS -----------------------------------------------------------------
dos: calc.exe

calc.exe: calc.obj calcico.obj 
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+calc+calcico.obj
$*, ,$(DOS_LIBS)
!

# ----- Windows -------------------------------------------------------------
windows: wcalc.exe

wcalc.exe: calc.obw calcico.obw
	$(LINK) $(WIN_LINK_OPTS) @&&!
$(WIN_OBJS)+calc.obw+calcico.obw
$*, ,$(WIN_LIBS),wcalc.def
!
	$(RC) $(WIN_RC_OPTS) wcalc.rc $<

# ----- Windows NT ----------------------------------------------------------
winnt: ncalc.exe

ncalc.exe: calc.obn calcico.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+calc.obn+calcico.obn
$*, ,$(WNT_LIBS),wcalc.def
!
#	$(WNT_RC) $(WNT_RC_OPTS) wcalc.rc $<

# ----- OS/2 ----------------------------------------------------------------
os2: ocalc.exe

ocalc.exe: calc.obo calcico.obo
	$(LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+calc.obo+calcico.obo
$*, ,$(OS2_LIBS),ocalc.def
!
	$(RC) ocalc.rc $<

# ----- DOS Extender ----------------------------------------------------------
dos16: calc16.exe

calc16.exe: calc.o16 calcico.o16 
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+calc.o16+calcico.o16
$*, ,$(D16_LIBS), b16.def
!
