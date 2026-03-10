//	CALENDAR.CPP (CALENDAR) - Calendar example.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#define USE_RAW_KEYS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ui_win.hpp>

#define USE_HELP_CONTEXTS
#include "calendar.hpp"

const int ID_CALENDAR				= 10000;
const int L_PREVIOUS_MONTH			= 10001;
const int L_NEXT_MONTH				= 10002;
const int L_CURRENT_MONTH			= 10003;
const int L_NEW_MONTH				= 10004;

char *days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

class EXPORT DAYS_OF_MONTH : public UI_WINDOW_OBJECT
{
public:
	UI_DATE date;

	DAYS_OF_MONTH(void) : UI_WINDOW_OBJECT(0, 0, 0, 0, WOF_NON_FIELD_REGION | WOF_NON_SELECTABLE, WOAF_NO_FLAGS) {}
	virtual ~DAYS_OF_MONTH(void) {}

	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);

private:
	int xLength, yLength;
};

class EXPORT CALENDAR : public UIW_WINDOW
{
public:
	CALENDAR(int left, int top, int width, int height,
		USHORT _woFlags, USHORT _woAdvancedFlags, int _helpContext,
		UI_EVENT_MAP *newEventMapTable);
	virtual ~CALENDAR(void) {}

	virtual EVENT_TYPE Event(const UI_EVENT &event);

private:
	DAYS_OF_MONTH *daysOfMonth;
	UIW_TITLE *calendarTitle;
   	UI_DATE date;
	int day;
	int month;
	int year;
};

// Event map table.
#if defined(ZIL_MSDOS)
static UI_EVENT_MAP myEventMapTable[] =
{
	{ ID_WINDOW_MANAGER, 	L_EXIT_FUNCTION,	E_KEY, 		SHIFT_F3 },
	{ ID_WINDOW_MANAGER, 	L_EXIT_FUNCTION,	E_KEY, 		ALT_F4 },
	{ ID_WINDOW_MANAGER, 	L_EXIT_FUNCTION,	E_KEY, 		CTRL_BREAK },
	{ ID_WINDOW_MANAGER, 	L_EXIT_FUNCTION,	E_KEY, 		CTRL_C },
	{ ID_WINDOW_MANAGER, 	S_CLOSE_TEMPORARY,	E_KEY, 		ESCAPE },
	{ ID_WINDOW_MANAGER, 	L_HELP,				E_KEY, 		ALT_F1 },
	{ ID_WINDOW_MANAGER, 	S_CLOSE,			E_KEY, 		SHIFT_F4 },
	{ ID_WINDOW_MANAGER, 	S_CLOSE,			E_KEY, 		F3	 },
	{ ID_WINDOW_MANAGER, 	L_MOVE,				E_KEY, 		ALT_F7 },
	{ ID_WINDOW_MANAGER, 	L_SIZE,				E_KEY, 		ALT_F8 },
	{ ID_WINDOW_MANAGER, 	L_NEXT,				E_KEY, 		ALT_F6 },
	{ ID_WINDOW_MANAGER, 	L_NEXT,				E_KEY, 		CTRL_TAB },
	{ ID_WINDOW_MANAGER, 	S_REDISPLAY,		E_KEY, 		F5 },
	{ ID_WINDOW_MANAGER,	L_HELP,				E_KEY, 		ALT_F1 },
	{ ID_WINDOW_MANAGER, 	L_VIEW,				E_MOUSE, 	0 },
	{ ID_WINDOW_MANAGER, 	L_BEGIN_SELECT,		E_MOUSE, 	M_LEFT | M_LEFT_CHANGE },
	{ ID_WINDOW_MANAGER, 	L_CONTINUE_SELECT,	E_MOUSE, 	M_LEFT },
	{ ID_WINDOW_MANAGER, 	L_END_SELECT,		E_MOUSE, 	M_LEFT_CHANGE },
	{ ID_WINDOW_MANAGER, 	L_BEGIN_SELECT,		E_MOUSE, 	M_RIGHT | M_RIGHT_CHANGE },
	{ ID_WINDOW_MANAGER, 	L_CONTINUE_SELECT,	E_MOUSE, 	M_RIGHT },
	{ ID_WINDOW_MANAGER, 	L_END_SELECT,		E_MOUSE, 	M_RIGHT_CHANGE },

	{ ID_WINDOW_OBJECT, 	L_SELECT,			E_KEY, 		ENTER },
	{ ID_WINDOW_OBJECT, 	L_SELECT,			E_KEY, 		GRAY_ENTER },
	{ ID_WINDOW_OBJECT, 	L_VIEW,				E_MOUSE, 	0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_SELECT,		E_MOUSE, 	M_LEFT | M_LEFT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_SELECT,	E_MOUSE, 	M_LEFT },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT,		E_MOUSE, 	M_LEFT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_SELECT,		E_MOUSE, 	M_RIGHT | M_RIGHT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_SELECT,	E_MOUSE, 	M_RIGHT },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT,		E_MOUSE, 	M_RIGHT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_RESTORE,			E_KEY, 		ALT_F5 },
	{ ID_WINDOW_OBJECT,		S_ALT_KEY,			E_KEY, 		F10 },
	{ ID_WINDOW_OBJECT,		L_HELP,				E_KEY, 		F1 },

	{ ID_WINDOW_OBJECT, 	L_UP,				E_KEY, 		GRAY_UP_ARROW },
	{ ID_WINDOW_OBJECT, 	L_UP,				E_KEY, 		WHITE_UP_ARROW },
	{ ID_WINDOW_OBJECT, 	L_DOWN,				E_KEY, 		GRAY_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, 	L_DOWN,				E_KEY, 		WHITE_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, 	L_LEFT,				E_KEY, 		GRAY_LEFT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_LEFT,				E_KEY, 		WHITE_LEFT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,			E_KEY, 		GRAY_RIGHT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,			E_KEY, 		WHITE_RIGHT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_PREVIOUS,			E_KEY, 		BACKTAB },
	{ ID_WINDOW_OBJECT, 	L_PREVIOUS,			E_KEY, 		SHIFT_F6 },
	{ ID_WINDOW_OBJECT, 	L_NEXT,				E_KEY, 		TAB },
	{ ID_WINDOW_OBJECT, 	L_NEXT,				E_KEY, 		F6 },
	{ ID_WINDOW_OBJECT, 	L_PGUP,				E_KEY, 		WHITE_PGUP },
	{ ID_WINDOW_OBJECT, 	L_PGUP,				E_KEY, 		GRAY_PGUP },
	{ ID_WINDOW_OBJECT, 	L_PGDN,				E_KEY, 		WHITE_PGDN },
	{ ID_WINDOW_OBJECT, 	L_PGDN,				E_KEY, 		GRAY_PGDN },
	{ ID_WINDOW_OBJECT,		L_TOP,				E_KEY,	 	CTRL_WHITE_HOME},
	{ ID_WINDOW_OBJECT,		L_TOP,				E_KEY,	 	CTRL_GRAY_HOME},
	{ ID_WINDOW_OBJECT,		L_BOTTOM,			E_KEY,	 	CTRL_WHITE_END},
	{ ID_WINDOW_OBJECT,		L_BOTTOM,			E_KEY,	 	CTRL_GRAY_END},

	{ ID_CALENDAR,		L_NEW_MONTH,		E_KEY, 		F2 },
   	{ ID_CALENDAR,		L_PREVIOUS_MONTH,	E_KEY, 		WHITE_PGUP },
	{ ID_CALENDAR,		L_PREVIOUS_MONTH,	E_KEY, 		GRAY_PGUP },
	{ ID_CALENDAR,		L_NEXT_MONTH,		E_KEY, 		GRAY_PGDN },
	{ ID_CALENDAR,		L_NEXT_MONTH,		E_KEY, 		WHITE_PGDN },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#elif defined(ZIL_MSWINDOWS)
static UI_EVENT_MAP myEventMapTable[] =
{
	{ ID_WINDOW_OBJECT, 	L_UP,				WM_KEYDOWN, 	GRAY_UP_ARROW },
	{ ID_WINDOW_OBJECT, 	L_UP,				WM_KEYDOWN, 	WHITE_UP_ARROW },
	{ ID_WINDOW_OBJECT, 	L_DOWN,				WM_KEYDOWN, 	GRAY_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, 	L_DOWN,				WM_KEYDOWN, 	WHITE_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, 	L_LEFT,				WM_KEYDOWN, 	GRAY_LEFT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_LEFT,				WM_KEYDOWN, 	WHITE_LEFT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,			WM_KEYDOWN, 	GRAY_RIGHT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,			WM_KEYDOWN, 	WHITE_RIGHT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_PREVIOUS,			WM_CHAR, 		BACKTAB },
	{ ID_WINDOW_OBJECT, 	L_NEXT,				WM_CHAR, 		TAB },
	{ ID_WINDOW_OBJECT, 	L_SELECT,			WM_CHAR, 		ENTER },
	{ ID_WINDOW_OBJECT,		L_HELP,				WM_KEYUP, 		F1 },
	{ ID_WINDOW_OBJECT, 	L_HELP,				WM_KEYUP, 		ALT_F1 },

	{ ID_CALENDAR,		L_NEW_MONTH,		WM_KEYDOWN,		F2 },
   	{ ID_CALENDAR,		L_PREVIOUS_MONTH,	WM_KEYDOWN,    	WHITE_PGUP },
	{ ID_CALENDAR,		L_PREVIOUS_MONTH,	WM_KEYDOWN,    	GRAY_PGUP },
	{ ID_CALENDAR,		L_NEXT_MONTH,		WM_KEYDOWN,    	GRAY_PGDN },
	{ ID_CALENDAR,		L_NEXT_MONTH,		WM_KEYDOWN,    	WHITE_PGDN },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#elif defined(ZIL_OS2)
static UI_EVENT_MAP myEventMapTable[] =
{
	{ ID_CALENDAR,		L_NEW_MONTH,		WM_CHAR,		F2,			KC_KEYUP },
   	{ ID_CALENDAR,		L_PREVIOUS_MONTH,	WM_CHAR,    	WHITE_PGUP,	KC_KEYUP },
	{ ID_CALENDAR,		L_PREVIOUS_MONTH,	WM_CHAR,    	GRAY_PGUP,	KC_KEYUP },
	{ ID_CALENDAR,		L_NEXT_MONTH,		WM_CHAR,    	GRAY_PGDN,	KC_KEYUP },
	{ ID_CALENDAR,		L_NEXT_MONTH,		WM_CHAR,    	WHITE_PGDN,	KC_KEYUP },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#elif defined(ZIL_MOTIF)
static UI_EVENT_MAP myEventMapTable[] =
{
    { ID_WINDOW_OBJECT,     S_REDISPLAY,    Expose,         0 },
	{ ID_WINDOW_OBJECT,     S_SIZE,         ConfigureNotify,0 },
	{ ID_WINDOW_OBJECT,     L_PGUP,         KeyPress,       XK_Prior },
	{ ID_WINDOW_OBJECT,     L_PGDN,         KeyPress,       XK_Next },
    { ID_WINDOW_OBJECT,     L_PGUP,         KeyPress,       osfXK_PageUp },
	{ ID_WINDOW_OBJECT,     L_PGDN,         KeyPress,       osfXK_PageDown },
	{ ID_CALENDAR,			L_NEW_MONTH,	KeyPress,		F2 },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#endif

// Special palettes
static UI_PALETTE dayPalette = { ' ', attrib(RED, WHITE),
	attrib(MONO_BLACK, MONO_BLACK), PTN_SOLID_FILL, RED, WHITE,
	BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK };

static UI_PALETTE currentPalette = { ' ', attrib(GREEN, WHITE),
	attrib(MONO_BLACK, MONO_BLACK), PTN_SOLID_FILL, GREEN, WHITE,
	BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK };

static UI_PALETTE linePalette = { ' ', attrib(DARKGRAY, WHITE),
	attrib(MONO_BLACK, MONO_BLACK), PTN_SOLID_FILL, DARKGRAY, WHITE,
	BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK };

static UI_PALETTE whitePalette = { ' ', attrib(WHITE, WHITE),
	attrib(MONO_HIGH, MONO_HIGH), PTN_SOLID_FILL, WHITE, WHITE,
	BW_WHITE, BW_WHITE, GS_WHITE, GS_WHITE };

// Put pull-down-item messages on the queue				
EVENT_TYPE Message(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != L_SELECT)
		return 0;
	UIW_BUTTON *item = (UIW_BUTTON *)object;
	if (item->value == L_CURRENT_MONTH)
	{
		object->eventManager->Put(UI_EVENT(S_CLOSE, 0));
		object->eventManager->Put(UI_EVENT(L_CURRENT_MONTH, 0));
	}
	else
		object->eventManager->Put(UI_EVENT(item->value, 0));
	return (S_CONTINUE);
}

EVENT_TYPE DAYS_OF_MONTH::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = UI_WINDOW_OBJECT::Event(event);
	switch (ccode)
	{
	case S_CREATE:
	case S_SIZE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		true -= display->cellWidth; 
		xLength = true.Width();
		yLength = true.Height() - display->cellHeight;

		// Register the object with Windows API.
		woStatus |= WOS_OWNERDRAW;
		RegisterObject("DAYS_OF_MONTH");
		break;

	}

	// Return the control code.
	return (ccode);
}

EVENT_TYPE DAYS_OF_MONTH::DrawItem(const UI_EVENT &, EVENT_TYPE )
{
    // Get the Current Day
	int currentDay;
	int year, month;
	date.Export(&year, &month, &currentDay);

	// Find the startingDay.
	date.Import(year, month, 1);
	int startingDay = date.DayOfWeek();

	// Now set date information.
	date.Import(year, month, currentDay);
   	int monthDays = date.DaysInMonth();
	int weeksInMonth = (startingDay + monthDays - 1) / 7;
	if ((startingDay + monthDays -1) % 7)
		weeksInMonth++;
	 
	// Change cellWidth and cellHeights
   	int cellWidth = xLength / 7;
   	int cellHeight = yLength / weeksInMonth;
   	if (cellHeight == 0)
   		cellHeight++;

   	int i, top = 1;
	int left = startingDay;
	int middle = cellWidth - cellWidth / 2;

	// Draw the calendar in the middle of the window.
	UI_REGION cal = true;
   	int offset = (cal.right - cal.left) % 7;
   	cal.left += offset / 2;
   	cal.right -= offset / 2;
	cal.top += display->cellHeight;
	if (!display->isText)
	{
   		offset = (cal.bottom - cal.top) % 7;
		cal.top += offset / 2;
		cal.bottom -= offset /2;
	}

	display->VirtualGet(screenID, true);

	display->Rectangle(screenID, true, &whitePalette, 1, TRUE);
	if (yLength > weeksInMonth && xLength >= 28)
		display->Rectangle(screenID, cal, &linePalette);
	for (i = 1;  i <=7; i++)
		display->Text(screenID, cal.left + i * cellWidth - middle - display->TextWidth(days[i-1]) / 2,
			cal.top - display->TextHeight(days[i-1]), days[i-1], &linePalette, -1, FALSE);
    if (!display->isText)
	{
   		for (i = 0;  i < weeksInMonth; i++)
			display->Line(screenID, cal.left, cal.top + i * cellHeight, cal.right, cal.top + i * cellHeight,
				&linePalette);
		for (i = 1;  i <= 6; i++)
			display->Line(screenID, cal.left + i * cellWidth, cal.top, cal.left + i * cellWidth, cal.bottom,
				&linePalette);
	}
	else if (yLength <= weeksInMonth)
		cal.top --;

	// Now print the days of the month;
	char buf[3];
	for (i = 1;  i <= monthDays; i++)
	{
		ui_itoa(i, buf, 10);	
//		if (i == currentDay)	  											// BUG.General
		UI_DATE today;
		if (date == today && i == currentDay)
			display->Text(screenID, cal.left + left * cellWidth - middle - display->TextWidth(buf) / 2,
				cal.top + top * cellHeight - cellHeight / 2 - display->TextHeight(buf) / 2, buf, &currentPalette,
				-1, FALSE);
		else display->Text(screenID, cal.left + left * cellWidth - middle - display->TextWidth(buf) / 2,
			cal.top + top * cellHeight - cellHeight / 2 - display->TextHeight(buf) / 2, buf, &dayPalette,
				-1, FALSE);
		if (left % 7 == 0)
		{
			left = 1;
			top++;
		}
		else left++;
	}

	display->VirtualPut(screenID);
	return (TRUE);
}

CALENDAR::CALENDAR(int left, int top, int width, int height,
	USHORT _woFlags, USHORT _woAdvancedFlags, UI_HELP_CONTEXT _helpContext = NULL,
	UI_EVENT_MAP *newEventMapTable = NULL) : UIW_WINDOW(left, top, width,
	height, _woFlags,_woAdvancedFlags, _helpContext)
{
	// Set the eventMapTable for the calendar.
	eventMapTable = newEventMapTable;

	// Assign IDs for both a calendar and window.
	windowID[0] = ID_CALENDAR;
	windowID[1] = ID_WINDOW;
	
	static char dateString[20];
	date.Export(&year, &month, &day, 0);
	sprintf(dateString, "%s %d", UI_DATE::monthTable[(month-1)*2+1], year);

	*this
		+ new UIW_BORDER
		+ new UIW_MAXIMIZE_BUTTON
		+ new UIW_MINIMIZE_BUTTON
		+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
  		+ (calendarTitle = new UIW_TITLE(dateString, WOF_JUSTIFY_CENTER))
		+ &(* new UIW_PULL_DOWN_MENU
			+ new UIW_PULL_DOWN_ITEM("&View", WNF_NO_FLAGS, Message, L_NEW_MONTH)
			+ new UIW_PULL_DOWN_ITEM("&Next", WNF_NO_FLAGS, Message, L_NEXT_MONTH)
			+ new UIW_PULL_DOWN_ITEM("&Previous", WNF_NO_FLAGS, Message, L_PREVIOUS_MONTH)
		  	+ new UIW_PULL_DOWN_ITEM("&Help", WNF_NO_FLAGS, Message, L_HELP))
		+ (daysOfMonth = new DAYS_OF_MONTH());

	// Sets the CALENDAR class date -- defaults to the system date.
	daysOfMonth->date = date;
}

EVENT_TYPE CALENDAR::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_CALENDAR);
	switch (ccode)
	{
	case L_NEW_MONTH:
		{
		int windowHeight = display->isText ? 5 : 3;
		UIW_WINDOW *window = new UIW_WINDOW(0, 0, 30, windowHeight, WOF_BORDER, WOAF_MODAL);
		*window
			+ new UIW_TITLE("View Month")
			+ new UIW_PROMPT(2, 0, "New Date...")
			+ new UIW_DATE(2, 1, 15, &date, NULL, DTF_NO_FLAGS, WOF_NO_ALLOCATE_DATA | WOF_AUTO_CLEAR)
			+ new UIW_BUTTON(20, 0, 8, "&OK", BTF_NO_TOGGLE,
				WOF_JUSTIFY_CENTER, Message, L_CURRENT_MONTH)
			+ new UIW_BUTTON(20, 1, 8, "&Cancel", BTF_NO_TOGGLE | BTF_SEND_MESSAGE,
				WOF_JUSTIFY_CENTER, NULL, S_CLOSE);
		windowManager->Add(window);
		}
		break;

//	case L_NEXT_MONTH:	 													// BUG.GENERAL
//    	// Increment the month by 2 to allow for the following decrement.
//    	month += 2;
//    	// Continue on to L_PREVIOUS_MONTH.
//
//	case L_PREVIOUS_MONTH:
//    	if (--month > 12)
//    	{
//        	month = 1;
//        	year++;
//    	}
//    	if (!month)
//    	{
//        	month = 12;
//        	year--;
//    	}
//    	date.Import(year, month, day);

	case L_NEXT_MONTH:
	case L_PREVIOUS_MONTH:
		if (ccode == L_NEXT_MONTH)
		{
			if (++month > 12)
			{
				month = 1;
				++year;
			}
		}
		else
		{
			if (--month < 1)
			{
				month = 12;
				--year;
			}
		}
		date.Import(year, month, 1);
		// Continue to L_CURRENT_MONTH.

	case L_CURRENT_MONTH:
		{
		date.Export(&year, &month, NULL, 0);
		char monthString[20];

		// Setup the new date with only the month and the year.
		// This is done to setup the title string.
		date.Import(year, month, 1);

		// Get the month and year string for the title.
		sprintf(monthString, "%s %d", UI_DATE::monthTable[(month-1)*2+1], year);

		// Reset the date to the current date.
		date.Import(year, month, day);

		// Change the title to show new month.
		calendarTitle->DataSet(monthString);
		daysOfMonth->date = date;
	  
		daysOfMonth->Event(UI_EVENT(S_REDISPLAY, 0));

		ccode = UIW_WINDOW::Event(event);
		break;
		}

	default:
		// Call the window event to process other events.
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	// Initialize the help system.
	UI_WINDOW_OBJECT::helpSystem = new UI_HELP_SYSTEM("calendar.dat", windowManager,
		GENERAL_HELP);

	*windowManager
		+  new CALENDAR(18, 2, 50, 13, WOF_NO_FLAGS, WOAF_NO_FLAGS,
			CALENDAR_HELP, myEventMapTable);
	windowManager->screenID = windowManager->First()->screenID;		// BUG.1965

	// Process user responses.
	UI_APPLICATION::Control();

	// Clean up.
	delete UI_WINDOW_OBJECT::helpSystem;

	return (0);
}
