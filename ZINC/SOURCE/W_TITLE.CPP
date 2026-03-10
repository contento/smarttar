//	Zinc Interface Library - W_TITLE.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_TITLE ----------------------------------------------------------

EVENT_TYPE UIW_TITLE::Event(const UI_EVENT &event)
{
	// Initialize the title.
	if (event.type == S_INITIALIZE)
	{
		UI_WINDOW_OBJECT::Event(event);
		dwStyle = WS_CAPTION;
		parent->Information(SET_TITLE, this);
		parent->Information(SET_DWSTYLE, &dwStyle);
	}
	return (event.type);
}

