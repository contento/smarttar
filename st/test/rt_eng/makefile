#
# MAKEFILE
#

.autodepend
.swap

APP=test
SRC_DIR=..\..\source
OBJ_DIR=obp
BIN_DIR=.
EXE=$(APP)rt.exe
OBJ_EXT=obj

CC=bcc286
ASM=tasm
ASM_OPTS=/mx /zi /o
LINK=tlink
BIND=bind286
RTK_DIR=..\..\..\vendor\pharlap\bin

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

MODULE_1=semaphor
MODULE_2=cfg
MODULE_3=st_util
MODULE_4=rt_eng
MODULE_5=rt_do
MODULE_6=rt_isr
MODULE_7=rt_store
MODULE_8=rt_util
MODULE_9=info
MODULE_10=filehdr
MODULE_11=ph_eng
MODULE_12=ph_util
MODULE_13=ph_places
MODULE_14=ph_tar
MODULE_15=parser

OBJS=$(OBJ_DIR)\$(APP).$(OBJ_EXT)      \
     $(OBJ_DIR)\$(MODULE_1).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_2).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_3).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_4).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_5).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_6).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_7).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_8).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_9).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_10).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_11).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_12).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_13).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_14).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_15).$(OBJ_EXT)

$(BIN_DIR)\$(EXE): $(OBJS)
    $(LINK) $(LINK_OPTS) @&&!
$(STARTUP_OBJS) $(OBJS)
$*, ,$(LIBS), $(DEF_FILE)
!
    $(BIND) $(RTK_DIR)\run286b $(BIN_DIR)\$(EXE) -dll $(RTK_DIR)\doscalls $(RTK_DIR)\int33

#-------------------------------------------------------------------------------

# --- LOCAL MODULES ---

$(OBJ_DIR)\$(APP).$(OBJ_EXT): $(APP).cpp
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(APP).$(OBJ_EXT) $(APP).cpp

# --- NON_LOCAL MODULES

$(OBJ_DIR)\$(MODULE_1).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_1).asm
    $(ASM) $(ASM_OPTS) $(SRC_DIR)\$(MODULE_1).asm, $(OBJ_DIR)\$(MODULE_1).$(OBJ_EXT)

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

$(OBJ_DIR)\$(MODULE_10).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_10).cpp
   $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_10).$(OBJ_EXT) $(SRC_DIR)\$(MODULE_10).cpp

$(OBJ_DIR)\$(MODULE_11).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_11).cpp
   $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_11).$(OBJ_EXT) $(SRC_DIR)\$(MODULE_11).cpp

$(OBJ_DIR)\$(MODULE_12).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_12).cpp
   $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_12).$(OBJ_EXT) $(SRC_DIR)\$(MODULE_12).cpp

$(OBJ_DIR)\$(MODULE_13).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_13).cpp
   $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_13).$(OBJ_EXT) $(SRC_DIR)\$(MODULE_13).cpp

$(OBJ_DIR)\$(MODULE_14).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_14).cpp
   $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_14).$(OBJ_EXT) $(SRC_DIR)\$(MODULE_14).cpp

$(OBJ_DIR)\$(MODULE_15).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_15).cpp
   $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_15).$(OBJ_EXT) $(SRC_DIR)\$(MODULE_15).cpp

#-------------------------------------------------------------------------------

