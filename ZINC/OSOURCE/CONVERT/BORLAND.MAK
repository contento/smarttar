# GENHELP and ICO2DAT program makefile

#    make -fborland.mak dos		      	(makes the DOS genhelp/ico2dat programs)
#    make -fborland.mak windows	      	(makes the Windows genhelp/ico2dat program)
#    make -fborland.mak os2	      	 	(makes the OS/2 genhelp/ico2dat program)

# Be sure to update your TURBOC.CFG file to include the Zinc paths, e.g.:
#   -I.;C:\ZINC\INCLUDE;C:BORLANDC\INCLUDE
#   -L.;C:\ZINC\LIB\BTCPP310;C:BORLANDC\LIB
# and your TLINK.CFG file to include the Zinc paths, e.g.:
#   -L.;C:\ZINC\LIB\BTCPP310;C:BORLANDC\LIB

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
WIN_CPP_OPTS=-c -ml -O -WE -w
WIN_LINK_OPTS=/c /C /Twe /x
WIN_OBJS=c0wl
WIN_LIBS=win_zil mathwl import cwl

# ----- Windows NT compiler options -----------------------------------------
WNT_CPP=bcc32
WNT_LINK=tlink32
WNT_LIBRARIAN=tlib32
WNT_RC=brc32

WNT_CPP_OPTS=-c
WNT_LINK_OPTS=/ap /Tpe /x /c
WNT_RC_OPTS=-w32
WNT_LIB_OPTS=/C

WNT_OBJS=c0x32
WNT_LIBS=wnt_zil imprtw32 import32 cx32

# ----- OS/2 compiler options -----------------------------------------------
OS2_CPP_OPTS=-c
OS2_LINK_OPTS=/c /ap
OS2_OBJS=c02.obj
OS2_LIBS=os2_zil.lib c2.lib os2.lib

CPP=bcc
LINK=tlink

.cpp.obj:
	$(CPP) $(DOS_CPP_OPTS) {$< }

.cpp.obw:
	$(CPP) $(WIN_CPP_OPTS) -o$*.obw {$< }

.cpp.obo:
	$(CPP) $(OS2_CPP_OPTS) -o$*.obo {$< }

.cpp.obn:
	$(WNT_CPP) $(WNT_CPP_OPTS) -o$*.obn {$< }

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To make the genhelp and ico2dat programs for Borland C++ type:
	@echo make -fborland.mak dos
	@echo make -fborland.mak os2
	@echo make -fborland.mak winnt
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean

# ----- DOS -----------------------------------------------------------------
dos: genhelp.exe bmp2dat.exe ico2dat.exe
	copy genhelp.exe ..\..\bin
	copy bmp2dat.exe ..\..\bin
	copy ico2dat.exe ..\..\bin

genhelp.exe: genhelp.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+genhelp.obj
$*, ,$(DOS_LIBS)
!

bmp2dat.exe : bmp2dat.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+bmp2dat.obj
$*, ,$(DOS_LIBS)
!

ico2dat.exe : ico2dat.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+ico2dat.obj
$*, ,$(DOS_LIBS)
!

# ----- Windows -------------------------------------------------------------
windows: genhelp.exe bmp2dat.exe ico2dat.exe
	copy genhelp.exe ..\..\bin
	copy bmp2dat.exe ..\..\bin
	copy ico2dat.exe ..\..\bin

# ----- Windows NT ----------------------------------------------------------
winnt: ngenhelp.exe nbmp2dat.exe nico2dat.exe
	copy ngenhelp.exe ..\..\bin
	copy nbmp2dat.exe ..\..\bin
	copy nico2dat.exe ..\..\bin

ngenhelp.exe: genhelp.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+genhelp.obn
$*, ,$(WNT_LIBS)
!

nbmp2dat.exe : bmp2dat.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+bmp2dat.obn
$*, ,$(WNT_LIBS)
!

nico2dat.exe : ico2dat.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+ico2dat.obn
$*, ,$(WNT_LIBS)
!

# ----- OS/2 ----------------------------------------------------------------
os2: ogenhelp.exe obmp2dat.exe oico2dat.exe
	copy ogenhelp.exe ..\..\bin
	copy obmp2dat.exe ..\..\bin
	copy oico2dat.exe ..\..\bin

ogenhelp.exe: genhelp.obo
	$(LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+genhelp.obo
$*, ,$(OS2_LIBS),
!

obmp2dat.exe: bmp2dat.obo
	$(LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+bmp2dat.obo
$*, ,$(OS2_LIBS),
!

oico2dat.exe: ico2dat.obo
	$(LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+ico2dat.obo
$*, ,$(OS2_LIBS),
!

