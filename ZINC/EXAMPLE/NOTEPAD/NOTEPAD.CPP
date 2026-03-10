//	NOTEPAD.CPP (NOTEPAD) - Simple "Notepad" in a window.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <ui_win.hpp>

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	// Create two notepad windows and add them to the window manager.
	UIW_WINDOW *window1 = new UIW_WINDOW(5, 2, 45, 12);
	*window1
		+ new UIW_BORDER
		+ new UIW_MAXIMIZE_BUTTON
		+ new UIW_MINIMIZE_BUTTON
		+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
		+ new UIW_TITLE("Notepad 1")
		+ new UIW_PROMPT(2, 1, "&Subject:  ")
		+ new UIW_STRING(2, 2, 40, "", 50)
		+ new UIW_PROMPT(2, 3, "&Date:  ")
		+ new UIW_DATE(2, 4, 40, NULL)
		+ new UIW_PROMPT(2, 5, "&Note:  ")
		+ new UIW_TEXT(2, 6, 40, 4, "", 256);
	*windowManager + window1;

	UIW_WINDOW *window2 = new UIW_WINDOW(10, 4, 45, 12);
	*window2
		+ new UIW_BORDER
		+ new UIW_MAXIMIZE_BUTTON
		+ new UIW_MINIMIZE_BUTTON
		+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
		+ new UIW_TITLE("Notepad 2")
		+ new UIW_PROMPT(2, 1, "&Subject:  ")
		+ new UIW_STRING(2, 2, 40, "", 50)
		+ new UIW_PROMPT(2, 3, "&Date:  ")
		+ new UIW_DATE(2, 4, 40, NULL)
		+ new UIW_PROMPT(2, 5, "&Note:  ")
		+ new UIW_TEXT(2, 6, 40, 4, "", 256);
	*windowManager + window2;

	// Process user responses.
	UI_APPLICATION::Control();

	return (0);
}
