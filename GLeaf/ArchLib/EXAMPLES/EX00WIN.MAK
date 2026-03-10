# Microsoft Visual C++ generated build script - Do not modify

PROJ = EX00WIN
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = D:\AL211\EXAMPLES\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC =             
FIRSTCPP = EX00WIN.CPP 
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AL /Od /D "_DEBUG" /FR /GA /Fd"EX00WIN.PDB"
CFLAGS_R_WEXE = /nologo /W3 /AM /O1 /D "NDEBUG" /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /CO /ALIGN:16 /STACK:10240
LFLAGS_R_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /ALIGN:16 /STACK:10240
LIBS_D_WEXE = oldnames libw commdlg shell olecli olesvr llibcew
LIBS_R_WEXE = oldnames libw commdlg shell olecli olesvr mlibcew
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = EX00WIN.DEF
OBJS_EXT = 
LIBS_EXT = CTL3D.LIB ..\V1X\ALLW.LIB 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE)
LFLAGS = $(LFLAGS_D_WEXE)
LIBS = $(LIBS_D_WEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WEXE)
LFLAGS = $(LFLAGS_R_WEXE)
LIBS = $(LIBS_R_WEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = EX00WIN.SBR


CTL3D_DEP = 

EX00WIN_RCDEP = d:\al211\examples\ex00win.h \
	d:\al211\examples\alicon.ico


EX00WIN_DEP = d:\al211\h\al.h \
	d:\al211\h\arclib.h \
	d:\al211\h\aldefs.h \
	d:\al211\h\_debug.h \
	d:\al211\h\status.h \
	d:\al211\h\status.inl \
	d:\al211\h\objname.h \
	d:\al211\h\objname.inl \
	d:\al211\h\stor.h \
	d:\al211\h\crypto.h \
	d:\al211\h\crypto.inl \
	d:\al211\h\timedate.h \
	d:\al211\h\timedate.inl \
	d:\al211\h\fileattr.h \
	d:\al211\h\fileattr.inl \
	d:\al211\h\stor.inl \
	d:\al211\h\engn.h \
	d:\al211\h\engn.inl \
	d:\al211\h\monitor.h \
	d:\al211\h\toolkit.h \
	d:\al211\h\arcentry.h \
	d:\al211\h\arcentry.inl \
	d:\al211\h\arclist.h \
	d:\al211\h\arclist.inl \
	d:\al211\h\cmpobj.h \
	d:\al211\h\cmpobj.inl \
	d:\al211\h\arc.h \
	d:\al211\h\arc.inl \
	d:\al211\h\filestor.h \
	d:\al211\h\memstore.h \
	d:\al211\h\copyengn.h \
	d:\al211\h\glengn.h \
	d:\al211\h\glengn.inl \
	d:\al211\h\glarc.h \
	d:\al211\h\pkengn.h \
	d:\al211\h\pkarc.h \
	d:\al211\h\winmon.h \
	d:\al211\h\winutil.h \
	d:\al211\h\algauge.h \
	d:\al211\h\pmmon.h \
	d:\al211\h\pmutil.h \
	d:\al211\h\bargraph.h \
	d:\al211\h\spinner.h \
	d:\al211\h\wildcard.h \
	d:\al211\examples\ex00win.h \
	d:\al211\examples\ctl3d.h


ALLW_DEP = 

all:	$(PROJ).EXE $(PROJ).BSC

EX00WIN.RES:	EX00WIN.RC $(EX00WIN_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r EX00WIN.RC

EX00WIN.OBJ:	EX00WIN.CPP $(EX00WIN_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c EX00WIN.CPP


$(PROJ).EXE::	EX00WIN.RES

$(PROJ).EXE::	EX00WIN.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
EX00WIN.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
d:\msvc\lib\+
CTL3D.LIB+
..\V1X\ALLW.LIB+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) EX00WIN.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	EX00WIN.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) EX00WIN.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
