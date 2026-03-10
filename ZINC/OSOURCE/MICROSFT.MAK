# ----- General Definitions -------------------------------------------------
VERSION=mscpp700
# font can be OEM or ISO
FONT=ISO

# ----- DOS compiler options ------------------------------------------------
DOS_CPP=cl
DOS_LINK=link
DOS_LIBRARIAN=lib

DOS_CPP_OPTS=-c -AL -BATCH -Gs -Os -D$(FONT) -nologo
DOS_LINK_OPTS=/NOD /NOI /BATCH /SEGMENTS:256 /STACK:5120 /EXE /FAR /ON:N
DOS_LIB_OPTS=/NOI

# --- Use the next line for UI_GRAPHICS_DISPLAY ---
DOS_OBJS=txtonly.obj
DOS_LIBS=dos_zil dos_gfx ms_lgfx llibce graphics
# --- Use the next line for UI_MSC_DISPLAY ---
#DOS_OBJS=
#DOS_LIBS=dos_zil dos_msc llibce graphics

.cpp.obj:
	$(DOS_CPP) $(DOS_CPP_OPTS) $<

# ----- Windows compiler options --------------------------------------------
WIN_CPP=cl
WIN_LINK=link
WIN_LIBRARIAN=lib
WIN_RC=rc

# --- Use the next 2 lines for Windows 3.0 compatibility ---
WIN_CPP_OPTS=-c -AL -BATCH -Gsw -Os -DWINVER=0x0300 -nologo
WIN_RC_OPTS=-k -30
# --- Use the next 2 lines for Windows 3.1 compatibility ---
#WIN_CPP_OPTS=-c -AL -BATCH -Gsw -Os -nologo
#WIN_RC_OPTS=-k
WIN_LINK_OPTS=/NOD /NOI /BATCH /SEGMENTS:256 /ON:N
WIN_LIB_OPTS=/NOI
WIN_OBJS=
WIN_LIBS=win_zil libw llibcew

.cpp.obw:
	$(WIN_CPP) $(WIN_CPP_OPTS) -Fo$*.obw $<

# ----- Windows NT compiler options -----------------------------------------
WINNT_CPP=$(cc)
WINNT_LINK=$(link)
WINNT_LIBRARIAN=lib

WINNT_CPP_OPTS=$(cflags) $(cvars)
WINNT_LINK_OPTS=$(guiflags)
#WINNT_LINK_OPTS=$(conflags)
WINNT_LIB_OPTS=/machine:i386 /subsystem:WINDOWS

# --- debug options ---
#WINNT_CPP_OPTS=$(cflags) $(cvars) /Zi
#WINNT_LINK_OPTS=$(guiflags) /DEBUG:MAPPED,FULL /DEBUGTYPE:CV
#WINNT_LIB_OPTS=/machine:i386 /subsystem:CONSOLE

WINNT_OBJS=
WINNT_LIBS=$(guilibs) wnt_zil.lib
#WINNT_LIBS=$(conlibs) wnt_zil.lib

.SUFFIXES : .cpp

.cpp.obn:
	$(WINNT_CPP) $(WINNT_CPP_OPTS) -Fo$*.obn $<

# ----- Windows DLL compiler options ----------------------------------------
WINDLL_CPP_OPTS=-c -AL -BATCH -Gsw -DWINVER=0x0300 -D__DLL__ -nologo
WINDLL_LINK_OPTS=/NOD /NOI /BATCH /SEGMENTS:256 /STACK:5120 /FAR /Ld /ON:N
WINDLL_LIB_OPTS=/NOI
WINDLL_OBJS=
WINDLL_LIBS=wdll_zil llibcew

.cpp.obd:
	$(WIN_CPP) $(WINDLL_CPP_OPTS) -Fo$*.obd $<

# ----- Dos Extender ---------------------------------------------------------
D16_CPP=cl
D16_LINK=link
D16_LIBRARIAN=lib

D16_CPP_OPTS=-c -AL -BATCH -Gs2 -DDOSX286 -D$(FONT) -nologo
D16_LINK_OPTS=/NOD /NOI /BATCH /SEGMENTS:256 /STACK:5120 /FAR /ON:N
D16_LIB_OPTS=/NOI

D16_BIND=bind286
PHARLAP_RTL=c:\phar286\rtk
D16_LOAD=run286a

# --- Use the next line for UI_GRAPHICS_DISPLAY ---
D16_OBJS=graphp.obj			# don't use txtonly.obj here
D16_LIBS=phapi d16_zil d16_gfx ms_16gfx llibcep graphics
# --- Use the next lines for UI_MSC_DISPLAY ---
#D16_OBJS=graphp.obj
#D16_LIBS=phapi llibcep graphics d16_msc d16_zil

.cpp.o16:
	$(D16_CPP) $(D16_CPP_OPTS) -Fo$*.o16 $<

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
	copy gfx\source\gfx.h ..\include
	copy gfx\source\gfx_pro.h ..\include

# ----- DOS Libraries and Programs ------------------------------------------
dos: copy dos_msc.lib dos_gfx.lib test.exe

test.exe: test.obj dos_zil.lib
	$(DOS_LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+test.obj
$*,NUL,$(DOS_LIBS),NUL
<<

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
	i_mscat.obj i_mapfon.obj
	-@del dos_zil.lib
	$(DOS_LIBRARIAN) $(DOS_LIB_OPTS) @<<zil.rsp
$* &
+d_tdsp.obj+d_keybrd.obj+d_mouse.obj+d_cursor.obj &
+d_border.obj+d_button.obj+d_combo.obj+d_group.obj+d_hlist.obj &
+d_icon.obj+d_min.obj+d_max.obj+d_scroll.obj+d_string.obj+d_sys.obj &
+d_text.obj+d_title.obj+d_popup.obj+d_popup1.obj+d_plldn.obj &
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
+i_mscat.obj+i_mapfon.obj
NUL
<<
	copy dos_zil.lib ..\lib\$(VERSION)

dos_msc.lib : d_mscdsp.obj g_mscdsp.obj z_appmsc.obj
	-@del dos_msc.lib
	$(DOS_LIBRARIAN) $(DOS_LIB_OPTS) @<<zil.rsp
$* &
+d_mscdsp.obj+g_mscdsp.obj+z_appmsc.obj
NUL
<<
	copy dos_msc.lib ..\lib\$(VERSION)

dos_gfx.lib : d_gfxdsp.obj $(FONT)_smal.obj $(FONT)_dial.obj $(FONT)_syst.obj z_appgfx.obj
	-@del dos_gfx.lib
	$(DOS_LIBRARIAN) $(DOS_LIB_OPTS) @<<zil.rsp
$* &
+d_gfxdsp.obj+$(FONT)_smal.obj+$(FONT)_dial.obj+$(FONT)_syst.obj+z_appgfx.obj
NUL
<<
	copy dos_gfx.lib ..\lib\$(VERSION)

ms_lgfx.lib:
	@cd gfx\source
	nmake -f microsft.mak dos
	@cd ..\..

# ----- Windows Libraries and Programs --------------------------------------
windows: copy wtest.exe

wtest.exe: test.obw win_zil.lib
	$(WIN_LINK) $(WIN_LINK_OPTS) @<<zil.rsp
$(WIN_OBJS)+test.obw
$*,NUL,$(WIN_LIBS),wtest.def
<<
	$(WIN_RC) $(WIN_RC_OPTS) wtest.rc $*.exe

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
	$(WIN_LIBRARIAN) $(WIN_LIB_OPTS) @<<zil.rsp
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
NUL
<<
	copy win_zil.lib ..\lib\$(VERSION)

# ----- OS/2 Libraries and Programs -----------------------------------------
os2:
	@echo Microsoft does not support OS/2 libraries and programs.


# ----- DOS Extender Libraries and Programs ----------------------------------
dos16: copy d16_gfx.lib d16_msc.lib test16.exe

test16.exe: test.o16 d16_zil.lib
	$(D16_LINK) $(D16_LINK_OPTS) @<<zil.rsp
$(D16_OBJS)+test.o16
$*,NUL,$(D16_LIBS),NUL
<<
	$(D16_BIND) @<<zil.rsp
$(PHARLAP_RTL)\$(D16_LOAD)
$*
-dll $(PHARLAP_RTL)\moucalls
     $(PHARLAP_RTL)\int33
     $(PHARLAP_RTL)\doscalls
<<

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
	i_mscat.o16 i_mapfon.o16
	-@del d16_zil.lib
	$(D16_LIBRARIAN) $(D16_LIB_OPTS) @<<zil.rsp
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
+i_mscat.o16+i_mapfon.o16
NUL
<<
	copy d16_zil.lib ..\lib\$(VERSION)

d16_msc.lib : d_mscdsp.o16 g_mscdsp.o16 z_appmsc.o16
	-@del d16_msc.lib
	$(D16_LIBRARIAN) $(D16_LIB_OPTS) @<<zil.rsp
$* &
+d_mscdsp.o16+g_mscdsp.o16+z_appmsc.o16
NUL
<<
	copy d16_msc.lib ..\lib\$(VERSION)

d16_gfx.lib : d_gfxdsp.o16 $(FONT)_smal.o16 $(FONT)_dial.o16 $(FONT)_syst.o16 z_appgfx.o16
	-@del d16_gfx.lib
	$(D16_LIBRARIAN) $(D16_LIB_OPTS) @<<zil.rsp
$* &
+d_gfxdsp.o16+$(FONT)_smal.o16+$(FONT)_dial.o16+$(FONT)_syst.o16+z_appgfx.o16
NUL
<<
	copy d16_gfx.lib ..\lib\$(VERSION)

ms_16gfx.lib:
	@cd gfx\source
	nmake -f microsft.mak dos16
	@cd ..\..

# --- Done with libraries ----------------------------------------------------

z_appmsc.obj: z_app.cpp
	$(DOS_CPP) -DMSC $(DOS_CPP_OPTS) -Fo$@ $?

z_appgfx.obj: z_app.cpp
	$(DOS_CPP) -DGFX $(DOS_CPP_OPTS) -Fo$@ $?

z_appmsc.o16: z_app.cpp
	$(D16_CPP) -DMSC $(D16_CPP_OPTS) -Fo$@ $?

z_appgfx.o16: z_app.cpp
	$(D16_CPP) -DGFX $(D16_CPP_OPTS) -Fo$@ $?

z_apptxt.obj: z_app.cpp
	$(DOS_CPP) -DTEXT $(DOS_CPP_OPTS) -Fo$@ $?

$(FONT)_dial.cpp: gfx\$(FONT)_dial.cpp
	copy gfx\$(FONT)_dial.cpp

$(FONT)_smal.cpp: gfx\$(FONT)_smal.cpp
	copy gfx\$(FONT)_smal.cpp

$(FONT)_syst.cpp: gfx\$(FONT)_syst.cpp
	copy gfx\$(FONT)_syst.cpp

