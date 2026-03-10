# ----- General Definitions -------------------------------------------------
VERSION=mscpp700
TRANS=-DZIL_TRANS

# ----- DOS compiler options ------------------------------------------------
DOS_CPP=cl
DOS_LIBRARIAN=lib

DOS_CPP_OPTS=-c -AL -Oas -Gs $(TRANS)
DOS_LIB_OPTS=/NOI

.c.obj:
	$(DOS_CPP) $(DOS_CPP_OPTS) $<

# ----- Dos Extender ---------------------------------------------------------
D16_CPP=cl
D16_LIBRARIAN=lib

D16_CPP_OPTS=-c -AL -Lp -Oas -Gs -DPM=16 -DDOSX16=1 $(TRANS)
D16_LIB_OPTS=/NOI

.c.o16:
	$(D16_CPP) $(D16_CPP_OPTS) -Fo$*.o16 $<

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To generate the GFX library for specific environments type:
	@echo nmake -fmicrosft.mak dos
	@echo nmake -fmicrosft.mak dos16
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean.bat

# ----- DOS Libraries -------------------------------------------------------
dos: ms_lgfx.lib

ms_lgfx.lib : gfx.obj ..\asm\msgfxasm.lib
	-@del ms_lgfx.lib
	$(DOS_LIBRARIAN) $(DOS_LIB_OPTS) @<<zil.rsp
$* &
+gfx.obj+..\asm\msgfxasm.lib
NUL
<<
	copy ms_lgfx.lib ..\..\..\lib\$(VERSION)


# ----- DOS Extender Libraries ----------------------------------------------
dos16: ms_16gfx.lib

ms_16gfx.lib : gfx.o16 ..\asm\msgfxasm.lib
	-@del ms_16gfx.lib
	$(D16_LIBRARIAN) $(D16_LIB_OPTS) @<<zil.rsp
$* &
+gfx.o16+..\asm\msgfxasm.lib
NUL
<<
	copy ms_16gfx.lib ..\..\..\lib\$(VERSION)
