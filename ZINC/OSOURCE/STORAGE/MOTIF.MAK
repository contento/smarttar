PROG = chkfile gc cppath rrmdir zdump browse 

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
NO_X_LIBS = -l$(ZINC_LIB_NAME) $(LIBS)
Z_LIB_DIRS = -L$(ZINC_LIB_DIR) $(LIB_DIRS)
RS6000 = -qlanglvl=compat
SYSTEM = $(HP)

.SUFFIXES:
.SUFFIXES: .o $(CPP_EXTENSION)

$(CPP_EXTENSION).o:
	$(CC) $(CC_FLAGS) $(Z_INCLUDE_DIR) -c $<

motif: chkfile cppath gc rrmdir zdump browse 

chkfile: chkfile.o $(ZINC_LIB_PATH)
	$(LINK) $(LINK_FLAGS) -o chkfile chkfile.o $(Z_LIB_DIRS) $(SYSTEM) $(NO_X_LIBS)

cppath: cppath.o $(ZINC_LIB_PATH)
	$(LINK) $(LINK_FLAGS) -o cppath cppath.o $(Z_LIB_DIRS) $(SYSTEM) $(NO_X_LIBS)

gc: gc.o $(ZINC_LIB_PATH)
	$(LINK) $(LINK_FLAGS) -o gc gc.o $(Z_LIB_DIRS) $(SYSTEM) $(NO_X_LIBS)

rrmdir: rrmdir.o $(ZINC_LIB_PATH)
	$(LINK) $(LINK_FLAGS) -o rrmdir rrmdir.o $(Z_LIB_DIRS) $(SYSTEM) $(NO_X_LIBS)

zdump: zdump.o $(ZINC_LIB_PATH)
	$(LINK) $(LINK_FLAGS) -o zdump zdump.o $(Z_LIB_DIRS) $(SYSTEM) $(NO_X_LIBS)

browse: browse.o $(ZINC_LIB_PATH)
	$(LINK) $(LINK_FLAGS) -o browse browse.o $(Z_LIB_DIRS) $(SYSTEM) $(NO_X_LIBS)

gc.o: cppath$(CPP_EXTENSION) $(ZINC_LIB_PATH)
	-mv cppath.o cppath.SAV
	$(CC) -c -DDOGC $(CC_FLAGS) -I.. $(Z_INCLUDE_DIR) cppath$(CPP_EXTENSION)
	mv cppath.o gc.o
	-mv cppath.SAV cppath.o

clean:
	-rm -f $(PROG) *.o

chkfile.o: chkfile$(CPP_EXTENSION)
	$(CC) $(CC_FLAGS) -DZ_STORE=\"z_store$(CPP_EXTENSION)\" -I.. $(Z_INCLUDE_DIR) -c chkfile$(CPP_EXTENSION) 
