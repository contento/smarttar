# GENHELP and ICO2DAT program makefile

#    make -fzortech.mak dos		      	(makes the DOS genhelp/ico2dat program)
#    make -fzortech.mak windows	      	(makes the Windows genhelp/ico2dat program)
#    make -fzortech.mak os2	      	 	(makes the OS/2 genhelp/ico2dat program)

# Be sure to set the LIB and INCLUDE environment variables for Zinc, e.g.:
#    set INCLUDE=.;C:\ZINC\INCLUDE;C:\ZTC\INCLUDE
#    set LIB=.;C:\ZINC\LIB\ZTCPP300;C:\ZTC\LIB

## Compiler and linker: (Add -g to CPP_OPTS and /CO to LINK_OPTS for debug.)

# ----- DOS compiler options ------------------------------------------------
DOS_CPP_OPTS=-c -a1 -bx -ml
DOS_LINK_OPTS=/NOI
DOS_OBJS=
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
DOS_LIBS=dos_zil dos_gfx zt_lgfx
# --- Use the next line for UI_FG_DISPLAY ---
#DOS_LIBS=dos_zil dos_fg fg

# ----- Windows compiler options --------------------------------------------
WIN_CPP_OPTS=-c -a1 -bx -ml -W2
WIN_LINK_OPTS=/NOI
WIN_OBJS=
WIN_LIBS=win_zil

# ----- OS/2 compiler options -----------------------------------------------
OS2_CPP_OPTS=-c -mf
OS2_LINK_OPTS=/PM:WINDOWCOMPAT
OS2_OBJS=
OS2_LIBS=os2_zil

CPP=ztc
#LINK=blinkx
#LIBRARIAN=zorlibx
LINK=blink
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

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To make the genhelp and ico2dat programs for Zortech C++ type:
	@echo make -fzortech.mak dos
	@echo make -fzortech.mak os2
	@echo make -fzortech.mak windows
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean.bat

# ----- DOS -----------------------------------------------------------------
dos: genhelp.exe bmp2dat.exe ico2dat.exe
	copy genhelp.exe ..\..\bin
	copy bmp2dat.exe ..\..\bin
	copy ico2dat.exe ..\..\bin

genhelp.exe: genhelp.obj
	$(LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+genhelp.obj,$*, ,$(DOS_LIBS),NUL

bmp2dat.exe: bmp2dat.obj
	$(LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+bmp2dat.obj,$*, ,$(DOS_LIBS),NUL

ico2dat.exe: ico2dat.obj
	$(LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+ico2dat.obj,$*, ,$(DOS_LIBS),NUL

# ----- Windows -------------------------------------------------------------
windows: genhelp.exe bmp2dat.exe ico2dat.exe
	copy genhelp.exe ..\..\bin
	copy bmp2dat.exe ..\..\bin
	copy ico2dat.exe ..\..\bin

# ----- OS/2 ----------------------------------------------------------------
os2: ogenhelp.exe obmp2dat.exe oico2dat.exe
	copy ogenhelp.exe ..\..\bin
	copy obmp2dat.exe ..\..\bin
	copy oico2dat.exe ..\..\bin

ogenhelp.exe: genhelp.obo
	$(LINK) $(OS2_LINK_OPTS) $(OS2_OBJS)+genhelp.obo,$*, ,$(OS2_LIBS),

obmp2dat.exe: bmp2dat.obo
	$(LINK) $(OS2_LINK_OPTS) $(OS2_OBJS)+bmp2dat.obo,$*, ,$(OS2_LIBS),

oico2dat.exe: ico2dat.obo
	$(LINK) $(OS2_LINK_OPTS) $(OS2_OBJS)+ico2dat.obo,$*, ,$(OS2_LIBS),
