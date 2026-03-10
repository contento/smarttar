//	Zinc Interface Library - W_MIN.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_MINIMIZE_BUTTON ------------------------------------------------

EVENT_TYPE UIW_MINIMIZE_BUTTON::Event(const UI_EVENT &event)
{
	// Initialize the minimize button.
	if (event.type == S_INITIALIZE)
	{
		UI_WINDOW_OBJECT::Event(event);
		dwStyle = WS_MINIMIZEBOX;
		parent->Information(SET_DWSTYLE, &dwStyle);
	}
	return (event.type);
}
