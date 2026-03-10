# ----- General Definitions -------------------------------------------------
!include <ntwin32.mak>
VERSION=winnt

# ----- Windows compiler options --------------------------------------------
WIN_CPP=$(cc)
WIN_LINK=$(link)
WIN_LIBRARIAN=lib
WIN_RC=rc

WIN_CPP_OPTS=$(cflags) $(cvars)
#WIN_CPP_OPTS=$(cflags) $(cvars) /Zi
#WIN_LINK_OPTS=$(conflags)
WIN_LINK_OPTS=$(guiflags)
#WIN_LINK_OPTS=$(guiflags) /DEBUG:MAPPED,FULL /DEBUGTYPE:CV
#WIN_LIB_OPTS=/machine:i386 /subsystem:CONSOLE
WIN_LIB_OPTS=/machine:i386 /subsystem:WINDOWS
WIN_RC_OPTS=

WIN_OBJS=
#WIN_LIBS=$(conlibs) wnt_zil.lib
WIN_LIBS=$(guilibs) wnt_zil.lib

.SUFFIXES : .cpp

.cpp.obn:
	$(WIN_CPP) $(WIN_CPP_OPTS) -Fo$*.obn $<

.rc.res:
    rc -r $*.rc

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To make the design program for Microsft C++ type:
	@echo nmake -fmicrosft.mak dos
	@echo nmake -fmicrosft.mak dos16
	@echo nmake -fmicrosft.mak windows
	@echo nmake -f mscwnt.mak winnt
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean.bat

# ----- Designer ------------------------------------------------------------
winnt: ndesign.exe
	copy ndesign.exe ..\..\bin
	copy p_design.znc ..\..\bin

ndesign.rbj: ndesign.res
    cvtres -$(CPU) $*.res -o $*.rbj

ndesign.exe: z_design.obn \
	z_bitobj.obn w_bitobj.obn z_scombo.obn z_config.obn z_ctrl.obn z_dialog.obn \
	z_file.obn z_cerror.obn z_frame.obn z_helped.obn z_help1.obn \
	z_hlpbar.obn z_hlpmsg.obn z_image.obn z_image1.obn z_jump.obn \
	z_objlst.obn z_resrc.obn z_util.obn z_object.obn w_object.obn z_advanc.obn \
	z_flag.obn z_import.obn z_image2.obn \
	ndesign.rbj
	$(WIN_LINK) $(WIN_LINK_OPTS) -out:ndesign.exe @<<zil.rsp
$(WIN_OBJS) z_design.obn 
z_bitobj.obn w_bitobj.obn z_scombo.obn z_config.obn z_ctrl.obn z_dialog.obn 
z_file.obn z_cerror.obn z_frame.obn z_helped.obn z_help1.obn z_hlpbar.obn 
z_hlpmsg.obn z_image.obn z_image1.obn z_jump.obn z_objlst.obn z_resrc.obn 
z_util.obn z_object.obn w_object.obn z_advanc.obn z_flag.obn z_import.obn z_image2.obn
$(WIN_LIBS)
ndesign.rbj
<<

