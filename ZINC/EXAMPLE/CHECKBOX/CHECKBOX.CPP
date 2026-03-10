//	CHECKBOX.CPP (CHECKBOX) - Checkbox example program.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <string.h>
#include <ui_win.hpp>

#include "checkwin.hpp"

EVENT_TYPE DefaultSettings(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != L_SELECT)
		return ccode;

	for (UI_WINDOW_OBJECT *window = object; window->parent;
		window = window->parent)
			;

	// Change the fields.
	UIW_BUTTON *displayObject;
	displayObject = (UIW_BUTTON *)window->Information(GET_STRINGID_OBJECT, "BAUD_9600");
	*((UIW_GROUP *)displayObject->parent)
		+ displayObject;
	
	displayObject = (UIW_BUTTON *)window->Information(GET_STRINGID_OBJECT, "FULL_DUPLEX");
	displayObject->woStatus &= ~WOS_SELECTED;
	displayObject->Information(CHANGED_STATUS, NULL);

	displayObject = (UIW_BUTTON *)window->Information(GET_STRINGID_OBJECT, "XON_XOFF");
	displayObject->woStatus &= ~WOS_SELECTED;
	displayObject->Information(CHANGED_STATUS, NULL);

	displayObject = (UIW_BUTTON *)window->Information(GET_STRINGID_OBJECT, "SCROLL");
	displayObject->woStatus &= ~WOS_SELECTED;
	displayObject->Information(CHANGED_STATUS, NULL);

	// Change the status window
	UI_WINDOW_OBJECT *baudObject = (UI_WINDOW_OBJECT *)window->Information(GET_STRINGID_OBJECT, "DISPLAY_BAUD");
	baudObject->Information(SET_TEXT, "9600 baud");

	displayObject = (UIW_BUTTON *)window->Information(GET_STRINGID_OBJECT, "DISPLAY_FULL_DUPLEX");
	displayObject->woStatus &= ~WOS_SELECTED;
	displayObject->Information(CHANGED_STATUS, NULL);

	displayObject = (UIW_BUTTON *)window->Information(GET_STRINGID_OBJECT, "DISPLAY_XON_XOFF");
	displayObject->woStatus &= ~WOS_SELECTED;
	displayObject->Information(CHANGED_STATUS, NULL);

	displayObject = (UIW_BUTTON *)window->Information(GET_STRINGID_OBJECT, "DISPLAY_SCROLL");
	displayObject->woStatus &= ~WOS_SELECTED;
	displayObject->Information(CHANGED_STATUS, NULL);

	return ccode;
}

EVENT_TYPE NewSettings(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != L_SELECT)
		return ccode;

	for (UI_WINDOW_OBJECT *window = object; window->parent;
		window = window->parent)
			;

	UI_WINDOW_OBJECT *displayObject;
	char stringID[30];
	strcpy(stringID, object->StringID());

	if (!strncmp(stringID, "BAUD", 4))
	{
       	char text[30];

		strcpy(text, (char *)object->Information(GET_TEXT, NULL));
       	displayObject = (UI_WINDOW_OBJECT *)window->Information(GET_STRINGID_OBJECT, "DISPLAY_BAUD");
       	displayObject->Information(SET_TEXT, text, ID_STRING);
	}
	else
	{
		char string[30];
		strcpy(string, "DISPLAY_");
		strcat(string, stringID);
       	displayObject = (UI_WINDOW_OBJECT *)window->Information(GET_STRINGID_OBJECT, string);
       	if (FlagSet(object->woStatus, WOS_SELECTED))
           	displayObject->woStatus |= WOS_SELECTED;
       	else
           	displayObject->woStatus &= ~WOS_SELECTED;
		displayObject->Information(CHANGED_STATUS, NULL);
	}

	return ccode;
}

UIW_WINDOW *MakeWindow(void)
{
	UIW_WINDOW *window = new UIW_WINDOW("checkwin.dat~SETUP_WINDOW");
   	if (FlagSet(window->woStatus, WOS_READ_ERROR))
       	return window;

	// Select the radio button to display the default baud rate.
	UIW_BUTTON *button = (UIW_BUTTON *)window->Information(GET_STRINGID_OBJECT, "BAUD_9600");
	button->woStatus |= WOS_SELECTED | WOS_CURRENT;

   	// Set the default display baud rate string.
   	UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)window->
       	Information(GET_STRINGID_OBJECT, "DISPLAY_BAUD");
   	object->Information(SET_TEXT, "9600 baud");

   	return window;
}

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	*windowManager
		+ MakeWindow();

	// Process user responses.
	UI_APPLICATION::Control();

	return (0);
}
