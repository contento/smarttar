//	Zinc Interface Library - W_POPUP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_POP_UP_MENU ----------------------------------------------------

EVENT_TYPE UIW_POP_UP_MENU::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_INITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
		screenID = CreatePopupMenu();
		woFlags |= WOF_SUPPORT_OBJECT;
		for (object = First(); object; object = object->Next())
			object->Event(event);
		break;

	case S_ADD_OBJECT:
		UIW_WINDOW::Event(event);
		break;

	case S_SUBTRACT_OBJECT:
		object = (UI_WINDOW_OBJECT *)event.data;
		if (screenID)
			DeleteMenu(screenID, UI_LIST::Index(object), MF_BYPOSITION);
		break;
	}

	// Return the control code.
	return (ccode);
}

