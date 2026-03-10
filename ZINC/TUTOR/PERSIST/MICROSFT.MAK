# PERSIST tutorial makefile

#    make -fmicrosft.mak dos		      	(makes all the DOS persist tutorials)
#    make -fmicrosft.mak dos16		      	(makes all the DOS extender persist tutorials)
#    make -fmicrosft.mak windows	      	(makes all the Windows persist tutorials)

# Be sure to set the LIB and INCLUDE environment variables for Zinc, e.g.:
#    set INCLUDE=.;C:\ZINC\INCLUDE;C:\C700\INCLUDE
#    set LIB=.;C:\ZINC\LIB\MSCPP700;C:\C700\LIB

## Compiler and linker: (Add -Zi to CPP_OPTS and /CO to LINK_OPTS for debug.)

# ----- DOS compiler options ------------------------------------------------
DOS_CPP_OPTS=-c -AL -BATCH -Gs
DOS_LINK_OPTS=/NOD /NOI /BATCH /STACK:5120 /SEGMENTS:256
DOS_OBJS=
DOS_LIBS=ms_lgfx llibce graphics oldnames

# ----- Windows compiler options --------------------------------------------

# --- Use the next 2 lines for Windows 3.0 compatibility ---
WIN_CPP_OPTS=-c -AL -BATCH -Gsw -DWINVER=0x0300
WIN_RC_OPTS=-k -30
# --- Use the next 2 lines for Windows 3.1 compatibility ---
#WIN_CPP_OPTS=-c -AL -BATCH -Gsw
#WIN_RC_OPTS=-k
WIN_LINK_OPTS=/NOD /NOI /BATCH /STACK:5120 /SEGMENTS:256
WIN_OBJS=
WIN_LIBS=libw llibcew oldnames

CPP=cl
LINK=link
RC=rc

.c.obj:
	$(CPP) $(DOS_CPP_OPTS) $<

.cpp.obj:
	$(CPP) $(DOS_CPP_OPTS) $<

.c.obw:
	$(CPP) $(WIN_CPP_OPTS) -Fo$*.obw $<

.cpp.obw:
	$(CPP) $(WIN_CPP_OPTS) -Fo$*.obw $<

# ----- Dos Extender ---------------------------------------------------------
D16_CPP=cl
D16_LINK=link

D16_CPP_OPTS=-c -AL -BATCH -Gs2 -DDOSX286 -D$(FONT) -nologo
D16_LINK_OPTS=/NOD /NOI /BATCH /SEGMENTS:256 /STACK:5120 /FAR /ON:N
D16_LIB_OPTS=/NOI

D16_BIND=bind286
PHARLAP_RTL=c:\phar286\rtk
D16_LOAD=run286a

# --- Use the next line for UI_GRAPHICS_DISPLAY ---
D16_OBJS=graphp.obj			# don't use txtonly.obj here
D16_LIBS=phapi d16_zil d16_gfx ms_16gfx llibcep graphics
# --- Use the next lines for UI_MSC_DISPLAY ---
#D16_OBJS=graphp.obj
#D16_LIBS=phapi llibcep graphics d16_msc d16_zil

.cpp.o16:
	$(D16_CPP) $(D16_CPP_OPTS) -Fo$*.o16 $<

.c.o16:
	$(D16_CPP) $(D16_CPP_OPTS) -Fo$*.o16 $<

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

# ----- DOS -----------------------------------------------------------------
dos: persist1.exe persist2.exe persist3.exe persist4.exe persist5.exe persist6.exe persist7.exe

persist1.exe: persist1.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+persist1.obj
$*,NUL,$(DOS_LIBS),NUL
<<

persist2.exe: persist2.obj draw.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+persist2.obj+draw.obj
$*,NUL,$(DOS_LIBS),NUL
<<

persist3.exe: persist3.obj draw.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+persist3.obj+draw.obj
$*,NUL,$(DOS_LIBS),NUL
<<

persist4.exe: persist4.obj draw.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+persist4.obj+draw.obj
$*,NUL,$(DOS_LIBS),NUL
<<

persist5.exe: persist5.obj draw.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+persist5.obj+draw.obj
$*,NUL,$(DOS_LIBS),NUL
<<

persist6.exe: persist6.obj draw.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+persist6.obj+draw.obj
$*,NUL,$(DOS_LIBS),NUL
<<

persist7.exe: persist7.obj draw.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+persist7.obj+draw.obj
$*,NUL,$(DOS_LIBS),NUL
<<

# ----- Windows -------------------------------------------------------------
windows: wpersist.exe

wpersist.exe: wpersist.obw wdraw.obw
	$(LINK) $(WIN_LINK_OPTS) @<<zil.rsp
$(WIN_OBJS)+wpersist.obw+wdraw.obw
$*,NUL,$(WIN_LIBS),wpersist.def
<<
	$(RC) $(WIN_RC_OPTS) wpersist.rc $*.exe

# ----- DOS Extender ---------------------------------------------------------
dos16: persi116.exe persi216.exe persi316.exe persi416.exe persi516.exe persi616.exe persi716.exe

persi116.exe: persist1.o16
	$(LINK) $(D16_LINK_OPTS) @<<zil.rsp
$(D16_OBJS)+persist1.o16
$*,NUL,$(D16_LIBS),NUL
<<
#	$(D16_BIND) @<<zil.rsp
#$(PHARLAP_RTL)\$(D16_LOAD)
#$*
#-dll $(PHARLAP_RTL)\moucalls
#     $(PHARLAP_RTL)\int33
#     $(PHARLAP_RTL)\doscalls
#<<

persi216.exe: persist2.o16 draw.o16
	$(LINK) $(D16_LINK_OPTS) @<<zil.rsp
$(D16_OBJS)+persist2.o16+draw.o16
$*,NUL,$(D16_LIBS),NUL
<<
#	$(D16_BIND) @<<zil.rsp
#$(PHARLAP_RTL)\$(D16_LOAD)
#$*
#-dll $(PHARLAP_RTL)\moucalls
#     $(PHARLAP_RTL)\int33
#     $(PHARLAP_RTL)\doscalls
#<<

persi316.exe: persist3.o16 draw.o16
	$(LINK) $(D16_LINK_OPTS) @<<zil.rsp
$(D16_OBJS)+persist3.o16+draw.o16
$*,NUL,$(D16_LIBS),NUL
<<
#	$(D16_BIND) @<<zil.rsp
#$(PHARLAP_RTL)\$(D16_LOAD)
#$*
#-dll $(PHARLAP_RTL)\moucalls
#     $(PHARLAP_RTL)\int33
#     $(PHARLAP_RTL)\doscalls
#<<

persi416.exe: persist4.o16 draw.o16
	$(LINK) $(D16_LINK_OPTS) @<<zil.rsp
$(D16_OBJS)+persist4.o16+draw.o16
$*,NUL,$(D16_LIBS),NUL
<<
#	$(D16_BIND) @<<zil.rsp
#$(PHARLAP_RTL)\$(D16_LOAD)
#$*
#-dll $(PHARLAP_RTL)\moucalls
#     $(PHARLAP_RTL)\int33
#     $(PHARLAP_RTL)\doscalls
#<<

persi516.exe: persist5.o16 draw.o16
	$(LINK) $(D16_LINK_OPTS) @<<zil.rsp
$(D16_OBJS)+persist5.o16+draw.o16
$*,NUL,$(D16_LIBS),NUL
<<
#	$(D16_BIND) @<<zil.rsp
#$(PHARLAP_RTL)\$(D16_LOAD)
#$*
#-dll $(PHARLAP_RTL)\moucalls
#     $(PHARLAP_RTL)\int33
#     $(PHARLAP_RTL)\doscalls
#<<

persi616.exe: persist6.o16 draw.o16
	$(LINK) $(D16_LINK_OPTS) @<<zil.rsp
$(D16_OBJS)+persist6.o16+draw.o16
$*,NUL,$(D16_LIBS),NUL
<<
#	$(D16_BIND) @<<zil.rsp
#$(PHARLAP_RTL)\$(D16_LOAD)
#$*
#-dll $(PHARLAP_RTL)\moucalls
#     $(PHARLAP_RTL)\int33
#     $(PHARLAP_RTL)\doscalls
#<<

persi716.exe: persist7.o16 draw.o16
	$(LINK) $(D16_LINK_OPTS) @<<zil.rsp
$(D16_OBJS)+persist7.o16+draw.o16
$*,NUL,$(D16_LIBS),NUL
<<
#	$(D16_BIND) @<<zil.rsp
#$(PHARLAP_RTL)\$(D16_LOAD)
#$*
#-dll $(PHARLAP_RTL)\moucalls
#     $(PHARLAP_RTL)\int33
#     $(PHARLAP_RTL)\doscalls
#<<
