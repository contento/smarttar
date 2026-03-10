//	Zinc Interface Library - G_HOTKEY.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define USE_RAW_KEYS
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_LINKBUG)
void z_hotkey_dummy(void) { }	// Bug fix for broken linkers.
#endif

//#if defined(ZIL_MSDOS) || defined(ZIL_OS2)								// BUG.2314
#if defined(ZIL_MSDOS) || defined(ZIL_OS2) || defined(ZIL_MSWINDOWS)
static UI_EVENT_MAP hotKeyMapTable[] =
{
	// ID_WINDOW_OBJECT.
	{ ID_WINDOW_OBJECT, 	HOT_KEY_SYSTEM,		E_KEY, 		ALT_PERIOD },
	{ ID_WINDOW_OBJECT, 	HOT_KEY_SYSTEM,		E_KEY, 		ALT_SPACE },
	{ ID_WINDOW_OBJECT, 	HOT_KEY_MINIMIZE,	E_KEY, 		ALT_WHITE_MINUS },
	{ ID_WINDOW_OBJECT, 	HOT_KEY_MINIMIZE,	E_KEY, 		ALT_GRAY_MINUS },
	{ ID_WINDOW_OBJECT, 	HOT_KEY_MINIMIZE,	E_KEY, 		ALT_F9 },
	{ ID_WINDOW_OBJECT, 	HOT_KEY_MAXIMIZE,	E_KEY, 		ALT_WHITE_PLUS },
	{ ID_WINDOW_OBJECT, 	HOT_KEY_MAXIMIZE,	E_KEY, 		ALT_GRAY_PLUS },
	{ ID_WINDOW_OBJECT, 	HOT_KEY_MAXIMIZE,	E_KEY, 		ALT_F10 },

	{ ID_WINDOW_OBJECT, 	'q',				E_KEY, 		ALT_Q },
	{ ID_WINDOW_OBJECT, 	'w',				E_KEY, 		ALT_W },
	{ ID_WINDOW_OBJECT, 	'e',				E_KEY, 		ALT_E },
	{ ID_WINDOW_OBJECT, 	'r',				E_KEY, 		ALT_R },
	{ ID_WINDOW_OBJECT, 	't',				E_KEY, 		ALT_T },
	{ ID_WINDOW_OBJECT, 	'y',				E_KEY, 		ALT_Y },
	{ ID_WINDOW_OBJECT, 	'u',				E_KEY, 		ALT_U },
	{ ID_WINDOW_OBJECT, 	'i',				E_KEY, 		ALT_I },
	{ ID_WINDOW_OBJECT, 	'o',				E_KEY, 		ALT_O },
	{ ID_WINDOW_OBJECT, 	'p',				E_KEY, 		ALT_P },

	{ ID_WINDOW_OBJECT, 	'a',				E_KEY, 		ALT_A },
	{ ID_WINDOW_OBJECT, 	's',				E_KEY, 		ALT_S },
	{ ID_WINDOW_OBJECT, 	'd',				E_KEY, 		ALT_D },
	{ ID_WINDOW_OBJECT, 	'f',				E_KEY, 		ALT_F },
	{ ID_WINDOW_OBJECT, 	'g',				E_KEY, 		ALT_G },
	{ ID_WINDOW_OBJECT, 	'h',				E_KEY, 		ALT_H },
	{ ID_WINDOW_OBJECT, 	'j',				E_KEY, 		ALT_J },
	{ ID_WINDOW_OBJECT, 	'k',				E_KEY, 		ALT_K },
	{ ID_WINDOW_OBJECT, 	'l',				E_KEY, 		ALT_L },

	{ ID_WINDOW_OBJECT, 	'z',				E_KEY, 		ALT_Z },
	{ ID_WINDOW_OBJECT, 	'x',				E_KEY, 		ALT_X },
	{ ID_WINDOW_OBJECT, 	'c',				E_KEY, 		ALT_C },
	{ ID_WINDOW_OBJECT, 	'v',				E_KEY, 		ALT_V },
	{ ID_WINDOW_OBJECT, 	'b',				E_KEY, 		ALT_B },
	{ ID_WINDOW_OBJECT, 	'n',				E_KEY, 		ALT_N },
	{ ID_WINDOW_OBJECT, 	'm',				E_KEY, 		ALT_M },

	{ ID_WINDOW_OBJECT, 	'1',				E_KEY, 		ALT_1 },
	{ ID_WINDOW_OBJECT, 	'2',				E_KEY, 		ALT_2 },
	{ ID_WINDOW_OBJECT, 	'3',				E_KEY, 		ALT_3 },
	{ ID_WINDOW_OBJECT, 	'4',				E_KEY, 		ALT_4 },
	{ ID_WINDOW_OBJECT, 	'5',				E_KEY, 		ALT_5 },
	{ ID_WINDOW_OBJECT, 	'6',				E_KEY, 		ALT_6 },
	{ ID_WINDOW_OBJECT, 	'7',				E_KEY, 		ALT_7 },
	{ ID_WINDOW_OBJECT, 	'8',				E_KEY, 		ALT_8 },
	{ ID_WINDOW_OBJECT, 	'9',				E_KEY, 		ALT_9 },
	{ ID_WINDOW_OBJECT, 	'0',				E_KEY, 		ALT_0 },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
//#elif defined(ZIL_MSWINDOWS) || defined(ZIL_MOTIF)						// BUG.2314
#elif defined(ZIL_MOTIF)
static UI_EVENT_MAP hotKeyMapTable[] =
{
	// End of array.
	{ ID_END, 0, 0, 0 }
};
#endif

UI_EVENT_MAP *_hotKeyMapTable = hotKeyMapTable;

