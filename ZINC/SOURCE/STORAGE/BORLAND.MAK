# ----- DOS compiler options ------------------------------------------------
DOS_CPP_OPTS=-c -ml -O -w -I..
DOS_LINK_OPTS=/c /x
DOS_OBJS=c0l
DOS_LIBS=dos_zil emu mathl cl

DOS_CPP=bcc
DOS_LINK=tlink

.cpp.obj:
	$(DOS_CPP) $(DOS_CPP_OPTS) {$< }

# ----- Windows NT compiler options -----------------------------------------
WNT_CPP=bcc32
WNT_LINK=tlink32
WNT_LIBRARIAN=tlib32
WNT_RC=brc32

WNT_CPP_OPTS=-c -I..
WNT_LINK_OPTS=/ap /Tpe /x /c
WNT_RC_OPTS=-w32
WNT_LIB_OPTS=/C

WNT_OBJS=c0x32
WNT_LIBS=wnt_zil imprtw32 import32 cx32

.cpp.obn:
	$(WNT_CPP) $(WNT_CPP_OPTS) -o$*.obn {$< }

# ----- OS/2 compiler options -----------------------------------------------
OS2_CPP_OPTS=-c -I..
OS2_LINK_OPTS=/c /ap
OS2_OBJS=c02.obj
OS2_LIBS=os2_zil.lib c2.lib os2.lib

OS2_CPP=bcc
OS2_LINK=tlink

.cpp.obo:
	$(OS2_CPP) $(OS2_CPP_OPTS) -o$*.obo {$< }

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To generate the storage utilities for type:
	@echo make -fborland.mak dos
	@echo make -fborland.mak winnt
	@echo make -fborland.mak os2
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean

# ----- DOS -----------------------------------------------------------------
dos: browse.exe zdump.exe rrmdir.exe cppath.exe chkfile.exe gc.exe

chkfile.exe: chkfile.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+chkfile.obj
$*, ,$(DOS_LIBS)
!

gc.obj: cppath.cpp
	$(DOS_CPP) $(DOS_CPP_OPTS) -DDOGC -ogc.obj cppath.cpp

gc.exe: gc.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+gc.obj
$*, ,$(DOS_LIBS)
!

cppath.exe: cppath.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+cppath.obj
$*, ,$(DOS_LIBS)
!

rrmdir.exe: rrmdir.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+rrmdir.obj
$*, ,$(DOS_LIBS)
!

zdump.exe: zdump.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+zdump.obj
$*, ,$(DOS_LIBS)
!

browse.exe: browse.obj
	$(DOS_LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+browse.obj
$*, ,$(DOS_LIBS)
!

# ----- Windows -------------------------------------------------------------
windows: browse.exe zdump.exe rrmdir.exe cppath.exe chkfile.exe gc.exe

# ----- Windows NT ----------------------------------------------------------
winnt: nbrowse.exe nzdump.exe nrrmdir.exe ncppath.exe nchkfile.exe ngc.exe

nchkfile.exe: chkfile.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+chkfile.obn
$*, ,$(WNT_LIBS)
!

gc.obn:
	$(WNT_CPP) $(WNT_CPP_OPTS) -DDOGC -ogc.obn cppath.cpp

ngc.exe: gc.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+gc.obn
$*, ,$(WNT_LIBS)
!

ncppath.exe: cppath.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+cppath.obn
$*, ,$(WNT_LIBS)
!

nrrmdir.exe: rrmdir.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+rrmdir.obn
$*, ,$(WNT_LIBS)
!

nzdump.exe: zdump.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+zdump.obn
$*, ,$(WNT_LIBS)
!

nbrowse.exe: browse.obn
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+browse.obn
$*, ,$(WNT_LIBS)
!

# ----- OS/2 ----------------------------------------------------------------
os2: obrowse.exe ozdump.exe orrmdir.exe ocppath.exe ochkfile.exe ogc.exe

ochkfile.exe: chkfile.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+chkfile.obo
$*, ,$(OS2_LIBS),
!

gc.obo:
	$(OS2_CPP) $(OS2_CPP_OPTS) -DDOGC -ogc.obo cppath.cpp

ogc.exe: gc.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+gc.obo
$*, ,$(OS2_LIBS),
!

ocppath.exe: cppath.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+cppath.obo
$*, ,$(OS2_LIBS),
!

orrmdir.exe: rrmdir.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+rrmdir.obo
$*, ,$(OS2_LIBS),
!

ozdump.exe: zdump.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+zdump.obo
$*, ,$(OS2_LIBS),
!

obrowse.exe: browse.obo
	$(OS2_LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+browse.obo
$*, ,$(OS2_LIBS),
!

