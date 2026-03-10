# WORD tutorial makefile

#    nmake -fmicrosft.mak dos		      	(makes all the DOS word tutorials)
#    nmake -fmicrosft.mak dos16		      	(makes all the DOS extender word tutorials)
#    nmake -fmicrosft.mak windows	      	(makes all the Windows word tutorials)

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

CPP=cl
LINK=link
RC=rc

.c.obj:
	$(CPP) $(DOS_CPP_OPTS) $<

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
dos: word1a.exe word1b.exe word2.exe word3.exe

word1a.exe: word1a.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+word1a.obj
$*,NUL,$(DOS_LIBS),NUL
<<

word1b.exe: word1b.obj list.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+word1b.obj+list.obj
$*,NUL,$(DOS_LIBS),NUL
<<

word2.exe: word2.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+word2.obj
$*,NUL,$(DOS_LIBS),NUL
<<

word3.exe: word3.obj word_win.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+word3.obj+word_win.obj
$*,NUL,$(DOS_LIBS),NUL
<<

# ----- Windows -------------------------------------------------------------
windows: wword2.exe wword3.exe

wword2.exe: word2.obw
	$(LINK) $(WIN_LINK_OPTS) @<<zil.rsp
$(WIN_OBJS)+word2.obw
$*,NUL,$(WIN_LIBS),wword.def
<<
	$(RC) $(WIN_RC_OPTS) wword.rc $*.exe

wword3.exe: word3.obw word_win.obw
	$(LINK) $(WIN_LINK_OPTS) @<<zil.rsp
$(WIN_OBJS)+word3.obw+word_win.obw
$*,NUL,$(WIN_LIBS),wword.def
<<
	$(RC) $(WIN_RC_OPTS) wword.rc $*.exe

# ----- DOS Extender ---------------------------------------------------------
dos16: word1a16.exe word1b16.exe word216.exe word316.exe

word1a16.exe: word1a.o16
	$(LINK) $(D16_LINK_OPTS) @<<zil.rsp
$(D16_OBJS)+word1a.o16
$*,NUL,$(D16_LIBS),NUL
<<
#	$(D16_BIND) @<<zil.rsp
#$(PHARLAP_RTL)\$(D16_LOAD)
#$*
#-dll $(PHARLAP_RTL)\moucalls
#     $(PHARLAP_RTL)\int33
#     $(PHARLAP_RTL)\doscalls
#<<

word1b16.exe: word1b.o16 list.o16
	$(LINK) $(D16_LINK_OPTS) @<<zil.rsp
$(D16_OBJS)+word1b.o16+list.o16
$*,NUL,$(D16_LIBS),NUL
<<
#	$(D16_BIND) @<<zil.rsp
#$(PHARLAP_RTL)\$(D16_LOAD)
#$*
#-dll $(PHARLAP_RTL)\moucalls
#     $(PHARLAP_RTL)\int33
#     $(PHARLAP_RTL)\doscalls
#<<

word216.exe: word2.o16
	$(LINK) $(D16_LINK_OPTS) @<<zil.rsp
$(D16_OBJS)+word2.o16
$*,NUL,$(D16_LIBS),NUL
<<
#	$(D16_BIND) @<<zil.rsp
#$(PHARLAP_RTL)\$(D16_LOAD)
#$*
#-dll $(PHARLAP_RTL)\moucalls
#     $(PHARLAP_RTL)\int33
#     $(PHARLAP_RTL)\doscalls
#<<

word316.exe: word3.o16 word_win.o16
	$(LINK) $(D16_LINK_OPTS) @<<zil.rsp
$(D16_OBJS)+word3.o16+word_win.o16
$*,NUL,$(D16_LIBS),NUL
<<
#	$(D16_BIND) @<<zil.rsp
#$(PHARLAP_RTL)\$(D16_LOAD)
#$*
#-dll $(PHARLAP_RTL)\moucalls
#     $(PHARLAP_RTL)\int33
#     $(PHARLAP_RTL)\doscalls
#<<
