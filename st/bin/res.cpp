#include <ui_win.hpp>
#include "RES.hpp"
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
	{ ID_BUTTON, VOIDF(UIW_BUTTON::New), "BUTTON", 0 },
	{ ID_COMBO_BOX, VOIDF(UIW_COMBO_BOX::New), "COMBO_BOX", 0 },
	{ ID_DATE, VOIDF(UIW_DATE::New), "DATE", 0 },
	{ ID_GROUP, VOIDF(UIW_GROUP::New), "GROUP", 0 },
	{ ID_INTEGER, VOIDF(UIW_INTEGER::New), "INTEGER", 0 },
	{ ID_PROMPT, VOIDF(UIW_PROMPT::New), "PROMPT", 0 },
	{ ID_SCROLL_BAR, VOIDF(UIW_SCROLL_BAR::New), "SCROLL_BAR", 0 },
	{ ID_STRING, VOIDF(UIW_STRING::New), "STRING", 0 },
	{ ID_SYSTEM_BUTTON, VOIDF(UIW_SYSTEM_BUTTON::New), "SYSTEM_BUTTON", 0 },
	{ ID_TEXT, VOIDF(UIW_TEXT::New), "TEXT", 0 },
	{ ID_TIME, VOIDF(UIW_TIME::New), "TIME", 0 },
	{ ID_TITLE, VOIDF(UIW_TITLE::New), "TITLE", 0 },
	{ ID_VT_LIST, VOIDF(UIW_VT_LIST::New), "VT_LIST", 0 },
	{ ID_WINDOW, VOIDF(UIW_WINDOW::New), "WINDOW", 0 },
	{ ID_UIW_PHONE, VOIDF(UIW_PHONE::New), "UIW_PHONE", 0 },
	{ ID_END, NULL, NULL, 0 }
};
UI_ITEM *UI_WINDOW_OBJECT::objectTable = _objectTable;
