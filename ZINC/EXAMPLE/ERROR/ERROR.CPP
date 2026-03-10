//	ERROR.CPP (ERROR) - Error window example program.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <ui_win.hpp>

EVENT_TYPE DateUserFunction(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != S_NON_CURRENT && ccode != L_SELECT)
		return (ccode);

	// Do specific validation.
	UI_DATE currentDate;
	UI_DATE *date = ((UIW_DATE *)object)->DataGet();

	// Call the default Validate function to check for valid date.
	int valid = object->Validate(TRUE);

	// Call error system if the date entered is later than the system date.
	if (valid == DTI_OK && currentDate < *date)
	{
		valid = DTI_INVALID;
		char dateString[64];
		currentDate.Export(dateString, DTF_NO_FLAGS);
		if (object->errorSystem)
			object->errorSystem->ReportError(object->windowManager, WOS_NO_STATUS,
				"The date must be before %s.", dateString);
	}

	// Return error status.
	return ((valid == DTI_OK) ? 0 : -1);
}

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

 	// Initialize the error system.
	UI_WINDOW_OBJECT::errorSystem = new UI_ERROR_SYSTEM;

	// Create the error date window.
	UIW_WINDOW *window = new UIW_WINDOW(5, 5, 40, 4);

	// Add the window objects to the window.
	UI_DATE date;
	*window
		+ new UIW_BORDER
		+ new UIW_MAXIMIZE_BUTTON
		+ new UIW_MINIMIZE_BUTTON
		+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
		+ new UIW_TITLE("Date Error Window")
		+ new UIW_PROMPT(2, 0, "Enter tomorrow's date for error:")
		+ new UIW_DATE(2, 1, 25, &date, "1-1-1900..1-1-2000", DTF_ALPHA_MONTH |
			DTF_SYSTEM, WOF_BORDER | WOF_AUTO_CLEAR, DateUserFunction);

	// Add the window to the window manager.
	*windowManager + window;

	// Process user responses.
	UI_APPLICATION::Control();

	// Clean up.
	delete UI_WINDOW_OBJECT::errorSystem;

	return (0);
}
