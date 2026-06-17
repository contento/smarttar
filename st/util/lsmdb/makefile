#
# MAKEFILE — builds lsmdb.exe (MiniDB .db file inspector)
#
# Self-contained: only needs lsmdb.cpp + minidb/page.h from include/.
# No app modules beyond page.h struct definitions.
#

APP=lsmdb
OBJ_DIR=obj
BIN_DIR=.

CC=bcc286
LINK=tlink
BIND=bind286
RTK_DIR=c:\vendor\pharlap\bin

# util.cfg provides -I paths for minidb/page.h and st_defs.h
CC_OPTS= +..\util.cfg
LIBS=..\..\..\vendor\pharlap\lib\phapi.lib ..\..\..\vendor\bc\lib\emu.lib ..\..\..\vendor\bc\lib\mathl.lib ..\..\..\vendor\bc\lib\cl.lib
LINK_OPTS=/c /x /C
all: $(BIN_DIR)\$(APP).exe

$(BIN_DIR)\$(APP).exe: $(OBJ_DIR)\$(APP).obj
	$(LINK) $(LINK_OPTS) @&&!
$(STARTUP_OBJS) $(OBJ_DIR)\$(APP).obj
$*, ,$(LIBS), $(DEF_FILE)
!
	$(BIND) $(RTK_DIR)\run286b $@ -dll $(RTK_DIR)\doscalls $(RTK_DIR)\int33

$(OBJ_DIR)\$(APP).obj: $(APP).cpp
	-@mkdir $(OBJ_DIR) > NUL
	$(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(APP).obj $(APP).cpp

clean:
	-@del $(OBJ_DIR)\*.obj
	-@del $(BIN_DIR)\$(APP).exe
