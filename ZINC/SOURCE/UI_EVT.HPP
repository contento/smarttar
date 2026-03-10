//	Zinc Interface Library - UI_EVT.HPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if !defined(UI_EVT_HPP)
#	define UI_EVT_HPP
#	if !defined(UI_DSP_HPP)
#		if defined(__OS2__)
#			if !defined(INCL_WININPUT)
#				define INCL_WININPUT
#			endif
#			if !defined(INCL_WINMESSAGEMGR)
#				define INCL_WINMESSAGEMGR
#			endif
#		endif
#		include <ui_dsp.hpp>
#	endif

// ----- Compiler/Environment Dependencies ----------------------------------

#if defined(ZIL_MSDOS)
	typedef unsigned MSG;
	typedef unsigned RAW_CODE;
	typedef int EVENT_TYPE;
#elif defined(ZIL_MSWINDOWS) && defined(WIN32)
	typedef unsigned RAW_CODE;
	typedef LRESULT EVENT_TYPE;
#elif defined(ZIL_MSWINDOWS)
	typedef unsigned RAW_CODE;
	typedef long EVENT_TYPE;
#elif defined(ZIL_OS2)
	typedef unsigned RAW_CODE;
	typedef long EVENT_TYPE;
#elif defined(ZIL_MOTIF)
	typedef XEvent MSG;
	typedef unsigned long RAW_CODE;
	typedef long EVENT_TYPE;
#endif

// --- Version 2.0 and 1.0 compatibility ---
#define UI_CURSOR			UID_CURSOR
#define UI_BIOS_KEYBOARD	UID_KEYBOARD
#define UI_MS_MOUSE			UID_MOUSE
#define S_SCROLL_VERTICAL	S_VSCROLL
#define S_SCROLL_HORIZONTAL	S_HSCROLL
#define S_SCROLL_VERTICAL_SET	S_VSCROLL_SET
#define S_SCROLL_HORIZONTAL_SET	S_HSCROLL_SET
#define S_DELETE			S_CLOSE
#define S_DELETE_LEVEL		S_CLOSE_TEMPORARY
#define S_CLEAR				S_DEINITIALIZE
#define S_DEFINE_REGION		S_REGION_DEFINE
#define S_CANCEL			L_CANCEL
#define L_ALT_KEY			S_ALT_KEY
#define S_RESTORE			L_RESTORE
#define S_MINIMIZE			L_MINIMIZE
#define S_MAXIMIZE			L_MAXIMIZE

// --- value ---
#if defined(USE_RAW_KEYS)
#if defined(ZIL_MSDOS)
const RAW_CODE ESCAPE					= 0x011B;	// Miscellaneous keys
const RAW_CODE ENTER					= 0x1C0D;
const RAW_CODE SPACE					= 0x3920;
const RAW_CODE BACKSPACE				= 0x0E08;
const RAW_CODE CTRL_BACKSPACE			= 0x0E7F;
const RAW_CODE ALT_BACKSPACE			= 0x0E00;
const RAW_CODE TAB						= 0x0F09;
const RAW_CODE CTRL_TAB					= 0x9400;
const RAW_CODE BACKTAB					= 0x0F00;

const RAW_CODE CTRL_BREAK				= 0x0000;
const RAW_CODE CTRL_C					= 0x2E03;

const RAW_CODE ALT_ESCAPE				= 0x0100;
const RAW_CODE ALT_PERIOD				= 0x3400;
const RAW_CODE ALT_SPACE				= 0x3920;
const RAW_CODE ALT_WHITE_MINUS			= 0x8200;
const RAW_CODE ALT_WHITE_PLUS			= 0x8300;

const RAW_CODE ALT_A					= 0x1E00;
const RAW_CODE ALT_B					= 0x3000;
const RAW_CODE ALT_C					= 0x2E00;
const RAW_CODE ALT_D					= 0x2000;
const RAW_CODE ALT_E					= 0x1200;
const RAW_CODE ALT_F					= 0x2100;
const RAW_CODE ALT_G					= 0x2200;
const RAW_CODE ALT_H					= 0x2300;
const RAW_CODE ALT_I					= 0x1700;
const RAW_CODE ALT_J					= 0x2400;
const RAW_CODE ALT_K					= 0x2500;
const RAW_CODE ALT_L					= 0x2600;
const RAW_CODE ALT_M					= 0x3200;
const RAW_CODE ALT_N					= 0x3100;
const RAW_CODE ALT_O					= 0x1800;
const RAW_CODE ALT_P					= 0x1900;
const RAW_CODE ALT_Q					= 0x1000;
const RAW_CODE ALT_R					= 0x1300;
const RAW_CODE ALT_S					= 0x1F00;
const RAW_CODE ALT_T					= 0x1400;
const RAW_CODE ALT_U					= 0x1600;
const RAW_CODE ALT_V					= 0x2F00;
const RAW_CODE ALT_W					= 0x1100;
const RAW_CODE ALT_X					= 0x2D00;
const RAW_CODE ALT_Y					= 0x1500;
const RAW_CODE ALT_Z					= 0x2C00;

const RAW_CODE ALT_1					= 0x7800;
const RAW_CODE ALT_2					= 0x7900;
const RAW_CODE ALT_3					= 0x7A00;
const RAW_CODE ALT_4					= 0x7B00;
const RAW_CODE ALT_5					= 0x7C00;
const RAW_CODE ALT_6					= 0x7D00;
const RAW_CODE ALT_7					= 0x7E00;
const RAW_CODE ALT_8					= 0x7F00;
const RAW_CODE ALT_9					= 0x8000;
const RAW_CODE ALT_0					= 0x8100;

const RAW_CODE GRAY_ENTER				= 0xE00D;	// Grey keys
const RAW_CODE GRAY_UP_ARROW			= 0x48E0;
const RAW_CODE GRAY_DOWN_ARROW			= 0x50E0;
const RAW_CODE GRAY_LEFT_ARROW			= 0x4BE0;
const RAW_CODE GRAY_RIGHT_ARROW			= 0x4DE0;
const RAW_CODE GRAY_INSERT				= 0x52E0;
const RAW_CODE GRAY_DELETE				= 0x53E0;
const RAW_CODE GRAY_HOME				= 0x47E0;
const RAW_CODE GRAY_END					= 0x4FE0;
const RAW_CODE GRAY_PGUP				= 0x49E0;
const RAW_CODE GRAY_PGDN				= 0x51E0;
const RAW_CODE GRAY_DIVIDE				= 0xE02F;
const RAW_CODE GRAY_MULTIPLY			= 0x372A;
const RAW_CODE GRAY_PLUS				= 0x4E2B;
const RAW_CODE GRAY_MINUS				= 0x4A2D;

const RAW_CODE CTRL_GRAY_UP_ARROW		= 0x8DE0;
const RAW_CODE CTRL_GRAY_DOWN_ARROW		= 0x91E0;
const RAW_CODE CTRL_GRAY_LEFT_ARROW		= 0x73E0;
const RAW_CODE CTRL_GRAY_RIGHT_ARROW	= 0x74E0;
const RAW_CODE CTRL_GRAY_INSERT			= 0x92E0;
const RAW_CODE CTRL_GRAY_DELETE			= 0x93E0;
const RAW_CODE CTRL_GRAY_HOME			= 0x77E0;
const RAW_CODE CTRL_GRAY_END			= 0x75E0;
const RAW_CODE CTRL_GRAY_PGUP			= 0x84E0;
const RAW_CODE CTRL_GRAY_PGDN			= 0x76E0;
const RAW_CODE CTRL_GRAY_DIVIDE			= 0x9500;
const RAW_CODE CTRL_GRAY_MULTIPLY		= 0x9600;
const RAW_CODE CTRL_GRAY_PLUS			= 0x9000;
const RAW_CODE CTRL_GRAY_MINUS			= 0x8E00;

const RAW_CODE ALT_GRAY_UP_ARROW		= 0x9800;
const RAW_CODE ALT_GRAY_DOWN_ARROW		= 0xA000;
const RAW_CODE ALT_GRAY_LEFT_ARROW		= 0x9B00;
const RAW_CODE ALT_GRAY_RIGHT_ARROW		= 0x9D00;
const RAW_CODE ALT_GRAY_INSERT			= 0xA200;
const RAW_CODE ALT_GRAY_DELETE			= 0xA300;
const RAW_CODE ALT_GRAY_HOME			= 0x9700;
const RAW_CODE ALT_GRAY_END				= 0x9F00;
const RAW_CODE ALT_GRAY_PGUP			= 0x9900;
const RAW_CODE ALT_GRAY_PGDN			= 0xA100;
const RAW_CODE ALT_GRAY_DIVIDE			= 0xA400;
const RAW_CODE ALT_GRAY_MULTIPLY		= 0x3700;
const RAW_CODE ALT_GRAY_PLUS			= 0x4E00;
const RAW_CODE ALT_GRAY_MINUS			= 0x4A00;

const RAW_CODE WHITE_UP_ARROW			= 0x4800;	// White keys
const RAW_CODE WHITE_DOWN_ARROW			= 0x5000;
const RAW_CODE WHITE_LEFT_ARROW			= 0x4B00;
const RAW_CODE WHITE_RIGHT_ARROW		= 0x4D00;
const RAW_CODE WHITE_INSERT				= 0x5200;
const RAW_CODE WHITE_DELETE				= 0x5300;
const RAW_CODE WHITE_HOME				= 0x4700;
const RAW_CODE WHITE_END				= 0x4F00;
const RAW_CODE WHITE_PGUP				= 0x4900;
const RAW_CODE WHITE_PGDN				= 0x5100;
const RAW_CODE WHITE_CENTER				= 0x4C00;

const RAW_CODE CTRL_WHITE_UP_ARROW		= 0x8D00;
const RAW_CODE CTRL_WHITE_DOWN_ARROW	= 0x9100;
const RAW_CODE CTRL_WHITE_LEFT_ARROW	= 0x7300;
const RAW_CODE CTRL_WHITE_RIGHT_ARROW	= 0x7400;
const RAW_CODE CTRL_WHITE_INSERT		= 0x9200;
const RAW_CODE CTRL_WHITE_DELETE		= 0x9300;
const RAW_CODE CTRL_WHITE_HOME			= 0x7700;
const RAW_CODE CTRL_WHITE_END			= 0x7500;
const RAW_CODE CTRL_WHITE_PGUP			= 0x8400;
const RAW_CODE CTRL_WHITE_PGDN			= 0x7600;
const RAW_CODE CTRL_WHITE_CENTER		= 0x8F00;

const RAW_CODE F1						= 0x3B00;	// Function keys
const RAW_CODE F2						= 0x3C00;
const RAW_CODE F3						= 0x3D00;
const RAW_CODE F4						= 0x3E00;
const RAW_CODE F5						= 0x3F00;
const RAW_CODE F6						= 0x4000;
const RAW_CODE F7						= 0x4100;
const RAW_CODE F8						= 0x4200;
const RAW_CODE F9						= 0x4300;
const RAW_CODE F10						= 0x4400;
const RAW_CODE F11						= 0x8500;
const RAW_CODE F12						= 0x8600;

const RAW_CODE SHIFT_F1					= 0x5400;
const RAW_CODE SHIFT_F2					= 0x5500;
const RAW_CODE SHIFT_F3					= 0x5600;
const RAW_CODE SHIFT_F4					= 0x5700;
const RAW_CODE SHIFT_F5					= 0x5800;
const RAW_CODE SHIFT_F6					= 0x5900;
const RAW_CODE SHIFT_F7					= 0x5A00;
const RAW_CODE SHIFT_F8					= 0x5B00;
const RAW_CODE SHIFT_F9					= 0x5C00;
const RAW_CODE SHIFT_F10				= 0x5D00;
const RAW_CODE SHIFT_F11				= 0x8700;
const RAW_CODE SHIFT_F12				= 0x8800;

const RAW_CODE CTRL_F1					= 0x5E00;
const RAW_CODE CTRL_F2					= 0x5F00;
const RAW_CODE CTRL_F3					= 0x6000;
const RAW_CODE CTRL_F4					= 0x6100;
const RAW_CODE CTRL_F5					= 0x6200;
const RAW_CODE CTRL_F6					= 0x6300;
const RAW_CODE CTRL_F7					= 0x6400;
const RAW_CODE CTRL_F8					= 0x6500;
const RAW_CODE CTRL_F9					= 0x6600;
const RAW_CODE CTRL_F10					= 0x6700;
const RAW_CODE CTRL_F11					= 0x8900;
const RAW_CODE CTRL_F12					= 0x8A00;

const RAW_CODE ALT_F1					= 0x6800;
const RAW_CODE ALT_F2					= 0x6900;
const RAW_CODE ALT_F3					= 0x6A00;
const RAW_CODE ALT_F4					= 0x6B00;
const RAW_CODE ALT_F5					= 0x6C00;
const RAW_CODE ALT_F6					= 0x6D00;
const RAW_CODE ALT_F7					= 0x6E00;
const RAW_CODE ALT_F8					= 0x6F00;
const RAW_CODE ALT_F9					= 0x7000;
const RAW_CODE ALT_F10					= 0x7100;
const RAW_CODE ALT_F11					= 0x8B00;
const RAW_CODE ALT_F12					= 0x8C00;
#elif defined(ZIL_MSWINDOWS)
const RAW_CODE ESCAPE					= 0x001B;	// Miscellaneous keys
const RAW_CODE ENTER					= 0x000D;
const RAW_CODE SPACE					= 0x0020;
const RAW_CODE BACKSPACE				= 0x0008;
const RAW_CODE CTRL_BACKSPACE			= 0x0408;
const RAW_CODE ALT_BACKSPACE			= 0x0808;
const RAW_CODE TAB						= 0x0009;
const RAW_CODE CTRL_TAB					= 0x0409;
const RAW_CODE BACKTAB					= 0x0309;

const RAW_CODE CTRL_BREAK				= 0x0403;
const RAW_CODE CTRL_C					= 0x0443;

const RAW_CODE ALT_ESCAPE				= 0x081B;
const RAW_CODE ALT_PERIOD				= 0x08BE;
const RAW_CODE ALT_SPACE				= 0x0820;
const RAW_CODE ALT_WHITE_MINUS			= 0x08BD;
const RAW_CODE ALT_WHITE_PLUS			= 0x08BB;

const RAW_CODE ALT_A					= 0x0861;
const RAW_CODE ALT_B					= 0x0862;
const RAW_CODE ALT_C					= 0x0863;
const RAW_CODE ALT_D					= 0x0864;
const RAW_CODE ALT_E					= 0x0865;
const RAW_CODE ALT_F					= 0x0866;
const RAW_CODE ALT_G					= 0x0867;
const RAW_CODE ALT_H					= 0x0868;
const RAW_CODE ALT_I					= 0x0869;
const RAW_CODE ALT_J					= 0x086A;
const RAW_CODE ALT_K					= 0x086B;
const RAW_CODE ALT_L					= 0x086C;
const RAW_CODE ALT_M					= 0x086D;
const RAW_CODE ALT_N					= 0x086E;
const RAW_CODE ALT_O					= 0x086F;
const RAW_CODE ALT_P					= 0x0870;
const RAW_CODE ALT_Q					= 0x0871;
const RAW_CODE ALT_R					= 0x0872;
const RAW_CODE ALT_S					= 0x0873;
const RAW_CODE ALT_T					= 0x0874;
const RAW_CODE ALT_U					= 0x0875;
const RAW_CODE ALT_V					= 0x0876;
const RAW_CODE ALT_W					= 0x0877;
const RAW_CODE ALT_X					= 0x0878;
const RAW_CODE ALT_Y					= 0x0879;
const RAW_CODE ALT_Z					= 0x087A;

const RAW_CODE ALT_1					= 0x0831;
const RAW_CODE ALT_2					= 0x0832;
const RAW_CODE ALT_3					= 0x0833;
const RAW_CODE ALT_4					= 0x0834;
const RAW_CODE ALT_5					= 0x0835;
const RAW_CODE ALT_6					= 0x0836;
const RAW_CODE ALT_7					= 0x0837;
const RAW_CODE ALT_8					= 0x0838;
const RAW_CODE ALT_9					= 0x0839;
const RAW_CODE ALT_0					= 0x0840;

const RAW_CODE GRAY_ENTER				= 0x000D;	// Grey keys
const RAW_CODE GRAY_UP_ARROW			= 0x0026;
const RAW_CODE GRAY_DOWN_ARROW			= 0x0028;
const RAW_CODE GRAY_LEFT_ARROW			= 0x0025;
const RAW_CODE GRAY_RIGHT_ARROW			= 0x0027;
const RAW_CODE GRAY_INSERT				= 0x002D;
const RAW_CODE GRAY_DELETE				= 0x002E;
const RAW_CODE GRAY_HOME				= 0x0024;
const RAW_CODE GRAY_END					= 0x0023;
const RAW_CODE GRAY_PGUP				= 0x0021;
const RAW_CODE GRAY_PGDN				= 0x0022;
const RAW_CODE GRAY_DIVIDE				= 0x006F;
const RAW_CODE GRAY_MULTIPLY			= 0x006A;
const RAW_CODE GRAY_PLUS				= 0x006B;
const RAW_CODE GRAY_MINUS				= 0x006D;

const RAW_CODE CTRL_GRAY_UP_ARROW		= 0x0426;
const RAW_CODE CTRL_GRAY_DOWN_ARROW		= 0x0428;
const RAW_CODE CTRL_GRAY_LEFT_ARROW		= 0x0425;
const RAW_CODE CTRL_GRAY_RIGHT_ARROW	= 0x0427;
const RAW_CODE CTRL_GRAY_INSERT			= 0x042D;
const RAW_CODE CTRL_GRAY_DELETE			= 0x042E;
const RAW_CODE CTRL_GRAY_HOME			= 0x0424;
const RAW_CODE CTRL_GRAY_END			= 0x0423;
const RAW_CODE CTRL_GRAY_PGUP			= 0x0421;
const RAW_CODE CTRL_GRAY_PGDN			= 0x0422;
const RAW_CODE CTRL_GRAY_DIVIDE			= 0x046F;
const RAW_CODE CTRL_GRAY_MULTIPLY		= 0x046A;
const RAW_CODE CTRL_GRAY_PLUS			= 0x046B;
const RAW_CODE CTRL_GRAY_MINUS			= 0x046D;

const RAW_CODE ALT_GRAY_UP_ARROW		= 0x0826;
const RAW_CODE ALT_GRAY_DOWN_ARROW		= 0x0828;
const RAW_CODE ALT_GRAY_LEFT_ARROW		= 0x0825;
const RAW_CODE ALT_GRAY_RIGHT_ARROW		= 0x0827;
const RAW_CODE ALT_GRAY_INSERT			= 0x082D;
const RAW_CODE ALT_GRAY_DELETE			= 0x082E;
const RAW_CODE ALT_GRAY_HOME			= 0x0824;
const RAW_CODE ALT_GRAY_END				= 0x0823;
const RAW_CODE ALT_GRAY_PGUP			= 0x0821;
const RAW_CODE ALT_GRAY_PGDN			= 0x0822;
const RAW_CODE ALT_GRAY_DIVIDE			= 0x086F;
const RAW_CODE ALT_GRAY_MULTIPLY		= 0x086A;
const RAW_CODE ALT_GRAY_PLUS			= 0x086B;
const RAW_CODE ALT_GRAY_MINUS			= 0x086D;

const RAW_CODE WHITE_UP_ARROW			= 0x0026;	// White keys
const RAW_CODE WHITE_DOWN_ARROW			= 0x0028;
const RAW_CODE WHITE_LEFT_ARROW			= 0x0025;
const RAW_CODE WHITE_RIGHT_ARROW		= 0x0027;
const RAW_CODE WHITE_INSERT				= 0x002D;
const RAW_CODE WHITE_DELETE				= 0x002E;
const RAW_CODE WHITE_HOME				= 0x0024;
const RAW_CODE WHITE_END				= 0x0023;
const RAW_CODE WHITE_PGUP				= 0x0021;
const RAW_CODE WHITE_PGDN				= 0x0022;
const RAW_CODE WHITE_CENTER				= 0x002A;

const RAW_CODE CTRL_WHITE_UP_ARROW		= 0x0426;
const RAW_CODE CTRL_WHITE_DOWN_ARROW	= 0x0428;
const RAW_CODE CTRL_WHITE_LEFT_ARROW	= 0x0425;
const RAW_CODE CTRL_WHITE_RIGHT_ARROW	= 0x0427;
const RAW_CODE CTRL_WHITE_INSERT		= 0x042D;
const RAW_CODE CTRL_WHITE_DELETE		= 0x042E;
const RAW_CODE CTRL_WHITE_HOME			= 0x0424;
const RAW_CODE CTRL_WHITE_END			= 0x0423;
const RAW_CODE CTRL_WHITE_PGUP			= 0x0421;
const RAW_CODE CTRL_WHITE_PGDN			= 0x0422;
const RAW_CODE CTRL_WHITE_CENTER		= 0x042A;

const RAW_CODE F1						= 0x0070;	// Function keys
const RAW_CODE F2						= 0x0071;
const RAW_CODE F3						= 0x0072;
const RAW_CODE F4						= 0x0073;
const RAW_CODE F5						= 0x0074;
const RAW_CODE F6						= 0x0075;
const RAW_CODE F7						= 0x0076;
const RAW_CODE F8						= 0x0077;
const RAW_CODE F9						= 0x0078;
const RAW_CODE F10						= 0x0079;
const RAW_CODE F11						= 0x007A;
const RAW_CODE F12						= 0x007B;

const RAW_CODE SHIFT_F1					= 0x0370;
const RAW_CODE SHIFT_F2					= 0x0371;
const RAW_CODE SHIFT_F3					= 0x0372;
const RAW_CODE SHIFT_F4					= 0x0373;
const RAW_CODE SHIFT_F5					= 0x0374;
const RAW_CODE SHIFT_F6					= 0x0375;
const RAW_CODE SHIFT_F7					= 0x0376;
const RAW_CODE SHIFT_F8					= 0x0377;
const RAW_CODE SHIFT_F9					= 0x0378;
const RAW_CODE SHIFT_F10				= 0x0379;
const RAW_CODE SHIFT_F11				= 0x037A;
const RAW_CODE SHIFT_F12				= 0x037B;

const RAW_CODE CTRL_F1					= 0x0470;
const RAW_CODE CTRL_F2					= 0x0471;
const RAW_CODE CTRL_F3					= 0x0472;
const RAW_CODE CTRL_F4					= 0x0473;
const RAW_CODE CTRL_F5					= 0x0474;
const RAW_CODE CTRL_F6					= 0x0475;
const RAW_CODE CTRL_F7					= 0x0476;
const RAW_CODE CTRL_F8					= 0x0477;
const RAW_CODE CTRL_F9					= 0x0478;
const RAW_CODE CTRL_F10					= 0x0479;
const RAW_CODE CTRL_F11					= 0x047A;
const RAW_CODE CTRL_F12					= 0x047B;

const RAW_CODE ALT_F1					= 0x0870;
const RAW_CODE ALT_F2					= 0x0871;
const RAW_CODE ALT_F3					= 0x0872;
const RAW_CODE ALT_F4					= 0x0873;
const RAW_CODE ALT_F5					= 0x0874;
const RAW_CODE ALT_F6					= 0x0875;
const RAW_CODE ALT_F7					= 0x0876;
const RAW_CODE ALT_F8					= 0x0877;
const RAW_CODE ALT_F9					= 0x0878;
const RAW_CODE ALT_F10					= 0x0879;
const RAW_CODE ALT_F11					= 0x087A;
const RAW_CODE ALT_F12					= 0x087B;
#elif defined(ZIL_OS2)
const RAW_CODE ESCAPE					= 0x001B;	// down-action scan code
const RAW_CODE ENTER					= 0x000D;	// down-action scan code
const RAW_CODE SPACE					= 0x3920;	// up-action scan code
const RAW_CODE BACKSPACE				= 0x0E08;	// up-action scan code
const RAW_CODE CTRL_BACKSPACE			= 0x0408;	// down-action scan code
const RAW_CODE ALT_BACKSPACE			= 0x0808;	// down-action scan code
const RAW_CODE TAB						= 0x0009;	// down-action scan code
const RAW_CODE CTRL_TAB					= 0x0409;	// down-action scan code
const RAW_CODE BACKTAB					= 0x0309;	// down-action scan code

const RAW_CODE CTRL_BREAK				= 0x0000;	// down-action scan code
const RAW_CODE CTRL_C					= 0x2E03;	// down-action scan code

const RAW_CODE ALT_ESCAPE				= 0x0100;	// down-action scan code
const RAW_CODE ALT_PERIOD				= 0x3400;
const RAW_CODE ALT_SPACE				= 0x3920;
const RAW_CODE ALT_WHITE_MINUS			= 0x8200;
const RAW_CODE ALT_WHITE_PLUS			= 0x8300;

const RAW_CODE ALT_A					= 0x1E00;	// down-action scan codes
const RAW_CODE ALT_B					= 0x3000;
const RAW_CODE ALT_C					= 0x2E00;
const RAW_CODE ALT_D					= 0x2000;
const RAW_CODE ALT_E					= 0x1200;
const RAW_CODE ALT_F					= 0x2100;
const RAW_CODE ALT_G					= 0x2200;
const RAW_CODE ALT_H					= 0x2300;
const RAW_CODE ALT_I					= 0x1700;
const RAW_CODE ALT_J					= 0x2400;
const RAW_CODE ALT_K					= 0x2500;
const RAW_CODE ALT_L					= 0x2600;
const RAW_CODE ALT_M					= 0x3200;
const RAW_CODE ALT_N					= 0x3100;
const RAW_CODE ALT_O					= 0x1800;
const RAW_CODE ALT_P					= 0x1900;
const RAW_CODE ALT_Q					= 0x1000;
const RAW_CODE ALT_R					= 0x1300;
const RAW_CODE ALT_S					= 0x1F00;
const RAW_CODE ALT_T					= 0x1400;
const RAW_CODE ALT_U					= 0x1600;
const RAW_CODE ALT_V					= 0x2F00;
const RAW_CODE ALT_W					= 0x1100;
const RAW_CODE ALT_X					= 0x2D00;
const RAW_CODE ALT_Y					= 0x1500;
const RAW_CODE ALT_Z					= 0x2C00;

const RAW_CODE ALT_1					= 0x7800;	// down-action scan codes
const RAW_CODE ALT_2					= 0x7900;
const RAW_CODE ALT_3					= 0x7A00;
const RAW_CODE ALT_4					= 0x7B00;
const RAW_CODE ALT_5					= 0x7C00;
const RAW_CODE ALT_6					= 0x7D00;
const RAW_CODE ALT_7					= 0x7E00;
const RAW_CODE ALT_8					= 0x7F00;
const RAW_CODE ALT_9					= 0x8000;
const RAW_CODE ALT_0					= 0x8100;

const RAW_CODE GRAY_ENTER				= 0xE00D;	// Grey keys
const RAW_CODE GRAY_UP_ARROW			= 0x48E0;
const RAW_CODE GRAY_DOWN_ARROW			= 0x50E0;
const RAW_CODE GRAY_LEFT_ARROW			= 0x4BE0;
const RAW_CODE GRAY_RIGHT_ARROW			= 0x4DE0;
const RAW_CODE GRAY_INSERT				= 0x52E0;
const RAW_CODE GRAY_DELETE				= 0x53E0;
const RAW_CODE GRAY_HOME				= 0x47E0;
const RAW_CODE GRAY_END					= 0x4FE0;
const RAW_CODE GRAY_PGUP				= 0x49E0;
const RAW_CODE GRAY_PGDN				= 0x51E0;
const RAW_CODE GRAY_DIVIDE				= 0xE02F;
const RAW_CODE GRAY_MULTIPLY			= 0x372A;
const RAW_CODE GRAY_PLUS				= 0x4E2B;
const RAW_CODE GRAY_MINUS				= 0x4A2D;

const RAW_CODE CTRL_GRAY_UP_ARROW		= 0x8DE0;
const RAW_CODE CTRL_GRAY_DOWN_ARROW		= 0x91E0;
const RAW_CODE CTRL_GRAY_LEFT_ARROW		= 0x73E0;
const RAW_CODE CTRL_GRAY_RIGHT_ARROW	= 0x74E0;
const RAW_CODE CTRL_GRAY_INSERT			= 0x92E0;
const RAW_CODE CTRL_GRAY_DELETE			= 0x93E0;
const RAW_CODE CTRL_GRAY_HOME			= 0x77E0;
const RAW_CODE CTRL_GRAY_END			= 0x75E0;
const RAW_CODE CTRL_GRAY_PGUP			= 0x84E0;
const RAW_CODE CTRL_GRAY_PGDN			= 0x76E0;
const RAW_CODE CTRL_GRAY_DIVIDE			= 0x9500;
const RAW_CODE CTRL_GRAY_MULTIPLY		= 0x9600;
const RAW_CODE CTRL_GRAY_PLUS			= 0x9000;
const RAW_CODE CTRL_GRAY_MINUS			= 0x8E00;

const RAW_CODE ALT_GRAY_UP_ARROW		= 0x9800;
const RAW_CODE ALT_GRAY_DOWN_ARROW		= 0xA000;
const RAW_CODE ALT_GRAY_LEFT_ARROW		= 0x9B00;
const RAW_CODE ALT_GRAY_RIGHT_ARROW		= 0x9D00;
const RAW_CODE ALT_GRAY_INSERT			= 0xA200;
const RAW_CODE ALT_GRAY_DELETE			= 0xA300;
const RAW_CODE ALT_GRAY_HOME			= 0x9700;
const RAW_CODE ALT_GRAY_END				= 0x9F00;
const RAW_CODE ALT_GRAY_PGUP			= 0x9900;
const RAW_CODE ALT_GRAY_PGDN			= 0xA100;
const RAW_CODE ALT_GRAY_DIVIDE			= 0xA400;
const RAW_CODE ALT_GRAY_MULTIPLY		= 0x3700;
const RAW_CODE ALT_GRAY_PLUS			= 0x4E00;
const RAW_CODE ALT_GRAY_MINUS			= 0x4A00;

const RAW_CODE WHITE_UP_ARROW			= 0x4800;	// White keys
const RAW_CODE WHITE_DOWN_ARROW			= 0x5000;
const RAW_CODE WHITE_LEFT_ARROW			= 0x4B00;
const RAW_CODE WHITE_RIGHT_ARROW		= 0x4D00;
const RAW_CODE WHITE_INSERT				= 0x5200;
const RAW_CODE WHITE_DELETE				= 0x5300;
const RAW_CODE WHITE_HOME				= 0x4700;
const RAW_CODE WHITE_END				= 0x4F00;
const RAW_CODE WHITE_PGUP				= 0x4900;
const RAW_CODE WHITE_PGDN				= 0x5100;
const RAW_CODE WHITE_CENTER				= 0x4C00;

const RAW_CODE CTRL_WHITE_UP_ARROW		= 0x8D00;
const RAW_CODE CTRL_WHITE_DOWN_ARROW	= 0x9100;
const RAW_CODE CTRL_WHITE_LEFT_ARROW	= 0x7300;
const RAW_CODE CTRL_WHITE_RIGHT_ARROW	= 0x7400;
const RAW_CODE CTRL_WHITE_INSERT		= 0x9200;
const RAW_CODE CTRL_WHITE_DELETE		= 0x9300;
const RAW_CODE CTRL_WHITE_HOME			= 0x7700;
const RAW_CODE CTRL_WHITE_END			= 0x7500;
const RAW_CODE CTRL_WHITE_PGUP			= 0x8400;
const RAW_CODE CTRL_WHITE_PGDN			= 0x7600;
const RAW_CODE CTRL_WHITE_CENTER		= 0x8F00;

const RAW_CODE F1						= 0x3B00;	// Function keys
const RAW_CODE F2						= 0x3C00;
const RAW_CODE F3						= 0x3D00;
const RAW_CODE F4						= 0x3E00;
const RAW_CODE F5						= 0x3F00;
const RAW_CODE F6						= 0x4000;
const RAW_CODE F7						= 0x4100;
const RAW_CODE F8						= 0x4200;
const RAW_CODE F9						= 0x4300;
const RAW_CODE F10						= 0x4400;
const RAW_CODE F11						= 0x8500;
const RAW_CODE F12						= 0x8600;

const RAW_CODE SHIFT_F1					= 0x5400;
const RAW_CODE SHIFT_F2					= 0x5500;
const RAW_CODE SHIFT_F3					= 0x5600;
const RAW_CODE SHIFT_F4					= 0x5700;
const RAW_CODE SHIFT_F5					= 0x5800;
const RAW_CODE SHIFT_F6					= 0x5900;
const RAW_CODE SHIFT_F7					= 0x5A00;
const RAW_CODE SHIFT_F8					= 0x5B00;
const RAW_CODE SHIFT_F9					= 0x5C00;
const RAW_CODE SHIFT_F10				= 0x5D00;
const RAW_CODE SHIFT_F11				= 0x8700;
const RAW_CODE SHIFT_F12				= 0x8800;

const RAW_CODE CTRL_F1					= 0x5E00;
const RAW_CODE CTRL_F2					= 0x5F00;
const RAW_CODE CTRL_F3					= 0x6000;
const RAW_CODE CTRL_F4					= 0x6100;
const RAW_CODE CTRL_F5					= 0x6200;
const RAW_CODE CTRL_F6					= 0x6300;
const RAW_CODE CTRL_F7					= 0x6400;
const RAW_CODE CTRL_F8					= 0x6500;
const RAW_CODE CTRL_F9					= 0x6600;
const RAW_CODE CTRL_F10					= 0x6700;
const RAW_CODE CTRL_F11					= 0x8900;
const RAW_CODE CTRL_F12					= 0x8A00;

const RAW_CODE ALT_F1					= 0x6800;
const RAW_CODE ALT_F2					= 0x6900;
const RAW_CODE ALT_F3					= 0x6A00;
const RAW_CODE ALT_F4					= 0x6B00;
const RAW_CODE ALT_F5					= 0x6C00;
const RAW_CODE ALT_F6					= 0x6D00;
const RAW_CODE ALT_F7					= 0x6E00;
const RAW_CODE ALT_F8					= 0x6F00;
const RAW_CODE ALT_F9					= 0x7000;
const RAW_CODE ALT_F10					= 0x7100;
const RAW_CODE ALT_F11					= 0x8B00;
const RAW_CODE ALT_F12					= 0x8C00;
#elif defined (ZIL_MOTIF)
#include <X11/keysym.h>
const RAW_CODE ESCAPE					= XK_Escape;	// Miscellaneous keys
const RAW_CODE ENTER					= XK_Return;
const RAW_CODE BACKSPACE				= XK_BackSpace;
const RAW_CODE TAB						= XK_Tab;

const RAW_CODE GRAY_ENTER				= XK_KP_Enter;	// Grey keys
const RAW_CODE GRAY_UP_ARROW			= XK_Up;
const RAW_CODE GRAY_DOWN_ARROW			= XK_Down;
const RAW_CODE GRAY_LEFT_ARROW			= XK_Left;
const RAW_CODE GRAY_RIGHT_ARROW			= XK_Right;
const RAW_CODE GRAY_DIVIDE				= XK_KP_Divide;
const RAW_CODE GRAY_MULTIPLY			= XK_KP_Multiply;
const RAW_CODE GRAY_PLUS				= XK_KP_Add;
const RAW_CODE GRAY_MINUS				= XK_KP_Subtract;

const RAW_CODE WHITE_UP_ARROW			= XK_Up;	// White keys
const RAW_CODE WHITE_DOWN_ARROW			= XK_Down;
const RAW_CODE WHITE_LEFT_ARROW			= XK_Left;
const RAW_CODE WHITE_RIGHT_ARROW		= XK_Right;

const RAW_CODE F1						= XK_F1;	// Function keys
const RAW_CODE F2						= XK_F2;
const RAW_CODE F3						= XK_F3;
const RAW_CODE F4						= XK_F4;
const RAW_CODE F5						= XK_F5;
const RAW_CODE F6						= XK_F6;
const RAW_CODE F7						= XK_F7;
const RAW_CODE F8						= XK_F8;
const RAW_CODE F9						= XK_F9;
const RAW_CODE F10						= XK_F10;
const RAW_CODE F11						= XK_F11;
const RAW_CODE F12						= XK_F12;
#endif
#endif

// ----- Special hotkey values ----------------------------------------------
const UCHAR HOT_KEY_MAXIMIZE		= 252;
const UCHAR HOT_KEY_MINIMIZE		= 253;
const UCHAR HOT_KEY_SYSTEM		= 254;
const UCHAR HOT_KEY_SUB_WINDOW		= 255;

// ----- Key Information ----------------------------------------------------
// --- event.type is E_KEY
// --- event.rawCode is the key shift state (high byte) and scan code (low byte)
// --- event.key contains the key shift-state and scan-code

// --- shiftState ---
const RAW_CODE S_SHIFT					= 0x0003;	// left- or right-shift
const RAW_CODE S_RIGHT_SHIFT			= 0x0001;
const RAW_CODE S_LEFT_SHIFT				= 0x0002;
const RAW_CODE S_CTRL					= 0x0004;
const RAW_CODE S_ALT					= 0x0008;
const RAW_CODE S_SCROLL_LOCK			= 0x0010;
const RAW_CODE S_NUM_LOCK				= 0x0020;
const RAW_CODE S_CAPS_LOCK				= 0x0040;
const RAW_CODE S_INSERT					= 0x0080;

struct EXPORT UI_KEY
{
	RAW_CODE shiftState;
	RAW_CODE value;
};

// ----- Mouse Information --------------------------------------------------
// --- event.type is E_MOUSE
// --- event.rawCode is the mouse shift state (high byte) and button status (low byte)
// --- event.position.line and event.position.column contains the mouse position

const RAW_CODE M_LEFT					= 0x0100;
const RAW_CODE M_RIGHT					= 0x0200;
const RAW_CODE M_MIDDLE					= 0x0400;
const RAW_CODE M_TOP_CHANGE				= 0x0800;	// only used in sizing operation
const RAW_CODE M_LEFT_CHANGE			= 0x1000;
const RAW_CODE M_RIGHT_CHANGE			= 0x2000;
const RAW_CODE M_MIDDLE_CHANGE			= 0x4000;
const RAW_CODE M_BOTTOM_CHANGE			= 0x8000;	// only used in sizing operation

// ----- UI_SCROLL_INFORMATION ----------------------------------------------

struct EXPORT UI_SCROLL_INFORMATION
{
	int current, minimum, maximum, showing, delta;
};

// ----- UI_EVENT -----------------------------------------------------------
// --- values less than -999 reserved for future use.
// --- values -999 through -1 reserved for LOGICAL_EVENT.
// --- values 0 through 99 reserved for RAW_CODE.
// --- values 100 through 9999 reserved for SYSTEM_EVENT.
// --- values 10000+ available for programmer's use.

// --- DEVICE_TYPE ---
typedef EVENT_TYPE DEVICE_TYPE;
const DEVICE_TYPE E_DEVICE_FIRST		= 0;
const DEVICE_TYPE E_DEVICE_LAST			= 99;

const DEVICE_TYPE E_MSWINDOWS			= 1;	// DEVICE events
const DEVICE_TYPE E_OS2					= 2;
const DEVICE_TYPE E_MOTIF				= 3;
const DEVICE_TYPE E_KEY 				= 10;
const DEVICE_TYPE E_MOUSE 				= 30;
const DEVICE_TYPE E_CURSOR				= 50;
const DEVICE_TYPE E_DEVICE				= 99;

// --- SYSTEM_EVENT ---
typedef EVENT_TYPE SYSTEM_EVENT;
const SYSTEM_EVENT S_SYSTEM_FIRST		= -999;
const SYSTEM_EVENT S_SYSTEM_LAST		= -1;

const SYSTEM_EVENT S_ERROR				= -1;	// No union information
const SYSTEM_EVENT S_UNKNOWN			= -2;
const SYSTEM_EVENT S_NO_OBJECT			= -3;
const SYSTEM_EVENT S_MINIMIZE			= -4;
const SYSTEM_EVENT S_MAXIMIZE			= -5;
const SYSTEM_EVENT S_ALT_KEY			= -6;
const SYSTEM_EVENT S_CONTINUE			= -7;
const SYSTEM_EVENT S_ERROR_RESPONSE		= -8;
const SYSTEM_EVENT S_INITIALIZE			= -9;
const SYSTEM_EVENT S_CREATE				= -10;
const SYSTEM_EVENT S_CLOSE				= -11;
const SYSTEM_EVENT S_NON_CURRENT		= -13;
const SYSTEM_EVENT S_MOVE				= -14;
const SYSTEM_EVENT S_REDISPLAY			= -17;
const SYSTEM_EVENT S_CLOSE_TEMPORARY	= -18;
const SYSTEM_EVENT S_DEINITIALIZE		= -20;
const SYSTEM_EVENT S_REGION_DEFINE		= -21;
const SYSTEM_EVENT S_CASCADE			= -24;
const SYSTEM_EVENT S_RESTORE			= -25;
const SYSTEM_EVENT S_REGISTER_OBJECT	= -26;

const SYSTEM_EVENT S_POSITION			= -100;		// UI_POSITION information

const SYSTEM_EVENT S_SIZE				= -200;		// UI_REGION information
const SYSTEM_EVENT S_CHANGE				= -201;
const SYSTEM_EVENT S_DISPLAY_ACTIVE		= -202;
const SYSTEM_EVENT S_DISPLAY_INACTIVE	= -203;
const SYSTEM_EVENT S_CURRENT			= -204;

const SYSTEM_EVENT S_VSCROLL			= -300;		// UI_SCROLL_INFORMATION information
const SYSTEM_EVENT S_HSCROLL			= -301;
const SYSTEM_EVENT S_VSCROLL_SET 		= -302;
const SYSTEM_EVENT S_HSCROLL_SET 		= -303;
const SYSTEM_EVENT S_VSCROLL_CHECK 		= -304;
const SYSTEM_EVENT S_HSCROLL_CHECK 		= -305;

const SYSTEM_EVENT S_ADD_OBJECT			= -400;		// void *, or special information
const SYSTEM_EVENT S_SUBTRACT_OBJECT	= -401;
const SYSTEM_EVENT S_RESET_DISPLAY		= -402;

const SYSTEM_EVENT S_VERIFY_STATUS		= -450;		// Motif private message!

const SYSTEM_EVENT S_MDICHILD_EVENT		= -500;

// --- LOGICAL_EVENT ---
typedef EVENT_TYPE LOGICAL_EVENT;
const LOGICAL_EVENT L_LOGICAL_FIRST		= 100;
const LOGICAL_EVENT L_LOGICAL_LAST		= 9999;

const LOGICAL_EVENT L_EXIT				= 1000;	// general
const LOGICAL_EVENT L_VIEW				= 1001;
const LOGICAL_EVENT L_SELECT			= 1002;
const LOGICAL_EVENT L_BEGIN_SELECT		= 1003;
const LOGICAL_EVENT L_CONTINUE_SELECT	= 1004;
const LOGICAL_EVENT L_END_SELECT		= 1005;
const LOGICAL_EVENT L_BEGIN_ESCAPE		= 1006;
const LOGICAL_EVENT L_CONTINUE_ESCAPE	= 1007;
const LOGICAL_EVENT L_END_ESCAPE		= 1008;
const LOGICAL_EVENT L_HELP				= 1009;
const LOGICAL_EVENT L_CANCEL			= 1010;
const LOGICAL_EVENT L_EXIT_FUNCTION		= 1011;
const LOGICAL_EVENT L_DOUBLE_CLICK		= 1012;
const LOGICAL_EVENT L_MOVE				= 1013;
const LOGICAL_EVENT L_SIZE				= 1014;

const LOGICAL_EVENT L_NEXT_WINDOW		= 1040;	// window manager

const LOGICAL_EVENT L_UP				= 1050;	// window object
const LOGICAL_EVENT L_DOWN				= 1051;
const LOGICAL_EVENT L_LEFT				= 1052;
const LOGICAL_EVENT L_RIGHT				= 1053;
const LOGICAL_EVENT L_PREVIOUS			= 1054;
const LOGICAL_EVENT L_NEXT				= 1055;
const LOGICAL_EVENT L_FIRST				= 1056;
const LOGICAL_EVENT L_TOP				= 1056;
const LOGICAL_EVENT L_LAST				= 1057;
const LOGICAL_EVENT L_BOTTOM			= 1057;
const LOGICAL_EVENT L_PGUP				= 1058;
const LOGICAL_EVENT L_PGDN				= 1059;

const LOGICAL_EVENT L_BEGIN_MARK		= 1101;	// string
const LOGICAL_EVENT L_CONTINUE_MARK		= 1102;
const LOGICAL_EVENT L_END_MARK			= 1103;
const LOGICAL_EVENT L_CUT				= 1104;
const LOGICAL_EVENT L_PASTE				= 1105;
const LOGICAL_EVENT L_CUT_PASTE			= 1106;
const LOGICAL_EVENT L_MARK				= 1107;
const LOGICAL_EVENT L_COPY_MARK			= 1127;
const LOGICAL_EVENT L_DELETE			= 1108;
const LOGICAL_EVENT L_DELETE_WORD		= 1109;
const LOGICAL_EVENT L_DELETE_EOL		= 1110;
const LOGICAL_EVENT L_INSERT_TOGGLE		= 1114;
const LOGICAL_EVENT L_WORD_LEFT			= 1115;
const LOGICAL_EVENT L_WORD_RIGHT		= 1116;
const LOGICAL_EVENT L_BOL				= 1117;
const LOGICAL_EVENT L_EOL				= 1118;
const LOGICAL_EVENT L_BACKSPACE			= 1119;

const LOGICAL_EVENT L_MDICHILD_EVENT	= 500;

// --- DESIGNER_EVENT (Private messages--Do Not Use!) ---
typedef EVENT_TYPE DESIGNER_EVENT;
const DESIGNER_EVENT D_DESIGNER_FIRST	= 5000;
const DESIGNER_EVENT D_DESIGNER_LAST	= 9999;

const DESIGNER_EVENT D_SET_OBJECT		= 5000;
const DESIGNER_EVENT D_EDIT_OBJECT		= 5001;
const DESIGNER_EVENT D_SET_POSITION		= 5002;
const DESIGNER_EVENT D_CREATE_OBJECT	= 6000;

// --- USER_EVENT ---
typedef EVENT_TYPE USER_EVENT;

struct EXPORT UI_EVENT
{
	EVENT_TYPE type;
	RAW_CODE rawCode;
	RAW_CODE modifiers;
#if defined(ZIL_MSWINDOWS)
	MSG message;
#elif defined(ZIL_OS2)
	QMSG message;
#elif defined(ZIL_MOTIF)
	XEvent message;
#endif

	union
	{
		UI_KEY key;
		UI_REGION region;
		UI_POSITION position;
		UI_SCROLL_INFORMATION scroll;
		void *data;
	};

	UI_EVENT(void) :
		type(0), rawCode(0), modifiers(0) { }
	UI_EVENT(EVENT_TYPE _type, RAW_CODE _rawCode = 0) :
		type(_type), rawCode(_rawCode), modifiers(0) { }
	UI_EVENT(EVENT_TYPE _type, RAW_CODE _rawCode, const UI_KEY &_key) :
		type(_type), rawCode(_rawCode), modifiers(0), key(_key) { }
	UI_EVENT(EVENT_TYPE _type, RAW_CODE _rawCode, const UI_REGION &_region) :
		type(_type), rawCode(_rawCode), modifiers(0), region(_region) { }
	UI_EVENT(EVENT_TYPE _type, RAW_CODE _rawCode, const UI_POSITION &_position) :
		type(_type), rawCode(_rawCode), modifiers(0), position(_position) { }
	UI_EVENT(EVENT_TYPE _type, RAW_CODE _rawCode, const UI_SCROLL_INFORMATION &_scroll) :
		type(_type), rawCode(_rawCode), modifiers(0), scroll(_scroll) { }
#if defined(ZIL_MSWINDOWS) && defined(WIN32)
	UI_EVENT(EVENT_TYPE type, HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
#elif defined(ZIL_MSWINDOWS)
#	if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
#		if __BORLANDC__ >= 0x0410		// Set by Borland C++ version 3.1
	UI_EVENT(EVENT_TYPE type, HWND hWnd, UINT wMsg, UINT wParam, LONG lParam);
#		else
	UI_EVENT(EVENT_TYPE type, HWND hWnd, WORD wMsg, WORD wParam, LONG lParam);
#		endif
#	else
	UI_EVENT(EVENT_TYPE type, HWND hWnd, WORD wMsg, WORD wParam, LONG lParam);
#	endif
#elif defined(ZIL_OS2)
	UI_EVENT(EVENT_TYPE type, HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#elif defined(ZIL_MOTIF)
	UI_EVENT(EVENT_TYPE _type, XEvent &xevent);
#endif
};

// ----- UI_DEVICE ----------------------------------------------------------
typedef EVENT_TYPE DEVICE_STATE;
const DEVICE_STATE D_OFF				= 0x0500;	// Device states
const DEVICE_STATE D_ON					= 0x0501;
const DEVICE_STATE D_HIDE				= 0x0502;
const DEVICE_STATE D_ACTIVATE			= 0x0503;
const DEVICE_STATE D_DEACTIVATE			= 0x0504;

typedef EVENT_TYPE DEVICE_IMAGE;
const DEVICE_IMAGE DC_INSERT			= 0x0510;	// Cursor types
const DEVICE_IMAGE DC_OVERSTRIKE		= 0x0511;

const DEVICE_IMAGE DM_VIEW				= 0x0510;	// Mouse types
const DEVICE_IMAGE DM_EDIT				= 0x0511;
const DEVICE_IMAGE DM_WAIT				= 0x0512;
const DEVICE_IMAGE DM_MOVE				= 0x0513;
const DEVICE_IMAGE DM_HORIZONTAL		= 0x0514;
const DEVICE_IMAGE DM_VERTICAL			= 0x0515;
const DEVICE_IMAGE DM_DIAGONAL_ULLR		= 0x0516;
const DEVICE_IMAGE DM_DIAGONAL_LLUR		= 0x0517;
const DEVICE_IMAGE DM_POSITION			= 0x0518;

// Private ALT key state enum for UI_DEVICE.
enum ALT_STATE
{
	ALT_NOT_PRESSED,
	ALT_PRESSED_NO_EVENTS,
	ALT_PRESSED_EVENTS
};

class EXPORT UI_DEVICE : public UI_ELEMENT
{
	friend class EXPORT UI_EVENT_MANAGER;
public:
	static ALT_STATE altState;
	static UI_DISPLAY *display;
	static UI_EVENT_MANAGER *eventManager;

	int installed;
	DEVICE_TYPE type;
	DEVICE_STATE state;

	virtual ~UI_DEVICE(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event) = 0;

	// List members.
	UI_DEVICE *Next(void) { return((UI_DEVICE *)next); }
	UI_DEVICE *Previous(void) { return((UI_DEVICE *)previous); }

protected:
	UI_DEVICE(DEVICE_TYPE _type, DEVICE_STATE _state);
	static int CompareDevices(void *device1, void *device2);
	virtual void Poll(void) = 0;
};

// ----- UID_CURSOR ---------------------------------------------------------

class EXPORT UID_CURSOR : public UI_DEVICE
{
public:
	static int blinkRate;

	DEVICE_IMAGE image;
	UI_POSITION position;

	UID_CURSOR(DEVICE_STATE state = D_OFF, DEVICE_IMAGE image = DC_INSERT);
	virtual ~UID_CURSOR(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);

protected:
	UI_POSITION offset;

	virtual void Poll(void);
};

// ----- UID_KEYBOARD -------------------------------------------------------

class EXPORT UID_KEYBOARD : public UI_DEVICE
{
public:
	static EVENT_TYPE breakHandlerSet;

	UID_KEYBOARD(DEVICE_STATE state = D_ON);
	virtual ~UID_KEYBOARD(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);

protected:
	virtual void Poll(void);
};

// ----- UID_MOUSE ----------------------------------------------------------

#if defined(_MSC_VER) && defined(ZIL_MSDOS)
extern "C" void MouseISR(void);
#endif

class EXPORT UID_MOUSE : public UI_DEVICE
{
#if defined(ZIL_MSDOS)
#	if defined(__ZTC__)
	friend void _cdecl MouseISR(unsigned, unsigned, unsigned, unsigned);
#	elif defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
	friend void MouseISR(void);
#	endif
#endif
public:
	DEVICE_IMAGE image;
	UI_POSITION position;

	UID_MOUSE(DEVICE_STATE state = D_ON, DEVICE_IMAGE image = DM_WAIT);
	virtual ~UID_MOUSE(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);

protected:
	UI_POSITION offset;

	virtual void Poll(void);
};

// ----- UI_EVENT_MANAGER ---------------------------------------------------

// --- Q_FLAGS ---
typedef unsigned Q_FLAGS;
const Q_FLAGS Q_NORMAL					= 0x0000;	// block, begin, destroy and poll
const Q_FLAGS Q_BLOCK					= 0x0000;
const Q_FLAGS Q_NO_BLOCK				= 0x0001;
const Q_FLAGS Q_BEGIN					= 0x0000;
const Q_FLAGS Q_END						= 0x0002;
const Q_FLAGS Q_DESTROY					= 0x0000;
const Q_FLAGS Q_NO_DESTROY				= 0x0004;
const Q_FLAGS Q_POLL					= 0x0000;
const Q_FLAGS Q_NO_POLL					= 0x0008;

class EXPORT UI_QUEUE_ELEMENT : public UI_ELEMENT
{
public:
	UI_QUEUE_ELEMENT(void);
	~UI_QUEUE_ELEMENT(void);
	UI_EVENT event;

	UI_QUEUE_ELEMENT *Next(void) { return((UI_QUEUE_ELEMENT *)next); }
	UI_QUEUE_ELEMENT *Previous(void) { return((UI_QUEUE_ELEMENT *)previous); }
};

class EXPORT UI_QUEUE_BLOCK : public UI_LIST_BLOCK
{
public:
	UI_QUEUE_BLOCK(int noOfElements);
	~UI_QUEUE_BLOCK(void);

	UI_QUEUE_ELEMENT *Current(void) { return((UI_QUEUE_ELEMENT *)current); }
	UI_QUEUE_ELEMENT *First(void) { return((UI_QUEUE_ELEMENT *)first); }
	UI_QUEUE_ELEMENT *Last(void) { return((UI_QUEUE_ELEMENT *)last); }
};

class EXPORT UI_EVENT_MANAGER : public UI_LIST
{
public:
	UI_EVENT_MANAGER(UI_DISPLAY *display, int noOfElements = 100);
	virtual ~UI_EVENT_MANAGER(void);
	EVENT_TYPE DevicePosition(DEVICE_TYPE deviceType, int column, int line);
	EVENT_TYPE DeviceState(DEVICE_TYPE deviceType, DEVICE_STATE deviceState);
	EVENT_TYPE DeviceImage(DEVICE_TYPE deviceType, DEVICE_IMAGE deviceImage);
	virtual EVENT_TYPE Event(const UI_EVENT &event, DEVICE_TYPE deviceType = E_DEVICE);
	virtual int Get(UI_EVENT &event, Q_FLAGS flags = Q_NORMAL);
	virtual void Put(const UI_EVENT &event, Q_FLAGS flags = Q_END);
	Q_FLAGS QFlags(void) { return (qFlags); }

	// List members.
	void Add(UI_DEVICE *device);
	UI_DEVICE *Current(void) { return((UI_DEVICE *)current); }
	UI_DEVICE *First(void) { return((UI_DEVICE *)first); }
	UI_DEVICE *Last(void) { return((UI_DEVICE *)last); }
	void Subtract(UI_DEVICE *device);
	UI_EVENT_MANAGER &operator+(UI_DEVICE *device) { Add(device); return(*this); }
	UI_EVENT_MANAGER &operator-(UI_DEVICE *device) { Subtract(device); return(*this); }

	// --- Version 2.0 and 1.0 compatibility ---
	UI_EVENT_MANAGER(int noOfElements, UI_DISPLAY *display);

protected:
	int level;
	Q_FLAGS qFlags;
	UI_DISPLAY *display;
	UI_QUEUE_BLOCK queueBlock;
#if defined(ZIL_OS2)
	HMQ hmq;
#endif
};

#endif // UI_EVT_HPP


