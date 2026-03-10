#include <ui_win.hpp>
#include "dir_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop
#endif
#if defined(ZIL_LINKBUG)
void z_jump_dummy(void) { }   // Bug fix for broken linkers.
#endif

extern EVENT_TYPE FilenameCallback(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE);
extern EVENT_TYPE FilterCallback(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE);
extern int DirectoryCompare(void *, void *);

static UI_ITEM _userTable[] =
{
	{ 0, VOIDF(FilenameCallback), "FilenameCallback", 0 },
	{ 0, VOIDF(FilterCallback), "FilterCallback", 0 },
	{ 0, VOIDF(DirectoryCompare), "DirectoryCompare", 0 },
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
	{ ID_ICON, VOIDF(UIW_ICON::New), "ICON", 0 },
	{ ID_MAXIMIZE_BUTTON, VOIDF(UIW_MAXIMIZE_BUTTON::New), "MAXIMIZE_BUTTON", 0 },
	{ ID_MINIMIZE_BUTTON, VOIDF(UIW_MINIMIZE_BUTTON::New), "MINIMIZE_BUTTON", 0 },
	{ ID_POP_UP_ITEM, VOIDF(UIW_POP_UP_ITEM::New), "POP_UP_ITEM", 0 },
	{ ID_POP_UP_MENU, VOIDF(UIW_POP_UP_MENU::New), "POP_UP_MENU", 0 },
	{ ID_PROMPT, VOIDF(UIW_PROMPT::New), "PROMPT", 0 },
	{ ID_PULL_DOWN_ITEM, VOIDF(UIW_PULL_DOWN_ITEM::New), "PULL_DOWN_ITEM", 0 },
	{ ID_PULL_DOWN_MENU, VOIDF(UIW_PULL_DOWN_MENU::New), "PULL_DOWN_MENU", 0 },
	{ ID_SCROLL_BAR, VOIDF(UIW_SCROLL_BAR::New), "SCROLL_BAR", 0 },
	{ ID_STRING, VOIDF(UIW_STRING::New), "STRING", 0 },
	{ ID_SYSTEM_BUTTON, VOIDF(UIW_SYSTEM_BUTTON::New), "SYSTEM_BUTTON", 0 },
	{ ID_TEXT, VOIDF(UIW_TEXT::New), "TEXT", 0 },
	{ ID_TIME, VOIDF(UIW_TIME::New), "TIME", 0 },
	{ ID_TITLE, VOIDF(UIW_TITLE::New), "TITLE", 0 },
	{ ID_VT_LIST, VOIDF(UIW_VT_LIST::New), "VT_LIST", 0 },
	{ ID_WINDOW, VOIDF(UIW_WINDOW::New), "WINDOW", 0 },
	{ ID_END, NULL, NULL, 0 }
};
UI_ITEM *UI_WINDOW_OBJECT::objectTable = _objectTable;
