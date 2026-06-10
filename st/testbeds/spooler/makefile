#
# MAKEFILE
#

.autodepend
.swap

APP=st

EXE_FILE=$(APP).exe
HELP_FILE=help.dat
RES_FILE=res.dat

OBJ_EXT=obj

SRC_DIR=source
OBJ_DIR=obj
BIN_DIR=bin
INC_DIR=include
DOC_DIR=doc

.PATH.cpp=$(SRC_DIR)
.PATH.asm=$(SRC_DIR)
.PATH.obj=$(OBJ_DIR)
.PATH.txt=$(DOC_DIR)
.PATH.doc=$(DOC_DIR)

CC=bcc286
ASM=tasm
LINK=tlink
BIND=bind286
RTK_DIR=..\..\..\vendor\pharlap\bin

# DEMO_DOS passed via make invocation (see makedemo_dos.bat).
IS_DEMO=
!if $d(DEMO_DOS)
IS_DEMO=1
!endif

!if $d(AUTO)
AUTO=__AUTO__             # kind of simula
!else
AUTO=
!endif

!if $d(EDA)
EDA=__EDA__             # it's for EDA !
!else
EDA=
!endif

!if $d(DEBUG)
CC_OPTS= +$(APP).cfg -D$(EDA);$(AUTO) -v
LINK_OPTS=/c /C /x /v
ASM_OPTS=/mx /zi
!else
CC_OPTS= +$(APP).cfg -D$(EDA);$(AUTO)
LINK_OPTS=/c /C /x
ASM_OPTS=/mx
!endif

STARTUP_OBJS=c0pl.obj
LIBS=phapi d16_zil d16_gfx bc_16gfx emu286 emu mathl bcl286 ptclassl pbidsl
DEF_FILE=$(APP).def

# --- [ OBJS ] ---------------------------------------------------------------
OBJS=$(OBJ_DIR)\$(APP).$(OBJ_EXT)      \
     $(OBJ_DIR)\calc.$(OBJ_EXT)        \
     $(OBJ_DIR)\calendar.$(OBJ_EXT)    \
     $(OBJ_DIR)\cfg.$(OBJ_EXT)         \
     $(OBJ_DIR)\control.$(OBJ_EXT)     \
     $(OBJ_DIR)\cstr.$(OBJ_EXT)        \
     $(OBJ_DIR)\ctrl_ev.$(OBJ_EXT)     \
     $(OBJ_DIR)\ctrl_pr.$(OBJ_EXT)     \
     $(OBJ_DIR)\ct_pr_nr.$(OBJ_EXT)    \
     $(OBJ_DIR)\ct_pr_sr.$(OBJ_EXT)    \
     $(OBJ_DIR)\ct_pr_st.$(OBJ_EXT)    \
     $(OBJ_DIR)\ctrl_rf.$(OBJ_EXT)     \
     $(OBJ_DIR)\db_eng.$(OBJ_EXT)      \
     $(OBJ_DIR)\db_view.$(OBJ_EXT)     \
     $(OBJ_DIR)\dongle.$(OBJ_EXT)      \
     $(OBJ_DIR)\dstatist.$(OBJ_EXT)    \
     $(OBJ_DIR)\dstorage.$(OBJ_EXT)    \
     $(OBJ_DIR)\d_ext_st.$(OBJ_EXT)    \
     $(OBJ_DIR)\mb_ext.$(OBJ_EXT)      \
     $(OBJ_DIR)\info.$(OBJ_EXT)        \
     $(OBJ_DIR)\mb_conf.$(OBJ_EXT)     \
     $(OBJ_DIR)\mb_file.$(OBJ_EXT)     \
     $(OBJ_DIR)\mb_help.$(OBJ_EXT)     \
     $(OBJ_DIR)\mb_inf.$(OBJ_EXT)      \
     $(OBJ_DIR)\mb_print.$(OBJ_EXT)    \
     $(OBJ_DIR)\ph_info.$(OBJ_EXT)     \
     $(OBJ_DIR)\plain_pr.$(OBJ_EXT)    \
     $(OBJ_DIR)\res.$(OBJ_EXT)         \
     $(OBJ_DIR)\rt_eng.$(OBJ_EXT)      \
     $(OBJ_DIR)\semaphor.$(OBJ_EXT)    \
     $(OBJ_DIR)\serial.$(OBJ_EXT)      \
     $(OBJ_DIR)\sid.$(OBJ_EXT)         \
     $(OBJ_DIR)\spooler.$(OBJ_EXT)     \
     $(OBJ_DIR)\ssaver.$(OBJ_EXT)      \
     $(OBJ_DIR)\stm2.$(OBJ_EXT)        \
     $(OBJ_DIR)\tb_conf.$(OBJ_EXT)     \
     $(OBJ_DIR)\tb_man.$(OBJ_EXT)      \
     $(OBJ_DIR)\tb_print.$(OBJ_EXT)    \
     $(OBJ_DIR)\tb_sp.$(OBJ_EXT)       \
     $(OBJ_DIR)\tb_tools.$(OBJ_EXT)    \
     $(OBJ_DIR)\tb_util.$(OBJ_EXT)     \
     $(OBJ_DIR)\util.$(OBJ_EXT)        \
     $(OBJ_DIR)\view.$(OBJ_EXT)        \
     $(OBJ_DIR)\view_ev.$(OBJ_EXT)     \
     $(OBJ_DIR)\view_hb.$(OBJ_EXT)     \
     $(OBJ_DIR)\w_phone.$(OBJ_EXT)     \
     $(OBJ_DIR)\w_sgroup.$(OBJ_EXT)    \
     $(OBJ_DIR)\w_statbr.$(OBJ_EXT)    \
     $(OBJ_DIR)\w_table.$(OBJ_EXT)

.cpp.$(OBJ_EXT):
    $(CC) $(CC_OPTS) -o$(OBJ_DIR)\$&.$(OBJ_EXT) {$< }

.asm.$(OBJ_EXT):
    $(ASM) $(ASM_OPTS) {$< }, $(OBJ_DIR)\$&.$(OBJ_EXT)

$(BIN_DIR)\$(EXE_FILE): $(BIN_DIR)\$(HELP_FILE) $(OBJS)
    $(LINK) $(LINK_OPTS) @&&!
$(STARTUP_OBJS) $(OBJS)
$*, ,$(LIBS), $(DEF_FILE)
!
!if $d(RUN)
    $(BIND) $(RTK_DIR)\run286b $(BIN_DIR)\$(EXE_FILE) -dll $(RTK_DIR)\doscalls $(RTK_DIR)\int33
    cfig286 $(BIN_DIR)\$(EXE_FILE) -NISTACK 10 -LDTSIZE 4096
!endif

!if $d(HELP)
$(BIN_DIR)\$(HELP_FILE): $(DOC_DIR)\help.txt
    @genhelp $(DOC_DIR)\help.txt $(HELP_FILE) > NUL
    @move $(HELP_FILE)  $(BIN_DIR) > NUL
    @move help.hpp $(INC_DIR) > NUL
!endif

