//	Zinc Interface Library - G_WIN1.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

UIW_WINDOW *UIW_WINDOW::Generic(int left, int top, int width, int height,
	char *title, UI_WINDOW_OBJECT *minObject, WOF_FLAGS woFlags,
	WOAF_FLAGS woAdvancedFlags, UI_HELP_CONTEXT helpContext)
{
	// Create the window and add default window objects.
	UIW_WINDOW *window = new UIW_WINDOW(left, top, width, height,
		woFlags, woAdvancedFlags, helpContext, minObject);
	(void)&(*window
		+ new UIW_BORDER
		+ new UIW_MAXIMIZE_BUTTON
		+ new UIW_MINIMIZE_BUTTON
		+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
		+ new UIW_TITLE(title));

	// Return a pointer to the new window.
	return (window);
}
