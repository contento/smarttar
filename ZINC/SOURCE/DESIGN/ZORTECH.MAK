# DESIGN program makefile

#    make -fzortech.mak dos		      	(makes the DOS design program)
#    make -fzortech.mak windows	      	(makes the Windows design program)
#    make -fzortech.mak os2	      	 	(makes the OS/2 design program)

# Be sure to set the LIB and INCLUDE environment variables for Zinc, e.g.:
#    set INCLUDE=.;C:\ZINC\INCLUDE;C:\ZTC\INCLUDE
#    set LIB=.;C:\ZINC\LIB\ZTCPP300;C:\ZTC\LIB

## Compiler and linker: (Add -g to CPP_OPTS and /CO to LINK_OPTS for debug.)

# ----- DOS compiler options ------------------------------------------------
DOS_CPP_OPTS=-c -a1 -bx -ml
DOS_LINK_OPTS=/NOI
DOS_OBJS=
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
DOS_LIBS=dos_zil+dos_gfx+zt_lgfx
# --- Use the next line for UI_FG_DISPLAY ---
#DOS_LIBS=dos_zil+dos_fg+fg

# ----- Windows compiler options --------------------------------------------
WIN_CPP_OPTS=-c -a1 -bx -ml -W2 -DWINVER=0x0300
WIN_LINK_OPTS=/NOI /PACKC:42000
WIN_RC_OPTS=-30
WIN_OBJS=
WIN_LIBS=win_zil
WIN_RC=rc

# ----- OS/2 compiler options -----------------------------------------------
OS2_CPP_OPTS=-c -mf
OS2_LINK_OPTS=/BASE:0x10000 /PM:PM 
OS2_RC_OPTS=
OS2_OBJS=
OS2_LIBS=os2_zil
OS2_RC=rc
OS2_LINK=blink

CPP=ztc
LINK=blinkx
LIBRARIAN=zorlib

.c.obj:
	$(CPP) $(DOS_CPP_OPTS) $<

.cpp.obj:
	$(CPP) $(DOS_CPP_OPTS) $<

.c.obw:
	$(CPP) $(WIN_CPP_OPTS) -o$*.obw $<

.cpp.obw:
	$(CPP) $(WIN_CPP_OPTS) -o$*.obw $<

.c.obo:
	$(CPP) $(OS2_CPP_OPTS) -o$*.obo $<

.cpp.obo:
	$(CPP) $(OS2_CPP_OPTS) -o$*.obo $<

# ----- DOS extender compiler options -----------------------------------------
D16_CPP=ztc
D16_LINK=blinkx
D16_LIBRARIAN=zorlibx

D16_CPP_OPTS=-c -bx -mz
D16_LINK_OPTS=/NOI
D16_LIB_OPTS=/C /N /B

D16_OBJS=
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
D16_LIBS=zpm d16_zil d16_gfx zt_16gfx
# --- Use the next line for UI_FG_DISPLAY ---
#D16_LIBS=zpm d16_zil d16_fg fg

.cpp.o16:
	$(D16_CPP) $(D16_CPP_OPTS) -o$@ $<

# ----- DOS extender compiler options -----------------------------------------
D32_CPP=ztc
D32_LINK=blinkx
D32_LIBRARIAN=zorlibx

D32_CPP_OPTS=-c -bx -mx
D32_LINK_OPTS=/NOI
D32_LIB_OPTS=/C /N /B

D32_OBJS=cx
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
D32_LIBS=d32_zil d32_gfx zt_32gfx
# --- Use the next line for UI_FG_DISPLAY ---
#D32_LIBS=d32_zil d32_fg fgp

.cpp.o32:
	$(D32_CPP) $(D32_CPP_OPTS) -o$@ $<

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To make the design program for Zortech C++ type:
	@echo make -fzortech.mak dos
	@echo make -fzortech.mak dos16
	@echo make -fzortech.mak dos32
	@echo make -fzortech.mak os2
	@echo make -fzortech.mak windows
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean.bat

# ----- DOS -----------------------------------------------------------------
dos: design.exe
	copy design.exe ..\..\bin
	copy p_design.znc ..\..\bin

design.exe: z_design.obj \
	z_bitobj.obj z_scombo.obj z_config.obj z_ctrl.obj z_dialog.obj \
	z_file.obj z_cerror.obj z_frame.obj z_helped.obj z_help1.obj \
	z_hlpbar.obj z_hlpmsg.obj z_image.obj z_image1.obj z_jump.obj \
	z_objlst.obj z_resrc.obj z_util.obj z_object.obj z_advanc.obj \
	z_flag.obj z_import.obj z_image.obj z_image1.obj z_image2.obj \
	d_bitobj.obj d_object.obj
	@echo $(DOS_OBJS)+z_design.obj+>dos_zil.rsp
	@echo z_bitobj.obj+z_scombo.obj+z_config.obj+z_ctrl.obj+>>dos_zil.rsp
	@echo z_dialog.obj+z_file.obj+z_cerror.obj+z_frame.obj+z_helped.obj+z_help1.obj+z_hlpbar.obj+>>dos_zil.rsp
	@echo z_hlpmsg.obj+z_image.obj+z_image1.obj+z_jump.obj+z_objlst.obj+z_resrc.obj+z_util.obj+>>dos_zil.rsp
	@echo z_object.obj+z_advanc.obj+z_flag.obj+z_import.obj+z_image2.obj+>>dos_zil.rsp
	@echo d_bitobj.obj+d_object.obj>>dos_zil.rsp
	@echo $*, , $(DOS_LIBS)>>dos_zil.rsp
	$(LINK) $(DOS_LINK_OPTS) @dos_zil.rsp
	@del dos_zil.rsp

# ----- Windows -------------------------------------------------------------
windows: wdesign.exe
	copy wdesign.exe ..\..\bin
	copy p_design.znc ..\..\bin

wdesign.exe: z_design.obw \
	z_bitobj.obw z_scombo.obw z_config.obw z_ctrl.obw z_dialog.obw \
	z_file.obw z_cerror.obw z_frame.obw z_helped.obw z_help1.obw \
	z_hlpbar.obw z_hlpmsg.obw z_image.obw z_image1.obw z_jump.obw \
	z_objlst.obw z_resrc.obw z_util.obw z_object.obw z_advanc.obw \
	z_flag.obw z_import.obw z_image.obw z_image1.obw z_image2.obw \
	w_object.obw w_bitobj.obw
	@echo $(WIN_OBJS)+z_design.obw+>win_zil.rsp
	@echo z_bitobj.obw+z_scombo.obw+z_config.obw+z_ctrl.obw+>>win_zil.rsp
	@echo z_dialog.obw+z_file.obw+z_cerror.obw+z_frame.obw+z_helped.obw+z_help1.obw+z_hlpbar.obw+>>win_zil.rsp
	@echo z_hlpmsg.obw+z_image.obw+z_image1.obw+z_jump.obw+z_objlst.obw+z_resrc.obw+z_util.obw+>>win_zil.rsp
	@echo z_object.obw+z_advanc.obw+z_flag.obw+z_import.obw+z_image2.obw+>>win_zil.rsp
	@echo w_object.obw+w_bitobj.obw>>win_zil.rsp
	@echo $*, , $(WIN_LIBS)>>win_zil.rsp
	@echo $*.def>>win_zil.rsp
	$(LINK) $(WIN_LINK_OPTS) @win_zil.rsp
	$(WIN_RC) $(WIN_RC_OPTS) $*.rc $*.exe
	@del win_zil.rsp

# ----- OS/2 -----------------------------------------------------------------
os2: odesign.exe
	copy odesign.exe ..\..\bin
	copy p_design.znc ..\..\bin

odesign.exe: z_design.obo \
	z_bitobj.obo z_scombo.obo z_config.obo z_ctrl.obo z_dialog.obo \
	z_file.obo z_cerror.obo z_frame.obo z_helped.obo z_help1.obo \
	z_hlpbar.obo z_hlpmsg.obo z_image.obo z_image1.obo z_jump.obo \
	z_objlst.obo z_resrc.obo z_util.obo z_object.obo z_advanc.obo \
	z_flag.obo z_import.obo z_image.obo z_image1.obo z_image2.obo \
	o_bitobj.obo o_object.obo
	@echo $(DOS_OBJS)+z_design.obo+>os2_zil.rsp
	@echo z_bitobj.obo+z_scombo.obo+z_config.obo+z_ctrl.obo+>>os2_zil.rsp
	@echo z_dialog.obo+z_file.obo+z_cerror.obo+z_frame.obo+z_helped.obo+z_help1.obo+z_hlpbar.obo+>>os2_zil.rsp
	@echo z_hlpmsg.obo+z_image.obo+z_image1.obo+z_jump.obo+z_objlst.obo+z_resrc.obo+z_util.obo+>>os2_zil.rsp
	@echo z_object.obo+z_advanc.obo+z_flag.obo+z_import.obo+z_image2.obo+>>os2_zil.rsp
	@echo o_bitobj.obo+o_object.obo>>os2_zil.rsp
	@echo $*, , $(OS2_LIBS)>>os2_zil.rsp
	$(OS2_LINK) $(OS2_LINK_OPTS) @os2_zil.rsp
	@del os2_zil.rsp

# ----- Dos extender Programs -------------------------------------------------

dos16: design16.exe

design16.exe: z_design.o16 \
	z_bitobj.o16 z_scombo.o16 z_config.o16 z_ctrl.o16 z_dialog.o16 \
	z_file.o16 z_cerror.o16 z_frame.o16 z_helped.o16 z_help1.o16 \
	z_hlpbar.o16 z_hlpmsg.o16 z_image.o16 z_image1.o16 z_jump.o16 \
	z_objlst.o16 z_resrc.o16 z_util.o16 z_object.o16 z_advanc.o16 \
	z_flag.o16 z_import.o16 z_image.o16 z_image1.o16 z_image2.o16 \
	d_bitobj.o16 d_object.o16
	@echo $(D16_OBJS)+z_design.o16+>d16_zil.rsp
	@echo z_bitobj.o16+z_scombo.o16+z_config.o16+z_ctrl.o16+>>d16_zil.rsp
	@echo z_dialog.o16+z_file.o16+z_cerror.o16+z_frame.o16+z_helped.o16+z_help1.o16+z_hlpbar.o16+>>d16_zil.rsp
	@echo z_hlpmsg.o16+z_image.o16+z_image1.o16+z_jump.o16+z_objlst.o16+z_resrc.o16+z_util.o16+>>d16_zil.rsp
	@echo z_object.o16+z_advanc.o16+z_flag.o16+z_import.o16+z_image2.o16+>>d16_zil.rsp
	@echo d_bitobj.o16+d_object.o16>>d16_zil.rsp
	@echo $*, , $(D16_LIBS)>>d16_zil.rsp
	@echo zdesign.def>>d16_zil.rsp
	$(D16_LINK) $(D16_LINK_OPTS) @d16_zil.rsp
	@del d16_zil.rsp

# ----- Dos extender Programs -------------------------------------------------

dos32: design32.exe

design32.exe: z_design.o32 \
	z_bitobj.o32 z_scombo.o32 z_config.o32 z_ctrl.o32 z_dialog.o32 \
	z_file.o32 z_cerror.o32 z_frame.o32 z_helped.o32 z_help1.o32 \
	z_hlpbar.o32 z_hlpmsg.o32 z_image.o32 z_image1.o32 z_jump.o32 \
	z_objlst.o32 z_resrc.o32 z_util.o32 z_object.o32 z_advanc.o32 \
	z_flag.o32 z_import.o32 z_image.o32 z_image1.o32 z_image2.o32 \
	d_bitobj.o32 d_object.o32
	@echo $(D32_OBJS)+z_design.o32+>d32_zil.rsp
	@echo z_bitobj.o32+z_scombo.o32+z_config.o32+z_ctrl.o32+>>d32_zil.rsp
	@echo z_dialog.o32+z_file.o32+z_cerror.o32+z_frame.o32+z_helped.o32+z_help1.o32+z_hlpbar.o32+>>d32_zil.rsp
	@echo z_hlpmsg.o32+z_image.o32+z_image1.o32+z_jump.o32+z_objlst.o32+z_resrc.o32+z_util.o32+>>d32_zil.rsp
	@echo z_object.o32+z_advanc.o32+z_flag.o32+z_import.o32+z_image2.o32+>>d32_zil.rsp
	@echo d_bitobj.o32+d_object.o32>>d32_zil.rsp
	@echo $*, , $(D32_LIBS)>>d32_zil.rsp
	$(D32_LINK) $(D32_LINK_OPTS) @d32_zil.rsp
	@del d32_zil.rsp
