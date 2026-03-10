//	HELLO2.CPP (HELLO) - Adding help and error window systems.
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

static UIW_WINDOW *HelloWorldWindow1(void)
{
	// Create the standard Hello World! window.
	UIW_WINDOW *window = UIW_WINDOW::Generic(2, 2, 40, 6, "Hello World Window",
		NULL, WOF_NO_FLAGS, WOAF_NO_FLAGS, HELP_HELLO_WORLD);

	// Add the window objects to the window.
	*window
		+ new UIW_TEXT(0, 0, 0, 0, "Hello, World!", 256,
			WNF_NO_FLAGS, WOF_NON_FIELD_REGION);

	// Return a pointer to the window.
	return (window);
}

static UIW_WINDOW *HelloWorldWindow2(void)
{
	// Create the world information window.
	UIW_WINDOW *window = UIW_WINDOW::Generic(5, 5, 52, 15, "World Information Window",
		NULL, WOF_NO_FLAGS, WOAF_NO_SIZE, HELP_WORLD_INFORMATION);

	// Add the window objects to the window.
	*window
		+ new UIW_PROMPT(2, 1, "Age:")
		+ new UIW_STRING(12, 1, 36, "At least 4 1/2 billion years.", 50)

		+ new UIW_PROMPT(2, 2, "Weight:")
		+ new UIW_STRING(12, 2, 36, "6.0 sextillion metric tons.", 50)

		+ new UIW_PROMPT(2, 3, "Size:")
		+ new UIW_STRING(12, 3, 36, "24,901.55 miles circumference.", 50)

		+ new UIW_PROMPT(2, 4, "Makeup:")
		+ &(*new UIW_VT_LIST(12, 4, 20, 4)
			+ new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL)
			+ new UIW_STRING(0, 0, 0, "oxygen -- 46.6%")
			+ new UIW_STRING(0, 0, 0, "silicon -- 27.7%")
			+ new UIW_STRING(0, 0, 0, "aluminum -- 8.1%")
			+ new UIW_STRING(0, 0, 0, "iron -- 5.0%")
			+ new UIW_STRING(0, 0, 0, "calcium -- 3.6%")
			+ new UIW_STRING(0, 0, 0, "other -- 9.0%"))

		+ &(*new UIW_TEXT(2, 8, 44, 4,
			"The earth is the third planet in distance outward "
			"from the sun.  It is the only planetary body in the "
			"solar system known to have conditions suitable for life.",
			2048, WNF_NO_FLAGS, WOF_BORDER)
			+ new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL));

	// Return a pointer to the window.
	return (window);
}

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	// Redirect the window manager's exit function.
	windowManager->exitFunction = ExitFunction;

	// Initialize the help and error systems.
	UI_WINDOW_OBJECT::errorSystem = new UI_ERROR_SYSTEM;
	UI_WINDOW_OBJECT::helpSystem = new UI_HELP_SYSTEM("hello.dat",
		windowManager, HELP_GENERAL);

	// Add two windows to the window manager.
	UIW_WINDOW *window1 = HelloWorldWindow1();
	UIW_WINDOW *window2 = HelloWorldWindow2();
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
