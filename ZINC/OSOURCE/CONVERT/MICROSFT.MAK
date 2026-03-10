# GENHELP and ICO2DAT program makefile

#    make -fmicrosft.mak dos		      	(makes all the DOS genhelp/ico2dat programs)
#    make -fmicrosft.mak windows	      	(makes all the Windows genhelp/ico2dat programs)

# Be sure to set the LIB and INCLUDE environment variables for Zinc, e.g.:
#    set INCLUDE=.;C:\ZINC\INCLUDE;C:\C700\INCLUDE
#    set LIB=.;C:\ZINC\LIB\MSCPP700;C:\C700\LIB

## Compiler and linker: (Add -Zi to CPP_OPTS and /CO to LINK_OPTS for debug.)

# ----- DOS compiler options ------------------------------------------------
DOS_CPP_OPTS=-c -AL -BATCH -Gs
DOS_LINK_OPTS=/NOD /NOI /BATCH /STACK:5120 /SEGMENTS:256
DOS_OBJS=
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
DOS_LIBS=dos_zil dos_gfx ms_lgfx llibce graphics oldnames
# --- Use the next line for UI_BGI_DISPLAY ---
#DOS_LIBS=dos_zil dos_msc llibce graphics oldnames

# ----- Windows compiler options --------------------------------------------
WIN_CPP_OPTS=-c -AL -BATCH -Gsw -DWINVER=0x0300
WIN_LINK_OPTS=/NOD /NOI /BATCH /STACK:5120 /SEGMENTS:256
WIN_OBJS=
WIN_LIBS=win_zil libw llibcew oldnames

CPP=cl
LINK=link

.cpp.obj:
	$(CPP) $(DOS_CPP_OPTS) $<

.cpp.obw:
	$(CPP) $(WIN_CPP_OPTS) -Fo$*.obw $<

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To make the genhelp and ico2dat programs for Microsoft C++ type:
	@echo nmake -fmicrosft.mak dos
	@echo (Microsoft does not support OS/2)
	@echo nmake -f mscwnt.mak winnt
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean

# ----- DOS -----------------------------------------------------------------
dos: genhelp.exe bmp2dat.exe ico2dat.exe
	copy genhelp.exe ..\..\bin
	copy bmp2dat.exe ..\..\bin
	copy ico2dat.exe ..\..\bin

genhelp.exe: genhelp.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+genhelp.obj
$*,NUL,$(DOS_LIBS),NUL
<<

bmp2dat.exe: bmp2dat.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+bmp2dat.obj
$*,NUL,$(DOS_LIBS),NUL
<<

ico2dat.exe: ico2dat.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+ico2dat.obj
$*,NUL,$(DOS_LIBS),NUL
<<

# ----- Windows -------------------------------------------------------------
windows: genhelp.exe bmp2dat.exe ico2dat.exe
	copy genhelp.exe ..\..\bin
	copy bmp2dat.exe ..\..\bin
	copy ico2dat.exe ..\..\bin

# ----- OS/2 Libraries and Programs -----------------------------------------
os2:
	@echo Microsoft does not support OS/2 libraries and programs.


