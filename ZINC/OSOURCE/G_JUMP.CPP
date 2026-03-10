//	Zinc Interface Library - G_JUMP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_LINKBUG)
void z_jump_dummy(void) { }		// Bug fix for broken linkers.
#endif

static UI_ITEM _objectTable[] =
{
	// Uncomment the objects below that are used in your application.
	// Zinc Designer generates the necessary table below to link in.
//	{ ID_BIGNUM, (void *)&UIW_BIGNUM::New, "BIGNUM", 0 },
//	{ ID_BORDER, (void *)&UIW_BORDER::New, "BORDER", 0 },
//	{ ID_BUTTON, (void *)&UIW_BUTTON::New, "BUTTON", 0 },
//	{ ID_COMBO_BOX, (void *)&UIW_COMBO_BOX::New, "COMBO_BOX", 0 },
//	{ ID_DATE, (void *)&UIW_DATE::New, "DATE", 0 },
//	{ ID_FORMATTED_STRING, (void *)&UIW_FORMATTED_STRING::New, "FORMATTED_STRING", 0 },
//	{ ID_GROUP, (void *)&UIW_GROUP::New, "GROUP", 0 },
//	{ ID_HZ_LIST, (void *)&UIW_HZ_LIST::New, "HZ_LIST", 0 },
//	{ ID_ICON, (void *)&UIW_ICON::New, "ICON", 0 },
//	{ ID_INTEGER, (void *)&UIW_INTEGER::New, "INTEGER", 0 },
//	{ ID_MAXIMIZE_BUTTON, (void *)&UIW_MAXIMIZE_BUTTON::New, "MAXIMIZE_BUTTON", 0 },
//	{ ID_MINIMIZE_BUTTON, (void *)&UIW_MINIMIZE_BUTTON::New, "MINIMIZE_BUTTON", 0 },
//	{ ID_POP_UP_ITEM, (void *)&UIW_POP_UP_ITEM::New, "POP_UP_ITEM", 0 },
//	{ ID_POP_UP_MENU, (void *)&UIW_POP_UP_MENU::New, "POP_UP_MENU", 0 },
//	{ ID_PROMPT, (void *)&UIW_PROMPT::New, "PROMPT", 0 },
//	{ ID_PULL_DOWN_ITEM, (void *)&UIW_PULL_DOWN_ITEM::New, "PULL_DOWN_ITEM", 0 },
//	{ ID_PULL_DOWN_MENU, (void *)&UIW_PULL_DOWN_MENU::New, "PULL_DOWN_MENU", 0 },
//	{ ID_REAL, (void *)&UIW_REAL::New, "REAL", 0 },
//	{ ID_SCROLL_BAR, (void *)&UIW_SCROLL_BAR::New, "SCROLL_BAR", 0 },
//	{ ID_STRING, (void *)&UIW_STRING::New, "STRING", 0 },
//	{ ID_SYSTEM_BUTTON, (void *)&UIW_SYSTEM_BUTTON::New, "SYSTEM_BUTTON", 0 },
//	{ ID_TEXT, (void *)&UIW_TEXT::New, "TEXT", 0 },
//	{ ID_TIME, (void *)&UIW_TIME::New, "TIME", 0 },
//	{ ID_TITLE, (void *)&UIW_TITLE::New, "TITLE", 0 },
//	{ ID_TOOL_BAR, (void *)&UIW_TOOL_BAR::New, "TOOL_BAR", WOF_NO_FLAGS },
//	{ ID_VT_LIST, (void *)&UIW_VT_LIST::New, "VT_LIST", 0 },
//	{ ID_WINDOW, (void *)&UIW_WINDOW::New, "WINDOW", 0 },
	{ ID_END, NULLP(void), NULLP(char), 0 }
};
UI_ITEM *UI_WINDOW_OBJECT::objectTable = _objectTable;

static UI_ITEM _userTable[] =
{
	{ ID_END, NULLP(void), NULLP(char), 0 }
};
UI_ITEM *UI_WINDOW_OBJECT::userTable = _userTable;
