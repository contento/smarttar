# DESIGN program makefile

#    make -f ibm.mak os2	      	 	(makes the OS/2 design program)

# ----- OS/2 compiler options -----------------------------------------------
OS2_CPP=icc
OS2_LINK=link386
OS2_LIBRARIAN=lib
OS2_RC=rc

OS2_CPP_OPTS=-c -D__OS2__
OS2_LINK_OPTS=/NOE /BASE:0x10000 /PM:PM
OS2_LIB_OPTS=
OS2_RC_OPTS=

OS2_OBJS=
OS2_LIBS=os2_zil.lib

.SUFFIXES : .cpp

.cpp.obj:
	$(OS2_CPP) $(OS2_CPP_OPTS) $<

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To make the design program for IBM C++ type:
	@echo make -f ibm.mak os2
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean

# ----- OS/2 ----------------------------------------------------------------
os2: odesign.exe
	copy odesign.exe ..\..\bin
	copy p_design.znc ..\..\bin

odesign.exe: z_design.obj \
	z_bitobj.obj o_bitobj.obj o_object.obj z_scombo.obj z_config.obj \
	z_ctrl.obj z_dialog.obj z_file.obj z_cerror.obj z_frame.obj \
	z_helped.obj z_help1.obj z_hlpbar.obj z_hlpmsg.obj z_image.obj \
	z_image1.obj z_jump.obj z_objlst.obj z_resrc.obj z_util.obj \
	z_object.obj z_advanc.obj z_flag.obj z_import.obj z_image2.obj
	$(OS2_LINK) $(OS2_LINK_OPTS) @<<zil.rsp
$(OS2_OBJS)+z_design.obj+
z_bitobj.obj+o_bitobj.obj+o_object.obj+z_scombo.obj+z_config.obj+
z_ctrl.obj+z_dialog.obj+z_file.obj+z_cerror.obj+z_frame.obj+
z_helped.obj+z_help1.obj+z_hlpbar.obj+z_hlpmsg.obj+z_image.obj+
z_image1.obj+z_jump.obj+z_objlst.obj+z_resrc.obj+z_util.obj+
z_object.obj+z_advanc.obj+z_flag.obj+z_import.obj+z_image2.obj
$*, , $(OS2_LIBS), odesign.def
<<
	$(OS2_RC) $(OS2_RC_OPTS) odesign.rc $*.exe

