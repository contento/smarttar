# HELLO tutorial makefile

#    make -fborland.mak dos		      	(makes all the DOS hello tutorials)
#    make -fborland.mak dos16	      	(makes all the DOS extender hello tutorials)
#    make -fborland.mak windows	      	(makes all the Windows hello tutorials)
#    make -fborland.mak os2		      	(makes all the OS/2 hello tutorials)

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
	@echo To make this Zinc tutorial for Borland C++ type:
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
dos: hello1.exe hello2.exe hello3.exe hellog.exe

hello1.exe: hello1.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+hello1.obj
$*, ,$(DOS_LIBS)
!

hello2.exe: hello2.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+hello2.obj
$*, ,$(DOS_LIBS)
!

hello3.exe: hello3.obj hello.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+hello3.obj+hello.obj
$*, ,$(DOS_LIBS)
!

hellog.exe: hellog.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+hellog.obj
$*, ,$(DOS_LIBS)
!

# ----- Windows -------------------------------------------------------------
windows: whello1.exe whello2.exe whello3.exe whellog.exe

whello1.exe: hello1.obw
	$(LINK) $(WIN_LINK_OPTS) @&&!
$(WIN_OBJS)+hello1.obw
$*, ,$(WIN_LIBS),whello.def
!
	$(RC) $(WIN_RC_OPTS) whello.rc $<

whello2.exe: hello2.obw
	$(LINK) $(WIN_LINK_OPTS) @&&!
$(WIN_OBJS)+hello2.obw
$*, ,$(WIN_LIBS),whello.def
!
	$(RC) $(WIN_RC_OPTS) whello.rc $<

whello3.exe: hello3.obw hello.obw
	$(LINK) $(WIN_LINK_OPTS) @&&!
$(WIN_OBJS)+hello3.obw+hello.obw
$*, ,$(WIN_LIBS),whello.def
!
	$(RC) $(WIN_RC_OPTS) whello.rc $<

whellog.exe: hellog.obw
	$(LINK) $(WIN_LINK_OPTS) @&&!
$(WIN_OBJS)+hellog.obw
$*, ,$(WIN_LIBS),whello.def
!
	$(RC) $(WIN_RC_OPTS) whello.rc $<

# ----- Windows NT ----------------------------------------------------------
winnt: nhello1.exe nhello2.exe nhello3.exe nhellog.exe

nhello1.exe: hello1.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+hello1.obn
$*, ,$(WNT_LIBS),whello.def
!
#	$(WNT_RC) $(WNT_RC_OPTS) whello.rc $<

nhello2.exe: hello2.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+hello2.obn
$*, ,$(WNT_LIBS),whello.def
!
#	$(WNT_RC) $(WNT_RC_OPTS) whello.rc $<

nhello3.exe: hello3.obn hello.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+hello3.obn+hello.obn
$*, ,$(WNT_LIBS),whello.def
!
#	$(WNT_RC) $(WNT_RC_OPTS) whello.rc $<

nhellog.exe: hellog.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+hellog.obn
$*, ,$(WNT_LIBS),whello.def
!
#	$(WNT_RC) $(WNT_RC_OPTS) whello.rc $<

# ----- OS/2 ----------------------------------------------------------------
os2: ohello1.exe ohello2.exe ohello3.exe ohellog.exe

ohello1.exe: hello1.obo
	$(LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+hello1.obo
$*, ,$(OS2_LIBS),ohello.def
!
	$(RC) ohello.rc $<

ohello2.exe: hello2.obo
	$(LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+hello2.obo
$*, ,$(OS2_LIBS),ohello.def
!
	$(RC) ohello.rc $<

ohello3.exe: hello3.obo hello.obo
	$(LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+hello3.obo+hello.obo
$*, ,$(OS2_LIBS),ohello.def
!
	$(RC) ohello.rc $<

ohellog.exe: hellog.obo
	$(LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+hellog.obo
$*, ,$(OS2_LIBS),ohello.def
!
	$(RC) ohello.rc $<

# ----- DOS Extender --------------------------------------------------------
dos16: hello116.exe hello216.exe hello316.exe hellog16.exe

hello116.exe: hello1.o16
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+hello1.o16
$*, ,$(D16_LIBS), b16.def
!

hello216.exe: hello2.o16
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+hello2.o16
$*, ,$(D16_LIBS), b16.def
!

hello316.exe: hello3.o16 hello.o16
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+hello3.o16+hello.o16
$*, ,$(D16_LIBS), b16.def
!

hellog16.exe: hellog.o16
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+hellog.o16
$*, ,$(D16_LIBS), b16.def
!
