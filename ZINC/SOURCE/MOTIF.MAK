CC = CC
LINK = CC
CPP_EXTENSION = .cpp
CC_FLAGS = 
LINK_FLAGS = 
ZINC_INC_DIR = ../include
ZINC_LIB_NAME = _mtf_zil
ZINC_LIB_DIR = ../lib
ZINC_LIB_PATH = $(ZINC_LIB_DIR)/lib$(ZINC_LIB_NAME).a
INCLUDE_DIR = -I/usr/include/X11R4 -I/usr/include/Motif1.1
LIBS = 
LIB_DIRS = -L/usr/lib/Motif1.1 -L/usr/lib/X11R4
LIB_INSTALL = /usr/lib
Z_INCLUDE_DIR = -I$(ZINC_INC_DIR) $(INCLUDE_DIR)
X_LIBS = -lXm -lXt -lX11
Z_LIBS = -l$(ZINC_LIB_NAME) $(X_LIBS) $(LIBS)
Z_LIB_DIRS = -L$(ZINC_LIB_DIR) $(LIB_DIRS)
INCLUDE_INSTALL = /usr/include

.SUFFIXES:
.SUFFIXES: .o $(CPP_EXTENSION)

$(CPP_EXTENSION).o:
	$(CC) $(CC_FLAGS) $(Z_INCLUDE_DIR) -c $<

LIBRARIAN = ar
UNIX_LIB_OPTS = r

## Library name:
LIB_NAME = $(ZINC_LIB_DIR)/lib$(ZINC_LIB_NAME).a

## Include files:
INCLUDE_FILES = $(ZINC_INC_DIR)/ui_env.hpp $(ZINC_INC_DIR)/ui_gen.hpp \
	$(ZINC_INC_DIR)/ui_dsp.hpp $(ZINC_INC_DIR)/ui_evt.hpp \
	$(ZINC_INC_DIR)/ui_win.hpp

all: motif

lib: $(LIB_NAME)


$(LIB_NAME): \
	g_bnum.o g_border.o g_date.o g_date1.o g_device.o \
	g_dsp.o g_event.o g_hotkey.o g_jump.o \
	g_perror.o g_phelp.o g_pnorm.o g_store.o g_time.o g_time1.o \
	g_win1.o g_win2.o g_sys.o g_int.o g_real.o \
	m_bitmap.o m_border.o m_button.o m_combo.o m_cursor.o m_dsp.o \
	m_error.o m_event.o m_group.o m_hlist.o m_icon.o m_intl.o m_keybrd.o \
	m_max.o m_min.o m_mouse.o m_plldn.o m_plldn1.o m_popup.o \
	m_popup1.o m_prompt.o m_scroll.o m_string.o m_sys.o m_tbar.o \
	m_text.o m_title.o m_vlist.o m_win.o m_win1.o m_win2.o \
	z_app.o z_bnum.o z_bnum1.o z_bnum2.o z_border.o z_button.o z_combo.o \
	z_cursor.o z_date.o z_date1.o z_device.o z_dsp.o z_error.o z_event.o \
	z_fmtstr.o z_group.o z_help.o z_hlist.o z_icon.o z_int.o \
	z_intl.o z_list.o z_list1.o z_map1.o z_map2.o z_max.o z_min.o \
	z_path.o z_plldn.o z_plldn1.o z_popup.o z_popup1.o z_prompt.o \
	z_real.o z_region.o z_scroll.o z_store.o z_string.o z_sys.o \
	z_tbar.o z_text.o z_time.o z_time1.o z_title.o z_vlist.o \
	z_win.o z_win1.o z_win2.o z_utils.o
	$(LIBRARIAN) $(UNIX_LIB_OPTS) $(LIB_NAME) $?
	-if test -s /bin/ranlib; then /bin/ranlib $(LIB_NAME); else exit 0; fi
	-if test -s /usr/bin/ranlib; then /usr/bin/ranlib $(LIB_NAME); else exit 0; fi


test: test.o $(LIB_NAME)
	$(LINK) $(LINK_FLAGS) -o $@ $? $(Z_LIB_DIRS) $(Z_LIBS)

motif:	$(INCLUDE_FILES) $(LIB_NAME)

$(INCLUDE_FILES):$$(@F)
	-rm -f $@
	cp $? $@
	chmod 644 $@

install: motif
	cp *.hpp $(INCLUDE_INSTALL)
	cp $(LIB_NAME) $(LIB_INSTALL)

clean:
	rm -f $(LIB_NAME) *.o test

###
g_bnum.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_border.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_cursor.o: ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_date.o: ui_gen.hpp ui_env.hpp
g_date1.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_device.o: ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_dsp.o: ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_event.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_fgdsp.o: ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_hotkey.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_int.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_jump.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_keybrd.o: ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_mscdsp.o: ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_perror.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_phelp.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_pnorm.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_real.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_store.o: ui_gen.hpp ui_env.hpp
g_sys.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_time.o: ui_gen.hpp ui_env.hpp
g_time1.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_win1.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_win2.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
g_windsp.o: ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_bitmap.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_border.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_button.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_combo.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_cursor.o: ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_dsp.o: ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_error.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_event.o: ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_group.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_hlist.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_icon.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_keybrd.o: ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_max.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_min.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_mouse.o: ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_plldn.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_plldn1.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_popup.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_popup1.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_prompt.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_scroll.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_string.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_sys.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_tbar.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_text.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_title.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_vlist.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_win.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_win1.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
m_win2.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_bnum.o: ui_gen.hpp ui_env.hpp
z_bnum1.o: ui_gen.hpp ui_env.hpp
z_bnum2.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_border.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_button.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_combo.o:  ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_cursor.o: ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_date.o: ui_gen.hpp ui_env.hpp
z_date1.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_device.o: ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_dsp.o: ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_error.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_event.o: ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_fmtstr.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_group.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_help.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_hlist.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_icon.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_int.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_intl.o: ui_gen.hpp ui_env.hpp
z_list.o: ui_gen.hpp ui_env.hpp
z_list1.o: ui_gen.hpp ui_env.hpp
z_map1.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_map2.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_max.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_min.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_path.o: ui_gen.hpp ui_env.hpp
z_plldn.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_plldn1.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_popup.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_popup1.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_prompt.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_real.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_region.o: ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_scroll.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_store.o: ui_gen.hpp ui_env.hpp
z_string.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_sys.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_tbar.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_text.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_time.o: ui_gen.hpp ui_env.hpp
z_time1.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_title.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_utils.o: ui_gen.hpp ui_env.hpp
z_vlist.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_win.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_win1.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
z_win2.o: ui_win.hpp ui_evt.hpp ui_dsp.hpp ui_gen.hpp ui_env.hpp
