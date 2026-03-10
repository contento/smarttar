#include <ui_win.hpp>
#include "checkwin.hpp"
#if defined(_MSC_VER)
#pragma hdrstop				// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_LINKBUG)
void z_jump_dummy(void) { }	// Bug fix for broken linkers.
#endif

extern EVENT_TYPE NewSettings(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE);
extern EVENT_TYPE DefaultSettings(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE);

static UI_ITEM _userTable[] =
{
	{ 0, VOIDF(NewSettings), "NewSettings", 0 },
	{ 0, VOIDF(DefaultSettings), "DefaultSettings", 0 },
	{ ID_END, NULL, NULL, 0 }
};
UI_ITEM *UI_WINDOW_OBJECT::userTable = _userTable;

static UI_ITEM _objectTable[] =
{
	{ ID_BORDER, VOIDF(UIW_BORDER::New), "BORDER", 0 },
	{ ID_BUTTON, VOIDF(UIW_BUTTON::New), "BUTTON", 0 },
	{ ID_GROUP, VOIDF(UIW_GROUP::New), "GROUP", 0 },
	{ ID_MAXIMIZE_BUTTON, VOIDF(UIW_MAXIMIZE_BUTTON::New), "MAXIMIZE_BUTTON", 0 },
	{ ID_MINIMIZE_BUTTON, VOIDF(UIW_MINIMIZE_BUTTON::New), "MINIMIZE_BUTTON", 0 },
	{ ID_PROMPT, VOIDF(UIW_PROMPT::New), "PROMPT", 0 },
	{ ID_SYSTEM_BUTTON, VOIDF(UIW_SYSTEM_BUTTON::New), "SYSTEM_BUTTON", 0 },
	{ ID_TITLE, VOIDF(UIW_TITLE::New), "TITLE", 0 },
	{ ID_WINDOW, VOIDF(UIW_WINDOW::New), "WINDOW", 0 },
	{ ID_END, NULL, NULL, 0 }
};
UI_ITEM *UI_WINDOW_OBJECT::objectTable = _objectTable;
