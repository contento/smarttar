# ----- General Definitions -------------------------------------------------
VERSION=ztcpp310
TRANS=-DZIL_TRANS

# ----- DOS compiler options ------------------------------------------------
DOS_CPP=ztc
DOS_LIBRARIAN=zorlibx

DOS_CPP_OPTS=-c -bx -o-da -ml -p -b -DPM=0 $(TRANS)
DOS_LIB_OPTS=/C /N /B

.c.obj:
	$(DOS_CPP) $(DOS_CPP_OPTS) $<

# ----- DOS extender compiler options ---------------------------------------
D16_CPP=ztc
D16_LIBRARIAN=zorlibx

D16_CPP_OPTS=-c -bx -mz -p -w7 -DDOSX16=2 -DPM=16 $(TRANS)
D16_LIB_OPTS=/C /N /B

.c.o16:
	$(D16_CPP) $(D16_CPP_OPTS) -o$@ $<

D32_CPP=ztc
D32_LIBRARIAN=zorlibx

D32_CPP_OPTS=-c -bx -mx -p -w7 -DDOSX16=4 -DPM=32 $(TRANS)
D32_LIB_OPTS=/C /N /B

.c.o32:
	$(D32_CPP) $(D32_CPP_OPTS) -o$@ $<

# ----- Usage ---------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To generate the GFX library for specific environments type:
	@echo make -fzortech.mak dos
	@echo make -fzortech.mak dos16
	@echo make -fzortech.mak dos32
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean.bat

# ----- DOS Libraries -------------------------------------------------------

dos: zt_lgfx.lib

zt_lgfx.lib : gfx.obj ..\asm\ztgfxasm.lib
	-@del zt_lgfx.lib
	$(DOS_LIBRARIAN) $@ $(DOS_LIB_OPTS) +gfx.obj+..\asm\ztgfxasm.lib, NUL;
	copy zt_lgfx.lib ..\..\..\lib\$(VERSION)

# ----- Dos extender Libraries ----------------------------------------------

dos16: zt_16gfx.lib

zt_16gfx.lib : gfx.o16 ..\asm\ztgfxasm.lib
	-@del zt_16gfx.lib
	$(DOS_LIBRARIAN) $@ $(DOS_LIB_OPTS) +gfx.o16+..\asm\ztgfxasm.lib, NUL;
	copy zt_16gfx.lib ..\..\..\lib\$(VERSION)

dos32: zt_32gfx.lib 

zt_32gfx.lib : gfx.o32 ..\asm\ztgfxa32.lib
	-@del zt_32gfx.lib
	$(DOS_LIBRARIAN) $@ $(DOS_LIB_OPTS) +gfx.o32+..\asm\ztgfxa32.lib, NUL;
	copy zt_32gfx.lib ..\..\..\lib\$(VERSION)

