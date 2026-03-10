//	HELLO3.CPP (HELLO) - Using windows created with Zinc Designer.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <ui_win.hpp>

#define USE_HELP_CONTEXTS
#include "hello.hpp"

static EVENT_TYPE ExitFunction(UI_DISPLAY *display,
	UI_EVENT_MANAGER *, UI_WINDOW_MANAGER *windowManager)
{
	// Compute the window region.
	int width = 44;
	int height = display->isText ? 7 : 6;
	int left = (display->columns / display->cellWidth - width) / 2;
	int top = (display->lines / display->cellHeight - height) / 2;

	// Create the exit window.
	UIW_WINDOW *window = new UIW_WINDOW(left, top, width, height,
		WOF_NO_FLAGS, WOAF_MODAL | WOAF_NO_SIZE);
	*window
		+ new UIW_BORDER
		+ &(*new UIW_SYSTEM_BUTTON
			+ new UIW_POP_UP_ITEM("&Move", MNIF_MOVE)
			+ new UIW_POP_UP_ITEM("&Close\tAlt+F4", MNIF_CLOSE))
		+ new UIW_TITLE("Hello World Tutorial")
		+ new UIW_STRING(4, 1, 36, "This will close Hello World.",
			100, STF_NO_FLAGS, WOF_NON_SELECTABLE | WOF_JUSTIFY_CENTER);
	*window
		+ new UIW_BUTTON(10, height - 3, 10, "~OK",
			BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE,
			WOF_JUSTIFY_CENTER, NULL, L_EXIT)
		+ new UIW_BUTTON(22, height - 3, 10, "~Cancel",
			BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE,
			WOF_JUSTIFY_CENTER, NULL, S_CLOSE);
	*windowManager + window;
	return (S_CONTINUE);
}

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	// Redirect the window manager's exit function.
	windowManager->exitFunction = ExitFunction;

 	// Initialize the help and error systems and persistant object table.
	UI_WINDOW_OBJECT::errorSystem = new UI_ERROR_SYSTEM;
	UI_WINDOW_OBJECT::helpSystem = new UI_HELP_SYSTEM("hello.dat",
		windowManager, HELP_GENERAL);

	// Check for a valid file.
	if (!UI_STORAGE::ValidName("hello.dat"))
	{
		UI_WINDOW_OBJECT::errorSystem->ReportError(windowManager, WOS_NO_STATUS,
			"ERROR: Unable to open or find the file \"hello.dat\".");
		delete UI_WINDOW_OBJECT::helpSystem;
		delete UI_WINDOW_OBJECT::errorSystem;
		return (0);
	}

	// Add the two windows to the window manager.
	UI_WINDOW_OBJECT *window1 = UI_WINDOW_OBJECT::New("hello.dat~HELLO_WORLD_WINDOW");
	UI_WINDOW_OBJECT *window2 = UI_WINDOW_OBJECT::New("hello.dat~WORLD_INFORMATION_WINDOW");
	*windowManager
		+ window1
		+ window2;

	// This line assigns the exit function to be called before the main
	// window is closed.  It MUST be after the window is added to 
	// windowManager.
	windowManager->screenID = window1->screenID;

	// Process user responses.
	UI_APPLICATION::Control();

	// Clean up.
	delete UI_WINDOW_OBJECT::helpSystem;
	delete UI_WINDOW_OBJECT::errorSystem;

	return (0);
}
