# DESIGN program makefile

#    make -fborland.mak dos		      	(makes the DOS design program)
#    make -fborland.mak windows	      	(makes the Windows design program)
#    make -fborland.mak os2	      	 	(makes the OS/2 design program)

# Be sure to update your TURBOC.CFG file to include the Zinc paths, e.g.:
#   -I.;C:\ZINC\INCLUDE;C:BORLANDC\INCLUDE
#   -L.;C:\ZINC\LIB\BTCPP310;C:BORLANDC\LIB
# and your TLINK.CFG file to include the Zinc paths, e.g.:
#   -L.;C:\ZINC\LIB\BTCPP310;C:BORLANDC\LIB

#MEMWIN=memwin.obj
#MEMWINADD=+$(MEMWIN)
#MEMWINCOMMA=,$(MEMWIN)

## Compiler and linker: (Add -v to CPP_OPTS and /v to LINK_OPTS for debug.)

# ----- DOS compiler options ------------------------------------------------
DOS_CPP_OPTS=-c -ml -O1 -Z -V -Y -w -H=ZIL.SYM
DOS_LINK_OPTS=/c /x
DOS_LIB_OPTS=/C
DOS_OBJS=c0l
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
DOS_LIBS=dos_gfx bc_lgfx emu mathl cl dos_zil overlay
# --- Use the next line for UI_BGI_DISPLAY ---
#DOS_LIBS=dos_zil dos_bgi graphics emu mathl cl
DOS_OVR_LIBS=/o- cl mathl emu

# ----- Windows compiler options --------------------------------------------
WIN_CPP_OPTS=-c -ml -O1 -Vf -WE -w -H=ZIL.SYM -DWINVER=0x0300
WIN_LINK_OPTS=/c /C /Twe /x
WIN_LIB_OPTS=/C
WIN_RC_OPTS=-k -30
WIN_OBJS=c0wl
WIN_LIBS=win_zil mathwl import cwl

# ----- Windows DLL compiler options ----------------------------------------
WINDLL_CPP_OPTS=-c -ml -O1 -Vf -WDE -w -H=ZIL.SYM
WINDLL_LINK_OPTS=/c /C /Twd /x
WINDLL_LIB_OPTS=/C
WINDLL_OBJS=c0dl
WINDLL_LIBS=cwl import mathwl

# ----- Windows NT compiler options -----------------------------------------
WNT_CPP=bcc32
WNT_LINK=tlink32
WNT_LIBRARIAN=tlib32
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
#OS2_CPP_OPTS=-c -v
#OS2_LINK_OPTS=/c /v /B:0x10000 /aa
OS2_RC_OPTS=
OS2_OBJS=c02.obj
OS2_LIBS=os2_zil.lib c2.lib os2.lib

# ----- Extender compiler options -----------------------------------------------
D16_CPP_OPTS=-c -ml -O1 -w 
D16_LINK_OPTS=/c /C /x
D16_OBJS=c0pl
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
D16_LIBS=phapi d16_zil d16_gfx bc_16gfx emu286 emu mathl bcl286
# --- Use the next lines for UI_BGI_DISPLAY ---
#D16_LIBS=phapi d16_zil d16_bgi graph286 emu286 emu mathl bcl286
D16_BIND=bind286
PHARLAP_RTL=c:\phar286\rtk
D16_LOAD=run286a

CPP=bcc
LINK=tlink
LIBRARIAN=tlib
CPP16=bcc286
RTLINK=rtlink
RC=rc

.SWAP

.cpp.obj:
	$(CPP) $(DOS_CPP_OPTS) -o$*.obj {$< }

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
	@echo To make the design program for Borland C++ type:
	@echo make -fborland.mak dos
	@echo make -fborland.mak dos16
	@echo make -fborland.mak dosrtl
	@echo make -fborland.mak os2
	@echo make -fborland.mak windows
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean

# ----- DOS -----------------------------------------------------------------
dos: design.exe
	copy design.exe ..\..\bin
	copy p_design.znc ..\..\bin

# DESIGN.EXE w/o overlay support
#design.exe: z_design.obj \
#	z_bitobj.obj d_bitobj.obj z_scombo.obj z_config.obj z_ctrl.obj \
#	z_dialog.obj z_cerror.obj z_file.obj z_frame.obj z_helped.obj \
#	z_help1.obj z_hlpbar.obj z_hlpmsg.obj z_image.obj z_image1.obj \
#	z_jump.obj z_objlst.obj z_resrc.obj z_util.obj z_object.obj \
#	z_advanc.obj z_flag.obj z_import.obj z_image2.obj $(MEMWIN)
#	$(LINK) $(DOS_LINK_OPTS) @&&!
#$(DOS_OBJS)+z_design.obj+
#z_bitobj.obj+d_bitobj.obj+z_scombo.obj+z_config.obj+z_ctrl.obj+
#z_dialog.obj+z_cerror.obj+z_file.obj+z_frame.obj+z_helped.obj+
#z_help1.obj+z_hlpbar.obj+z_hlpmsg.obj+z_image.obj+z_image1.obj+
#z_jump.obj+z_objlst.obj+z_resrc.obj+z_util.obj+z_object.obj+
#z_advanc.obj+z_flag.obj+z_import.obj+z_image2.obj $(MEMWINADD)
#$*, , $(DOS_LIBS)
#!
#	@del *.sym

# DESIGN.EXE w/ overlay support
design.exe: z_design.obj \
	z_bitobj.obj d_bitobj.obj z_scombo.obj z_config.obj z_ctrl.obj \
	z_dialog.obj z_cerror.obj z_file.obj z_frame.obj z_helped.obj \
	z_help1.obj z_hlpbar.obj z_hlpmsg.obj z_image.obj z_image1.obj \
	z_jump.obj z_objlst.obj z_resrc.obj z_util.obj z_object.obj d_object.obj\
	z_advanc.obj z_flag.obj z_import.obj z_image2.obj \
	..\Z_STRING.OBJ ..\Z_TIME1.OBJ ..\Z_TITLE.OBJ ..\Z_REAL.OBJ ..\Z_TIME.OBJ ..\Z_REGION.OBJ \
	..\Z_POPUP1.OBJ ..\Z_POPUP.OBJ ..\Z_PLLDN1.OBJ ..\Z_PLLDN.OBJ ..\Z_PATH.OBJ ..\Z_ICON.OBJ \
	..\Z_LIST1.OBJ ..\Z_LIST.OBJ ..\Z_BNUM.OBJ ..\Z_SYS.OBJ ..\Z_EVENT.OBJ ..\Z_WIN.OBJ \
	..\Z_TEXT.OBJ ..\Z_CURSOR.OBJ ..\Z_SCROLL.OBJ ..\Z_GROUP.OBJ ..\Z_HLIST.OBJ \
	..\Z_UTILS.OBJ ..\Z_INTL.OBJ ..\Z_MAP2.OBJ ..\Z_COMBO.OBJ ..\Z_VLIST.OBJ ..\Z_TBAR.OBJ \
	..\Z_DATE1.OBJ ..\Z_WIN1.OBJ ..\Z_BUTTON.OBJ ..\Z_MIN.OBJ ..\Z_BNUM1.OBJ \
	..\Z_MAX.OBJ ..\Z_MAP1.OBJ ..\Z_HELP.OBJ ..\Z_DSP.OBJ ..\Z_WIN2.OBJ ..\Z_FMTSTR.OBJ \
	..\Z_ERROR.OBJ ..\Z_DEVICE.OBJ ..\Z_DATE.OBJ ..\Z_BORDER.OBJ ..\Z_PROMPT.OBJ \
	..\Z_STORE.OBJ ..\Z_INT.OBJ ..\Z_BNUM2.OBJ  ..\D_WIN2.OBJ ..\D_POPUP1.OBJ ..\D_STRING.OBJ \
	..\D_ICON.OBJ ..\D_TITLE.OBJ ..\D_MAX.OBJ ..\D_GFXDSP.OBJ ..\D_WIN.OBJ ..\D_CURSOR.OBJ \
	..\D_SCROLL.OBJ ..\D_KEYBRD.OBJ ..\D_BORDER.OBJ ..\D_COMBO.OBJ \
	..\D_VLIST.OBJ ..\D_BGIDSP.OBJ ..\D_TEXT.OBJ ..\D_SYS.OBJ ..\D_WIN1.OBJ \
	..\D_PLLDN1.OBJ ..\D_GROUP.OBJ ..\D_MOUSE.OBJ ..\D_MIN.OBJ ..\D_PROMPT.OBJ ..\D_TBAR.OBJ \
	..\D_TDSP.OBJ ..\D_POPUP.OBJ ..\D_PLLDN.OBJ ..\D_HLIST.OBJ ..\D_ERROR.OBJ ..\D_BUTTON.OBJ
	$(LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+
#
# Overlayed Modules...
/o+ z_design.obj+
z_bitobj.obj+d_bitobj.obj+z_hlpbar.obj+z_hlpmsg.obj+z_util.obj+
z_object.obj+d_object.obj+z_flag.obj+z_image2.obj+
..\Z_STRING.OBJ+..\Z_TIME1.OBJ+..\Z_TITLE.OBJ+..\Z_REAL.OBJ+..\Z_TIME.OBJ+..\Z_REGION.OBJ+
..\Z_PATH.OBJ+..\Z_ICON.OBJ+
..\Z_LIST1.OBJ+..\Z_LIST.OBJ+..\Z_BNUM.OBJ+..\Z_SYS.OBJ+..\Z_EVENT.OBJ+
..\Z_CURSOR.OBJ+..\Z_UTILS.OBJ+..\Z_INTL.OBJ+..\Z_MAP2.OBJ+
..\Z_DATE1.OBJ+..\Z_MIN.OBJ+..\Z_BNUM1.OBJ+
..\Z_MAX.OBJ+..\Z_MAP1.OBJ+..\Z_HELP.OBJ+..\Z_DSP.OBJ+..\Z_FMTSTR.OBJ+
..\Z_ERROR.OBJ+..\Z_DEVICE.OBJ+..\Z_DATE.OBJ+..\Z_BORDER.OBJ+..\Z_PROMPT.OBJ+
..\Z_INT.OBJ+..\Z_BNUM2.OBJ+..\D_STRING.OBJ+
..\D_ICON.OBJ+..\D_TITLE.OBJ+..\D_MAX.OBJ+..\D_CURSOR.OBJ+
..\D_SCROLL.OBJ+..\D_BORDER.OBJ+..\D_COMBO.OBJ+
..\D_VLIST.OBJ+..\D_TEXT.OBJ+..\D_SYS.OBJ+
..\D_GROUP.OBJ+..\D_MIN.OBJ+..\D_PROMPT.OBJ+
..\D_HLIST.OBJ+..\D_ERROR.OBJ+..\D_BUTTON.OBJ+
#
# Non-Overlayed Modules...
/o- z_scombo.obj+z_config.obj+z_ctrl.obj+z_dialog.obj+z_file.obj+z_frame.obj+z_helped.obj+z_jump.obj+
z_help1.obj+z_image.obj+z_image1.obj+z_objlst.obj+z_resrc.obj+z_advanc.obj+z_import.obj+z_cerror.obj+
..\D_MOUSE.OBJ+..\D_KEYBRD.OBJ+..\D_GFXDSP.OBJ+..\D_TDSP.OBJ+..\D_WIN.OBJ+..\D_WIN1.OBJ+..\Z_POPUP.OBJ+
..\D_WIN2.OBJ+..\Z_WIN2.OBJ+..\Z_STORE.OBJ+..\Z_PLLDN.OBJ+..\Z_WIN.OBJ+..\Z_TEXT.OBJ+..\Z_SCROLL.OBJ+
..\Z_GROUP.OBJ+..\Z_HLIST.OBJ+..\Z_COMBO.OBJ+..\Z_VLIST.OBJ+..\Z_TBAR.OBJ+..\Z_WIN1.OBJ+..\D_POPUP.OBJ+
..\D_PLLDN1.OBJ+..\D_PLLDN.OBJ+..\D_POPUP1.OBJ+..\Z_BUTTON.OBJ+..\D_TBAR.OBJ
$*, , $(DOS_LIBS)
!
	@del *.sym

# ----- Windows -------------------------------------------------------------
windows: wdesign.exe
	copy wdesign.exe ..\..\bin
	copy p_design.znc ..\..\bin

wdesign.exe: z_design.obw \
	z_bitobj.obw w_bitobj.obw z_scombo.obw z_config.obw z_ctrl.obw z_dialog.obw \
	z_file.obw z_cerror.obw z_frame.obw z_helped.obw z_help1.obw \
	z_hlpbar.obw z_hlpmsg.obw z_image.obw z_image1.obw z_jump.obw \
	z_objlst.obw z_resrc.obw z_util.obw z_object.obw w_object.obw z_advanc.obw \
	z_flag.obw z_import.obw z_image2.obw
	$(LINK) $(WIN_LINK_OPTS) @&&!
$(WIN_OBJS)+z_design.obw+
z_bitobj.obw+w_bitobj.obw+z_scombo.obw+z_config.obw+z_ctrl.obw+z_dialog.obw+
z_file.obw+z_cerror.obw+z_frame.obw+z_helped.obw+z_help1.obw+z_hlpbar.obw+
z_hlpmsg.obw+z_image.obw+z_image1.obw+z_jump.obw+z_objlst.obw+z_resrc.obw+
z_util.obw+z_object.obw+w_object.obw+z_advanc.obw+z_flag.obw+z_import.obw+z_image2.obw
$*, , $(WIN_LIBS)
$*.def
!
	$(RC) $(WIN_RC_OPTS) wdesign.rc $<
	@del zil.sym

# ----- Windows NT ----------------------------------------------------------
winnt: ndesign.exe
	copy ndesign.exe ..\..\bin
	copy p_design.znc ..\..\bin

ndesign.exe: z_design.obn \
	z_bitobj.obn w_bitobj.obn z_scombo.obn z_config.obn z_ctrl.obn z_dialog.obn \
	z_file.obn z_cerror.obn z_frame.obn z_helped.obn z_help1.obn \
	z_hlpbar.obn z_hlpmsg.obn z_image.obn z_image1.obn z_jump.obn \
	z_objlst.obn z_resrc.obn z_util.obn z_object.obn w_object.obn z_advanc.obn \
	z_flag.obn z_import.obn z_image2.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+z_design.obn+
z_bitobj.obn+w_bitobj.obn+z_scombo.obn+z_config.obn+z_ctrl.obn+z_dialog.obn+
z_file.obn+z_cerror.obn+z_frame.obn+z_helped.obn+z_help1.obn+z_hlpbar.obn+
z_hlpmsg.obn+z_image.obn+z_image1.obn+z_jump.obn+z_objlst.obn+z_resrc.obn+
z_util.obn+z_object.obn+w_object.obn+z_advanc.obn+z_flag.obn+z_import.obn+z_image2.obn
$*, , $(WNT_LIBS)
wdesign.def
!
#	$(WNT_RC) $(WNT_RC_OPTS) wdesign.rc $<
	@del zil.sym

# ----- OS/2 ----------------------------------------------------------------
os2: odesign.exe
	copy odesign.exe ..\..\bin
	copy p_design.znc ..\..\bin

odesign.exe: z_design.obo \
	z_bitobj.obo o_bitobj.obo o_object.obo z_scombo.obo z_config.obo \
	z_ctrl.obo z_dialog.obo z_file.obo z_cerror.obo z_frame.obo \
	z_helped.obo z_help1.obo z_hlpbar.obo z_hlpmsg.obo z_image.obo \
	z_image1.obo z_jump.obo z_objlst.obo z_resrc.obo z_util.obo \
	z_object.obo z_advanc.obo z_flag.obo z_import.obo z_image2.obo
	$(LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+z_design.obo+
z_bitobj.obo+o_bitobj.obo+o_object.obo+z_scombo.obo+z_config.obo+
z_ctrl.obo+z_dialog.obo+z_file.obo+z_cerror.obo+z_frame.obo+
z_helped.obo+z_help1.obo+z_hlpbar.obo+z_hlpmsg.obo+z_image.obo+
z_image1.obo+z_jump.obo+z_objlst.obo+z_resrc.obo+z_util.obo+
z_object.obo+z_advanc.obo+z_flag.obo+z_import.obo+z_image2.obo
$*, , $(OS2_LIBS), odesign.def
!
	$(RC) $(OS2_RC_OPTS) odesign.rc $<

# ----- Extender --------------------------------------------------------------
dos16: design16.exe
	copy design16.exe ..\..\bin
	copy p_design.znc ..\..\bin

# DESIGN.EXE w/o overlay support
design16.exe: z_design.o16 \
	z_bitobj.o16 d_bitobj.o16 z_scombo.o16 z_config.o16 z_ctrl.o16 \
	z_dialog.o16 z_cerror.o16 z_file.o16 z_frame.o16 z_helped.o16 \
	z_help1.o16 z_hlpbar.o16 z_hlpmsg.o16 z_image.o16 z_image1.o16 \
	z_jump.o16 z_objlst.o16 z_resrc.o16 z_util.o16 z_object.o16 d_object.o16 \
	z_advanc.o16 z_flag.o16 z_import.o16 z_image2.o16 $(MEMWIN)
	$(LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+z_design.o16+
z_bitobj.o16+d_bitobj.o16+z_scombo.o16+z_config.o16+z_ctrl.o16+
z_dialog.o16+z_cerror.o16+z_file.o16+z_frame.o16+z_helped.o16+
z_help1.o16+z_hlpbar.o16+z_hlpmsg.o16+z_image.o16+z_image1.o16+
z_jump.o16+z_objlst.o16+z_resrc.o16+z_util.o16+z_object.o16+d_object.o16+
z_advanc.o16+z_flag.o16+z_import.o16+z_image2.o16 $(MEMWINADD)
$*, , $(D16_LIBS),bdesign.def
!
	$(D16_BIND) @&&!
$(PHARLAP_RTL)\$(D16_LOAD)
$*
-dll $(PHARLAP_RTL)\moucalls
     $(PHARLAP_RTL)\int33
     $(PHARLAP_RTL)\doscalls
!
	@del *.sym

# ----- DOS RTLink -----------------------------------------------------------
dosrtl: designrt.exe
	copy designrt.exe ..\..\bin
	copy p_design.znc ..\..\bin

designrt.exe: z_design.obj \
	z_bitobj.obj d_bitobj.obj z_scombo.obj z_config.obj z_ctrl.obj \
	z_dialog.obj z_cerror.obj z_file.obj z_frame.obj z_helped.obj \
	z_help1.obj z_hlpbar.obj z_hlpmsg.obj z_image.obj z_image1.obj \
	z_jump.obj z_objlst.obj z_resrc.obj z_util.obj z_object.obj d_object.obj \
	z_advanc.obj z_flag.obj z_import.obj z_image2.obj $(MEMWIN)
	$(RTLINK) @&&!
SILENT
OUTPUT $*.exe
FILE c0l.obj
file z_design.obj
file z_bitobj.obj, d_bitobj.obj, z_scombo.obj, z_config.obj, z_ctrl.obj 
file z_dialog.obj, z_cerror.obj, z_file.obj, z_frame.obj, z_helped.obj 
file z_help1.obj, z_hlpbar.obj, z_hlpmsg.obj, z_image.obj, z_image1.obj 
file z_jump.obj, z_objlst.obj, z_resrc.obj, z_util.obj, z_object.obj, d_object.obj
file z_advanc.obj, z_flag.obj, z_import.obj, z_image2.obj $(MEMWINCOMMA)
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
@bcppemvm.300	# Borland Emulator library
@bcppmlvm.300	# Borland Math library
!
	@del *.sym
