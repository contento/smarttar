# ----- General Definitions -------------------------------------------------
VERSION=btcpp310
# font can be OEM or ISO
FONT=ISO

# ----- DOS compiler options ------------------------------------------------
DOS_CPP=bcc
DOS_LINK=tlink
DOS_LIBRARIAN=tlib

DOS_CPP_OPTS=-c -ml -O1 -V -Y -w -H=ZIL.SYM -D$(FONT)
DOS_LINK_OPTS=/c /x
#DOS_CPP_OPTS=-c -ml -O1 -V -Y -w -v -H=ZIL.SYM -D$(FONT)
#DOS_LINK_OPTS=/c /x /v
DOS_LIB_OPTS=/C
DOS_OBJS=c0l

# --- Use the next line for UI_GRAPHICS_DISPLAY ---
DOS_LIBS=dos_zil dos_gfx bc_lgfx emu mathl cl
# --- Use the next line for UI_BGI_DISPLAY ---
#DOS_LIBS=dos_zil dos_bgi graphics emu mathl cl

.cpp.obj:
	$(DOS_CPP) $(DOS_CPP_OPTS) {$< }

# ----- Windows compiler options --------------------------------------------
WIN_CPP=bcc
WIN_LINK=tlink
WIN_LIBRARIAN=tlib
WIN_RC=rc

# --- Use the next 2 lines for Windows 3.0 compatibility ---
WIN_CPP_OPTS=-c -ml -O1 -Vf -WE -w -DWINVER=0x0300 -H=ZIL.SYM
WIN_RC_OPTS=-k -30
# --- Use the next 2 lines for Windows 3.1 compatibility ---
#WIN_CPP_OPTS=-c -ml -O1 -Vf -WE -w -H=ZIL.SYM
#WIN_RC_OPTS=-k
WIN_LINK_OPTS=/c /C /Twe /x
#WIN_CPP_OPTS=-c -ml -O1 -Vf -WE -w -v -H=ZIL.SYM
#WIN_LINK_OPTS=/c /C /Twe /x /v
WIN_LIB_OPTS=/C

WIN_OBJS=c0wl
WIN_LIBS=win_zil import mathwl cwl

.cpp.obw:
	$(WIN_CPP) $(WIN_CPP_OPTS) -o$*.obw {$< }

# ----- Windows NT compiler options -----------------------------------------
WNT_CPP=bcc32
WNT_LINK=tlink32
WNT_LIBRARIAN=tlib32
WNT_RC=brc32

WNT_CPP_OPTS=-c -W
WNT_LINK_OPTS=-Tpe -aa -x -c
WNT_RC_OPTS=-w32
WNT_LIB_OPTS=/C

WNT_OBJS=c0w32
WNT_LIBS=wnt_zil imprtw32 import32 cw32

.cpp.obn:
	$(WNT_CPP) $(WNT_CPP_OPTS) -o$*.obn {$< }

# ----- OS/2 compiler options -----------------------------------------------
OS2_CPP=bcc
OS2_LINK=tlink
OS2_LIBRARIAN=tlib
OS2_RC=rc

OS2_CPP_OPTS=-c -H=ZIL.SYM -D$(FONT)
OS2_LINK_OPTS=/c /B:0x10000 /aa
#OS2_CPP_OPTS=-c -H=ZIL.SYM -v -D$(FONT)
#OS2_LINK_OPTS=/c /B:0x10000 /v /aa
OS2_LIB_OPTS=
OS2_RC_OPTS=

OS2_OBJS=c02.obj
OS2_LIBS=os2_zil.lib os2.lib c2.lib

.cpp.obo:
	$(OS2_CPP) $(OS2_CPP_OPTS) -o$*.obo {$< }

# --- Use the next line for PharLap Extended programs. ---
D16_CPP=bcc286
D16_LINK=tlink
D16_LIBRARIAN=tlib

D16_CPP_OPTS=-c -ml -O1 -V -w -H=ZIL.SYM -D$(FONT)
D16_LINK_OPTS=/c /x /C
D16_LIB_OPTS=/C

D16_OBJS=c0pl.obj
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
D16_LIBS=phapi d16_zil d16_gfx bc_16gfx emu286 emu mathl bcl286
# --- Use the next line for UI_BGI_DISPLAY ---
#D16_LIBS=phapi d16_zil d16_bgi graph286 emu286 mathl bcl286

.cpp.o16:
	$(D16_CPP) $(D16_CPP_OPTS) -o$*.o16 {$< }

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To generate the library modules for specific environments type:
	@echo make -fborland.mak dos
	@echo make -fborland.mak dos16
	@echo make -fborland.mak os2
	@echo make -fborland.mak windows
	@echo make -fborland.mak winnt
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean

# ----- Copy files ----------------------------------------------------------
copy :
	copy ui_*.hpp ..\include
	copy z_clean.* ..\bin
	copy gfx\source\gfx.h ..\include
	copy gfx\source\gfx_pro.h ..\include

# ----- DOS -----------------------------------------------------------------

dos: copy dos_bgi.lib dos_gfx.lib test.exe

test.exe: test.obj dos_zil.lib 
	$(DOS_LINK) $(DOS_LINK_OPTS) @&&!
$(DOS_OBJS)+test.obj
$*, , $(DOS_LIBS)
!
	-@del zil.sym

dos_zil.lib : d_tdsp.obj d_keybrd.obj d_mouse.obj d_cursor.obj \
	d_border.obj d_button.obj d_combo.obj d_group.obj d_hlist.obj \
	d_icon.obj d_min.obj d_max.obj d_scroll.obj d_string.obj d_sys.obj \
	d_text.obj d_title.obj d_popup.obj d_popup1.obj d_plldn.obj \
	d_plldn1.obj d_prompt.obj d_tbar.obj d_vlist.obj \
	d_win.obj d_win1.obj d_win2.obj \
	d_error.obj d_intl.obj\
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
	z_error.obj z_help.obj \
	g_bnum.obj g_border.obj g_date.obj g_date1.obj \
	g_device.obj g_dsp.obj g_event.obj g_hotkey.obj \
	g_jump.obj g_perror.obj g_phelp.obj g_pnorm.obj g_store.obj \
	g_time.obj g_time1.obj g_win1.obj g_win2.obj \
	g_sys.obj g_int.obj g_real.obj \
	i_btcat.obj i_mapfon.obj
	-@del dos_zil.lib
	$(DOS_LIBRARIAN) $(DOS_LIB_OPTS) @&&!
$* &
+d_tdsp.obj+d_keybrd.obj+d_mouse.obj+d_cursor.obj &
+d_border.obj+d_button.obj+d_combo.obj+d_group.obj &
+d_hlist.obj+d_icon.obj+d_min.obj+d_max.obj+d_scroll.obj+d_string.obj &
+d_sys.obj+d_text.obj+d_title.obj+d_popup.obj+d_popup1.obj+d_plldn.obj &
+d_plldn1.obj+d_prompt.obj+d_tbar.obj+d_vlist.obj &
+d_win.obj+d_win1.obj+d_win2.obj &
+d_error.obj+d_intl.obj &
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
+z_error.obj+z_help.obj &
+g_bnum.obj+g_border.obj+g_date.obj+g_date1.obj &
+g_device.obj+g_dsp.obj+g_event.obj+g_hotkey.obj &
+g_jump.obj+g_perror.obj+g_phelp.obj+g_pnorm.obj+g_store.obj &
+g_time.obj+g_time1.obj+g_win1.obj+g_win2.obj &
+g_sys.obj+g_int.obj+g_real.obj &
+i_btcat.obj+i_mapfon.obj
!
	@del zil.sym
	copy dos_zil.lib ..\lib\$(VERSION)

dos_bgi.lib : d_bgidsp.obj g_bgidsp.obj z_appbgi.obj
	-@del dos_bgi.lib
#	@cd bgi
#    @bgiobj /f $(FONT)_syst.chr $(FONT)_syst.obj _SystemFont
#    @bgiobj /f $(FONT)_dial.chr $(FONT)_dial.obj _DialogFont
#    @bgiobj /f $(FONT)_smal.chr $(FONT)_smal.obj _SmallFont
#	@cd ..
	$(DOS_LIBRARIAN) $(DOS_LIB_OPTS) @&&!
$* &
+d_bgidsp.obj+g_bgidsp.obj+bgi\$(FONT)_smal.obj+bgi\$(FONT)_dial.obj+bgi\$(FONT)_syst.obj+z_appbgi.obj
!
	@del zil.sym
	copy dos_bgi.lib ..\lib\$(VERSION)

dos_gfx.lib : d_gfxdsp.obj $(FONT)_smal.obj $(FONT)_dial.obj $(FONT)_syst.obj z_appgfx.obj
	-@del dos_gfx.lib
	$(DOS_LIBRARIAN) $(DOS_LIB_OPTS) @&&!
$* &
+d_gfxdsp.obj+$(FONT)_smal.obj+$(FONT)_dial.obj+$(FONT)_syst.obj+z_appgfx.obj
!
	@del zil.sym
	copy dos_gfx.lib ..\lib\$(VERSION)

bc_lgfx.lib :
	@cd gfx\source
	make -f borland.mak dos
	@cd ..\..

# ----- Windows -------------------------------------------------------------

windows: copy wtest.exe

wtest.exe: test.obw win_zil.lib 
	$(WIN_LINK) $(WIN_LINK_OPTS) @&&!
$(WIN_OBJS)+test.obw
$*, , $(WIN_LIBS), wtest.def
!
	$(WIN_RC) $(WIN_RC_OPTS) wtest.rc $<
	-@del zil.sym

win_zil.lib : w_dsp.obw w_keybrd.obw w_mouse.obw w_cursor.obw w_event.obw \
	w_border.obw w_button.obw w_combo.obw w_group.obw w_hlist.obw \
	w_icon.obw w_min.obw w_max.obw w_scroll.obw w_string.obw w_sys.obw \
	w_text.obw w_title.obw w_popup.obw w_popup1.obw w_plldn.obw \
	w_plldn1.obw w_prompt.obw w_tbar.obw w_vlist.obw \
	w_win.obw w_win1.obw w_win2.obw \
	w_error.obw w_intl.obw\
	z_list.obw z_list1.obw \
	z_intl.obw z_date.obw z_time.obw z_bnum.obw z_bnum1.obw \
	z_region.obw z_dsp.obw z_path.obw z_store.obw \
	z_device.obw z_cursor.obw z_event.obw z_map1.obw z_map2.obw \
	z_border.obw z_button.obw z_combo.obw z_date1.obw z_fmtstr.obw \
	z_group.obw z_hlist.obw z_icon.obw z_int.obw z_min.obw z_max.obw \
	z_bnum2.obw z_scroll.obw z_string.obw z_sys.obw z_text.obw z_time1.obw \
	z_title.obw z_popup.obw z_popup1.obw z_plldn.obw z_plldn1.obw \
	z_prompt.obw z_tbar.obw z_real.obw z_vlist.obw \
	z_win.obw z_win1.obw z_win2.obw z_utils.obw \
	z_error.obw z_help.obw z_app.obw \
	g_bnum.obw g_border.obw g_date.obw g_date1.obw \
	g_device.obw g_dsp.obw g_event.obw g_hotkey.obw \
	g_jump.obw g_perror.obw g_phelp.obw g_pnorm.obw g_store.obw \
	g_time.obw g_time1.obw g_win1.obw g_win2.obw g_windsp.obw \
	g_sys.obw g_int.obw g_real.obw \
	i_mapfon.obw
	-@del win_zil.lib
	$(WIN_LIBRARIAN) $(WIN_LIB_OPTS) @&&!
$* &
+w_dsp.obw+w_keybrd.obw+w_mouse.obw+w_cursor.obw+w_event.obw &
+w_border.obw+w_button.obw+w_combo.obw+w_group.obw &
+w_hlist.obw+w_icon.obw+w_min.obw+w_max.obw+w_scroll.obw+w_string.obw &
+w_sys.obw+w_text.obw+w_title.obw+w_popup.obw+w_popup1.obw+w_plldn.obw &
+w_plldn1.obw+w_prompt.obw+w_tbar.obw+w_vlist.obw &
+w_win.obw+w_win1.obw+w_win2.obw &
+w_error.obw+w_intl.obw &
+z_list.obw+z_list1.obw &
+z_intl.obw+z_date.obw+z_time.obw+z_bnum.obw+z_bnum1.obw &
+z_region.obw+z_dsp.obw+z_path.obw+z_store.obw &
+z_device.obw+z_cursor.obw+z_event.obw+z_map1.obw+z_map2.obw &
+z_border.obw+z_button.obw+z_combo.obw+z_date1.obw+z_fmtstr.obw &
+z_group.obw+z_hlist.obw+z_icon.obw+z_int.obw+z_min.obw+z_max.obw &
+z_bnum2.obw+z_scroll.obw+z_string.obw+z_sys.obw+z_text.obw+z_time1.obw &
+z_title.obw+z_popup.obw+z_popup1.obw+z_plldn.obw+z_plldn1.obw &
+z_prompt.obw+z_tbar.obw+z_real.obw+z_vlist.obw &
+z_win.obw+z_win1.obw+z_win2.obw+z_utils.obw &
+z_error.obw+z_help.obw+z_app.obw &
+g_bnum.obw+g_border.obw+g_date.obw+g_date1.obw &
+g_device.obw+g_dsp.obw+g_event.obw+g_hotkey.obw &
+g_jump.obw+g_perror.obw+g_phelp.obw+g_pnorm.obw+g_store.obw &
+g_time.obw+g_time1.obw+g_win1.obw+g_win2.obw+g_windsp.obw &
+g_sys.obw+g_int.obw+g_real.obw &
+i_mapfon.obw
!
	-@del zil.sym
	copy win_zil.lib ..\lib\$(VERSION)

# ----- Windows NT ----------------------------------------------------------

winnt: copy ntest.exe

ntest.exe: test.obn wnt_zil.lib 
	$(WNT_LINK) $(WNT_LINK_OPTS) @&&!
$(WNT_OBJS)+test.obn
$*, ,$(WNT_LIBS),wtest.def
!

wnt_zil.lib : w_dsp.obn w_keybrd.obn w_mouse.obn w_cursor.obn w_event.obn \
	w_border.obn w_button.obn w_combo.obn w_group.obn w_hlist.obn \
	w_icon.obn w_min.obn w_max.obn w_scroll.obn w_string.obn w_sys.obn \
	w_text.obn w_title.obn w_popup.obn w_popup1.obn w_plldn.obn \
	w_plldn1.obn w_prompt.obn w_tbar.obn w_vlist.obn \
	w_win.obn w_win1.obn n_win2.obn \
	w_error.obn n_intl.obn\
	z_list.obn z_list1.obn \
	z_intl.obn z_date.obn z_time.obn z_bnum.obn z_bnum1.obn \
	z_region.obn z_dsp.obn z_path.obn z_store.obn \
	z_device.obn z_cursor.obn z_event.obn z_map1.obn z_map2.obn \
	z_border.obn z_button.obn z_combo.obn z_date1.obn z_fmtstr.obn \
	z_group.obn z_hlist.obn z_icon.obn z_int.obn z_min.obn z_max.obn \
	z_bnum2.obn z_scroll.obn z_string.obn z_sys.obn z_text.obn z_time1.obn \
	z_title.obn z_popup.obn z_popup1.obn z_plldn.obn z_plldn1.obn \
	z_prompt.obn z_tbar.obn z_real.obn z_vlist.obn \
	z_win.obn z_win1.obn z_win2.obn z_utils.obn \
	z_error.obn z_help.obn z_app.obn \
	g_bnum.obn g_border.obn g_date.obn g_date1.obn \
	g_device.obn g_dsp.obn g_event.obn g_hotkey.obn \
	g_jump.obn g_perror.obn g_phelp.obn g_pnorm.obn g_store.obn \
	g_time.obn g_time1.obn g_win1.obn g_win2.obn g_windsp.obn \
	g_sys.obn g_int.obn g_real.obn \
	i_mapfon.obn
	-@del wnt_zil.lib
	$(WNT_LIBRARIAN) $(WNT_LIB_OPTS) @&&!
$* &
+w_dsp.obn+w_keybrd.obn+w_mouse.obn+w_cursor.obn+w_event.obn &
+w_border.obn+w_button.obn+w_combo.obn+w_group.obn &
+w_hlist.obn+w_icon.obn+w_min.obn+w_max.obn+w_scroll.obn+w_string.obn &
+w_sys.obn+w_text.obn+w_title.obn+w_popup.obn+w_popup1.obn+w_plldn.obn &
+w_plldn1.obn+w_prompt.obn+w_tbar.obn+w_vlist.obn &
+w_win.obn+w_win1.obn+n_win2.obn &
+w_error.obn+n_intl.obn &
+z_list.obn+z_list1.obn &
+z_intl.obn+z_date.obn+z_time.obn+z_bnum.obn+z_bnum1.obn &
+z_region.obn+z_dsp.obn+z_path.obn+z_store.obn &
+z_device.obn+z_cursor.obn+z_event.obn+z_map1.obn+z_map2.obn &
+z_border.obn+z_button.obn+z_combo.obn+z_date1.obn+z_fmtstr.obn &
+z_group.obn+z_hlist.obn+z_icon.obn+z_int.obn+z_min.obn+z_max.obn &
+z_bnum2.obn+z_scroll.obn+z_string.obn+z_sys.obn+z_text.obn+z_time1.obn &
+z_title.obn+z_popup.obn+z_popup1.obn+z_plldn.obn+z_plldn1.obn &
+z_prompt.obn+z_tbar.obn+z_real.obn+z_vlist.obn &
+z_win.obn+z_win1.obn+z_win2.obn+z_utils.obn &
+z_error.obn+z_help.obn+z_app.obn &
+g_bnum.obn+g_border.obn+g_date.obn+g_date1.obn &
+g_device.obn+g_dsp.obn+g_event.obn+g_hotkey.obn &
+g_jump.obn+g_perror.obn+g_phelp.obn+g_pnorm.obn+g_store.obn &
+g_time.obn+g_time1.obn+g_win1.obn+g_win2.obn+g_windsp.obn &
+g_sys.obn+g_int.obn+g_real.obn &
+i_mapfon.obn
!
#	-@del zil.sym
	copy wnt_zil.lib ..\lib\$(VERSION)

# ----- OS/2 ----------------------------------------------------------------

os2: copy otest.exe

otest.exe: test.obo os2_zil.lib 
	$(OS2_LINK) $(OS2_LINK_OPTS) @&&!
$(OS2_OBJS)+test.obo
$*, , $(OS2_LIBS),otest.def
!
	$(OS2_RC) $(OS2_RC_OPTS) otest.rc $<
	-@del zil.sym

os2_zil.lib : o_dsp.obo o_keybrd.obo o_mouse.obo o_cursor.obo o_event.obo \
	o_border.obo o_button.obo o_combo.obo o_group.obo o_hlist.obo \
	o_icon.obo o_min.obo o_max.obo o_scroll.obo o_string.obo o_sys.obo \
	o_text.obo o_title.obo o_popup.obo o_popup1.obo o_plldn.obo \
	o_plldn1.obo o_prompt.obo o_tbar.obo o_vlist.obo \
	o_win.obo o_win1.obo o_win2.obo \
	o_error.obo o_intl.obo\
	z_list.obo z_list1.obo \
	z_intl.obo z_date.obo z_time.obo z_bnum.obo z_bnum1.obo \
	z_region.obo z_dsp.obo z_path.obo z_store.obo \
	z_device.obo z_cursor.obo z_event.obo z_map1.obo z_map2.obo \
	z_border.obo z_button.obo z_combo.obo z_date1.obo z_fmtstr.obo \
	z_group.obo z_hlist.obo z_icon.obo z_int.obo z_min.obo z_max.obo \
	z_bnum2.obo z_scroll.obo z_string.obo z_sys.obo z_text.obo z_time1.obo \
	z_title.obo z_popup.obo z_popup1.obo z_plldn.obo z_plldn1.obo \
	z_prompt.obo z_tbar.obo z_real.obo z_vlist.obo \
	z_win.obo z_win1.obo z_win2.obo z_utils.obo \
	z_error.obo z_help.obo z_app.obo \
	g_bnum.obo g_border.obo g_date.obo g_date1.obo \
	g_device.obo g_dsp.obo g_event.obo g_hotkey.obo \
	g_jump.obo g_perror.obo g_phelp.obo g_pnorm.obo g_store.obo \
	g_time.obo g_time1.obo g_win1.obo g_win2.obo \
	g_sys.obo g_int.obo g_real.obo
	-@del os2_zil.lib
	$(OS2_LIBRARIAN) $(OS2_LIB_OPTS) @&&!
$* &
+o_dsp.obo+o_keybrd.obo+o_mouse.obo+o_cursor.obo+o_event.obo+&
+o_border.obo+o_button.obo+o_combo.obo+o_group.obo+o_hlist.obo+&
+o_icon.obo+o_min.obo+o_max.obo+o_scroll.obo+o_string.obo+o_sys.obo+&
+o_text.obo+o_title.obo+o_popup.obo+o_popup1.obo+o_plldn.obo+&
+o_plldn1.obo+o_prompt.obo+o_tbar.obo+o_vlist.obo+&
+o_win.obo+o_win1.obo+o_win2.obo+&
+o_error.obo+o_intl.obo+&
+z_list.obo+z_list1.obo+&
+z_intl.obo+z_date.obo+z_time.obo+z_bnum.obo+z_bnum1.obo+&
+z_region.obo+z_dsp.obo+z_path.obo+z_store.obo+&
+z_device.obo+z_cursor.obo+z_event.obo+z_map1.obo+z_map2.obo+&
+z_border.obo+z_button.obo+z_combo.obo+z_date1.obo+z_fmtstr.obo+&
+z_group.obo+z_hlist.obo+z_icon.obo+z_int.obo+z_min.obo+z_max.obo+&
+z_bnum2.obo+z_scroll.obo+z_string.obo+z_sys.obo+z_text.obo+z_time1.obo+&
+z_title.obo+z_popup.obo+z_popup1.obo+z_plldn.obo+z_plldn1.obo+&
+z_prompt.obo+z_tbar.obo+z_real.obo+z_vlist.obo+&
+z_win.obo+z_win1.obo+z_win2.obo+z_utils.obo+&
+z_error.obo+z_help.obo+z_app.obo+&
+g_bnum.obo+g_border.obo+g_date.obo+g_date1.obo+&
+g_device.obo+g_dsp.obo+g_event.obo+g_hotkey.obo+&
+g_jump.obo+g_perror.obo+g_phelp.obo+g_pnorm.obo+g_store.obo+&
+g_time.obo+g_time1.obo+g_win1.obo+g_win2.obo+&
+g_sys.obo+g_int.obo+g_real.obo
!
	-@del zil.sym
	copy os2_zil.lib ..\lib\$(VERSION)

# ----- DOS Extender ----------------------------------------------------------

dos16: copy d16_gfx.lib d16_bgi.lib test16.exe

test16.exe: test.o16 d16_zil.lib 
	$(D16_LINK) $(D16_LINK_OPTS) @&&!
$(D16_OBJS)+test.o16
$*, , $(D16_LIBS),b16.def
!
	-@del zil.sym

d16_zil.lib : d_tdsp.o16 d_keybrd.o16 d_mouse.o16 d_cursor.o16 \
	d_border.o16 d_button.o16 d_combo.o16 d_group.o16 d_hlist.o16 \
	d_icon.o16 d_min.o16 d_max.o16 d_scroll.o16 d_string.o16 d_sys.o16 \
	d_text.o16 d_title.o16 d_popup.o16 d_popup1.o16 d_plldn.o16 \
	d_plldn1.o16 d_prompt.o16 d_tbar.o16 d_vlist.o16 \
	d_win.o16 d_win1.o16 d_win2.o16 \
	d_error.o16 d_intl.o16\
	z_list.o16 z_list1.o16 \
	z_intl.o16 z_date.o16 z_time.o16 z_bnum.o16 z_bnum1.o16 \
	z_region.o16 z_dsp.o16 z_path.o16 z_store.o16 \
	z_device.o16 z_cursor.o16 z_event.o16 z_map1.o16 z_map2.o16 \
	z_border.o16 z_button.o16 z_combo.o16 z_date1.o16 z_fmtstr.o16 \
	z_group.o16 z_hlist.o16 z_icon.o16 z_int.o16 z_min.o16 z_max.o16 \
	z_bnum2.o16 z_scroll.o16 z_string.o16 z_sys.o16 z_text.o16 z_time1.o16 \
	z_title.o16 z_popup.o16 z_popup1.o16 z_plldn.o16 z_plldn1.o16 \
	z_prompt.o16 z_tbar.o16 z_real.o16 z_vlist.o16 \
	z_win.o16 z_win1.o16 z_win2.o16 z_utils.o16 \
	z_error.o16 z_help.o16 \
	g_bnum.o16 g_border.o16 g_date.o16 g_date1.o16 \
	g_device.o16 g_dsp.o16 g_event.o16 g_hotkey.o16 \
	g_jump.o16 g_perror.o16 g_phelp.o16 g_pnorm.o16 g_store.o16 \
	g_time.o16 g_time1.o16 g_win1.o16 g_win2.o16 \
	g_sys.o16 g_int.o16 g_real.o16 \
	i_btcat.o16 i_mapfon.o16
	-@del d16_zil.lib
	$(D16_LIBRARIAN) $(D16_LIB_OPTS) @&&!
$* &
+d_tdsp.o16+d_keybrd.o16+d_mouse.o16+d_cursor.o16 &
+d_border.o16+d_button.o16+d_combo.o16+d_group.o16+d_hlist.o16 &
+d_icon.o16+d_min.o16+d_max.o16+d_scroll.o16+d_string.o16+d_sys.o16 &
+d_text.o16+d_title.o16+d_popup.o16+d_popup1.o16+d_plldn.o16 &
+d_plldn1.o16+d_prompt.o16+d_tbar.o16+d_vlist.o16 &
+d_win.o16+d_win1.o16+d_win2.o16 &
+d_error.o16+d_intl.o16 &
+z_list.o16+z_list1.o16 &
+z_intl.o16+z_date.o16+z_time.o16+z_bnum.o16+z_bnum1.o16 &
+z_region.o16+z_dsp.o16+z_path.o16+z_store.o16 &
+z_device.o16+z_cursor.o16+z_event.o16+z_map1.o16+z_map2.o16 &
+z_border.o16+z_button.o16+z_combo.o16+z_date1.o16+z_fmtstr.o16 &
+z_group.o16+z_hlist.o16+z_icon.o16+z_int.o16+z_min.o16+z_max.o16 &
+z_bnum2.o16+z_scroll.o16+z_string.o16+z_sys.o16+z_text.o16+z_time1.o16 &
+z_title.o16+z_popup.o16+z_popup1.o16+z_plldn.o16+z_plldn1.o16 &
+z_prompt.o16+z_tbar.o16+z_real.o16+z_vlist.o16 &
+z_win.o16+z_win1.o16+z_win2.o16+z_utils.o16 &
+z_error.o16+z_help.o16 &
+g_bnum.o16+g_border.o16+g_date.o16+g_date1.o16 &
+g_device.o16+g_dsp.o16+g_event.o16+g_hotkey.o16 &
+g_jump.o16+g_perror.o16+g_phelp.o16+g_pnorm.o16+g_store.o16 &
+g_time.o16+g_time1.o16+g_win1.o16+g_win2.o16 &
+g_sys.o16+g_int.o16+g_real.o16 &
+i_btcat.o16+i_mapfon.o16
!
	@del zil.sym
	copy d16_zil.lib ..\lib\$(VERSION)

d16_bgi.lib : d_bgidsp.o16 g_bgidsp.o16 z_appbgi.o16
	-@del d16_bgi.lib
	$(D16_LIBRARIAN) $(D16_LIB_OPTS) @&&!
$* &
+d_bgidsp.o16+g_bgidsp.o16+z_appbgi.o16
!
	@del zil.sym
	copy d16_bgi.lib ..\lib\$(VERSION)

d16_gfx.lib : d_gfxdsp.o16 $(FONT)_smal.o16 $(FONT)_dial.o16 $(FONT)_syst.o16 z_appgfx.o16
	-@del d16_gfx.lib
	$(D16_LIBRARIAN) $(D16_LIB_OPTS) @&&!
$* &
+d_gfxdsp.o16+$(FONT)_smal.o16+$(FONT)_dial.o16+$(FONT)_syst.o16+z_appgfx.o16
!
	@del zil.sym
	copy d16_gfx.lib ..\lib\$(VERSION)

bc_16gfx.lib :
	@cd gfx\source
	make -f borland.mak dos16
	@cd ..\..

# --- Done with libraries ----------------------------------------------------

z_appbgi.obj: z_app.cpp
	$(DOS_CPP) -DBGI $(DOS_CPP_OPTS) -o$@ $?

z_appgfx.obj: z_app.cpp
	$(DOS_CPP) -DGFX $(DOS_CPP_OPTS) -o$@ $?

z_appbgi.o16: z_app.cpp
	$(D16_CPP) -DBGI $(D16_CPP_OPTS) -o$@ $?

z_appgfx.o16: z_app.cpp
	$(D16_CPP) -DGFX $(D16_CPP_OPTS) -o$@ $?

z_apptxt.obj: z_app.cpp
	$(DOS_CPP) -DTEXT $(DOS_CPP_OPTS) -o$@ $?

$(FONT)_dial.cpp: gfx\$(FONT)_dial.cpp
	copy gfx\$(FONT)_dial.cpp

$(FONT)_smal.cpp: gfx\$(FONT)_smal.cpp
	copy gfx\$(FONT)_smal.cpp

$(FONT)_syst.cpp: gfx\$(FONT)_syst.cpp
	copy gfx\$(FONT)_syst.cpp
