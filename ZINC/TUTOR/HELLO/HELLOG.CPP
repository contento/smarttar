//	HELLOG.CPP (HELLO) - Generate the HELLO.DAT windows.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <ui_win.hpp>

#define USE_HELP_CONTEXTS
#include "hello.hpp"

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

	// Add two windows to the hello.dat object file as window resources.
	UIW_WINDOW *window1 = HelloWorldWindow1();
	window1->Store("hello.dat~HELLO_WORLD_WINDOW");

	UIW_WINDOW *window2 = HelloWorldWindow2();
	window2->Store("hello.dat~WORLD_INFORMATION_WINDOW");

	// Show completion window.
	UIW_WINDOW *window = new UIW_WINDOW(0, 0, 50, 8, WOF_NO_FLAGS, WOAF_MODAL | WOAF_NO_SIZE);
	*window
		+ new UIW_BORDER
		+ &(*new UIW_SYSTEM_BUTTON
			+ new UIW_POP_UP_ITEM("&Move", MNIF_MOVE)
			+ new UIW_POP_UP_ITEM("&Close\tAlt+F4", MNIF_CLOSE))
		+ new UIW_TITLE("HELLOG")
		+ new UIW_TEXT(2, 1, 46, 4,
			"The windows HELLO_WORLD_WINDOW and WORLD_INFORMATION_WINDOW "
			"have been written to the \"hello.dat\" file.  "
			"Select <OK> to exit.",
			1024, WNF_NO_FLAGS, WOF_NON_SELECTABLE)
		+ new UIW_BUTTON(15, 5, 15, "~OK",
			BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE,
			WOF_JUSTIFY_CENTER, NULL, S_CLOSE);
	windowManager->Center(window);
	*windowManager + window;

	// Process user responses.
	UI_APPLICATION::Control();

	return (0);
}
