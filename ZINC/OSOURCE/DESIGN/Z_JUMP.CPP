//	Zinc Interface Library Designer - Z_JUMP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stddef.h>
#include "ui_dsn.hpp"
#include "p_design.hpp"
#if defined(_MSC_VER)
#pragma hdrstop
#endif
#if defined(ZIL_LINKBUG)
void z_jump_dummy(void) { }   // Bug fix for broken linkers.
#endif

extern EVENT_TYPE ResourceNameFunction(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE);
extern EVENT_TYPE FileFieldFunction(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE);
extern EVENT_TYPE HelpFunction(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE);
extern EVENT_TYPE ImageType(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE);
extern EVENT_TYPE ImageNameFunction(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE);
extern EVENT_TYPE ContextNameFunction(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE);
extern EVENT_TYPE HelpIndexFunction(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE);
extern EVENT_TYPE FlagValidate(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE);

static UI_ITEM _userTable[] =
{
	{ 0, VOIDF(ResourceNameFunction), "ResourceNameFunction", 0 },
	{ 0, VOIDF(FileFieldFunction), "FileFieldFunction", 0 },
	{ 0, VOIDF(HelpFunction), "HelpFunction", 0 },
	{ 0, VOIDF(ImageType), "ImageType", 0 },
	{ 0, VOIDF(ImageNameFunction), "ImageNameFunction", 0 },
	{ 0, VOIDF(ContextNameFunction), "ContextNameFunction", 0 },
	{ 0, VOIDF(HelpIndexFunction), "HelpIndexFunction", 0 },
	{ 0, VOIDF(FlagValidate), "FlagValidate", 0 },
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
	{ ID_FORMATTED_STRING, VOIDF(UIW_FORMATTED_STRING::New), "FORMATTED_STRING", 0 },
	{ ID_GROUP, VOIDF(UIW_GROUP::New), "GROUP", 0 },
    { ID_HELP_BAR, VOIDF(HELP_BAR::New), "HELP_BAR", 0 },
	{ ID_HZ_LIST, VOIDF(UIW_HZ_LIST::New), "HZ_LIST", 0 },
	{ ID_ICON, VOIDF(UIW_ICON::New), "ICON", 0 },
	{ ID_INTEGER, VOIDF(UIW_INTEGER::New), "INTEGER", 0 },
	{ ID_MAXIMIZE_BUTTON, VOIDF(UIW_MAXIMIZE_BUTTON::New), "MAXIMIZE_BUTTON", 0 },
	{ ID_MINIMIZE_BUTTON, VOIDF(UIW_MINIMIZE_BUTTON::New), "MINIMIZE_BUTTON", 0 },
	{ ID_OBJECT_LIST, VOIDF(OBJECT_LIST::New), "OBJECT_LIST", 0 },
	{ ID_POP_UP_ITEM, VOIDF(UIW_POP_UP_ITEM::New), "POP_UP_ITEM", 0 },
	{ ID_POP_UP_MENU, VOIDF(UIW_POP_UP_MENU::New), "POP_UP_MENU", 0 },
	{ ID_PROMPT, VOIDF(UIW_PROMPT::New), "PROMPT", 0 },
	{ ID_PULL_DOWN_ITEM, VOIDF(UIW_PULL_DOWN_ITEM::New), "PULL_DOWN_ITEM", 0 },
	{ ID_PULL_DOWN_MENU, VOIDF(UIW_PULL_DOWN_MENU::New), "PULL_DOWN_MENU", 0 },
	{ ID_REAL, VOIDF(UIW_REAL::New), "REAL", 0 },
	{ ID_SCROLL_BAR, VOIDF(UIW_SCROLL_BAR::New), "SCROLL_BAR", 0 },
	{ ID_STORAGE_COMBO_BOX, VOIDF(STORAGE_COMBO_BOX::New), "STORAGE_COMBO_BOX", 0 },
	{ ID_STRING, VOIDF(UIW_STRING::New), "STRING", 0 },
	{ ID_SYSTEM_BUTTON, VOIDF(UIW_SYSTEM_BUTTON::New), "SYSTEM_BUTTON", 0 },
	{ ID_TEXT, VOIDF(UIW_TEXT::New), "TEXT", 0 },
	{ ID_TIME, VOIDF(UIW_TIME::New), "TIME", 0 },
	{ ID_TITLE, VOIDF(UIW_TITLE::New), "TITLE", 0 },
	{ ID_TOOL_BAR, VOIDF(UIW_TOOL_BAR::New), "TOOL_BAR", 0 },
	{ ID_VT_LIST, VOIDF(UIW_VT_LIST::New), "VT_LIST", 0 },
	{ ID_WINDOW, VOIDF(UIW_WINDOW::New), "WINDOW", 0 },

	{ ID_AUTO_PROMPT, VOIDF(AUTO_PROMPT::New), "AUTO_PROMPT", 0 },
	{ ID_BITMAP_OBJECT, VOIDF(BITMAP_OBJECT::New), "BITMAP_OBJECT", 0 },

//	Switch these lines with those above to edit the bitmap editor resource.
//	{ ID_AUTO_PROMPT, VOIDF(UIW_PROMPT::New), "AUTO_PROMPT", 0 },
// 	{ ID_BITMAP_OBJECT, VOIDF(UIW_DERIVE_OBJECT::New), "BITMAP_OBJECT", 0 },

	{ ID_FLAG_FIELD, VOIDF(FLAG_FIELD::New), "FLAG_FIELD", 0 },
	{ ID_DERIVE_OBJECT, VOIDF(UIW_DERIVE_OBJECT::New), "DERIVE_OBJECT", 0 },
	{ ID_END, NULL, NULL, 0 },

	// Extra table space for derived objects (32).
	// If more derived objects are needed, add more entries to this table.
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_END, NULL, NULL, 0 },
	{ ID_DERIVE_END, NULL, NULL, 0 }
};
UI_ITEM *UI_WINDOW_OBJECT::objectTable = _objectTable;
