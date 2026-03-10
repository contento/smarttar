# ----- General Definitions -------------------------------------------------
VERSION=wccpp950
# font can be OEM or ISO
FONT=ISO

.EXTENSIONS:
.EXTENSIONS: .o32 .obw .obn .obo .obm .cpp .c

# Be sure to set the LIB and INCLUDE environment variables for Zinc, e.g.:
#    set INCLUDE=.;C:\DVX\INCLUDE;C:\WATCOM\H;C:\ZINC\INCLUDE
#    set LIB=.;C:\DVX\LIB\C386;C:\WATCOM\LIB386\DOS;C:\WATCOM\LIB386;C:\ZINC\LIB\WCCPP950

# ----- DOS extender compiler options ---------------------------------------
D32_CPP=wpp386
D32_LINK=wlink
D32_LIBRARIAN=wlib

D32_CPP_OPTS=/bt=dos /d$(FONT) /dDOS386
D32_LINK_OPTS=SYSTEM dos4g OP stack=20000
D32_LIB_OPTS=-p=32

# --- Use the next line for UI_GRAPHICS_DISPLAY ---
D32_OBJS=
D32_LIBS=d32_zil,d32_gfx,wc_32gfx,math3r,emu387,clib3r
# --- Use the next line for UI_WCC_DISPLAY ---
#D32_OBJS=
#D32_LIBS=d32_zil,d32_wcc,math3r,emu387,clib3r

.cpp.o32:
	$(D32_CPP) $(D32_CPP_OPTS) -fo=$[*.o32 $<

# ----- Windows compiler options --------------------------------------------
WIN_CPP=wpp
WIN_LINK=wlink
WIN_LIBRARIAN=wlib
WIN_RC=wrc

WIN_CPP_OPTS= -zW -oaxt -d1 -w4 -ml -DWINVER=0x0300
WIN_RC_OPTS=/30 /bt=windows
WIN_LINK_OPTS=SYS windows OP heapsize=16k OP stack=16k OP version=3.0
WIN_LIB_OPTS=

WIN_OBJS=
WIN_LIBS=windows,win_zil

.cpp.obw:
	$(WIN_CPP) $(WIN_CPP_OPTS) -fo=$[*.obw $<

# ----- OS/2 compiler options -----------------------------------------------
OS2_CPP=wpp386
OS2_LINK=wlink
OS2_LIBRARIAN=wlib
OS2_RC=rc

OS2_CPP_OPTS=/bt=OS2 /d$(FONT)
OS2_LINK_OPTS=SYSTEM os2v2_pm OP ST=20000
OS2_LIB_OPTS=
OS2_RC_OPTS=

OS2_OBJS=
OS2_LIBS=os2_zil,os2386

.cpp.obo:
	$(OS2_CPP) $(OS2_CPP_OPTS) -fo=$[*.obo $<

# ----- DESQview/X MOTIF compiler options -----------------------------------
DVX_CPP=wpp386
DVX_LINK=wlink
DVX_LIBRARIAN=wlib

DVX_CPP_OPTS=/bt=dos /d__DVX__ /dXTFUNCPROTO /d$(FONT) -w0
DVX_LINK_OPTS=SYSTEM dos4g OP stack=20000
DVX_LIB_OPTS=-p=32

DVX_OBJS=
DVX_LIBS=dvx_zil,math3r,emu387,clib3r,Xm,Xt,Xlib,sys

.cpp.obm:
	$(DVX_CPP) $(DVX_CPP_OPTS) -fo=$[*.obm $<

# ----- Usage --------------------------------------------------------------
usage: .SYMBOLIC
	@echo ...........
	@echo ...........
	@echo To generate the library modules for specific environments type:
	@echo wmake -f watcom.mak dos32
	@echo wmake -f watcom.mak windows
	@echo wmake -f watcom.mak os2
	@echo wmake -f watcom.mak dvx
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean: .SYMBOLIC
	z_clean.bat

# ----- Copy files ----------------------------------------------------------
copy: .SYMBOLIC
	copy ui_*.hpp ..\include
	copy z_clean.* ..\bin
	copy gfx\source\gfx.h ..\include
	copy gfx\source\gfx_pro.h ..\include


# ----- DOS Libraries and Programs ------------------------------------------
dos32: copy d32_gfx.lib d32_wcc.lib test32.exe .SYMBOLIC

test32.exe: test.o32 d32_zil.lib 
	$(D32_LINK) $(D32_LINK_OPTS) N $* F test.o32 $(D32_OBJS) L $(D32_LIBS)

d32_zil.lib : d_tdsp.o32 d_keybrd.o32 d_mouse.o32 d_cursor.o32 &
	d_border.o32 d_button.o32 d_combo.o32 d_group.o32 d_hlist.o32 &
	d_icon.o32 d_min.o32 d_max.o32 d_scroll.o32 d_string.o32 d_sys.o32 &
	d_text.o32 d_title.o32 d_popup.o32 d_popup1.o32 d_plldn.o32 &
	d_plldn1.o32 d_prompt.o32 d_tbar.o32 d_vlist.o32 &
	d_win.o32 d_win1.o32 d_win2.o32 &
	d_error.o32 d_intl.o32&
	z_list.o32 z_list1.o32 &
	z_intl.o32 z_date.o32 z_time.o32 z_bnum.o32 z_bnum1.o32 &
	z_region.o32 z_dsp.o32 z_path.o32 z_store.o32 &
	z_device.o32 z_cursor.o32 z_event.o32 z_map1.o32 z_map2.o32 &
	z_border.o32 z_button.o32 z_combo.o32 z_date1.o32 z_fmtstr.o32 &
	z_group.o32 z_hlist.o32 z_icon.o32 z_int.o32 z_min.o32 z_max.o32 &
	z_bnum2.o32 z_scroll.o32 z_string.o32 z_sys.o32 z_text.o32 z_time1.o32 &
	z_title.o32 z_popup.o32 z_popup1.o32 z_plldn.o32 z_plldn1.o32 &
	z_prompt.o32 z_tbar.o32 z_real.o32 z_vlist.o32 &
	z_win.o32 z_win1.o32 z_win2.o32 z_utils.o32 &
	z_error.o32 z_help.o32 &
	g_bnum.o32 g_border.o32 g_date.o32 g_date1.o32 &
	g_device.o32 g_dsp.o32 g_event.o32 g_hotkey.o32 &
	g_jump.o32 g_perror.o32 g_phelp.o32 g_pnorm.o32 g_store.o32 &
	g_time.o32 g_time1.o32 g_win1.o32 g_win2.o32 &
	g_sys.o32 g_int.o32 g_real.o32 &
	i_wccat.o32 i_mapfon.o32
	del d32_zil.lib
	$(D32_LIBRARIAN) $(D32_LIB_OPTS) @d32_zil.wcc
	-@md ..\lib\$(VERSION)
	copy d32_zil.lib ..\lib\$(VERSION)

d32_wcc.lib : d_wccdsp.o32 g_wccdsp.o32 z_appwcc.o32
	del d32_wcc.lib
	$(D32_LIBRARIAN) $(D32_LIB_OPTS) $* +d_wccdsp.o32+g_wccdsp.o32+z_appwcc.o32
	-@md ..\lib\$(VERSION)
	copy d32_wcc.lib ..\lib\$(VERSION)

d32_gfx.lib : d_gfxdsp.o32 z_appgfx.o32 $(FONT)_smal.o32 $(FONT)_dial.o32 $(FONT)_syst.o32
	del d32_gfx.lib
	$(D32_LIBRARIAN) $(D32_LIB_OPTS) $* +d_gfxdsp.o32+z_appgfx.o32+$(FONT)_smal.o32+$(FONT)_dial.o32+$(FONT)_syst.o32
	-@md ..\lib\$(VERSION)
	copy d32_gfx.lib ..\lib\$(VERSION)

wc_32gfx.lib : .SYMBOLIC
	@cd gfx\source
	wmake -f watcom.mak dos32
	@cd ..\..

# ----- Windows Libraries and Programs --------------------------------------
windows: copy wtest.exe .SYMBOLIC

wtest.exe: test.obw win_zil.lib
	$(WIN_LINK) $(WIN_LINK_OPTS) N wtest F test.obw$(WIN_OBJS) L $(WIN_LIBS)
#	wrc $(WIN_RC_OPTS) wtest wtest

win_zil.lib : w_dsp.obw w_keybrd.obw w_mouse.obw w_cursor.obw w_event.obw &
	w_border.obw w_button.obw w_combo.obw w_group.obw w_hlist.obw &
	w_icon.obw w_min.obw w_max.obw w_scroll.obw w_string.obw w_sys.obw &
	w_text.obw w_title.obw w_popup.obw w_popup1.obw w_plldn.obw &
	w_plldn1.obw w_prompt.obw w_tbar.obw w_vlist.obw &
	w_win.obw w_win1.obw w_win2.obw &
	w_error.obw w_intl.obw &
	z_list.obw z_list1.obw &
	z_intl.obw z_date.obw z_time.obw z_bnum.obw z_bnum1.obw &
	z_region.obw z_dsp.obw z_path.obw z_store.obw &
	z_device.obw z_cursor.obw z_event.obw z_map1.obw z_map2.obw &
	z_border.obw z_button.obw z_combo.obw z_date1.obw z_fmtstr.obw &
	z_group.obw z_hlist.obw z_icon.obw z_int.obw z_min.obw z_max.obw &
	z_bnum2.obw z_scroll.obw z_string.obw z_sys.obw z_text.obw z_time1.obw &
	z_title.obw z_popup.obw z_popup1.obw z_plldn.obw z_plldn1.obw &
	z_prompt.obw z_tbar.obw z_real.obw z_vlist.obw &
	z_win.obw z_win1.obw z_win2.obw z_utils.obw &
	z_error.obw z_help.obw z_app.obw &
	g_bnum.obw g_border.obw g_date.obw g_date1.obw &
	g_device.obw g_dsp.obw g_event.obw g_hotkey.obw &
	g_jump.obw g_perror.obw g_phelp.obw g_pnorm.obw g_store.obw &
	g_time.obw g_time1.obw g_win1.obw g_win2.obw g_windsp.obw &
	g_sys.obw g_int.obw g_real.obw &
	i_mapfon.obw
	del win_zil.lib
	$(WIN_LIBRARIAN) $(WIN_LIB_OPTS) @win_zil.wcc
	-@md ..\lib\$(VERSION)
	copy win_zil.lib ..\lib\$(VERSION)


# ----- OS/2 Libraries and Programs -----------------------------------------
os2: copy otest.exe .SYMBOLIC

otest.exe: test.obo os2_zil.lib
	$(OS2_LINK) $(OS2_LINK_OPTS) N otest.exe F test.obo $(OS2_OBJS) L $(OS2_LIBS)

os2_zil.lib : o_dsp.obo o_keybrd.obo o_mouse.obo o_cursor.obo o_event.obo &
	o_border.obo o_button.obo o_combo.obo o_group.obo o_hlist.obo &
	o_icon.obo o_min.obo o_max.obo o_scroll.obo o_string.obo o_sys.obo &
	o_text.obo o_title.obo o_popup.obo o_popup1.obo o_plldn.obo &
	o_plldn1.obo o_prompt.obo o_tbar.obo o_vlist.obo &
	o_win.obo o_win1.obo o_win2.obo &
	o_error.obo o_intl.obo &
	z_list.obo z_list1.obo &
	z_intl.obo z_date.obo z_time.obo z_bnum.obo z_bnum1.obo &
	z_region.obo z_dsp.obo z_path.obo z_store.obo &
	z_device.obo z_cursor.obo z_event.obo z_map1.obo z_map2.obo &
	z_border.obo z_button.obo z_combo.obo z_date1.obo z_fmtstr.obo &
	z_group.obo z_hlist.obo z_icon.obo z_int.obo z_min.obo z_max.obo &
	z_bnum2.obo z_scroll.obo z_string.obo z_sys.obo z_text.obo z_time1.obo &
	z_title.obo z_popup.obo z_popup1.obo z_plldn.obo z_plldn1.obo &
	z_prompt.obo z_tbar.obo z_real.obo z_vlist.obo &
	z_win.obo z_win1.obo z_win2.obo z_utils.obo &
	z_error.obo z_help.obo z_app.obo &
	g_bnum.obo g_border.obo g_date.obo g_date1.obo &
	g_device.obo g_dsp.obo g_event.obo g_hotkey.obo &
	g_jump.obo g_perror.obo g_phelp.obo g_pnorm.obo g_store.obo &
	g_time.obo g_time1.obo g_win1.obo g_win2.obo &
	g_sys.obo g_int.obo g_real.obo
	-@del os2_zil.lib
	$(OS2_LIBRARIAN) $(OS2_LIB_OPTS) @os2_zil.wcc
	-@md ..\lib\$(VERSION)
	copy os2_zil.lib ..\lib\$(VERSION)


# ----- DESQview/X MOTIF Libraries and Programs -----------------------------
dvx: copy mtest.exe .SYMBOLIC

mtest.exe: test.obm dvx_zil.lib 
	$(DVX_LINK) $(DVX_LINK_OPTS) N mtest.exe F test.obm $(DVX_OBJS) L $(DVX_LIBS)

dvx_zil.lib : g_bnum.obm g_border.obm g_date.obm g_date1.obm g_device.obm &
	g_dsp.obm g_event.obm g_hotkey.obm g_jump.obm &
	g_perror.obm g_phelp.obm g_pnorm.obm g_store.obm g_time.obm g_time1.obm &
	g_win1.obm g_win2.obm g_sys.obm g_int.obm g_real.obm &
	m_bitmap.obm m_border.obm m_button.obm m_combo.obm m_cursor.obm m_dsp.obm &
	m_error.obm m_event.obm m_group.obm m_hlist.obm m_icon.obm m_intl.obm m_keybrd.obm &
	m_max.obm m_min.obm m_mouse.obm m_plldn.obm m_plldn1.obm m_popup.obm &
	m_popup1.obm m_prompt.obm m_scroll.obm m_string.obm m_sys.obm m_tbar.obm &
	m_text.obm m_title.obm m_vlist.obm m_win.obm m_win1.obm m_win2.obm &
	z_app.obm z_bnum.obm z_bnum1.obm z_bnum2.obm z_border.obm z_button.obm z_combo.obm &
	z_cursor.obm z_date.obm z_date1.obm z_device.obm z_dsp.obm z_error.obm z_event.obm &
	z_fmtstr.obm z_group.obm z_help.obm z_hlist.obm z_icon.obm z_int.obm &
	z_intl.obm z_list.obm z_list1.obm z_map1.obm z_map2.obm z_max.obm z_min.obm &
	z_path.obm z_plldn.obm z_plldn1.obm z_popup.obm z_popup1.obm z_prompt.obm &
	z_real.obm z_region.obm z_scroll.obm z_store.obm z_string.obm z_sys.obm &
	z_tbar.obm z_text.obm z_time.obm z_time1.obm z_title.obm z_vlist.obm &
	z_win.obm z_win1.obm z_win2.obm z_utils.obm
	del dvx_zil.lib
	$(DVX_LIBRARIAN) $(DVX_LIB_OPTS) @dvx_zil.wcc
	-@md ..\lib\$(VERSION)
	copy dvx_zil.lib ..\lib\$(VERSION)


# --- Done with libraries ----------------------------------------------------

z_appwcc.o32: z_app.cpp
	$(D32_CPP) /dWCC $(D32_CPP_OPTS) -fo=$@ $?

z_appgfx.o32: z_app.cpp
	$(D32_CPP) /dGFX $(D32_CPP_OPTS) -fo=$@ $?

$(FONT)_dial.cpp: gfx\$(FONT)_dial.cpp
	copy gfx\$(FONT)_dial.cpp

$(FONT)_smal.cpp: gfx\$(FONT)_smal.cpp
	copy gfx\$(FONT)_smal.cpp

$(FONT)_syst.cpp: gfx\$(FONT)_syst.cpp
	copy gfx\$(FONT)_syst.cpp
