//	CONTROL.CPP (ZINCAPP) - This file contains the main control functions.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#define USE_RAW_KEYS	// For accelarator key definitions.
#include <ui_win.hpp>
#include "zincapp.hpp"

EVENT_TYPE About(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != L_SELECT)
		return (ccode);

	// Compute the window region.
	UI_DISPLAY *display = object->display;
	int width = 42;
	int height = display->isText ? 7 : 6;
	int left = (display->columns / display->cellWidth - width) / 2;
	int top = (display->lines / display->cellHeight - height) / 2;

	char *message = "Zinc Application 3.5";
	UIW_WINDOW *window = new UIW_WINDOW(left, top, width, height, WOF_NO_FLAGS, WOAF_MODAL | WOAF_NO_SIZE);
	*window
		+ new UIW_BORDER
		+ &(*new UIW_SYSTEM_BUTTON
			+ new UIW_POP_UP_ITEM("&Move", MNIF_MOVE)
			+ new UIW_POP_UP_ITEM("&Close\tAlt+F4", MNIF_CLOSE))
		+ new UIW_TITLE("About...");
	if (display->isText)
		*window
			+ new UIW_STRING(2, 1, 32, message, 100, STF_NO_FLAGS, WOF_NON_SELECTABLE | WOF_JUSTIFY_CENTER);
	else
		*window
			+ new UIW_ICON(3, 1, "minIcon")
			+ new UIW_STRING(8, 1, 32, message, 100, STF_NO_FLAGS, WOF_NON_SELECTABLE | WOF_JUSTIFY_CENTER);
	*window
		+ new UIW_BUTTON(15, height - 3, 10, "~OK",
			BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE,
			WOF_JUSTIFY_CENTER, NULL, S_CLOSE);
	object->windowManager->Add(window);
	return (ccode);
}

EVENT_TYPE ZINCAPP_WINDOW_MANAGER::Event(const UI_EVENT &event)
{
	// Allow the base window manager to process the event.
	EVENT_TYPE ccode = UI_WINDOW_MANAGER::Event(event);

	// Send the event to any event monitor windows.
	for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
		if (object->userFlags == MSG_EVENT_MONITOR && event.type != S_RESET_DISPLAY)
		{
			UI_EVENT tEvent(event.type, 0xFFFF);
			tEvent.data = (void *)&event;
			object->Event(tEvent);
		}

	// Return the control code.
	return (ccode);
}

CONTROL_WINDOW::CONTROL_WINDOW(void) :
	UIW_WINDOW(0, 0, 76, 6, WOF_NO_FLAGS, WOAF_LOCKED)
{
	// Control menu items.
	static UI_ITEM controlItems[] =
	{
		{ S_REDISPLAY,			VOIDF(CONTROL_WINDOW::Message),	"&Refresh\tShift+F6",	MNIF_NO_FLAGS },
		{ 0,					VOIDF(0),			"",						MNIF_NO_FLAGS },	// item separator
		{ L_EXIT_FUNCTION,		VOIDF(CONTROL_WINDOW::Message),	"E&xit\tAlt+F4",		MNIF_NO_FLAGS },
		{ 0, 0, 0, 0 }	// End of array.
	};

	// Display menu items (some are only selectable in DOS).
#if defined(ZIL_MSDOS)
	static UI_ITEM displayItems[] =
	{
		{ MSG_25x40_MODE,		VOIDF(CONTROL_WINDOW::Message),	"&1-25x40 text mode",	MNIF_NO_FLAGS },
		{ MSG_25x80_MODE,		VOIDF(CONTROL_WINDOW::Message),	"&2-25x80 text mode",	MNIF_NO_FLAGS },
		{ MSG_43x80_MODE,		VOIDF(CONTROL_WINDOW::Message),	"&3-(43/50)x80 text mode", MNIF_NO_FLAGS },
		{ MSG_GRAPHICS_MODE,	VOIDF(CONTROL_WINDOW::Message),	"&4-Graphics mode",		MNIF_NO_FLAGS },
//		{ MSG_WINDOWS_MODE,		VOIDF(CONTROL_WINDOW::Message),	"&5-Windows 3.x mode",	MNIF_NON_SELECTABLE },
		{ 0, 0, 0, 0 }	// End of array.
	};
#endif

	// Event menu items.
	static UI_ITEM eventItems[] =
	{
		{ MSG_EVENT_MONITOR,	VOIDF(CONTROL_WINDOW::Message),	"&Event monitor",		MNIF_NO_FLAGS },
		{ 0, 0, 0, 0 }	// End of array.
	};

	// Help menu items.	
	static UI_ITEM helpItems[] =
	{
		{ MSG_HELP_KEYBOARD,	VOIDF(CONTROL_WINDOW::Message),	"&Keyboard",			MNIF_NO_FLAGS },
		{ MSG_HELP_MOUSE,		VOIDF(CONTROL_WINDOW::Message),	"&Mouse",				MNIF_NO_FLAGS },
		{ MSG_HELP_COMMANDS,	VOIDF(CONTROL_WINDOW::Message),	"&Commands",			MNIF_NO_FLAGS },
		{ MSG_HELP_PROCEDURES,	VOIDF(CONTROL_WINDOW::Message),	"&Procedures",			MNIF_NO_FLAGS },
		{ MSG_HELP_OBJECTS,		VOIDF(CONTROL_WINDOW::Message),	"&Objects",				MNIF_NO_FLAGS },
		{ MSG_HELP_HELP,		VOIDF(CONTROL_WINDOW::Message),	"&Using help",			MNIF_NO_FLAGS },
		{ 0,					VOIDF(0),			"",						MNIF_SEPARATOR },
		{ MSG_HELP_ZINCAPP,		VOIDF(About),		"&About ...",			MNIF_NO_FLAGS },
		{ 0, 0, 0, 0 }	// End of array.
	};


	// Create the objects submenu.
	UIW_POP_UP_ITEM *controlObjects = new UIW_POP_UP_ITEM("&Control objects");
	*controlObjects
		+ new UIW_POP_UP_ITEM("&Button window...", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, CONTROL_WINDOW::Message, MSG_BUTTON_WINDOW)
		+ new UIW_POP_UP_ITEM("&Generic window...", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, CONTROL_WINDOW::Message, MSG_GENERIC_WINDOW)
		+ new UIW_POP_UP_ITEM("&Icon window...", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, CONTROL_WINDOW::Message, MSG_ICON_WINDOW)
		+ new UIW_POP_UP_ITEM("&MDI window...", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, CONTROL_WINDOW::Message, MSG_MDI_WINDOW);
	UIW_POP_UP_ITEM *inputObjects = new UIW_POP_UP_ITEM("&Input objects");
	*inputObjects
		+ new UIW_POP_UP_ITEM("&Date window...", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, CONTROL_WINDOW::Message, MSG_DATE_WINDOW)
		+ new UIW_POP_UP_ITEM("&Number window...", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, CONTROL_WINDOW::Message, MSG_NUMBER_WINDOW)
		+ new UIW_POP_UP_ITEM("&String window...", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, CONTROL_WINDOW::Message, MSG_STRING_WINDOW)
		+ new UIW_POP_UP_ITEM("&Text window...", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, CONTROL_WINDOW::Message, MSG_TEXT_WINDOW)
		+ new UIW_POP_UP_ITEM("T&ime window...", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, CONTROL_WINDOW::Message, MSG_TIME_WINDOW);
	UIW_POP_UP_ITEM *selectObjects = new UIW_POP_UP_ITEM("&Selection objects");
	*selectObjects
		+ new UIW_POP_UP_ITEM("&Combo Box window...", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, CONTROL_WINDOW::Message, MSG_COMBO_BOX_WINDOW)
		+ new UIW_POP_UP_ITEM("&List window...", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, CONTROL_WINDOW::Message, MSG_LIST_WINDOW)
		+ new UIW_POP_UP_ITEM("&Menu window...", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, CONTROL_WINDOW::Message, MSG_MENU_WINDOW)
		+ new UIW_POP_UP_ITEM("&Tool Bar window...", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, CONTROL_WINDOW::Message, MSG_TOOL_BAR_WINDOW);

	// Attach the menu and support objects to the control window.
	*this
		+ new UIW_BORDER
		+ new UIW_MAXIMIZE_BUTTON
		+ new UIW_MINIMIZE_BUTTON
		+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
		+ new UIW_TITLE("Zinc Application")
		+ &(*new UIW_PULL_DOWN_MENU
			+ new UIW_PULL_DOWN_ITEM("&Control", WNF_NO_FLAGS, controlItems)
#if defined(ZIL_MSDOS)
			+ new UIW_PULL_DOWN_ITEM("&Display", WNF_NO_FLAGS, displayItems)
#endif
			+ &(*new UIW_PULL_DOWN_ITEM("&Window", WNF_NO_FLAGS)
				+ controlObjects
				+ inputObjects
				+ selectObjects)
			+ new UIW_PULL_DOWN_ITEM("&Event", WNF_NO_FLAGS, eventItems)
			+ new UIW_PULL_DOWN_ITEM("&Help", WNF_NO_FLAGS, helpItems));
}

EVENT_TYPE CONTROL_WINDOW::Event(const UI_EVENT &event)
{
	// Check for an accelerator key.
	EVENT_TYPE ccode = event.type;
	if (ccode == L_EXIT_FUNCTION)
		eventManager->Put(UI_EVENT(L_EXIT_FUNCTION, 0));

	if (ccode == E_KEY)
	{
		// Define the set of accelerator keys.
		static struct ACCELERATOR_PAIR
		{
			RAW_CODE rawCode;
			LOGICAL_EVENT logicalType;
		} acceleratorTable[] =
		{
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
			{ SHIFT_F5,		S_DEINITIALIZE },
			{ SHIFT_F6,		S_REDISPLAY },
			{ ALT_F4,		L_EXIT_FUNCTION },
#elif defined(ZIL_MOTIF)
			{ XK_F5,		S_DEINITIALIZE },
			{ XK_F6,		S_REDISPLAY },
			{ XK_F4,		L_EXIT_FUNCTION },
#endif
			{ 0, 0 }	// End of array.
		};

		for (int i = 0; acceleratorTable[i].rawCode; i++)
			if (event.rawCode == acceleratorTable[i].rawCode)
			{
				UI_EVENT tEvent(acceleratorTable[i].logicalType);
				eventManager->Put(tEvent);	// Put the accelarator key
				return (ccode);				// into the system.
			}
	}

	// Process the event according to its type.
	if (ccode >= MSG_LAST || ccode <= MSG_FIRST)
		ccode = UIW_WINDOW::Event(event);	// Not a MSG event.
	else if (ccode >= MSG_HELP)
		OptionHelp(event.type);				// Help menu option selected.
	else if (ccode >= MSG_EVENT)
		OptionEvent(event.type);			// Event menu option selected.
	else if (ccode >= MSG_WINDOW)
		OptionWindow(event.type);			// Window menu option selected.
	else if (ccode >= MSG_DISPLAY)
		OptionDisplay(event.type);			// Display menu option selected.
	else
		ccode = S_UNKNOWN;

	// Return the control code.
	return (ccode);
}

EVENT_TYPE CONTROL_WINDOW::Message(UI_WINDOW_OBJECT *object, UI_EVENT &event,
	EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		for (UI_WINDOW_OBJECT *tObject = object->windowManager->First();
			tObject && FlagSet(tObject->woAdvancedFlags, WOAF_TEMPORARY);
			tObject = tObject->Next())
			object->eventManager->Put(UI_EVENT(S_CLOSE_TEMPORARY, 0));
		event.type = ((UIW_POP_UP_ITEM *)object)->value;
		object->eventManager->Put(event);
	}
	return (ccode);
}

EVENT_TYPE ZINCAPP_WINDOW_MANAGER::ExitFunction(UI_DISPLAY *display,
	UI_EVENT_MANAGER *, UI_WINDOW_MANAGER *windowManager)
{
	// Compute the window region.
	int width = 48;
	int height = display->isText ? 7 : 6;
	int left = (display->columns / display->cellWidth - width) / 2;
	int top = (display->lines / display->cellHeight - height) / 2;

	// Create the exit window.
	char *message = "This will close the ZincApp tutorial.";
	UIW_WINDOW *window = new UIW_WINDOW(left, top, width, height,
		WOF_NO_FLAGS, WOAF_MODAL | WOAF_NO_SIZE);
	*window
		+ new UIW_BORDER
		+ &(*new UIW_SYSTEM_BUTTON
			+ new UIW_POP_UP_ITEM("&Move", MNIF_MOVE)
			+ new UIW_POP_UP_ITEM("&Close\tAlt+F4", MNIF_CLOSE))
		+ new UIW_TITLE("Zinc Application");
	if (display->isText)
		*window
			+ new UIW_STRING(2, 1, 37, message, 100, STF_NO_FLAGS, WOF_NON_SELECTABLE | WOF_JUSTIFY_CENTER);
	else
		*window
			+ new UIW_ICON(2, 1, "ASTERISK")
			+ new UIW_STRING(10, 1, 34, message, 100, STF_NO_FLAGS, WOF_NON_SELECTABLE | WOF_JUSTIFY_CENTER);
	*window
		+ new UIW_BUTTON(10, height - 3, 10, "~OK",
			BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE,
			WOF_JUSTIFY_CENTER, NULL, L_EXIT)
		+ new UIW_BUTTON(25, height - 3, 10, "~Cancel",
			BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE,
			WOF_JUSTIFY_CENTER, NULL, S_CLOSE);
	*windowManager + window;
	return (S_CONTINUE);
}
