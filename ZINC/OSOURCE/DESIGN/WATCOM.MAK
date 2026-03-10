# DESIGN program makefile

#    wmake -f watcom.mak dos32       (makes the DOS design program)
#    wmake -f watcom.mak windows     (makes the Windows design program)
#    wmake -f watcom.mak os2         (makes the OS/2 design program)
#    wmake -f watcom.mak dvx         (makes the DESQView/X design program)

# Be sure to set the LIB and INCLUDE environment variables for Zinc, e.g.:
#    set INCLUDE=.;C:\DVX\INCLUDE;C:\WATCOM\H;C:\ZINC\INCLUDE
#    set LIB=.;C:\DVX\LIB\C386;C:\WATCOM\LIB386\DOS;C:\WATCOM\LIB386;C:\ZINC\LIB\WCCPP950

.EXTENSIONS:
.EXTENSIONS: .o32 .obw .obn .obo .obm .cpp

# ----- DOS extender compiler options ---------------------------------------
D32_CPP=wpp386
D32_LINK=wlink
D32_CPP_OPTS=/bt=dos
D32_LINK_OPTS=SYSTEM dos4g OP stack=20000

# --- Use the next line for UI_GRAPHICS_DISPLAY ---
D32_OBJS=
D32_LIBS=d32_zil,d32_gfx,wc_32gfx,math3r,emu387,clib3r
# --- Use the next line for UI_WCC_DISPLAY ---
#D32_OBJS=
#D32_LIBS=d32_zil,d32_wcc,math3r,emu387,clib3r

# ----- Windows compiler options --------------------------------------------
WIN_CPP=wpp
WIN_LINK=wlink
WIN_LIBRARIAN=wlib
WIN_RC=wrc
WIN_CPP_OPTS= -zW -oaxt -w4 -ml -DWINVER=0x0300
WIN_RC_OPTS=/30 /bt=windows -k
WIN_LINK_OPTS=SYS windows OP heapsize=10k OP stack=10k OP version=3.0
WIN_LIB_OPTS=
WIN_OBJS=
WIN_LIBS=windows,win_zil

# ----- OS/2 compiler options -----------------------------------------------
OS2_CPP=wpp386
OS2_LINK=wlink
OS2_RC=rc
OS2_CPP_OPTS=/bt=OS2
OS2_LINK_OPTS=SYSTEM os2v2_pm OP ST=20000
OS2_RC_OPTS=
OS2_OBJS=
OS2_LIBS=os2_zil

# ----- DESQview/X MOTIF compiler options -----------------------------------
DVX_CPP=wpp386
DVX_LINK=wlink
DVX_CPP_OPTS=/bt=dos /d__DVX__ /dXTFUNCPROTO -w0
DVX_LINK_OPTS=SYSTEM dos4g OP stack=20000
DVX_OBJS=
DVX_LIBS=dvx_zil,math3r,emu387,clib3r,Xm,Xt,Xlib,sys

.cpp.o32:
	$(D32_CPP) $(D32_CPP_OPTS) -fo=$[*.o32 $<

.cpp.obw:
	$(WIN_CPP) $(WIN_CPP_OPTS) -fo=$[*.obw $<

.cpp.obo:
	$(OS2_CPP) $(OS2_CPP_OPTS) -fo=$[*.obo $<

.cpp.obm:
	$(DVX_CPP) $(DVX_CPP_OPTS) -fo=$[*.obm $<

# ----- Usage --------------------------------------------------------------
usage: .SYMBOLIC
	@echo ...........
	@echo ...........
	@echo To make the design program for Watcom C++ type:
	@echo wmake -f watcom.mak dos32
	@echo wmake -f watcom.mak os2
	@echo wmake -f watcom.mak windows
	@echo wmake -f watcom.mak dvx
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean: .SYMBOLIC
	z_clean

# ----- DOS extender --------------------------------------------------------
dos32: design32.exe .SYMBOLIC
	copy design32.exe ..\..\bin
	copy p_design.znc ..\..\bin

design32.exe: z_design.o32 &
	z_bitobj.o32 z_scombo.o32 z_config.o32 z_ctrl.o32 z_dialog.o32 &
	z_file.o32 z_cerror.o32 z_frame.o32 z_helped.o32 z_help1.o32 &
	z_hlpbar.o32 z_hlpmsg.o32 z_image.o32 z_image1.o32 z_jump.o32 &
	z_objlst.o32 z_resrc.o32 z_util.o32 z_object.o32 z_advanc.o32 &
	z_flag.o32 z_import.o32 z_image2.o32 d_bitobj.o32 d_object.o32
	%create d32_zil.wcc
	@%append d32_zil.wcc $(D32_LINK_OPTS) 
	@%append d32_zil.wcc N design32
	@%append d32_zil.wcc F z_design.o32 
	@%append d32_zil.wcc F z_bitobj.o32,z_scombo.o32,z_config.o32,z_ctrl.o32,z_dialog.o32
	@%append d32_zil.wcc F z_file.o32,z_cerror.o32,z_frame.o32,z_helped.o32,z_help1.o32
	@%append d32_zil.wcc F z_hlpbar.o32,z_hlpmsg.o32,z_image.o32,z_image1.o32,z_jump.o32
	@%append d32_zil.wcc F z_objlst.o32,z_resrc.o32,z_util.o32,z_object.o32,z_advanc.o32
	@%append d32_zil.wcc F z_flag.o32,z_import.o32,z_image2.o32,d_bitobj.o32,d_object.o32
	@%append d32_zil.wcc L $(D32_LIBS)
	$(D32_LINK) @d32_zil.wcc
	del d32_zil.wcc

# ----- Windows -------------------------------------------------------------
windows: wdesign.exe .SYMBOLIC
	copy wdesign.exe ..\..\bin
	copy p_design.znc ..\..\bin

wdesign.exe: z_design.obw &
	z_bitobj.obw z_scombo.obw z_config.obw z_ctrl.obw z_dialog.obw &
	z_file.obw z_cerror.obw z_frame.obw z_helped.obw z_help1.obw &
	z_hlpbar.obw z_hlpmsg.obw z_image.obw z_image1.obw z_jump.obw &
	z_objlst.obw z_resrc.obw z_util.obw z_object.obw z_advanc.obw &
	z_flag.obw z_import.obw z_image2.obw d_bitobj.obw d_object.obw
	%create win_zil.wcc
	@%append win_zil.wcc $(WIN_LINK_OPTS) 
	@%append win_zil.wcc N wdesign
	@%append win_zil.wcc F z_design.obw
	@%append win_zil.wcc F z_bitobj.obw,z_scombo.obw,z_config.obw,z_ctrl.obw,z_dialog.obw
	@%append win_zil.wcc F z_file.obw,z_cerror.obw,z_frame.obw,z_helped.obw,z_help1.obw
	@%append win_zil.wcc F z_hlpbar.obw,z_hlpmsg.obw,z_image.obw,z_image1.obw,z_jump.obw
	@%append win_zil.wcc F z_objlst.obw,z_resrc.obw,z_util.obw,z_object.obw,z_advanc.obw
	@%append win_zil.wcc F z_flag.obw,z_import.obw,z_image2.obw,d_bitobj.obw,d_object.obw
	@%append win_zil.wcc L $(WIN_LIBS)
	$(WIN_LINK) @win_zil.wcc
#	$(WIN_RC) $(WIN_RC_OPTS) wdesign.exe
	del win_zil.wcc

# ----- OS/2 ----------------------------------------------------------------
os2: odesign.exe .SYMBOLIC
	copy odesign.exe ..\..\bin
	copy p_design.znc ..\..\bin

odesign.exe: z_design.obo &
	z_bitobj.obo z_scombo.obo z_config.obo z_ctrl.obo z_dialog.obo &
	z_file.obo z_cerror.obo z_frame.obo z_helped.obo z_help1.obo &
	z_hlpbar.obo z_hlpmsg.obo z_image.obo z_image1.obo z_jump.obo &
	z_objlst.obo z_resrc.obo z_util.obo z_object.obo z_advanc.obo &
	z_flag.obo z_import.obo z_image2.obo d_bitobj.obo d_object.obo
	%create os2_zil.wcc
	@%append os2_zil.wcc $(OS2_LINK_OPTS) 
	@%append os2_zil.wcc N odesign
	@%append os2_zil.wcc F z_design.obo
	@%append os2_zil.wcc F z_bitobj.obo,z_scombo.obo,z_config.obo,z_ctrl.obo,z_dialog.obo
	@%append os2_zil.wcc F z_file.obo,z_cerror.obo,z_frame.obo,z_helped.obo,z_help1.obo
	@%append os2_zil.wcc F z_hlpbar.obo,z_hlpmsg.obo,z_image.obo,z_image1.obo,z_jump.obo
	@%append os2_zil.wcc F z_objlst.obo,z_resrc.obo,z_util.obo,z_object.obo,z_advanc.obo
	@%append os2_zil.wcc F z_flag.obo,z_import.obo,z_image2.obo,d_bitobj.obo,d_object.obo
	@%append os2_zil.wcc L $(OS2_LIBS)
	$(OS2_LINK) @os2_zil.wcc
	del os2_zil.wcc

# ----- DESQview/X Motif ----------------------------------------------------
dvx: mdesign.exe .SYMBOLIC
	copy mdesign.exe ..\..\bin
	copy p_design.znc ..\..\bin

mdesign.exe: z_design.obm &
	z_bitobj.obm z_scombo.obm z_config.obm z_ctrl.obm z_dialog.obm &
	z_file.obm z_cerror.obm z_frame.obm z_helped.obm z_help1.obm &
	z_hlpbar.obm z_hlpmsg.obm z_image.obm z_image1.obm z_jump.obm &
	z_objlst.obm z_resrc.obm z_util.obm z_object.obm z_advanc.obm &
	z_flag.obm z_import.obm z_image2.obm m_bitobj.obm m_object.obm
	%create dvx_zil.wcc
	@%append dvx_zil.wcc $(DVX_LINK_OPTS) 
	@%append dvx_zil.wcc N mdesign.exe
	@%append dvx_zil.wcc F z_design.obm
	@%append dvx_zil.wcc F z_bitobj.obm,z_scombo.obm,z_config.obm,z_ctrl.obm,z_dialog.obm
	@%append dvx_zil.wcc F z_file.obm,z_cerror.obm,z_frame.obm,z_helped.obm,z_help1.obm
	@%append dvx_zil.wcc F z_hlpbar.obm,z_hlpmsg.obm,z_image.obm,z_image1.obm,z_jump.obm
	@%append dvx_zil.wcc F z_objlst.obm,z_resrc.obm,z_util.obm,z_object.obm,z_advanc.obm
	@%append dvx_zil.wcc F z_flag.obm,z_import.obm,z_image2.obm,m_bitobj.obm,m_object.obm
	@%append dvx_zil.wcc L $(DVX_LIBS)
	$(DVX_LINK) @dvx_zil.wcc
	del dvx_zil.wcc

