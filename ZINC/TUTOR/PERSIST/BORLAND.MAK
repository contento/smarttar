# PERSIST tutorial makefile

#    make -fborland.mak dos		      	(makes all the DOS persist tutorials)
#    make -fborland.mak dos16	      	(makes all the DOS extender persist tutorials)
#    make -fborland.mak windows	      	(makes all the Windows persist tutorials)
#    make -fborland.mak os2		      	(makes all the OS/2 persist tutorials)

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
DOS_LIBS=graphics emu mathl cl

# ----- Windows compiler options --------------------------------------------

# --- Use the next 2 lines for Windows 3.0 compatibility ---
WIN_CPP_OPTS=-c -ml -O -WE -w -DWINVER=0x0300
WIN_RC_OPTS=-k -30
# --- Use the next 2 lines for Windows 3.1 compatibility ---
#WIN_CPP_OPTS=-c -ml -O -WE -w
#WIN_RC_OPTS=-k
WIN_LINK_OPTS=/c /C /Twe /x
WIN_OBJS=c0wl
WIN_LIBS=mathwl import cwl

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

.c.obj:
	$(CPP) $(DOS_CPP_OPTS) {$< }

.cpp.obj:
	$(CPP) $(DOS_CPP_OPTS) {$< }

.c.obw:
	$(CPP) $(WIN_CPP_OPTS) -o$*.obw {$< }

.cpp.obw:
	$(CPP) $(WIN_CPP_OPTS) -o$*.obw {$< }

.c.obn:
	$(WNT_CPP) $(WNT_CPP_OPTS) -o$*.obn {$< }

.cpp.obn:
	$(WNT_CPP) $(WNT_CPP_OPTS) -o$*.obn {$< }

.c.obo:
	$(CPP) $(OS2_CPP_OPTS) -o$*.obo {$< }

.cpp.obo:
	$(CPP) $(OS2_CPP_OPTS) -o$*.obo {$< }

# --- Use the next line for PharLap Extended programs. ---
D16_CPP=bcc286
D16_LINK=tlink

D16_CPP_OPTS=-c -ml -O1 -V -w
D16_LINK_OPTS=/c /x /C
D16_LIB_OPTS=/C

D16_OBJS=c0pl.obj
# --- Use the next line for UI_BGI_DISPLAY ---
D16_LIBS=phapi d16_zil d16_bgi graph286 emu286 mathl bcl286

.cpp.o16:
	$(D16_CPP) $(D16_CPP_OPTS) -o$*.o16 {$< }

.c.o16:
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
dos: persist1.exe persist2.exe persist3.exe persist4.exe persist5.exe persist6.exe persist7.exe

persist1.exe: persist1.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+persist1.obj
$*, ,$(DOS_LIBS)
!

persist2.exe: persist2.obj draw.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+persist2.obj+draw.obj
$*, ,$(DOS_LIBS)
!

persist3.exe: persist3.obj draw.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+persist3.obj+draw.obj
$*, ,$(DOS_LIBS)
!

persist4.exe: persist4.obj draw.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+persist4.obj+draw.obj
$*, ,$(DOS_LIBS)
!

persist5.exe: persist5.obj draw.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+persist5.obj+draw.obj
$*, ,$(DOS_LIBS)
!

persist6.exe: persist6.obj draw.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+persist6.obj+draw.obj
$*, ,$(DOS_LIBS)
!

persist7.exe: persist7.obj draw.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+persist7.obj+draw.obj
$*, ,$(DOS_LIBS)
!

# ----- Windows -------------------------------------------------------------
windows: wpersist.exe

wpersist.exe: wpersist.obw wdraw.obw
	$(LINK) $(WIN_LINK_OPTS) @&&!
$(WIN_OBJS)+wpersist.obw wdraw.obw
$*, ,$(WIN_LIBS),wpersist.def
!
	$(RC) $(WIN_RC_OPTS) wpersist.rc $<

# ----- Windows NT ----------------------------------------------------------
winnt: npersist.exe

npersist.exe: wpersist.obn wdraw.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+wpersist.obn+wdraw.obn
$*, ,$(WNT_LIBS),wpersist.def
!
#	$(WNT_RC) $(WNT_RC_OPTS) wpersist.rc $<

# ----- OS/2 ----------------------------------------------------------------
os2: opersist.exe

opersist.exe: opersist.obo odraw.obo
	$(LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+opersist.obo odraw.obo
$*, ,$(OS2_LIBS),opersist.def
!
	$(RC) opersist.rc $<

# ----- DOS Extender ----------------------------------------------------------
dos16: persi116.exe persi216.exe persi316.exe persi416.exe persi516.exe persi616.exe persi716.exe

persi116.exe: persist1.o16
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+persist1.o16
$*, ,$(D16_LIBS), b16.def
!

persi216.exe: persist2.o16 draw.o16
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+persist2.o16+draw.o16
$*, ,$(D16_LIBS), b16.def
!

persi316.exe: persist3.o16 draw.o16
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+persist3.o16+draw.o16
$*, ,$(D16_LIBS), b16.def
!

persi416.exe: persist4.o16 draw.o16
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+persist4.o16+draw.o16
$*, ,$(D16_LIBS), b16.def
!

persi516.exe: persist5.o16 draw.o16
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+persist5.o16+draw.o16
$*, ,$(D16_LIBS), b16.def
!

persi616.exe: persist6.o16 draw.o16
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+persist6.o16+draw.o16
$*, ,$(D16_LIBS), b16.def
!

persi716.exe: persist7.o16 draw.o16
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+persist7.o16+draw.o16
$*, ,$(D16_LIBS), b16.def
!
