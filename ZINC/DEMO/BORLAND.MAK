# DEMONSTRATION program makefile

#    make -fborland.mak dos		(makes the DOS demo program)
#    make -fborland.mak dosvrm		(makes the DOS VROOM demo program)
#    make -fborland.mak dosrtl		(makes the DOS RTLink demo program)
#    make -fborland.mak windows	 	(makes the Windows demo program)
#    make -fborland.mak os2	 	(makes the OS/2 demo program)
#    make -fborland.mak dos16	 	(makes the Extended demo program)

# Be sure to update your TURBOC.CFG file to include the Zinc paths, e.g.:
#   -I.;C:\ZINC\INCLUDE;C:\BORLANDC\INCLUDE
#   -L.;C:\ZINC\LIB\BTCPP310;C:\BORLANDC\LIB
# and your TLINK.CFG file to include the Zinc paths, e.g.:
#   -L.;C:\ZINC\LIB\BTCPP310;C:\BORLANDC\LIB

## Compiler and linker: (Add -v to CPP_OPTS and /v to LINK_OPTS for debug.)

# ----- DOS compiler options ------------------------------------------------
DOS_CPP_OPTS=-c -ml -O1 -w -Y -Z -V
DOS_LINK_OPTS=/c /x
DOS_OBJS=c0l
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
DOS_LIBS=dos_zil dos_gfx bc_lgfx emu mathl cl
# --- Use the next line for UI_BGI_DISPLAY ---
#DOS_LIBS=dos_zil dos_bgi graphics emu mathl cl
DOS_OVR_LIBS=/o- cl mathl emu

# ----- Windows compiler options --------------------------------------------

# --- Use the next 2 lines for Windows 3.0 compatibility ---
WIN_CPP_OPTS=-c -ml -O1 -WE -w -DWINVER=0x0300
WIN_RC_OPTS=-k -30
# --- Use the next 2 lines for Windows 3.1 compatibility ---
#WIN_CPP_OPTS=-c -ml -O1 -WE -w
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
OS2_RC_OPTS=
OS2_OBJS=c02.obj
OS2_LIBS=os2_zil.lib c2.lib os2.lib

# --- PharLap Extended options ---
D16_CPP_OPTS=-c -ml -O1 -w -Z -V
D16_LINK_OPTS=/c /x /C
D16_OBJS=c0pl
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
D16_LIBS=phapi d16_zil d16_gfx bc_16gfx emu286 emu mathl bcl286
# --- Use the next lines for UI_BGI_DISPLAY ---
#D16_LIBS=phapi d16_zil d16_bgi graph286 emu286 emu mathl bcl286
D16_BIND=bind286
PHARLAP_RTL=c:\phar286\rtk
D16_LOAD=run286a

CPP=bcc
CPP16=bcc286
LINK=tlink
RTLINK=rtlink
RC=rc

.cpp.obj:
	$(CPP) $(DOS_CPP_OPTS) {$< }

.cpp.obw:
	$(CPP) $(WIN_CPP_OPTS) -o$*.obw {$< }

.cpp.obn:
	$(WNT_CPP) $(WNT_CPP_OPTS) -o$*.obn {$< }

.cpp.obo:
	$(CPP) $(OS2_CPP_OPTS) -o$*.obo {$< }

.cpp.o16:
	$(CPP16) $(D16_CPP_OPTS) -o$*.o16 {$< }

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To make the Zinc demonstration for Borland C++ type:
	@echo make -fborland.mak dos
	@echo make -fborland.mak dosvrm
	@echo make -fborland.mak dosrtl
	@echo make -fborland.mak pen
	@echo make -fborland.mak windows
	@echo make -fborland.mak os2
	@echo make -fborland.mak dos16
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
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+zincdemo.obj
$*, , $(DOS_LIBS)
!

# ----- PEN -----------------------------------------------------------------
pen: zincpost.dat zinc_pen.exe

.cpp.obp:
	$(CPP) $(DOS_CPP_OPTS) -DZIL_PENDOS -o$*.obp {$< }

d_pendos.obj:
	copy ..\lib\btcpp310\d_pendos.obj

zinc_pen.exe: zincdemo.obp d_pendos.obj
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+zincdemo.obp+d_pendos.obj
$*, , $(DOS_LIBS)
!

# ----- DOS VROOM ------------------------------------------------------------
dosvrm: zincpost.dat zinc_vrm.exe

zinc_vrm.exe: zincdemo.obj \
	..\SOURCE\Z_STRING.OBJ ..\SOURCE\Z_TIME1.OBJ ..\SOURCE\Z_TITLE.OBJ \
	..\SOURCE\Z_REAL.OBJ ..\SOURCE\Z_TIME.OBJ ..\SOURCE\Z_REGION.OBJ \
	..\SOURCE\Z_POPUP1.OBJ ..\SOURCE\Z_POPUP.OBJ ..\SOURCE\Z_PLLDN1.OBJ \
	..\SOURCE\Z_PLLDN.OBJ ..\SOURCE\Z_PATH.OBJ ..\SOURCE\Z_ICON.OBJ \
	..\SOURCE\Z_LIST1.OBJ ..\SOURCE\Z_LIST.OBJ ..\SOURCE\Z_BNUM.OBJ \
	..\SOURCE\Z_SYS.OBJ ..\SOURCE\Z_EVENT.OBJ ..\SOURCE\Z_WIN.OBJ \
	..\SOURCE\Z_TEXT.OBJ ..\SOURCE\Z_CURSOR.OBJ ..\SOURCE\Z_SCROLL.OBJ \
	..\SOURCE\Z_GROUP.OBJ ..\SOURCE\Z_HLIST.OBJ ..\SOURCE\Z_UTILS.OBJ \
	..\SOURCE\Z_INTL.OBJ ..\SOURCE\Z_MAP2.OBJ ..\SOURCE\Z_COMBO.OBJ \
	..\SOURCE\Z_VLIST.OBJ ..\SOURCE\Z_TBAR.OBJ ..\SOURCE\Z_DATE1.OBJ \
	..\SOURCE\Z_WIN1.OBJ ..\SOURCE\Z_BUTTON.OBJ ..\SOURCE\Z_MIN.OBJ \
	..\SOURCE\Z_BNUM1.OBJ ..\SOURCE\Z_MAX.OBJ ..\SOURCE\Z_MAP1.OBJ \
	..\SOURCE\Z_HELP.OBJ ..\SOURCE\Z_DSP.OBJ ..\SOURCE\Z_WIN2.OBJ \
	..\SOURCE\Z_FMTSTR.OBJ ..\SOURCE\Z_ERROR.OBJ ..\SOURCE\Z_DEVICE.OBJ \
	..\SOURCE\Z_DATE.OBJ ..\SOURCE\Z_BORDER.OBJ ..\SOURCE\Z_PROMPT.OBJ \
	..\SOURCE\Z_STORE.OBJ ..\SOURCE\Z_INT.OBJ ..\SOURCE\Z_BNUM2.OBJ \
	..\SOURCE\D_WIN2.OBJ ..\SOURCE\D_POPUP1.OBJ ..\SOURCE\D_STRING.OBJ \
	..\SOURCE\D_ICON.OBJ ..\SOURCE\D_TITLE.OBJ ..\SOURCE\D_MAX.OBJ \
	..\SOURCE\D_GFXDSP.OBJ ..\SOURCE\D_WIN.OBJ ..\SOURCE\D_CURSOR.OBJ \
	..\SOURCE\D_SCROLL.OBJ ..\SOURCE\D_KEYBRD.OBJ ..\SOURCE\D_BORDER.OBJ \
	..\SOURCE\D_COMBO.OBJ ..\SOURCE\D_VLIST.OBJ ..\SOURCE\D_BGIDSP.OBJ \
	..\SOURCE\D_TEXT.OBJ ..\SOURCE\D_SYS.OBJ ..\SOURCE\D_WIN1.OBJ \
	..\SOURCE\D_PLLDN1.OBJ ..\SOURCE\D_GROUP.OBJ ..\SOURCE\D_MOUSE.OBJ \
	..\SOURCE\D_MIN.OBJ ..\SOURCE\D_PROMPT.OBJ ..\SOURCE\D_TBAR.OBJ \
	..\SOURCE\D_TDSP.OBJ ..\SOURCE\D_POPUP.OBJ ..\SOURCE\D_PLLDN.OBJ \
	..\SOURCE\D_HLIST.OBJ ..\SOURCE\D_ERROR.OBJ ..\SOURCE\D_BUTTON.OBJ
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+
#
# Overlayed Modules...
/o+ zincdemo.obj+
..\SOURCE\Z_STRING.OBJ+..\SOURCE\Z_TIME1.OBJ+..\SOURCE\Z_TITLE.OBJ+
..\SOURCE\Z_REAL.OBJ+..\SOURCE\Z_TIME.OBJ+..\SOURCE\Z_REGION.OBJ+
..\SOURCE\Z_PATH.OBJ+..\SOURCE\Z_ICON.OBJ+..\SOURCE\Z_LIST1.OBJ+
..\SOURCE\Z_LIST.OBJ+..\SOURCE\Z_BNUM.OBJ+..\SOURCE\Z_SYS.OBJ+
..\SOURCE\Z_EVENT.OBJ+..\SOURCE\Z_CURSOR.OBJ+..\SOURCE\Z_UTILS.OBJ+
..\SOURCE\Z_INTL.OBJ+..\SOURCE\Z_MAP2.OBJ+..\SOURCE\Z_DATE1.OBJ+
..\SOURCE\Z_MIN.OBJ+..\SOURCE\Z_BNUM1.OBJ+..\SOURCE\Z_MAX.OBJ+
..\SOURCE\Z_MAP1.OBJ+..\SOURCE\Z_HELP.OBJ+..\SOURCE\Z_DSP.OBJ+
..\SOURCE\Z_FMTSTR.OBJ+..\SOURCE\Z_ERROR.OBJ+..\SOURCE\Z_DEVICE.OBJ+
..\SOURCE\Z_DATE.OBJ+..\SOURCE\Z_BORDER.OBJ+..\SOURCE\Z_PROMPT.OBJ+
..\SOURCE\Z_INT.OBJ+..\SOURCE\Z_BNUM2.OBJ+..\SOURCE\D_STRING.OBJ+
..\SOURCE\D_ICON.OBJ+..\SOURCE\D_TITLE.OBJ+..\SOURCE\D_MAX.OBJ+
..\SOURCE\D_CURSOR.OBJ+..\SOURCE\D_SCROLL.OBJ+..\SOURCE\D_BORDER.OBJ+
..\SOURCE\D_COMBO.OBJ+..\SOURCE\D_VLIST.OBJ+..\SOURCE\D_TEXT.OBJ+
..\SOURCE\D_SYS.OBJ+..\SOURCE\D_GROUP.OBJ+..\SOURCE\D_MIN.OBJ+
..\SOURCE\D_PROMPT.OBJ+..\SOURCE\D_HLIST.OBJ+..\SOURCE\D_ERROR.OBJ+
..\SOURCE\D_BUTTON.OBJ+
#
# Non-Overlayed Modules...
/o- ..\SOURCE\D_MOUSE.OBJ+..\SOURCE\D_KEYBRD.OBJ+..\SOURCE\D_GFXDSP.OBJ+
..\SOURCE\D_TDSP.OBJ+..\SOURCE\D_WIN.OBJ+..\SOURCE\D_WIN1.OBJ+
..\SOURCE\Z_POPUP.OBJ+..\SOURCE\D_WIN2.OBJ+..\SOURCE\Z_WIN2.OBJ+
..\SOURCE\Z_STORE.OBJ+..\SOURCE\Z_PLLDN.OBJ+..\SOURCE\Z_WIN.OBJ+
..\SOURCE\Z_TEXT.OBJ+..\SOURCE\Z_SCROLL.OBJ+..\SOURCE\Z_GROUP.OBJ+
..\SOURCE\Z_HLIST.OBJ+..\SOURCE\Z_COMBO.OBJ+..\SOURCE\Z_VLIST.OBJ+
..\SOURCE\Z_TBAR.OBJ+..\SOURCE\Z_WIN1.OBJ+..\SOURCE\D_POPUP.OBJ+
..\SOURCE\D_PLLDN1.OBJ+..\SOURCE\D_PLLDN.OBJ+..\SOURCE\D_POPUP1.OBJ+
..\SOURCE\Z_BUTTON.OBJ+..\SOURCE\D_TBAR.OBJ
$*, , $(DOS_LIBS) overlay
!

# ----- DOS RTLink -----------------------------------------------------------
dosrtl: zincpost.dat zinc_rtl.exe

zinc_rtl.exe: zincdemo.obj
	$(RTLINK) @&&!
SILENT
OUTPUT $*.exe
FILE c0l.obj
FILE zincdemo.obj
NODEFLIB
NOIGNORECASE
MUSTBEVERSION 510.
AUTOBP
AUTOLOCAL
VIRTUALIZE CODE
CODECLASS CODE
DROPUNREF
WARNING 3
VMRELOAD 100.
VCACHE CONVENTIONAL  50.%,200.K     # 50.%,100.K
VCACHE EXPANDED      0,0            # Don't use expanded
VCACHE EXTENDED      0,0            # Don't use extended
NOTABLEOFCONTENTS
LIBRARY dos_gfx 
LIBRARY bc_lgfx 
#	LOCALON
#	MODULE $(FONT)_dial.cpp
#	MODULE $(FONT)_smal.cpp
#	MODULE $(FONT)_syst.cpp
#	LOCALUNKNOWN
#	MODULE d_gfxdsp.cpp
#	MODULE z_app.cpp
@zil.rtl	# Zinc library
@bcppclvm.300	# Borland C library
!

# ----- Windows -------------------------------------------------------------
windows: zincpost.dat zinc_win.exe

zinc_win.exe: zincdemo.obw
	$(LINK) $(WIN_LINK_OPTS) @&&!
$(WIN_OBJS)+zincdemo.obw
$*, ,$(WIN_LIBS),wdemo.def
!
	$(RC) $(WIN_RC_OPTS) wdemo.rc $<

# ----- Windows NT ----------------------------------------------------------
winnt: zincpost.dat zinc_nt.exe

zinc_nt.exe: zincdemo.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+zincdemo.obn
$*, ,$(WNT_LIBS),wdemo.def
!
#	$(WNT_RC) $(WNT_RC_OPTS) wdemo.rc $<

# ----- OS/2 ----------------------------------------------------------------
os2: zinc_os2.exe

zinc_os2.exe: zincdemo.obo
	$(LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+zincdemo.obo
$*, ,$(OS2_LIBS),odemo.def
!
	$(RC) $(OS2_RC_OPTS) odemo.rc $<

# ----- PharLap Extended ------------------------------------------------------
dos16: zinc_d16.exe zincpost.dat

zinc_d16.exe: zincdemo.o16
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+zincdemo.o16
$*, ,$(D16_LIBS),b16.def
!
	$(D16_BIND) @&&!
$(PHARLAP_RTL)\$(D16_LOAD)
$*
-dll $(PHARLAP_RTL)\moucalls
     $(PHARLAP_RTL)\int33
     $(PHARLAP_RTL)\doscalls
!
