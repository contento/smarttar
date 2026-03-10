#include <ui_win.hpp>
#include "LANG_WIN.hpp"
#if defined(_MSC_VER)
#pragma hdrstop
#endif
#if defined(ZIL_LINKBUG)
void z_jump_dummy(void) { }   // Bug fix for broken linkers.
#endif


static UI_ITEM _userTable[] =
{
	{ ID_END, NULL, NULL, 0 }
};
UI_ITEM *UI_WINDOW_OBJECT::userTable = _userTable;

static UI_ITEM _objectTable[] =
{
	{ ID_BIGNUM, VOIDF(UIW_BIGNUM::New), "BIGNUM", 0 },
	{ ID_BORDER, VOIDF(UIW_BORDER::New), "BORDER", 0 },
	{ ID_DATE, VOIDF(UIW_DATE::New), "DATE", 0 },
	{ ID_INTEGER, VOIDF(UIW_INTEGER::New), "INTEGER", 0 },
	{ ID_MAXIMIZE_BUTTON, VOIDF(UIW_MAXIMIZE_BUTTON::New), "MAXIMIZE_BUTTON", 0 },
	{ ID_MINIMIZE_BUTTON, VOIDF(UIW_MINIMIZE_BUTTON::New), "MINIMIZE_BUTTON", 0 },
	{ ID_PROMPT, VOIDF(UIW_PROMPT::New), "PROMPT", 0 },
	{ ID_STRING, VOIDF(UIW_STRING::New), "STRING", 0 },
	{ ID_SYSTEM_BUTTON, VOIDF(UIW_SYSTEM_BUTTON::New), "SYSTEM_BUTTON", 0 },
	{ ID_TEXT, VOIDF(UIW_TEXT::New), "TEXT", 0 },
	{ ID_TITLE, VOIDF(UIW_TITLE::New), "TITLE", 0 },
	{ ID_WINDOW, VOIDF(UIW_WINDOW::New), "WINDOW", 0 },
	{ ID_END, NULL, NULL, 0 }
};
UI_ITEM *UI_WINDOW_OBJECT::objectTable = _objectTable;
