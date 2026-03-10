//	Zinc Interface Library - W_BORDER.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_BORDER ---------------------------------------------------------

EVENT_TYPE UIW_BORDER::DrawItem(const UI_EVENT &, EVENT_TYPE )
{
	// Windows automatically displays the border.
	return (FALSE);
}

EVENT_TYPE UIW_BORDER::Event(const UI_EVENT &event)
{
	// Initialize the border.
	if (event.type == S_INITIALIZE)
	{
		UI_WINDOW_OBJECT::Event(event);
		dwStyle = WS_THICKFRAME;
		if (!FlagSet(parent->woAdvancedFlags, WOAF_DIALOG_OBJECT))
			parent->Information(SET_DWSTYLE, &dwStyle);
	}
	return (event.type);
}

