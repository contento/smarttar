# ----- General Definitions -------------------------------------------------
VERSION=ztcpp310
# font can be OEM or ISO
FONT=ISO

# ----- DOS compiler options ------------------------------------------------
DOS_CPP=ztc
DOS_LINK=blinkx
DOS_LIBRARIAN=zorlibx

DOS_CPP_OPTS=-c -a1 -bx -ml -D$(FONT)
DOS_LINK_OPTS=/NOI
DOS_LIB_OPTS=/C /N /B

DOS_OBJS=
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
DOS_LIBS=dos_zil dos_gfx zt_lgfx
# --- Use the next line for UI_BGI_DISPLAY ---
#DOS_LIBS=dos_zil dos_fg fg

.cpp.obj:
	$(DOS_CPP) $(DOS_CPP_OPTS) $<

# ----- Windows compiler options --------------------------------------------
WIN_CPP=ztc
WIN_LINK=blinkx
WIN_LIBRARIAN=zorlibx
WIN_RC=rc

# --- Use the next 2 lines for Windows 3.0 compatibility ---
WIN_CPP_OPTS=-c -a1 -bx -ml -W2 -DWINVER=0x0300
WIN_RC_OPTS=-k -30
# --- Use the next 2 lines for Windows 3.1 compatibility ---
#WIN_CPP_OPTS=-c -a1 -bx -ml -W2
#WIN_RC_OPTS=-k
WIN_LINK_OPTS=/NOI /PACKC:42000
WIN_LIB_OPTS=/C /N /B

WIN_OBJS=
WIN_LIBS=win_zil libw

.cpp.obw:
	$(WIN_CPP) $(WIN_CPP_OPTS) -o$@ $<

# ----- Windows DLL compiler options ----------------------------------------
WINDLL_CPP=ztc
WINDLL_LINK=blinkx
WINDLL_LIBRARIAN=zorlibx

WINDLL_CPP_OPTS=-c -a1 -bx -mluw -W2 -D__DLL__
WINDLL_LINK_OPTS=/NOI
WINDLL_LIB_OPTS=/C /N /B

WINDLL_OBJS=
WINDLL_LIBS=wdll_zil

.cpp.obd:
	$(WINDLL_CPP) $(WINDLL_CPP_OPTS) -o$@ $<

# ----- OS/2 compiler options -----------------------------------------------
OS2_CPP=ztc
OS2_LINK=blink
OS2_LIBRARIAN=zorlib
OS2_RC=rc

OS2_CPP_OPTS=-c -mf -D$(FONT)
OS2_LINK_OPTS=/BASE:0x10000 /PM:PM 
OS2_LIB_OPTS=
OS2_RC_OPTS=

OS2_OBJS=
OS2_LIBS=os2_zil

.cpp.obo:
	$(OS2_CPP) $(OS2_CPP_OPTS) -o$*.obo $<

# ----- DOS extender compiler options -----------------------------------------
D16_CPP=ztc
D16_LINK=blinkx
D16_LIBRARIAN=zorlibx

D16_CPP_OPTS=-c -bx -mz -D$(FONT)
D16_LINK_OPTS=/NOI
D16_LIB_OPTS=/C /N /B

D16_OBJS=
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
D16_LIBS=zpm d16_zil d16_gfx zt_16gfx
# --- Use the next line for UI_FG_DISPLAY ---
#D16_LIBS=zpm d16_zil d16_fg fg

.cpp.o16:
	$(D16_CPP) $(D16_CPP_OPTS) -o$@ $<

# ----- DOS extender compiler options -----------------------------------------
D32_CPP=ztc
D32_LINK=blinkx
D32_LIBRARIAN=zorlibx

D32_CPP_OPTS=-c -bx -mx -D$(FONT)
D32_LINK_OPTS=/NOI
D32_LIB_OPTS=/C /N /B

D32_OBJS=cx
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
D32_LIBS=d32_zil d32_gfx zt_32gfx
# --- Use the next line for UI_FG_DISPLAY ---
#D32_LIBS=d32_zil d32_fg fgp

.cpp.o32:
	$(D32_CPP) $(D32_CPP_OPTS) -o$@ $<

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To generate the library modules for specific environments type:
	@echo make -fzortech.mak dos
	@echo make -fzortech.mak dos16
	@echo make -fzortech.mak dos32
	@echo make -fzortech.mak os2
	@echo make -fzortech.mak windows
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean.bat

# ----- Copy files ----------------------------------------------------------
copy :
	copy ui_*.hpp ..\include
	copy z_clean.* ..\bin
	copy gfx\source\gfx.h ..\include
	copy gfx\source\gfx_pro.h ..\include

# ----- DOS Libraries and Programs ------------------------------------------

dos: copy dos_fg.lib dos_gfx.lib test.exe

test.exe: test.obj dos_zil.lib
	$(DOS_LINK) $(DOS_LINK_OPTS) $(DOS_OBJS)+test.obj, $*, , $(DOS_LIBS)

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
	i_ztcat.obj i_mapfon.obj
	-@del dos_zil.lib
	$(DOS_LIBRARIAN) $(DOS_LIB_OPTS) @dos_zil.rsp
	copy dos_zil.lib ..\lib\$(VERSION)

dos_fg.lib : d_fgdsp.obj g_fgdsp.obj z_appfg.obj
	-@del dos_fg.lib
	$(DOS_LIBRARIAN) $(DOS_LIB_OPTS) dos_fg+d_fgdsp.obj+g_fgdsp.obj+z_appfg.obj;
	copy dos_fg.lib ..\lib\$(VERSION)

dos_gfx.lib : d_gfxdsp.obj $(FONT)_smal.obj $(FONT)_dial.obj $(FONT)_syst.obj z_appgfx.obj
	-@del dos_gfx.lib
	$(DOS_LIBRARIAN) $@ $(DOS_LIB_OPTS) +d_gfxdsp.obj+$(FONT)_smal.obj+$(FONT)_dial.obj+$(FONT)_syst.obj+z_appgfx.obj;
	copy dos_gfx.lib ..\lib\$(VERSION)

zt_lgfx.lib :
	@cd gfx\source
	make -f zortech.mak dos
	@cd ..\..

# ----- Windows Libraries and Programs --------------------------------------

windows: copy wtest.exe

wtest.exe: test.obw win_zil.lib
	$(WIN_LINK) $(WIN_LINK_OPTS) $(WIN_OBJS)+test.obw, $*, , $(WIN_LIBS), wtest.def
	$(WIN_RC) wtest.rc $*.exe

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
	$(WIN_LIBRARIAN) $(WIN_LIB_OPTS) @win_zil.rsp
	copy win_zil.lib ..\lib\$(VERSION)

# ----- OS/2 Libraries and Programs -----------------------------------------

os2: copy otest.exe

otest.exe: test.obo os2_zil.lib
	$(OS2_LINK) $(OS2_LINK_OPTS) $(OS2_OBJS)+test.obo, $*, , $(OS2_LIBS), otest.def
	$(OS2_RC) $(OS2_RC_OPTS) otest.rc

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
	$(OS2_LIBRARIAN) $(OS2_LIB_OPTS) @os2_zil.rsp
	copy os2_zil.lib ..\lib\$(VERSION)

# ----- Dos extender Libraries and Programs -----------------------------------

dos16: copy d16_gfx.lib d16_fg.lib test16.exe

test16.exe: test.o16 d16_zil.lib
	$(D16_LINK) $(D16_LINK_OPTS) $(D16_OBJS)+test.o16, $*, , $(D16_LIBS), z16.def

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
	i_ztcat.o16 i_mapfon.o16
	-@del d16_zil.lib
	$(D16_LIBRARIAN) $(D16_LIB_OPTS) @d16_zil.rsp
	copy d16_zil.lib ..\lib\$(VERSION)

d16_fg.lib : d_fgdsp.o16 g_fgdsp.o16 z_appfg.o16
	-@del d16_fg.lib
	$(D16_LIBRARIAN) $(D16_LIB_OPTS) d16_fg+d_fgdsp.o16+g_fgdsp.o16+z_appfg.o16;
	copy d16_fg.lib ..\lib\$(VERSION)

d16_gfx.lib : d_gfxdsp.o16 $(FONT)_smal.o16 $(FONT)_dial.o16 $(FONT)_syst.o16 z_appgfx.o16
	-@del d16_gfx.lib
	$(D16_LIBRARIAN) $(D16_LIB_OPTS) d16_gfx+d_gfxdsp.o16+$(FONT)_smal.o16+$(FONT)_dial.o16+$(FONT)_syst.o16+z_appgfx.o16;
	copy d16_gfx.lib ..\lib\$(VERSION)

zt_16gfx.lib :
	@cd gfx\source
	make -f zortech.mak dos16
	@cd ..\..

# ----- Dos extender Libraries and Programs -----------------------------------

dos32: copy d32_gfx.lib d32_fg.lib test32.exe

test32.exe: test.o32 d32_zil.lib
	$(D32_LINK) $(D32_LINK_OPTS) $(D32_OBJS)+test.o32, $*, , $(D32_LIBS)

d32_zil.lib : d_tdsp.o32 d_keybrd.o32 d_mouse.o32 d_cursor.o32 \
	d_border.o32 d_button.o32 d_combo.o32 d_group.o32 d_hlist.o32 \
	d_icon.o32 d_min.o32 d_max.o32 d_scroll.o32 d_string.o32 d_sys.o32 \
	d_text.o32 d_title.o32 d_popup.o32 d_popup1.o32 d_plldn.o32 \
	d_plldn1.o32 d_prompt.o32 d_tbar.o32 d_vlist.o32 \
	d_win.o32 d_win1.o32 d_win2.o32 \
	d_error.o32 d_intl.o32\
	z_list.o32 z_list1.o32 \
	z_intl.o32 z_date.o32 z_time.o32 z_bnum.o32 z_bnum1.o32 \
	z_region.o32 z_dsp.o32 z_path.o32 z_store.o32 \
	z_device.o32 z_cursor.o32 z_event.o32 z_map1.o32 z_map2.o32 \
	z_border.o32 z_button.o32 z_combo.o32 z_date1.o32 z_fmtstr.o32 \
	z_group.o32 z_hlist.o32 z_icon.o32 z_int.o32 z_min.o32 z_max.o32 \
	z_bnum2.o32 z_scroll.o32 z_string.o32 z_sys.o32 z_text.o32 z_time1.o32 \
	z_title.o32 z_popup.o32 z_popup1.o32 z_plldn.o32 z_plldn1.o32 \
	z_prompt.o32 z_tbar.o32 z_real.o32 z_vlist.o32 \
	z_win.o32 z_win1.o32 z_win2.o32 z_utils.o32 \
	z_error.o32 z_help.o32 \
	g_bnum.o32 g_border.o32 g_date.o32 g_date1.o32 \
	g_device.o32 g_dsp.o32 g_event.o32 g_hotkey.o32 \
	g_jump.o32 g_perror.o32 g_phelp.o32 g_pnorm.o32 g_store.o32 \
	g_time.o32 g_time1.o32 g_win1.o32 g_win2.o32 \
	g_sys.o32 g_int.o32 g_real.o32 \
	i_ztcat.o32 i_mapfon.o32
	-@del d32_zil.lib
	$(D32_LIBRARIAN) $(D32_LIB_OPTS) @d32_zil.rsp
	copy d32_zil.lib ..\lib\$(VERSION)

d32_fg.lib : d_fgdsp.o32 g_fgdsp.o32 z_appfg.o32
	-@del d32_fg.lib
	$(D32_LIBRARIAN) $(D32_LIB_OPTS) d32_fg+d_fgdsp.o32+g_fgdsp.o32+z_appfg.o32;
	copy d32_fg.lib ..\lib\$(VERSION)

d32_gfx.lib : d_gfxdsp.o32 $(FONT)_smal.o32 $(FONT)_dial.o32 $(FONT)_syst.o32 z_appgfx.o32
	-@del d32_gfx.lib
	$(D32_LIBRARIAN) $(D32_LIB_OPTS) d32_gfx+d_gfxdsp.o32+$(FONT)_smal.o32+$(FONT)_dial.o32+$(FONT)_syst.o32+z_appgfx.o32; 
	copy d32_gfx.lib ..\lib\$(VERSION)

zt_32gfx.lib :
	@cd gfx\source
	make -f zortech.mak dos32
	@cd ..\..

# --- Done with libraries ----------------------------------------------------

z_appfg.obj: z_app.cpp
	$(DOS_CPP) -DFG $(DOS_CPP_OPTS) -o$@ $?

z_appgfx.obj: z_app.cpp
	$(DOS_CPP) -DGFX $(DOS_CPP_OPTS) -o$@ $?

z_appfg.o16: z_app.cpp
	$(D16_CPP) -DFG $(D16_CPP_OPTS) -o$@ $?

z_appgfx.o16: z_app.cpp
	$(D16_CPP) -DGFX $(D16_CPP_OPTS) -o$@ $?

z_appfg.o32: z_app.cpp
	$(D32_CPP) -DFG $(D32_CPP_OPTS) -o$@ $?

z_appgfx.o32: z_app.cpp
	$(D32_CPP) -DGFX $(D32_CPP_OPTS) -o$@ $?

z_apptxt.obj: z_app.cpp
	$(DOS_CPP) -DTEXT $(DOS_CPP_OPTS) -o$@ $?

$(FONT)_dial.cpp: gfx\$(FONT)_dial.cpp
	copy gfx\$(FONT)_dial.cpp

$(FONT)_smal.cpp: gfx\$(FONT)_smal.cpp
	copy gfx\$(FONT)_smal.cpp

$(FONT)_syst.cpp: gfx\$(FONT)_syst.cpp
	copy gfx\$(FONT)_syst.cpp

