# DEMONSTRATION program makefile

#    make -f mscwnt.mak winnt	      		(makes all the Windows demo tutorials)

# Be sure to set the LIB and INCLUDE environment variables for Zinc, e.g.:
#    set INCLUDE=.;C:\ZINC\INCLUDE;C:\C700\INCLUDE
#    set LIB=.;C:\ZINC\LIB\MSCPP700;C:\C700\LIB

## Compiler and linker: (Add -Zi to CPP_OPTS and /CO to LINK_OPTS for debug.)

# ----- General Definitions -------------------------------------------------
!include <ntwin32.mak>
VERSION=winnt

# ----- Windows NT compiler options -----------------------------------------
WIN_CPP=$(cc)
WIN_LINK=$(link)

WIN_CPP_OPTS=$(cflags) $(cvars)
#WIN_CPP_OPTS=$(cflags) $(cvars) /Zi
#WIN_LINK_OPTS=$(conflags)
WIN_LINK_OPTS=$(guiflags)
#WIN_LINK_OPTS=$(guiflags) /DEBUG:MAPPED,FULL /DEBUGTYPE:CV
#WIN_LIB_OPTS=/machine:i386 /subsystem:CONSOLE
WIN_LIB_OPTS=/machine:i386 /subsystem:WINDOWS

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
	@echo To make the Zinc demonstration for Microsoft C++ type:
	@echo nmake -f mscwnt.mak winnt
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean

# ----- Windows NT ----------------------------------------------------------
winnt: zinc_nt.exe

ndemo.rbj: ndemo.res
    cvtres -$(CPU) $*.res -o $*.rbj

zinc_nt.exe: zincdemo.obn ndemo.rbj
	$(WIN_LINK) $(WIN_LINK_OPTS) -out:zinc_nt.exe $(WIN_OBJS) zincdemo.obn ndemo.rbj $(WIN_LIBS)

