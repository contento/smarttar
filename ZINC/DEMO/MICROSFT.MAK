# DEMONSTRATION program makefile

#    make -fmicrosft.mak dos		(makes the DOS demo program)
#    make -fmicrosft.mak windows	(makes the Windows demo program)

# Be sure to set the LIB and INCLUDE environment variables for Zinc, e.g.:
#    set INCLUDE=.;C:\ZINC\INCLUDE;C:\C700\INCLUDE
#    set LIB=.;C:\ZINC\LIB\MSCPP700;C:\C700\LIB

## Compiler and linker: (Add -Zi to CPP_OPTS and /CO to LINK_OPTS for debug.)

# ----- DOS compiler options ------------------------------------------------
DOS_CPP_OPTS=-c -AL -BATCH -Gs -Os
DOS_LINK_OPTS=/NOD /NOI /BATCH /STACK:5120 /SEGMENTS:256
DOS_OBJS=txtonly.obj
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
DOS_LIBS=dos_zil dos_gfx ms_lgfx llibce graphics
# --- Use the next line for UI_BGI_DISPLAY ---
#DOS_LIBS=dos_zil dos_msc llibce graphics

# ----- Windows compiler options --------------------------------------------

# --- Use the next 2 lines for Windows 3.0 compatibility ---
WIN_CPP_OPTS=-c -AL -BATCH -Gsw -Os -DWINVER=0x0300
WIN_RC_OPTS=-k -30
# --- Use the next 2 lines for Windows 3.1 compatibility ---
#WIN_CPP_OPTS=-c -AL -BATCH -Gsw
#WIN_RC_OPTS=-k
WIN_LINK_OPTS=/NOD /NOI /BATCH /STACK:5120 /SEGMENTS:256
WIN_OBJS=
WIN_LIBS=win_zil libw llibcew

CPP=cl
LINK=link
RC=rc

.cpp.obj:
	$(CPP) $(DOS_CPP_OPTS) $<

.cpp.obw:
	$(CPP) $(WIN_CPP_OPTS) -Fo$*.obw $<

# ----- Dos Extender ---------------------------------------------------------
D16_CPP=cl
D16_LINK=link

D16_CPP_OPTS=-c -AL -BATCH -Gs2 -nologo -DDOSX286
D16_LINK_OPTS=/NOD /NOI /BATCH /SEGMENTS:256 /STACK:5120 /FAR
D16_LIB_OPTS=/NOI

# --- Use the next line for UI_GRAPHICS_DISPLAY ---
D16_OBJS=graphp.obj
D16_LIBS=phapi d16_zil d16_gfx ms_16gfx llibcep graphics
# --- Use the next lines for UI_MSC_DISPLAY ---
#D16_OBJS=graphp.obj
#D16_LIBS=phapi llibcep graphics d16_msc d16_zil

D16_BIND=bind286
PHARLAP_RTL=c:\pharlap\rtk
D16_LOAD=run286a

.cpp.o16:
	$(D16_CPP) $(D16_CPP_OPTS) -Fo$*.o16 $<

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To make the Zinc demonstration for Microsoft C++ type:
	@echo make -fmicrosft.mak dos
	@echo make -fmicrosft.mak dos16
	@echo make -fmicrosft.mak pen
	@echo make -fmicrosft.mak windows
	@echo (Microsoft does not support OS/2 programs.)
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean

# ----- DOS -----------------------------------------------------------------
dos: zincpost.dat zinc_dos.exe

zincpost.dat: zincpost.txt
	genhelp zincpost

zinc_dos.exe: zincdemo.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+zincdemo.obj
$*,NUL,$(DOS_LIBS),NUL
<<

# ----- PEN -----------------------------------------------------------------
pen: zincpost.dat zinc_pen.exe

.cpp.obp:
	$(CPP) $(DOS_CPP_OPTS) -DPEN_DEMO -Fo$*.obp $<

d_pendos.obj:
	copy ..\lib\mscpp700\d_pendos.obj

zinc_pen.exe: zincdemo.obp d_pendos.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+zincdemo.obp+d_pendos.obj
$*,NUL,$(DOS_LIBS),NUL
<<

# ----- Windows -------------------------------------------------------------
windows: zincpost.dat zinc_win.exe

zinc_win.exe: zincdemo.obw
	$(LINK) $(WIN_LINK_OPTS) @<<zil.rsp
$(WIN_OBJS)+zincdemo.obw
$*,NUL,$(WIN_LIBS),wdemo.def
<<
	$(RC) $(WIN_RC_OPTS) wdemo.rc $*.exe

# ----- DOS Extender ---------------------------------------------------------
dos16: zincpost.dat zinc_d16.exe

zinc_d16.exe: zincdemo.o16
	$(D16_LINK) $(D16_LINK_OPTS) @<<zil.rsp
$(D16_OBJS)+zincdemo.o16
$*,NUL,$(D16_LIBS),NUL
<<
	$(D16_BIND) @<<zil.rsp
$(PHARLAP_RTL)\$(D16_LOAD)
$*
-dll $(PHARLAP_RTL)\moucalls
     $(PHARLAP_RTL)\int33
     $(PHARLAP_RTL)\doscalls
<<
	@del zil.rsp
