# WORD tutorial makefile

#    make -fborland.mak dos		      	(makes all the DOS word tutorials)
#    make -fborland.mak dos16	      	(makes all the DOS extender word tutorials)
#    make -fborland.mak windows	      	(makes all the Windows word tutorials)
#    make -fborland.mak os2	      		(makes all the OS/2 word tutorials)

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

.c.obj:
	$(CPP) $(DOS_CPP_OPTS) {$< }

.cpp.obj:
	$(CPP) $(DOS_CPP_OPTS) {$< }

.cpp.obw:
	$(CPP) $(WIN_CPP_OPTS) -o$*.obw {$< }

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
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
D16_LIBS=phapi d16_zil d16_gfx bc_16gfx emu286 emu mathl bcl286
# --- Use the next line for UI_BGI_DISPLAY ---
#D16_LIBS=phapi d16_zil d16_bgi graph286 emu286 mathl bcl286

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
dos: word1a.exe word1b.exe word2.exe word3.exe

word1a.exe: word1a.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+word1a.obj
$*, ,$(DOS_LIBS)
!

word1b.exe: word1b.obj list.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+word1b.obj+list.obj
$*, ,$(DOS_LIBS)
!

word2.exe: word2.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+word2.obj
$*, ,$(DOS_LIBS)
!

word3.exe: word3.obj word_win.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+word3.obj+word_win.obj
$*, ,$(DOS_LIBS)
!

# ----- Windows -------------------------------------------------------------
windows: wword2.exe wword3.exe

wword2.exe: word2.obw
	$(LINK) $(WIN_LINK_OPTS) @&&!
$(WIN_OBJS)+word2.obw
$*, ,$(WIN_LIBS),wword.def
!
	$(RC) $(WIN_RC_OPTS) wword.rc $<

wword3.exe: word3.obw word_win.obw
	$(LINK) $(WIN_LINK_OPTS) @&&!
$(WIN_OBJS)+word3.obw+word_win.obw
$*, ,$(WIN_LIBS),wword.def
!
	$(RC) $(WIN_RC_OPTS) wword.rc $<

# ----- Windows NT ----------------------------------------------------------
winnt: nword2.exe nword3.exe

nword2.exe: word2.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+word2.obn
$*, ,$(WNT_LIBS),wword.def
!
#	$(WNT_RC) $(WNT_RC_OPTS) wword.rc $<

nword3.exe: word3.obn word_win.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+word3.obn+word_win.obn
$*, ,$(WNT_LIBS),wword.def
!
#	$(WNT_RC) $(WNT_RC_OPTS) wword.rc $<

# ----- OS/2 ----------------------------------------------------------------
os2: oword1a.exe oword1b.exe oword2.exe oword3.exe

oword1a.exe: word1a.obo
	$(LINK) /c /B:0x10000 /ap @&&!
$(OS2_OBJS)+word1a.obo
$*, ,$(OS2_LIBS)
!

oword1b.exe: word1b.obo list.obo
	$(LINK) /c /B:0x10000 /ap @&&!
$(OS2_OBJS)+word1b.obo+list.obo
$*, ,$(OS2_LIBS)
!

oword2.exe: word2.obo
	$(LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+word2.obo
$*, ,$(OS2_LIBS),oword.def
!
	$(RC) oword.rc $<

oword3.exe: word3.obo word_win.obo
	$(LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+word3.obo+word_win.obo
$*, ,$(OS2_LIBS),oword.def
!
	$(RC) oword.rc $<

# ----- DOS Extender ----------------------------------------------------------
dos16: word1a16.exe word1b16.exe word216.exe word316.exe

word1a16.exe: word1a.o16
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+word1a.o16
$*, ,$(D16_LIBS), b16.def
!

word1b16.exe: word1b.o16 list.o16
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+word1b.o16+list.o16
$*, ,$(D16_LIBS), b16.def
!

word216.exe: word2.o16
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+word2.o16
$*, ,$(D16_LIBS), b16.def
!

word316.exe: word3.o16 word_win.o16
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+word3.o16+word_win.o16
$*, ,$(D16_LIBS), b16.def
!
