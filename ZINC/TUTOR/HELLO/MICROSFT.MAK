# HELLO tutorial makefile

#    nmake -fmicrosft.mak dos		      	(makes all the DOS hello tutorials)
#    nmake -fmicrosft.mak dos16		      	(makes all the DOS extender hello tutorials)
#    nmake -fmicrosft.mak windows	      	(makes all the Windows hello tutorials)

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
# --- Use the next line for UI_MSC_DISPLAY ---
#DOS_LIBS=dos_zil dos_msc llibce graphics oldnames

# ----- Windows compiler options --------------------------------------------
# --- Use the next 2 lines for Windows 3.0 compatibility ---
WIN_CPP_OPTS=-c -AL -BATCH -Gsw -DWINVER=0x0300
WIN_RC_OPTS=-k -30
# --- Use the next 2 lines for Windows 3.1 compatibility ---
#WIN_CPP_OPTS=-c -AL -BATCH -Gsw
#WIN_RC_OPTS=-k
WIN_LINK_OPTS=/NOD /NOI /BATCH /STACK:5120 /SEGMENTS:256
WIN_OBJS=
WIN_LIBS=win_zil libw llibcew oldnames

RC=rc
CPP=cl
LINK=link

.cpp.obj:
	$(CPP) $(DOS_CPP_OPTS) $<

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
dos: hello1.exe hello2.exe hello3.exe hellog.exe

hello1.exe: hello1.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+hello1.obj
$*,NUL,$(DOS_LIBS),NUL
<<

hello2.exe: hello2.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+hello2.obj
$*,NUL,$(DOS_LIBS),NUL
<<

hello3.exe: hello3.obj hello.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+hello3.obj+hello.obj
$*,NUL,$(DOS_LIBS),NUL
<<

hellog.exe: hellog.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+hellog.obj
$*,NUL,$(DOS_LIBS),NUL
<<

# ----- Windows -------------------------------------------------------------
windows: whello1.exe whello2.exe whello3.exe whellog.exe

whello1.exe: hello1.obw
	$(LINK) $(WIN_LINK_OPTS) @<<zil.rsp
$(WIN_OBJS)+hello1.obw
$*,NUL,$(WIN_LIBS),whello.def
<<
	$(RC) $(WIN_RC_OPTS) whello.rc $*.exe

whello2.exe: hello2.obw
	$(LINK) $(WIN_LINK_OPTS) @<<zil.rsp
$(WIN_OBJS)+hello2.obw
$*,NUL,$(WIN_LIBS),whello.def
<<
	$(RC) $(WIN_RC_OPTS) whello.rc $*.exe

whello3.exe: hello3.obw hello.obw
	$(LINK) $(WIN_LINK_OPTS) @<<zil.rsp
$(WIN_OBJS)+hello3.obw+hello.obw
$*,NUL,$(WIN_LIBS),whello.def
<<
	$(RC) $(WIN_RC_OPTS) whello.rc $*.exe

whellog.exe: hellog.obw
	$(LINK) $(WIN_LINK_OPTS) @<<zil.rsp
$(WIN_OBJS)+hellog.obw
$*,NUL,$(WIN_LIBS),whello.def
<<
	$(RC) $(WIN_RC_OPTS) whello.rc $*.exe

# ----- DOS Extender ---------------------------------------------------------
dos16: hello116.exe hello216.exe hello316.exe hellog16.exe

hello116.exe: hello1.o16
	$(LINK) $(D16_LINK_OPTS) @<<zil.rsp
$(D16_OBJS)+hello1.o16
$*,NUL,$(D16_LIBS),NUL
<<
#	$(D16_BIND) @<<zil.rsp
#$(PHARLAP_RTL)\$(D16_LOAD)
#$*
#-dll $(PHARLAP_RTL)\moucalls
#     $(PHARLAP_RTL)\int33
#     $(PHARLAP_RTL)\doscalls
#<<

hello216.exe: hello2.o16
	$(LINK) $(D16_LINK_OPTS) @<<zil.rsp
$(D16_OBJS)+hello2.o16
$*,NUL,$(D16_LIBS),NUL
<<
#	$(D16_BIND) @<<zil.rsp
#$(PHARLAP_RTL)\$(D16_LOAD)
#$*
#-dll $(PHARLAP_RTL)\moucalls
#     $(PHARLAP_RTL)\int33
#     $(PHARLAP_RTL)\doscalls
#<<

hello316.exe: hello3.o16 hello.o16
	$(LINK) $(D16_LINK_OPTS) @<<zil.rsp
$(D16_OBJS)+hello3.o16+hello.o16
$*,NUL,$(D16_LIBS),NUL
<<
#	$(D16_BIND) @<<zil.rsp
#$(PHARLAP_RTL)\$(D16_LOAD)
#$*
#-dll $(PHARLAP_RTL)\moucalls
#     $(PHARLAP_RTL)\int33
#     $(PHARLAP_RTL)\doscalls
#<<

hellog16.exe: hellog.o16
	$(LINK) $(D16_LINK_OPTS) @<<zil.rsp
$(D16_OBJS)+hellog.o16
$*,NUL,$(D16_LIBS),NUL
<<
#	$(D16_BIND) @<<zil.rsp
#$(PHARLAP_RTL)\$(D16_LOAD)
#$*
#-dll $(PHARLAP_RTL)\moucalls
#     $(PHARLAP_RTL)\int33
#     $(PHARLAP_RTL)\doscalls
#<<
