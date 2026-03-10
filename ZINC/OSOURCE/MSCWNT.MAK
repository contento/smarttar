# ----- General Definitions -------------------------------------------------
!include <ntwin32.mak>
VERSION=mscpp700

# ----- Windows compiler options --------------------------------------------
WIN_CPP=$(cc)
WIN_LINK=$(link)
WIN_LIBRARIAN=lib32

WIN_CPP_OPTS=$(cflags) $(cvars)
#WIN_CPP_OPTS=$(cflags) $(cvars) /Zi
#WIN_LINK_OPTS=$(conflags)
WIN_LINK_OPTS=$(guiflags)
#WIN_LINK_OPTS=$(guiflags) /DEBUG:MAPPED,FULL /DEBUGTYPE:CV
#WIN_LIB_OPTS=/machine:i386 /subsystem:CONSOLE
WIN_LIB_OPTS=/machine:i386 /subsystem:WINDOWS

WIN_OBJS=
#WIN_LIBS=$(conlibs) wnt_zil.lib
WIN_LIBS=$(guilibs) wnt_zil.lib

.SUFFIXES : .cpp

.cpp.obn:
	$(WIN_CPP) $(WIN_CPP_OPTS) -Fo$*.obn $<

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To generate the library modules for specific environments type:
	@echo nmake -fmicrosft.mak dos
	@echo nmake -fmicrosft.mak dos16
	@echo (Microsoft does not support OS/2)
	@echo nmake -fmicrosft.mak windows
	@echo nmake -f mscwnt.mak winnt
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean.bat

# ----- Copy files ----------------------------------------------------------
copy :
	copy ui_*.hpp ..\include
	copy z_clean.* ..\bin

# ----- Windows NT Libraries and Programs -----------------------------------
winnt: copy ntest.exe

ntest.exe: test.obn wnt_zil.lib
	$(WIN_LINK) $(WIN_LINK_OPTS) -out:ntest.exe $(WIN_OBJS) test.obn $(WIN_LIBS)

wnt_zil.lib : w_dsp.obn w_keybrd.obn w_mouse.obn w_cursor.obn w_event.obn \
	w_border.obn w_button.obn w_combo.obn w_group.obn w_hlist.obn \
	w_icon.obn w_min.obn w_max.obn w_scroll.obn w_string.obn w_sys.obn \
	w_text.obn w_title.obn w_popup.obn w_popup1.obn w_plldn.obn \
	w_plldn1.obn w_prompt.obn w_tbar.obn w_vlist.obn \
	w_win.obn w_win1.obn n_win2.obn \
	w_error.obn n_intl.obn\
	z_list.obn z_list1.obn \
	z_utils.obn z_intl.obn z_date.obn z_time.obn z_bnum.obn z_bnum1.obn \
	z_region.obn z_dsp.obn z_path.obn z_store.obn \
	z_device.obn z_cursor.obn z_event.obn z_map1.obn z_map2.obn \
	z_border.obn z_button.obn z_combo.obn z_date1.obn z_fmtstr.obn \
	z_group.obn z_hlist.obn z_icon.obn z_int.obn z_min.obn z_max.obn \
	z_bnum2.obn z_scroll.obn z_string.obn z_sys.obn z_text.obn z_time1.obn \
	z_title.obn z_popup.obn z_popup1.obn z_plldn.obn z_plldn1.obn \
	z_prompt.obn z_tbar.obn z_real.obn z_vlist.obn \
	z_win.obn z_win1.obn z_win2.obn \
	z_error.obn z_help.obn z_app.obn \
	g_bnum.obn g_border.obn g_date.obn g_date1.obn \
	g_device.obn g_dsp.obn g_event.obn g_hotkey.obn \
	g_jump.obn g_perror.obn g_phelp.obn g_pnorm.obn g_store.obn \
	g_time.obn g_time1.obn g_win1.obn g_win2.obn g_windsp.obn \
	g_sys.obn g_int.obn g_real.obn \
	i_mapfon.obn
	-@del wnt_zil.lib
	$(WIN_LIBRARIAN) /out:wnt_zil.lib $(WIN_LIB_OPTS) @<<zil.rsp
w_dsp.obn w_keybrd.obn w_mouse.obn w_cursor.obn w_event.obn
w_border.obn w_button.obn w_combo.obn w_group.obn w_hlist.obn
w_icon.obn w_min.obn w_max.obn w_scroll.obn w_string.obn w_sys.obn
w_text.obn w_title.obn w_popup.obn w_popup1.obn w_plldn.obn
w_plldn1.obn w_prompt.obn w_tbar.obn w_vlist.obn
w_win.obn w_win1.obn n_win2.obn
w_error.obn n_intl.obn
z_list.obn z_list1.obn
z_utils.obn z_intl.obn z_date.obn z_time.obn z_bnum.obn z_bnum1.obn
z_region.obn z_dsp.obn z_path.obn z_store.obn
z_device.obn z_cursor.obn z_event.obn z_map1.obn z_map2.obn
z_border.obn z_button.obn z_combo.obn z_date1.obn z_fmtstr.obn
z_group.obn z_hlist.obn z_icon.obn z_int.obn z_min.obn z_max.obn
z_bnum2.obn z_scroll.obn z_string.obn z_sys.obn z_text.obn z_time1.obn
z_title.obn z_popup.obn z_popup1.obn z_plldn.obn z_plldn1.obn
z_prompt.obn z_tbar.obn z_real.obn z_vlist.obn
z_win.obn z_win1.obn z_win2.obn
z_error.obn z_help.obn z_app.obn 
g_bnum.obn g_border.obn g_date.obn g_date1.obn
g_device.obn g_dsp.obn g_event.obn g_hotkey.obn
g_jump.obn g_perror.obn g_phelp.obn g_pnorm.obn g_store.obn
g_time.obn g_time1.obn g_win1.obn g_win2.obn g_windsp.obn
g_sys.obn g_int.obn g_real.obn
i_mapfon.obn
<<
	copy wnt_zil.lib ..\lib\$(VERSION)

