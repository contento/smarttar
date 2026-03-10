# FREESTOR example makefile

#    wmake -f watcom.mak dos32       (makes the DOS freestor tutorial)

# Be sure to set the LIB and INCLUDE environment variables for Zinc, e.g.:
#    set INCLUDE=.;C:\DVX\INCLUDE;C:\WATCOM\H;C:\ZINC\INCLUDE
#    set LIB=.;C:\DVX\LIB\C386;C:\WATCOM\LIB386\DOS;C:\WATCOM\LIB386;C:\ZINC\LIB\WCCPP950

.EXTENSIONS:
.EXTENSIONS: .o32 .obw .obn .obo .obm .cpp

# Compiler and linker: (Add -d2 to CPP_OPTS and DEBUG ALL to LINK_OPTS for debug.)

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

.cpp.o32:
	$(D32_CPP) $(D32_CPP_OPTS) -fo=$[*.o32 $<

# ----- Usage --------------------------------------------------------------
usage: .SYMBOLIC
	@echo ...........
	@echo ...........
	@echo To make this Zinc example for Watcom C++ type:
	@echo wmake -f watcom.mak dos32
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean: .SYMBOLIC
	z_clean

# ----- DOS extender --------------------------------------------------------
dos32: freestor.exe .SYMBOLIC

freestor.exe: freestor.o32
	$(D32_LINK) $(D32_LINK_OPTS) N freestor.exe F freestor.o32 $(D32_OBJS) L $(D32_LIBS)

# ----- Windows -------------------------------------------------------------
windows: .SYMBOLIC
	@echo (This tutorial is not supported in Windows)

# ----- OS/2 ----------------------------------------------------------------
os2: .SYMBOLIC
	@echo (This tutorial is not supported in OS/2)

# ----- DESQview/X Motif ----------------------------------------------------
dvx: .SYMBOLIC
	@echo (This tutorial is not supported in DESQview/X Motif)
