# CURRENCY tutorial makefile

#    nmake -f mscwnt.mak winnt	      	(makes the Windows currency tutorial)

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
WIN_LIBRARIAN=lib

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

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To make this Zinc tutorial for Microsoft C++ type:
	@echo nmake -fmicrosft.mak dos
	@echo nmake -fmicrosft.mak dos16
	@echo (Microsoft does not support OS/2 programs.)
	@echo nmake -fmicrosft.mak windows
	@echo nmake -f mscwnt.mak winnt
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean

# ----- Windows NT ----------------------------------------------------------
winnt: nmoney.exe

nmoney.exe: money.obn
	$(WIN_LINK) $(WIN_LINK_OPTS) -out:nmoney.exe $(WIN_OBJS) money.obn $(WIN_LIBS)

