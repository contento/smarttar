//	FREESTOR.CPP (FREESTOR) - Demonstrate the use of the Free Store Exception Handler.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <stdio.h>
#include <ui_win.hpp>
#include "mem_err.hpp"

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	// Set up the Free Store Exception handler.
	MEMORY_ALLOCATION_ERROR_SYSTEM *memErrorSystem =
		new MEMORY_ALLOCATION_ERROR_SYSTEM(display, eventManager, windowManager);
	if (!memErrorSystem->installed)
	{
		delete memErrorSystem;
		delete windowManager;
		delete eventManager;
		delete display;
		printf("Not enough memory to install memory error handler.");
	}

	int centerX = display->columns / display->cellWidth / 2;
	*windowManager
		+ &(*UIW_WINDOW::Generic(centerX - 20, 0, 40, display->isText ? 14 : 8, "Free Store Tester")
			+ new UIW_TEXT(0, 0, 0, 0,
				"This is a test of the Free Store Exception Handler. "
				"When the new operator fails, a NULL is usually returned. "
				"For this example a \"new handler\" is called instead. This "
				"handler takes control, notifies the user, and does clean up "
				"and exit. It may take a few hundred windows to use up all of "
				"memory. Be patient and watch.", 512,
				WNF_NO_FLAGS, WOF_VIEW_ONLY | WOF_NON_FIELD_REGION));

	// Now add windows until a Free Store Exception error is found.
	int i = 0;

	// Wait for user response.
	UI_EVENT event;
	EVENT_TYPE ccode;
	do
	{
		// Get input from the user.
		ccode = eventManager->Get(event, Q_NO_BLOCK);
		if (!ccode)
			ccode = windowManager->Event(event);
		else
		{
			i++;
			*windowManager
				+ &(*new UIW_WINDOW(centerX - 20, display->isText ? 14 : 11, 40, 7,
					WOF_NO_FLAGS, WOAF_NO_FLAGS)
					+ new UIW_BORDER
					+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
					+ new UIW_TITLE("Memory Test Window")
					+ new UIW_INTEGER(15, 2, 8, &i));
		}
	} while (ccode != L_EXIT && ccode != S_NO_OBJECT);

	// Clean up.
	delete memErrorSystem;

	return (0);
}
