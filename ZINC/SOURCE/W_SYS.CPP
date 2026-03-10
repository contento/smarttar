//	Zinc Interface Library - W_SYS.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_SYSTEM_BUTTON --------------------------------------------------

EVENT_TYPE UIW_SYSTEM_BUTTON::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_INITIALIZE:
		{
		UI_WINDOW_OBJECT::Event(event);
		dwStyle = WS_SYSMENU;
		menu.parent = this;
		menu.woFlags |= WOF_SUPPORT_OBJECT;
		for (UI_WINDOW_OBJECT *object = menu.First(); object; object = object->Next())
			object->Event(event);											// BUG.1680
		parent->Information(SET_DWSTYLE, &dwStyle);
		}
		break;

	case S_CREATE:
		if (!FlagSet(syFlags, SYF_GENERIC))
		{
			menu.screenID = GetSystemMenu(parent->screenID, FALSE);
			while (DeleteMenu(menu.screenID, 0, MF_BYPOSITION))
				;
			for (UI_WINDOW_OBJECT *object = menu.First(); object; object = object->Next())
				object->Event(event);

		}
		break;

	case S_ADD_OBJECT:
	case S_SUBTRACT_OBJECT:
		menu.Event(event);
		break;

	default:
		ccode = S_UNKNOWN;
	}

	// Return the control code.
	return (ccode);
}

