PROG = design
CC = CC
LINK = CC
CPP_EXTENSION = .cpp
CC_FLAGS = 
LINK_FLAGS = 
ZINC_INC_DIR = ../../include
ZINC_LIB_NAME = _mtf_zil
ZINC_LIB_DIR = ../../lib
INCLUDE_DIR = -I/usr/include/X11R4 -I/usr/include/Motif1.1
ZINC_LIB_PATH = $(ZINC_LIB_DIR)/lib$(ZINC_LIB_NAME).a
LIBS = 
LIB_DIRS = -L/usr/lib/Motif1.1 -L/usr/lib/X11R4
BIN_INSTALL = /usr/local/bin
Z_INCLUDE_DIR = -I$(ZINC_INC_DIR) $(INCLUDE_DIR)
X_LIBS = -lXm -lXt -lX11
Z_LIBS = -l$(ZINC_LIB_NAME) $(X_LIBS) $(LIBS)
Z_LIB_DIRS = -L$(ZINC_LIB_DIR) $(LIB_DIRS)

.SUFFIXES:
.SUFFIXES: .o $(CPP_EXTENSION)

$(CPP_EXTENSION).o:
	$(CC) $(CC_FLAGS) $(Z_INCLUDE_DIR) -c $<

OBJS = m_bitobj.o m_object.o z_advanc.o \
	z_bitobj.o z_scombo.o z_config.o z_ctrl.o z_design.o z_dialog.o \
	z_cerror.o z_file.o z_flag.o z_frame.o z_help1.o z_helped.o z_hlpbar.o \
	z_hlpmsg.o z_image.o z_image1.o z_import.o z_jump.o z_objlst.o \
	z_object.o z_resrc.o z_util.o 

motif: $(PROG) 

$(PROG): $(OBJS) $(ZINC_LIB_PATH)
	$(LINK) $(LINK_FLAGS) -o $@ $(OBJS) $(Z_LIB_DIRS) $(Z_LIBS)

install: motif
	cp design $(BIN_INSTALL)
	cp p_design.znc $(BIN_INSTALL)

clean:
	-rm -f $(PROG) *.o
