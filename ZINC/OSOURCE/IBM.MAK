# ----- General Definitions -------------------------------------------------
VERSION=ibm210
# font can be OEM or ISO
FONT=ISO

# ----- OS/2 compiler options -----------------------------------------------
OS2_CPP=icc
OS2_LINK=link386
OS2_LIBRARIAN=lib
OS2_RC=rc

OS2_CPP_OPTS=-c -D__OS2__ -D$(FONT)
OS2_LINK_OPTS=/BASE:0x10000 /PM:PM
#OS2_CPP_OPTS=/C /Ti /D__OS2__ /D$(FONT)
#OS2_LINK_OPTS=/BASE:0x10000 /PM:PM /DE /NOE
OS2_LIB_OPTS=
OS2_RC_OPTS=

OS2_OBJS=
OS2_LIBS=os2_zil.lib

.SUFFIXES : .cpp

.cpp.obj:
	$(OS2_CPP) $(OS2_CPP_OPTS) $<

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To generate the library modules for specific environments type:
	@echo nmake -f ibm.mak os2
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean.cmd

# ----- Copy files ----------------------------------------------------------
copy :
	copy ui_*.hpp ..\include
	copy z_clean.* ..\bin

# ----- DOS -----------------------------------------------------------------
dos:
	@echo IBM does not support DOS libraries and programs.

# ----- Windows -------------------------------------------------------------
windows:
	@echo IBM does not support Windows libraries and programs.

# ----- OS/2 ----------------------------------------------------------------

os2: copy otest.exe

otest.exe: test.obj os2_zil.lib
	$(OS2_LINK) $(OS2_LINK_OPTS) @<<zil.rsp
$(OS2_OBJS)+test.obj
$*,NUL,$(OS2_LIBS),otest.def
<<
	$(OS2_RC) $(OS2_RC_OPTS) otest.rc $*.exe

os2_zil.lib : o_dsp.obj o_keybrd.obj o_mouse.obj o_cursor.obj o_event.obj \
	o_border.obj o_button.obj o_combo.obj o_group.obj o_hlist.obj \
	o_icon.obj o_min.obj o_max.obj o_scroll.obj o_string.obj o_sys.obj \
	o_text.obj o_title.obj o_popup.obj o_popup1.obj o_plldn.obj \
	o_plldn1.obj o_prompt.obj o_tbar.obj o_vlist.obj \
	o_win.obj o_win1.obj o_win2.obj \
	o_error.obj o_intl.obj\
	z_list.obj z_list1.obj \
	z_intl.obj z_date.obj z_time.obj z_bnum.obj z_bnum1.obj \
	z_region.obj z_dsp.obj z_path.obj z_store.obj \
	z_device.obj z_cursor.obj z_event.obj z_map1.obj z_map2.obj \
	z_border.obj z_button.obj z_combo.obj z_date1.obj z_fmtstr.obj \
	z_group.obj z_hlist.obj z_icon.obj z_int.obj z_min.obj z_max.obj \
	z_bnum2.obj z_scroll.obj z_string.obj z_sys.obj z_text.obj z_time1.obj \
	z_title.obj z_popup.obj z_popup1.obj z_plldn.obj z_plldn1.obj \
	z_prompt.obj z_tbar.obj z_real.obj z_vlist.obj \
	z_win.obj z_win1.obj z_win2.obj z_utils.obj \
	z_error.obj z_help.obj z_app.obj \
	g_bnum.obj g_border.obj g_date.obj g_date1.obj \
	g_device.obj g_dsp.obj g_event.obj g_hotkey.obj \
	g_jump.obj g_perror.obj g_phelp.obj g_pnorm.obj g_store.obj \
	g_time.obj g_time1.obj g_win1.obj g_win2.obj \
	g_sys.obj g_int.obj g_real.obj
	-@del os2_zil.lib
	$(OS2_LIBRARIAN) $(OS2_LIB_OPTS) @<<zil.rsp
$* &
+o_dsp.obj+o_keybrd.obj+o_mouse.obj+o_cursor.obj+o_event.obj &
+o_border.obj+o_button.obj+o_combo.obj+o_group.obj+o_hlist.obj &
+o_icon.obj+o_min.obj+o_max.obj+o_scroll.obj+o_string.obj+o_sys.obj &
+o_text.obj+o_title.obj+o_popup.obj+o_popup1.obj+o_plldn.obj &
+o_plldn1.obj+o_prompt.obj+o_tbar.obj+o_vlist.obj &
+o_win.obj+o_win1.obj+o_win2.obj &
+o_error.obj+o_intl.obj &
+z_list.obj+z_list1.obj &
+z_intl.obj+z_date.obj+z_time.obj+z_bnum.obj+z_bnum1.obj &
+z_region.obj+z_dsp.obj+z_path.obj+z_store.obj &
+z_device.obj+z_cursor.obj+z_event.obj+z_map1.obj+z_map2.obj &
+z_border.obj+z_button.obj+z_combo.obj+z_date1.obj+z_fmtstr.obj &
+z_group.obj+z_hlist.obj+z_icon.obj+z_int.obj+z_min.obj+z_max.obj &
+z_bnum2.obj+z_scroll.obj+z_string.obj+z_sys.obj+z_text.obj+z_time1.obj &
+z_title.obj+z_popup.obj+z_popup1.obj+z_plldn.obj+z_plldn1.obj &
+z_prompt.obj+z_tbar.obj+z_real.obj+z_vlist.obj &
+z_win.obj+z_win1.obj+z_win2.obj+z_utils.obj &
+z_error.obj+z_help.obj+z_app.obj &
+g_bnum.obj+g_border.obj+g_date.obj+g_date1.obj &
+g_device.obj+g_dsp.obj+g_event.obj+g_hotkey.obj &
+g_jump.obj+g_perror.obj+g_phelp.obj+g_pnorm.obj+g_store.obj &
+g_time.obj+g_time1.obj+g_win1.obj+g_win2.obj &
+g_sys.obj+g_int.obj+g_real.obj
NUL
<<
	copy os2_zil.lib ..\lib\$(VERSION)

