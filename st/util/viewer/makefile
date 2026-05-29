#
# MAKEFILE
#

.autodepend
.swap

APP=viewer
SRC_DIR=..\..\source
OBJ_DIR=obj
BIN_DIR=.
EXE=$(APP).exe
OBJ_EXT=obj

CC=bcc286
ASM=tasm
ASM_OPTS=/mx /zi /o
LINK=tlink
BIND=bind286
RTK_DIR=c:\vendor\pharlap\bin

#-------------------------------------------------------------------------------

!if $d(DEBUG)
CC_OPTS= -D__NOSTM2__;__NOAPPINFO__ +..\util.cfg -v
LINK_OPTS=/c /x /C /v
ASM_OPTS=/mx /zi
!else
CC_OPTS= -D__NOSTM2__;__NOAPPINFO__ +..\util.cfg
LINK_OPTS=/c /x /C
ASM_OPTS=/mx
!endif

STARTUP_OBJS=c0pl.obj
LIBS=phapi d16_zil d16_gfx bc_16gfx emu286 emu mathl bcl286 ptclassl pbidsl
DEF_FILE=..\util.def

#-------------------------------------------------------------------------------

MODULE_1=db_view
MODULE_2=db_eng
MODULE_3=dstorage
MODULE_4=dstatist
MODULE_5=cfg
MODULE_6=st_util
MODULE_7=res
MODULE_8=filehdr
MODULE_9=info
MODULE_A=parser

OBJS=$(APP).$(OBJ_EXT)                 \
     $(OBJ_DIR)\$(MODULE_1).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_2).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_3).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_4).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_5).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_6).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_7).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_8).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_9).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_A).$(OBJ_EXT)

$(BIN_DIR)\$(EXE): $(OBJS)
    $(LINK) $(LINK_OPTS) @&&!
$(STARTUP_OBJS) $(OBJS)
$*, ,$(LIBS), $(DEF_FILE)
!

!if $d(RUN)
    $(BIND) $(RTK_DIR)\run286b $(BIN_DIR)\$(EXE) -dll $(RTK_DIR)\doscalls $(RTK_DIR)\int33
!endif

#-------------------------------------------------------------------------------

# --- LOCAL MODULES ---

$(APP).$(OBJ_EXT): $(APP).cpp
    $(CC) $(CC_OPTS) -o$(APP).$(OBJ_EXT) $(APP).cpp

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

$(OBJ_DIR)\$(MODULE_8).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_8).cpp
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_8).$(OBJ_EXT) $(SRC_DIR)\$(MODULE_8).cpp

$(OBJ_DIR)\$(MODULE_9).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_9).cpp
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_9).$(OBJ_EXT) $(SRC_DIR)\$(MODULE_9).cpp

#-------------------------------------------------------------------------------


