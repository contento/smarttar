//	Zinc Interface Library - Z_OBJECT.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stddef.h>
#include "ui_dsn.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_DERIVE_OBJECT ---------------------------------------------------------

EVENT_TYPE UIW_DERIVE_OBJECT::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
#if !defined(ZIL_MOTIF)
	// Virtualize the display.
	display->VirtualGet(screenID, true);

	// Draw the object.														// BUG.1573
//	display->Rectangle(screenID, true, LogicalPalette(ccode), 1, TRUE, FALSE, &clip);
	display->Rectangle(screenID, true, lastPalette, 1, TRUE, FALSE, &clip);

	// Restore the display and return the control code.
	display->VirtualPut(screenID);
#endif
	return (ccode);
}

void *UIW_DERIVE_OBJECT::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_DERIVE_OBJECT;
	switch (request)
	{
	case INITIALIZE_CLASS:
		searchID = windowID[0] = ID_DERIVE_OBJECT;
		woStatus |= WOS_OWNERDRAW;
		// Continue to CHANGED_FLAGS.

	case CHANGED_FLAGS:
		// Check the base class flag settings.
		if (request == CHANGED_FLAGS)
			UI_WINDOW_OBJECT::Information(CHANGED_FLAGS, data, ID_DERIVE_OBJECT);

		// See if the field needs to be re-displayed.
		if (request == CHANGED_FLAGS && objectID == ID_DERIVE_OBJECT)
			Event(UI_EVENT(S_CREATE, 0));
		break;

	default:
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

