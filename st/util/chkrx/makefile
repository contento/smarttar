.AUTODEPEND

#		*Translator Definitions*
CC = bcc +CHKRX.CFG
TASM = TASM
TLIB = tlib
TLINK = tlink
LIBPATH = ..\..\..\BC\LIB;
INCLUDEPATH = ..\..\..\BC\INCLUDE;..\..\..\ZINC\INCLUDE;..\..\INCLUDE


#		*Implicit Rules*
.c.obj:
  $(CC) -c {$< }

.cpp.obj:
  $(CC) -c {$< }

#		*List Macros*


EXE_dependencies =  \
 obj\chkrx.obj \
 obj\db_eng.obj \
 obj\dstorage.obj \
 obj\dstatist.obj \
 obj\cfg.obj \
 obj\st_util.obj \
 obj\parser.obj \
 obj\filehdr.obj \
 obj\info.obj

#		*Explicit Rules*
chkrx.exe: chkrx.cfg $(EXE_dependencies)
  $(TLINK) /v/x/c/P-/L$(LIBPATH) @&&|
c0l.obj+
obj\chkrx.obj+
obj\db_eng.obj+
obj\dstorage.obj+
obj\dstatist.obj+
obj\cfg.obj+
obj\st_util.obj+
obj\parser.obj+
obj\filehdr.obj+
obj\info.obj
chkrx
		# no map file
tclassl.lib+
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
obj\chkrx.obj: chkrx.cfg chkrx.cpp 

obj\db_eng.obj: chkrx.cfg ..\..\source\db_eng.cpp 
	$(CC) -c ..\..\source\db_eng.cpp

obj\dstorage.obj: chkrx.cfg ..\..\source\dstorage.cpp 
	$(CC) -c ..\..\source\dstorage.cpp

obj\dstatist.obj: chkrx.cfg ..\..\source\dstatist.cpp 
	$(CC) -c ..\..\source\dstatist.cpp

obj\cfg.obj: chkrx.cfg ..\..\source\cfg.cpp 
	$(CC) -c ..\..\source\cfg.cpp

obj\st_util.obj: chkrx.cfg ..\..\source\st_util.cpp 
	$(CC) -c ..\..\source\st_util.cpp

obj\parser.obj: chkrx.cfg ..\..\source\parser.cpp 
	$(CC) -c ..\..\source\parser.cpp

obj\filehdr.obj: chkrx.cfg ..\..\source\filehdr.cpp 
	$(CC) -c ..\..\source\filehdr.cpp

obj\info.obj: chkrx.cfg ..\..\source\info.cpp 
	$(CC) -c ..\..\source\info.cpp

#		*Compiler Configuration File*
chkrx.cfg: makefile
  copy &&|
-ml
-2
-H=CHKRX.SYM
-wpro
-weas
-wpre
-I$(INCLUDEPATH)
-L$(LIBPATH)
-D__UTIL__;__FHEADER=3;__NOSTM2__;__NOAPPINFO__
| chkrx.cfg


