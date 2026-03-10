PROG = bio

CC = CC
LINK = CC
CPP_EXTENSION = .cpp
CC_FLAGS = 
LINK_FLAGS = 
ZINC_INC_DIR = ../../include
ZINC_LIB_NAME = _mtf_zil
ZINC_LIB_DIR = ../../lib
ZINC_LIB_PATH = $(ZINC_LIB_DIR)/lib$(ZINC_LIB_NAME).a
INCLUDE_DIR = -I/usr/include/X11R4 -I/usr/include/Motif1.1
LIBS = 
LIB_DIRS = -L/usr/lib/Motif1.1 -L/usr/lib/X11R4
Z_INCLUDE_DIR = -I$(ZINC_INC_DIR) $(INCLUDE_DIR)
X_LIBS = -lXm -lXt -lX11
Z_LIBS = -l$(ZINC_LIB_NAME) $(X_LIBS) $(LIBS)
Z_LIB_DIRS = -L$(ZINC_LIB_DIR) $(LIB_DIRS)
OBJS = $(PROG).o

.SUFFIXES:
.SUFFIXES: .o $(CPP_EXTENSION)

$(CPP_EXTENSION).o:
	$(CC) $(CC_FLAGS) $(Z_INCLUDE_DIR) -c $<

motif: $(PROG)

$(PROG): $(OBJS) $(ZINC_LIB_PATH)
	$(LINK) $(LINK_FLAGS) -o $@ $(OBJS) $(Z_LIB_DIRS) $(Z_LIBS)

clean:
	-rm -f $(PROG) *.o
