# ----- General Definitions -------------------------------------------------
!include <ntwin32.mak>
VERSION=winnt

# ----- Windows NT compiler options -----------------------------------------
WNT_CPP=$(cc)
WNT_LINK=$(link)

WNT_CPP_OPTS=$(cflags) $(cvars) -I..
#WNT_CPP_OPTS=$(cflags) $(cvars) /Zi
WNT_LINK_OPTS=$(conflags)
#WNT_LINK_OPTS=$(guiflags)
#WNT_LINK_OPTS=$(guiflags) /DEBUG:MAPPED,FULL /DEBUGTYPE:CV
#WNT_LIB_OPTS=/machine:i386 /subsystem:CONSOLE
WNT_LIB_OPTS=/machine:i386 /subsystem:WINDOWS

WNT_OBJS=
WNT_LIBS=$(conlibs) wnt_zil.lib
#WNT_LIBS=$(guilibs) wnt_zil.lib

.SUFFIXES : .cpp

.cpp.obn:
	$(WNT_CPP) $(WNT_CPP_OPTS) -Fo$*.obn $<

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To generate the storage utilities type:
	@echo nmake -fmicrosft.mak dos
	@echo nmake -f mscwnt.mak winnt
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean

# ----- Windows NT ----------------------------------------------------------
winnt: nbrowse.exe nzdump.exe nrrmdir.exe ncppath.exe nchkfile.exe ngc.exe

nchkfile.exe: chkfile.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) -out:nchkfile.exe $(WNT_OBJS) chkfile.obn $(WNT_LIBS)
gc.obn:
	$(WNT_CPP) $(WNT_CPP_OPTS) -DDOGC -Fogc.obn cppath.cpp

ngc.exe: gc.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) -out:ngc.exe $(WNT_OBJS) gc.obn $(WNT_LIBS)

ncppath.exe: cppath.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) -out:ncppath.exe $(WNT_OBJS) cppath.obn $(WNT_LIBS)

nrrmdir.exe: rrmdir.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) -out:nrrmdir.exe $(WNT_OBJS) rrmdir.obn $(WNT_LIBS)

nzdump.exe: zdump.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) -out:nzdump.exe $(WNT_OBJS) zdump.obn $(WNT_LIBS)

nbrowse.exe: browse.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) -out:nbrowse.exe $(WNT_OBJS) browse.obn $(WNT_LIBS)

