//	DRAW.CPP (DRAW) - Sketch pad example program.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <ui_win.hpp>
#if defined(ZIL_MSDOS)
#	include <stdio.h>
#endif
#include "sketch.hpp"

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	// Check for graphics display.
#if defined(ZIL_MSDOS)
	if (display->isText)
	{
		delete windowManager; windowManager = NULL;
		delete eventManager; eventManager = NULL;
		delete display; display = NULL;
		printf("ERROR: Initialization of the graphics display failed.\n");
		return (0);
	}
#endif

 	// Add the sketch pad to the window manager.
	*windowManager + new SKETCH_PAD(1, 1);

	// Process user responses.
	UI_APPLICATION::Control();

	return (0);
}

