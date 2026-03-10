# ----- General Definitions -------------------------------------------------
VERSION=btcpp310
TRANS=-DZIL_TRANS

# ----- DOS compiler options ------------------------------------------------
DOS_CPP=bcc
DOS_LIBRARIAN=tlib

DOS_CPP_OPTS=-c -ml -Y -Z -DPM=0 $(TRANS)
DOS_LIB_OPTS=/C


.c.obj:
	$(DOS_CPP) $(DOS_CPP_OPTS) {$< }


# ----- Dos Extender ---------------------------------------------------------
D16_CPP=bcc286
D16_LIBRARIAN=tlib

D16_CPP_OPTS=-c -ml -Z -DPM=16 -DDOSX16=1 $(TRANS)
D16_LIB_OPTS=/C

.c.o16:
	$(D16_CPP) $(D16_CPP_OPTS) -o$*.o16 {$< }


# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To generate the GFX library for specific environments type:
	@echo make -fborland.mak dos
	@echo make -fborland.mak dos16
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean

# ----- DOS Libraries and Programs ------------------------------------------

dos: bc_lgfx.lib

bc_lgfx.lib : gfx.obj ..\asm\bcgfxasm.lib
	-@del bc_lgfx.lib
	$(DOS_LIBRARIAN) $(DOS_LIB_OPTS) @&&!
$* &
+gfx.obj+..\asm\bcgfxasm.lib
!
	copy bc_lgfx.lib ..\..\..\lib\$(VERSION)


# ----- DOS Extender Libraries and Programs -----------------------------------

dos16: bc_16gfx.lib


bc_16gfx.lib : gfx.o16 ..\asm\bcgfxasm.lib
	-@del bc_16gfx.lib
	$(D16_LIBRARIAN) $(D16_LIB_OPTS) @&&!
$* &
+gfx.o16+..\asm\bcgfxasm.lib
!
	copy bc_16gfx.lib ..\..\..\lib\$(VERSION)

