//	HELPBAR.CPP (HELPBAR) - This file contains the main program loop.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <ui_win.hpp>
#include "hlpbar.hpp"

// Help bar message indices.
enum HELP_BAR_MESSAGE
{
	HELP_FIRST_NAME = 1,
	HELP_LAST_NAME,
	HELP_ADDRESS,
	HELP_PHONE,
	HELP_CLOSE_WINDOW,
	HELP_EXIT
};

// User function to set help bar information.
EVENT_TYPE SetHelp(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	// Declare the help message/context pairs.
	static struct HELP_PAIR
	{
		UI_HELP_CONTEXT helpContext;
		char *message;
	} helpMessageTable[] =
	{
		{ HELP_FIRST_NAME,		"First name of customer" },
		{ HELP_LAST_NAME,		"Last name of customer" },
		{ HELP_ADDRESS,			"Address of customer" },
		{ HELP_PHONE,			"Phone number of customer" },
		{ HELP_CLOSE_WINDOW,	"This will close the window" },
		{ HELP_EXIT,			"This will exit the program" },
		{ 0, 0 } // End of array.
	};

	// If you are not setting or clearing the help bar then just exit.
	if (ccode != S_CURRENT && ccode != S_NON_CURRENT)
		return (0);

	// Find the parent window.
	for (UI_WINDOW_OBJECT *parentWindow = object; parentWindow->parent; )
		parentWindow = parentWindow->parent;

	// Get the help bar.
	UI_WINDOW_OBJECT *helpBar =
		(UI_WINDOW_OBJECT *)parentWindow->Information(GET_STRINGID_OBJECT, "HELP_BAR");

	// If there was a help bar then set or clear its message.
	if (helpBar)
	{
		// Set default message to clear bar.
		char *message = "";

		if (ccode == S_CURRENT)
		{
			// Get the message associated with the help context.
			for (int i = 0; helpMessageTable[i].helpContext; i++)
				if (object->helpContext == helpMessageTable[i].helpContext)
				{
					message = helpMessageTable[i].message;
					break;
				}

			// Update the help bar text.
			helpBar->Information(SET_TEXT, message, ID_HELP_BAR);					// BUG.General
		}
	}
	return (0);
}

// User function for action buttons.
EVENT_TYPE ActionFunction(UI_WINDOW_OBJECT *object, UI_EVENT &event,
	EVENT_TYPE ccode)
{
	// See if all that has to be done is set the help bar.
	if (ccode != L_SELECT)
		return(SetHelp(object, event, ccode));

	// Otherwise put the event on the queue.
	object->eventManager->Put(UI_EVENT(((UIW_BUTTON *)object)->value));
	return (0);
}

// Create an information window located at left, top.
UIW_WINDOW *InformationWindow(int left, int top)
{
	// Create the window.
//	UIW_WINDOW *window = UIW_WINDOW::Generic(left, top, 48, 13, "Customer Window",
//		NULL, WOF_NO_FLAGS, WOAF_NO_SIZE);
	UIW_WINDOW *window = new UIW_WINDOW(left, top, 48, 13, WOF_NO_FLAGS,	// BUG.1916
		WOAF_NO_SIZE);
	*window
		+ new UIW_BORDER
		+ new UIW_MINIMIZE_BUTTON
		+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
		+ new UIW_TITLE("Customer Window");

	// Create the window objects.
	HELP_BAR *helpBar = new HELP_BAR("Zinc Help Bar Tutorial");
	helpBar->StringID("HELP_BAR");

	UIW_STRING *firstName = new UIW_STRING(18, 1, 20, "", 20, STF_NO_FLAGS,
		WOF_BORDER, SetHelp);
	firstName->helpContext = HELP_FIRST_NAME;

	UIW_STRING *lastName = new UIW_STRING(18, 2, 20, "", 20, STF_NO_FLAGS,
		WOF_BORDER, SetHelp);
	lastName->helpContext = HELP_LAST_NAME;

	UIW_TEXT *address = new UIW_TEXT(18, 3, 20, 3, "", 256, WNF_NO_WRAP,
		WOF_BORDER, SetHelp);
	address->helpContext = HELP_ADDRESS;

	UIW_FORMATTED_STRING *phone = new UIW_FORMATTED_STRING(18, 6, 20, "",
		"LNNNLLNNNLNNNN", "(...) ...-....", WOF_BORDER, SetHelp);
	phone->helpContext = HELP_PHONE;

	UIW_BUTTON *closeButton = new UIW_BUTTON(10, 9, 10, "&Close", BTF_AUTO_SIZE,
		WOF_JUSTIFY_CENTER, ActionFunction, S_CLOSE);
	closeButton->helpContext = HELP_CLOSE_WINDOW;

	UIW_BUTTON *exitButton = new UIW_BUTTON(25, 9, 10, "E&xit", BTF_AUTO_SIZE,
		WOF_JUSTIFY_CENTER, ActionFunction, L_EXIT);
	exitButton->helpContext = HELP_EXIT;

	// Add the fields to the window;
	*window
		+ helpBar
		+ new UIW_PROMPT(5, 1, "&First Name :")
		+ firstName
		+ new UIW_PROMPT(5, 2, "&Last Name :")
		+ lastName
		+ new UIW_PROMPT(5, 3, "&Address :")
		+ address
		+ new UIW_PROMPT(5, 6, "&Phone :")
		+ phone
		+ closeButton
		+ exitButton;

	return (window);
}

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	// Put the windows onto the screen.
	*windowManager
		+ InformationWindow(1, 1)
		+ InformationWindow(15, 5);

	// Process user responses.
	UI_APPLICATION::Control();

	return (0);
}
