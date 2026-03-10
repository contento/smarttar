#include <ui_win.hpp>
#if defined(_MSC_VER)
#pragma hdrstop				// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_LINKBUG)
void z_jump_dummy(void) { }	// Bug fix for broken linkers.
#endif

static UI_ITEM _userTable[] =
{
	{ ID_END, NULL, NULL, 0 }
};
UI_ITEM *UI_WINDOW_OBJECT::userTable = _userTable;

static UI_ITEM _objectTable[] =
{
	{ ID_ICON, VOIDF(UIW_ICON::New), "ICON", 0 },
	{ ID_END, NULL, NULL, 0 }
};
UI_ITEM *UI_WINDOW_OBJECT::objectTable = _objectTable;
