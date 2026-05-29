#
# MAKEFILE
#

.autodepend
.swap

APP=inf2dat
SRC_DIR=..\..\src
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

# TLINK library search path. The -L in util.cfg is BCC286's (forwarded to
# TLINK only when BCC286 invokes the linker). We call TLINK directly, so
# we must pass the lib path here too. Mirrors LINK_LIBPATH in st\MAKEFILE.
LINK_LIBPATH=/L..\..\..\vendor\bc\lib;..\..\..\vendor\pharlap\lib;..\..\..\vendor\zinc\lib;..\..\lib

!if $d(DEBUG)
CC_OPTS= +..\util.cfg -v
LINK_OPTS=/c /x /C /v $(LINK_LIBPATH)
ASM_OPTS=/mx /zi
!else
CC_OPTS= +..\util.cfg
LINK_OPTS=/c /x /C $(LINK_LIBPATH)
ASM_OPTS=/mx
!endif

# Borland C++ 3.1 startup module for Pharlap 286 protected mode.
# Full path needed because TLINK's /L applies only to .lib, not .obj.
STARTUP_OBJS=..\..\..\vendor\pharlap\lib\c0pl.obj
LIBS=phapi d16_zil d16_gfx bc_16gfx emu286 emu mathl bcl286 ptclassl pbidsl
DEF_FILE=..\util.def

#-------------------------------------------------------------------------------

MODULE_1=cfg
MODULE_2=ph_eng
MODULE_3=ph_place
MODULE_4=ph_tar
MODULE_5=ph_util
MODULE_6=st_util
MODULE_7=parser
MODULE_8=traceinf
MODULE_9=filehdr
MODULE_10=info

OBJS=$(APP).$(OBJ_EXT)      \
     $(OBJ_DIR)\$(MODULE_1).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_2).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_3).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_4).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_5).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_6).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_7).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_8).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_9).$(OBJ_EXT) \
     $(OBJ_DIR)\$(MODULE_10).$(OBJ_EXT)

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

# Source files moved into per-subsystem subdirs (ph/, etc.) post-2003.
# cfg.cpp and st_util.cpp stay at src\ root; the rest live under src\ph\.
$(OBJ_DIR)\$(MODULE_1).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_1).cpp
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_1).$(OBJ_EXT) $(SRC_DIR)\$(MODULE_1).cpp

$(OBJ_DIR)\$(MODULE_2).$(OBJ_EXT): $(SRC_DIR)\ph\$(MODULE_2).cpp
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_2).$(OBJ_EXT) $(SRC_DIR)\ph\$(MODULE_2).cpp

$(OBJ_DIR)\$(MODULE_3).$(OBJ_EXT): $(SRC_DIR)\ph\$(MODULE_3).cpp
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_3).$(OBJ_EXT) $(SRC_DIR)\ph\$(MODULE_3).cpp

$(OBJ_DIR)\$(MODULE_4).$(OBJ_EXT): $(SRC_DIR)\ph\$(MODULE_4).cpp
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_4).$(OBJ_EXT) $(SRC_DIR)\ph\$(MODULE_4).cpp

$(OBJ_DIR)\$(MODULE_5).$(OBJ_EXT): $(SRC_DIR)\ph\$(MODULE_5).cpp
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_5).$(OBJ_EXT) $(SRC_DIR)\ph\$(MODULE_5).cpp

$(OBJ_DIR)\$(MODULE_6).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_6).cpp
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_6).$(OBJ_EXT) $(SRC_DIR)\$(MODULE_6).cpp

$(OBJ_DIR)\$(MODULE_7).$(OBJ_EXT): $(SRC_DIR)\ph\$(MODULE_7).cpp
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_7).$(OBJ_EXT) $(SRC_DIR)\ph\$(MODULE_7).cpp

$(OBJ_DIR)\$(MODULE_8).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_8).cpp
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_8).$(OBJ_EXT) $(SRC_DIR)\$(MODULE_8).cpp

$(OBJ_DIR)\$(MODULE_9).$(OBJ_EXT): $(SRC_DIR)\db\$(MODULE_9).cpp
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_9).$(OBJ_EXT) $(SRC_DIR)\db\$(MODULE_9).cpp

$(OBJ_DIR)\$(MODULE_10).$(OBJ_EXT): $(SRC_DIR)\$(MODULE_10).cpp
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$(MODULE_10).$(OBJ_EXT) $(SRC_DIR)\$(MODULE_10).cpp

#-------------------------------------------------------------------------------


