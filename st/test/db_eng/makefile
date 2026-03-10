#
# MAKEFILE
#

.autodepend
.swap

APP=test
SRC_DIR=..\..\source
OBJ_DIR=obp
BIN_DIR=.
EXE=$(APP)rx.exe
OBJ_EXT=obj

CC=bcc286
ASM=tasm
ASM_OPTS=/mx /zi /o
LINK=tlink
BIND=bind286
RTK_DIR=..\..\..\pharlap\bin

#-------------------------------------------------------------------------------

!if $d(DEBUG)
CC_OPTS= +..\test.cfg -v
LINK_OPTS=/c /x /C /v
ASM_OPTS=/mx /zi
!else
CC_OPTS= +..\test.cfg
LINK_OPTS=/c /x /C
ASM_OPTS=/mx
!endif

STARTUP_OBJS=c0pl.obj
LIBS=phapi d16_zil d16_gfx bc_16gfx emu286 emu mathl bcl286 ptclassl pbidsl
DEF_FILE=..\test.def

#-------------------------------------------------------------------------------

MODULE_1=db_eng
MODULE_2=dstorage
MODULE_3=dstatist
MODULE_4=cfg
MODULE_5=st_util
MODULE_6=filehdr
MODULE_7=parser

OBJS=$(OBJ_DIR)\$(APP).$(OBJ_EXT)      \
     $(OBJ_DIR)\$(MODULE_1).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_2).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_3).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_4).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_5).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_6).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_7).$(OBJ_EXT)

$(BIN_DIR)\$(EXE): $(OBJS)
    $(LINK) $(LINK_OPTS) @&&!
$(STARTUP_OBJS) $(OBJS)
$*, ,$(LIBS), $(DEF_FILE)
!
    $(BIND) $(RTK_DIR)\run286b $(BIN_DIR)\$(EXE) -dll $(RTK_DIR)\doscalls $(RTK_DIR)\int33

# !if $d(RUN)
#    $(BIND) $(RTK_DIR)\run286b $(BIN_DIR)\$(EXE) -dll $(RTK_DIR)\doscalls $(RTK_DIR)\int33
# !endif

#-------------------------------------------------------------------------------

# --- LOCAL MODULES ---

$(OBJ_DIR)\$(APP).$(OBJ_EXT): $(APP).cpp
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(APP).$(OBJ_EXT) $(APP).cpp

# --- NON_LOCAL MODULES

$(OBJ_DIR)\$(MODULE_1).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_1).cpp
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_1).$(OBJ_EXT) $(SRC_DIR)\$(MODULE_1).cpp

$(OBJ_DIR)\$(MODULE_2).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_2).cpp
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_2).$(OBJ_EXT) $(SRC_DIR)\$(MODULE_2).cpp

$(OBJ_DIR)\$(MODULE_3).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_3).cpp
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_3).$(OBJ_EXT) $(SRC_DIR)\$(MODULE_3).cpp

$(OBJ_DIR)\$(MODULE_4).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_4).cpp
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_4).$(OBJ_EXT) $(SRC_DIR)\$(MODULE_4).cpp

$(OBJ_DIR)\$(MODULE_5).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_5).cpp
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_5).$(OBJ_EXT) $(SRC_DIR)\$(MODULE_5).cpp

$(OBJ_DIR)\$(MODULE_6).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_6).cpp
   $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_6).$(OBJ_EXT) $(SRC_DIR)\$(MODULE_6).cpp

$(OBJ_DIR)\$(MODULE_7).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_7).cpp
   $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_7).$(OBJ_EXT) $(SRC_DIR)\$(MODULE_7).cpp

#-------------------------------------------------------------------------------


