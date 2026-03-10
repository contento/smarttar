# PERSIST tutorial makefile

#    make -fborland.mak dos		      	(makes all the DOS persist tutorials)
#    make -fborland.mak dos16	      	(makes all the DOS extender persist tutorials)
#    make -fborland.mak dos32	      	(makes all the DOS extender persist tutorials)
#    make -fborland.mak windows	      	(makes all the Windows persist tutorials)
#    make -fborland.mak os2		      	(makes all the OS/2 persist tutorials)

# Be sure to set the LIB and INCLUDE environment variables for Zinc, e.g.:
#    set INCLUDE=.;C:\ZINC\INCLUDE;C:\ZTC\INCLUDE
#    set LIB=.;C:\ZINC\LIB\ZTCPP300;C:\ZTC\LIB

## Compiler and linker: (Add -g to CPP_OPTS and /CO to LINK_OPTS for debug.)

# ----- DOS compiler options ------------------------------------------------
DOS_CPP=ztc
DOS_LINK=blinkx

DOS_CPP_OPTS=-c -a1 -bx -ml
DOS_LINK_OPTS=/NOI
DOS_OBJS=
DOS_LIBS=fg

# ----- Windows compiler options --------------------------------------------
WIN_CPP=ztc
WIN_LINK=blinkx
WIN_RC=rc

# --- Use the next 2 lines for Windows 3.0 compatibility ---
WIN_CPP_OPTS=-c -a1 -bx -ml -W2 -DWINVER=0x0300
WIN_RC_OPTS=-k -30
# --- Use the next 2 lines for Windows 3.1 compatibility ---
#WIN_CPP_OPTS=-c -a1 -bx -ml -W2
#WIN_RC_OPTS=-k
WIN_LINK_OPTS=/NOI /PACKC:42000
WIN_OBJS=
WIN_LIBS=libw

# ----- OS/2 compiler options -----------------------------------------------
OS2_CPP=ztc
OS2_LINK=blink
OS2_RC=rc

OS2_CPP_OPTS=-c -mf
OS2_LINK_OPTS=/BASE:0x10000 /PM:PM 
OS2_RC_OPTS=
OS2_OBJS=
OS2_LIBS=os2_zil

.c.obj:
	$(DOS_CPP) $(DOS_CPP_OPTS) $<

.cpp.obj:
	$(DOS_CPP) $(DOS_CPP_OPTS) $<

.c.obw:
	$(WIN_CPP) $(WIN_CPP_OPTS) -o$*.obw $<

.cpp.obw:
	$(WIN_CPP) $(WIN_CPP_OPTS) -o$*.obw $<

.c.obo:
	$(OS2_CPP) $(OS2_CPP_OPTS) -o$*.obo $<

.cpp.obo:
	$(OS2_CPP) $(OS2_CPP_OPTS) -o$*.obo $<

# ----- DOS extender compiler options -----------------------------------------
D16_CPP=ztc
D16_LINK=blinkx

D16_CPP_OPTS=-c -bx -mz -D$(FONT)
D16_LINK_OPTS=/NOI
D16_LIB_OPTS=/C /N /B

D16_OBJS=
# --- Use the next line for UI_FG_DISPLAY ---
D16_LIBS=zpm d16_zil d16_fg fg

.cpp.o16:
	$(D16_CPP) $(D16_CPP_OPTS) -o$@ $<

.c.o16:
	$(D16_CPP) $(D16_CPP_OPTS) -o$@ $<

# ----- DOS extender compiler options -----------------------------------------
D32_CPP=ztc
D32_LINK=blinkx

D32_CPP_OPTS=-c -bx -mx -D$(FONT)
D32_LINK_OPTS=/NOI
D32_LIB_OPTS=/C /N /B

D32_OBJS=cx
# --- Use the next line for UI_FG_DISPLAY ---
D32_LIBS=d32_zil d32_fg fgp

.cpp.o32:
	$(D32_CPP) $(D32_CPP_OPTS) -o$@ $<

.c.o32:
	$(D32_CPP) $(D32_CPP_OPTS) -o$@ $<

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To make this Zinc tutorial for Zortech C++ type:
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
dos: persist1.exe persist2.exe persist3.exe persist4.exe persist5.exe persist6.exe persist7.exe

persist1.exe: persist1.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+persist1.obj,$*, ,$(DOS_LIBS),NUL

persist2.exe: persist2.obj draw.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+persist2.obj+draw.obj,$*, ,$(DOS_LIBS),NUL

persist3.exe: persist3.obj draw.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+persist3.obj+draw.obj,$*, ,$(DOS_LIBS),NUL

persist4.exe: persist4.obj draw.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+persist4.obj+draw.obj,$*, ,$(DOS_LIBS),NUL

persist5.exe: persist5.obj draw.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+persist5.obj+draw.obj,$*, ,$(DOS_LIBS),NUL

persist6.exe: persist6.obj draw.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+persist6.obj+draw.obj,$*, ,$(DOS_LIBS),NUL

persist7.exe: persist7.obj draw.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+persist7.obj+draw.obj,$*, ,$(DOS_LIBS),NUL

# ----- Windows -------------------------------------------------------------
windows: wpersist.exe

wpersist.exe: wpersist.obw wdraw.obw
	$(WIN_LINK) $(WIN_LINK_OPTS) $(WIN_OBJS)+wpersist.obw+wdraw.obw,$*, ,$(WIN_LIBS),wpersist.def
	$(WIN_RC) $(WIN_RC_OPTS) wpersist.rc $*.exe

# ----- OS/2 ----------------------------------------------------------------
os2: opersist.exe

opersist.exe: opersist.obo odraw.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) $(OS2_OBJS)+opersist.obo+odraw.obo,$*, ,$(OS2_LIBS),opersist.def
	$(OS2_RC) $(OS2_RC_OPTS) opersist.rc $*.exe

# ----- Dos extender ----------------------------------------------------------
dos16: persi116.exe persi216.exe persi316.exe persi416.exe persi516.exe persi616.exe persi716.exe

persi116.exe: persist1.o16
	$(D16_LINK) $(D16_LINK_OPTS) $(D16_OBJS)+persist1.o16,$*, ,$(D16_LIBS),z16.def

persi216.exe: persist2.o16 draw.o16
	$(D16_LINK) $(D16_LINK_OPTS) $(D16_OBJS)+persist2.o16+draw.o16,$*, ,$(D16_LIBS),z16.def

persi316.exe: persist3.o16 draw.o16
	$(D16_LINK) $(D16_LINK_OPTS) $(D16_OBJS)+persist3.o16+draw.o16,$*, ,$(D16_LIBS),z16.def

persi416.exe: persist4.o16 draw.o16
	$(D16_LINK) $(D16_LINK_OPTS) $(D16_OBJS)+persist4.o16+draw.o16,$*, ,$(D16_LIBS),z16.def

persi516.exe: persist5.o16 draw.o16
	$(D16_LINK) $(D16_LINK_OPTS) $(D16_OBJS)+persist5.o16+draw.o16,$*, ,$(D16_LIBS),z16.def

persi616.exe: persist6.o16 draw.o16
	$(D16_LINK) $(D16_LINK_OPTS) $(D16_OBJS)+persist6.o16+draw.o16,$*, ,$(D16_LIBS),z16.def

persi716.exe: persist7.o16 draw.o16
	$(D16_LINK) $(D16_LINK_OPTS) $(D16_OBJS)+persist7.o16+draw.o16,$*, ,$(D16_LIBS),z16.def

# ----- Dos extender ----------------------------------------------------------
dos32: persi132.exe persi232.exe persi332.exe persi432.exe persi532.exe persi632.exe persi732.exe

persi132.exe: persist1.o32
	$(D32_LINK) $(D32_LINK_OPTS) $(D32_OBJS)+persist1.o32,$*, ,$(D32_LIBS)

persi232.exe: persist2.o32 draw.o32
	$(D32_LINK) $(D32_LINK_OPTS) $(D32_OBJS)+persist2.o32+draw.o32,$*, ,$(D32_LIBS)

persi332.exe: persist3.o32 draw.o32
	$(D32_LINK) $(D32_LINK_OPTS) $(D32_OBJS)+persist3.o32+draw.o32,$*, ,$(D32_LIBS)

persi432.exe: persist4.o32 draw.o32
	$(D32_LINK) $(D32_LINK_OPTS) $(D32_OBJS)+persist4.o32+draw.o32,$*, ,$(D32_LIBS)

persi532.exe: persist5.o32 draw.o32
	$(D32_LINK) $(D32_LINK_OPTS) $(D32_OBJS)+persist5.o32+draw.o32,$*, ,$(D32_LIBS)

persi632.exe: persist6.o32 draw.o32
	$(D32_LINK) $(D32_LINK_OPTS) $(D32_OBJS)+persist6.o32+draw.o32,$*, ,$(D32_LIBS)

persi732.exe: persist7.o32 draw.o32
	$(D32_LINK) $(D32_LINK_OPTS) $(D32_OBJS)+persist7.o32+draw.o32,$*, ,$(D32_LIBS)
