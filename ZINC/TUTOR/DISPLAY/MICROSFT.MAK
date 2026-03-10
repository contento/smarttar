# DISPLAY tutorial makefile

#	nmake -fmicrosft.mak dos		      	(makes the DOS display program)
#	nmake -fmicrosft.mak dos16		      	(makes the DOS extender display program)

# Be sure to set the LIB and INCLUDE environment variables for Zinc, e.g.:
#    set INCLUDE=.;C:\ZINC\INCLUDE;C:\C700\INCLUDE
#    set LIB=.;C:\ZINC\LIB\MSCPP700;C:\C700\LIB

## Compiler and linker: (Add -Zi to CPP_OPTS and /CO to LINK_OPTS for debug.)

# ----- DOS compiler options ------------------------------------------------
CPP=cl
LINK=link

# --- MSC display options ---
DOS_CPP_OPTS=-c -AL -BATCH -Gs
DOS_LINK_OPTS=/NOD /NOI /BATCH /STACK:5120 /SEGMENTS:256
DOS_OBJS=
DOS_LIBS=dos_zil dos_msc llibce graphics oldnames
DOS_LIBS=dos_zil dos_msc llibce graphics oldnames

# --- GFX display options ---
#DOS_CPP_OPTS=-c -AL -BATCH -Gs -DGFX_DISPLAY
#DOS_LINK_OPTS=/NOD /NOI /BATCH /STACK:5120 /SEGMENTS:256
#DOS_OBJS=
#DOS_LIBS=dos_zil dos_gfx ms_lgfx llibce graphics oldnames

.cpp.obj:
	$(CPP) $(DOS_CPP_OPTS) $<

# ----- Dos Extender ---------------------------------------------------------
D16_CPP=cl
D16_LINK=link

# --- MSC display options ---
D16_CPP_OPTS=-c -AL -BATCH -Gs2 -DDOSX286 -D$(FONT) -nologo
D16_LINK_OPTS=/NOD /NOI /BATCH /SEGMENTS:256 /STACK:5120 /FAR /ON:N
D16_LIB_OPTS=/NOI
D16_OBJS=graphp.obj
D16_LIBS=phapi llibcep graphics d16_msc d16_zil

# --- GFX display options ---
#D16_CPP_OPTS=-c -AL -BATCH -Gs2 -DDOSX286 -D$(FONT) -nologo
#D16_LINK_OPTS=/NOD /NOI /BATCH /SEGMENTS:256 /STACK:5120 /FAR /ON:N
#D16_LIB_OPTS=/NOI
#D16_OBJS=graphp.obj			# don't use txtonly.obj here
#D16_LIBS=phapi llibcep graphics d16_zil d16_gfx ms_16gfx 

D16_BIND=bind286
PHARLAP_RTL=c:\phar286\rtk
D16_LOAD=run286a

.cpp.o16:
	$(D16_CPP) $(D16_CPP_OPTS) -Fo$*.o16 $<

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To make this Zinc tutorial for Microsoft C++ type:
	@echo nmake -fmicrosft.mak dos
	@echo nmake -fmicrosft.mak dos16
	@echo (Microsoft does not support OS/2 programs.)
	@echo (This tutorial is not supported in Windows)
	@echo (This tutorial is not supported in Windows NT)
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean

# ----- DOS -----------------------------------------------------------------
dos: display.exe

display.exe: display.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+display.obj
$*,NUL,$(DOS_LIBS),NUL
<<

windows:
	@echo (This tutorial is not supported in Windows or OS/2)

# ----- DOS Extender ---------------------------------------------------------
dos16: disp16.exe

disp16.exe: display.o16
	$(LINK) $(D16_LINK_OPTS) @<<zil.rsp
$(D16_OBJS)+display.o16
$*,NUL,$(D16_LIBS),NUL
<<
#	$(D16_BIND) @<<zil.rsp
#$(PHARLAP_RTL)\$(D16_LOAD)
#$*
#-dll $(PHARLAP_RTL)\moucalls
#     $(PHARLAP_RTL)\int33
#     $(PHARLAP_RTL)\doscalls
#<<
