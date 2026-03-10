//	VLIST.HPP (VLIST) - This file contains source code for virtual list tutorial.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#define USE_RAW_KEYS			// Zinc raw keys.
#include <stdio.h>
#include <ui_win.hpp>
#include "vlist.hpp"
#if defined(ZIL_POSIX)
#	include <unistd.h>
#endif

// The information file "vlist.txt" really contains fixed length records.
// The words are listed in alphabetical order and padded to 78 characters
// with spaces.  The file is formatted as a text file, so each operating 
// system has its own representation of the end-of-line.

#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(__DVX__)
#	define EOL_LENGTH	2
#elif defined(ZIL_UNIX)
#	define EOL_LENGTH	1
#endif

#if defined(ZIL_MSDOS)
static UI_EVENT_MAP _eventMapTable[] =
{
	{ ID_WINDOW_OBJECT, L_UP,				E_KEY, 		GRAY_UP_ARROW },	// BUG.7029
	{ ID_WINDOW_OBJECT, L_UP,				E_KEY, 		WHITE_UP_ARROW },
	{ ID_WINDOW_OBJECT, L_DOWN,				E_KEY, 		GRAY_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, L_DOWN,				E_KEY, 		WHITE_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, L_PREVIOUS,			E_KEY, 		BACKTAB },
	{ ID_WINDOW_OBJECT, L_PREVIOUS,			E_KEY, 		SHIFT_F6 },
	{ ID_WINDOW_OBJECT, L_NEXT,				E_KEY, 		TAB },
	{ ID_WINDOW_OBJECT, L_NEXT,				E_KEY, 		F6 },
	{ ID_WINDOW_OBJECT, L_PGUP,				E_KEY, 		GRAY_PGUP },
	{ ID_WINDOW_OBJECT, L_PGUP,				E_KEY, 		WHITE_PGUP },
	{ ID_WINDOW_OBJECT, L_PGDN,				E_KEY, 		GRAY_PGDN },
	{ ID_WINDOW_OBJECT, L_PGDN,				E_KEY, 		WHITE_PGDN },
	{ ID_WINDOW_OBJECT,	L_TOP,				E_KEY, 		CTRL_WHITE_HOME },
	{ ID_WINDOW_OBJECT,	L_TOP,				E_KEY, 		CTRL_GRAY_HOME },
	{ ID_WINDOW_OBJECT,	L_BOTTOM,			E_KEY, 		CTRL_WHITE_END },
	{ ID_WINDOW_OBJECT,	L_BOTTOM,			E_KEY, 		CTRL_GRAY_END },
	{ ID_WINDOW_OBJECT,	L_BOL,				E_KEY, 		WHITE_HOME },
	{ ID_WINDOW_OBJECT,	L_BOL,				E_KEY, 		GRAY_HOME },
	{ ID_WINDOW_OBJECT,	L_EOL,				E_KEY, 		WHITE_END },
	{ ID_WINDOW_OBJECT,	L_EOL,				E_KEY, 		GRAY_END },
	{ ID_WINDOW_OBJECT, L_BEGIN_SELECT,		E_MOUSE, 	M_LEFT | M_LEFT_CHANGE },
	{ ID_WINDOW_OBJECT, L_CONTINUE_SELECT,	E_MOUSE, 	M_LEFT },
	{ ID_WINDOW_OBJECT, L_END_SELECT,		E_MOUSE, 	M_LEFT_CHANGE },

	{ ID_WINDOW_OBJECT, S_CLOSE,			E_KEY, ALT_F4 },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#elif defined(ZIL_MSWINDOWS)
static UI_EVENT_MAP _eventMapTable[] =
{
	{ ID_WINDOW_OBJECT, L_UP,		WM_KEYDOWN, GRAY_UP_ARROW },
	{ ID_WINDOW_OBJECT, L_UP,		WM_KEYDOWN, WHITE_UP_ARROW },
	{ ID_WINDOW_OBJECT, L_DOWN,		WM_KEYDOWN, GRAY_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, L_DOWN,		WM_KEYDOWN, WHITE_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, L_PREVIOUS,	WM_KEYDOWN, BACKTAB },
	{ ID_WINDOW_OBJECT, L_PREVIOUS,	WM_KEYDOWN, SHIFT_F6 },
	{ ID_WINDOW_OBJECT, L_NEXT,		WM_KEYDOWN, TAB },
	{ ID_WINDOW_OBJECT, L_NEXT,		WM_KEYDOWN, F6 },
	{ ID_WINDOW_OBJECT, L_PGUP,		WM_KEYDOWN, GRAY_PGUP },
	{ ID_WINDOW_OBJECT, L_PGUP,		WM_KEYDOWN, WHITE_PGUP },
	{ ID_WINDOW_OBJECT, L_PGDN,		WM_KEYDOWN, GRAY_PGDN },
	{ ID_WINDOW_OBJECT, L_PGDN,		WM_KEYDOWN, WHITE_PGDN },
	{ ID_WINDOW_OBJECT,	L_TOP,		WM_KEYDOWN, CTRL_WHITE_HOME },
	{ ID_WINDOW_OBJECT,	L_TOP,		WM_KEYDOWN, CTRL_GRAY_HOME },
	{ ID_WINDOW_OBJECT,	L_BOTTOM,	WM_KEYDOWN, CTRL_WHITE_END },
	{ ID_WINDOW_OBJECT,	L_BOTTOM,	WM_KEYDOWN, CTRL_GRAY_END },
	{ ID_WINDOW_OBJECT,	L_BOL,		WM_KEYDOWN, WHITE_HOME },
	{ ID_WINDOW_OBJECT,	L_BOL,		WM_KEYDOWN, GRAY_HOME },
	{ ID_WINDOW_OBJECT,	L_EOL,		WM_KEYDOWN, WHITE_END },
	{ ID_WINDOW_OBJECT,	L_EOL,		WM_KEYDOWN, GRAY_END },

	{ ID_WINDOW_OBJECT, S_CLOSE,	WM_KEYDOWN, ALT_F4 },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#elif defined(ZIL_OS2)
static UI_EVENT_MAP _eventMapTable[] =
{
	{ ID_WINDOW_OBJECT, L_UP,		WM_CHAR, GRAY_UP_ARROW,		0 },
	{ ID_WINDOW_OBJECT, L_UP,		WM_CHAR, WHITE_UP_ARROW,	0 },
	{ ID_WINDOW_OBJECT, L_DOWN,		WM_CHAR, GRAY_DOWN_ARROW,	0 },
	{ ID_WINDOW_OBJECT, L_DOWN,		WM_CHAR, WHITE_DOWN_ARROW,	0 },
	{ ID_WINDOW_OBJECT, L_PREVIOUS,	WM_CHAR, BACKTAB,			KC_KEYUP },
	{ ID_WINDOW_OBJECT, L_PREVIOUS,	WM_CHAR, SHIFT_F6,			KC_KEYUP },
	{ ID_WINDOW_OBJECT, L_NEXT,		WM_CHAR, TAB,				KC_KEYUP },
	{ ID_WINDOW_OBJECT, L_NEXT,		WM_CHAR, F6,				KC_KEYUP },
	{ ID_WINDOW_OBJECT, L_PGUP,		WM_CHAR, GRAY_PGUP,			0 },
	{ ID_WINDOW_OBJECT, L_PGUP,		WM_CHAR, WHITE_PGUP,		0 },
	{ ID_WINDOW_OBJECT, L_PGDN,		WM_CHAR, GRAY_PGDN,			0 },
	{ ID_WINDOW_OBJECT, L_PGDN,		WM_CHAR, WHITE_PGDN,		0 },
	{ ID_WINDOW_OBJECT,	L_TOP,		WM_CHAR, CTRL_WHITE_HOME,	0 },
	{ ID_WINDOW_OBJECT,	L_TOP,		WM_CHAR, CTRL_GRAY_HOME,	0 },
	{ ID_WINDOW_OBJECT,	L_BOTTOM,	WM_CHAR, CTRL_WHITE_END,	0 },
	{ ID_WINDOW_OBJECT,	L_BOTTOM,	WM_CHAR, CTRL_GRAY_END,		0 },
	{ ID_WINDOW_OBJECT,	L_BOL,		WM_CHAR, WHITE_HOME, 		0 },
	{ ID_WINDOW_OBJECT,	L_BOL,		WM_CHAR, GRAY_HOME, 		0 },
	{ ID_WINDOW_OBJECT,	L_EOL,		WM_CHAR, WHITE_END, 		0 },
	{ ID_WINDOW_OBJECT,	L_EOL,		WM_CHAR, GRAY_END, 			0 },

	{ ID_WINDOW_OBJECT, S_CLOSE,	WM_CHAR, ALT_F4,			KC_KEYUP },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#elif defined(ZIL_MOTIF)
static UI_EVENT_MAP _eventMapTable[] =
{
	{ ID_WINDOW_OBJECT, L_UP,		KeyPress, XK_Up },
	{ ID_WINDOW_OBJECT, L_DOWN,		KeyPress, XK_Down },
	{ ID_WINDOW_OBJECT,	L_PGUP,		KeyPress, XK_Prior },
	{ ID_WINDOW_OBJECT,	L_PGDN,		KeyPress, XK_Next },
	{ ID_WINDOW_OBJECT,	L_TOP,		KeyPress, XK_Home },
	{ ID_WINDOW_OBJECT,	L_BOTTOM,	KeyPress, XK_End },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#endif

EVENT_TYPE VIRTUAL_ELEMENT::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW);

	switch (ccode)
	{
	case S_INITIALIZE:
		eventMapTable = _eventMapTable;
		ccode = UIW_STRING::Event(event);
		if (!display->isText)
			relative.bottom = relative.top + height;
		break;

	case L_UP:
	case L_DOWN:
	case L_PREVIOUS:
	case L_NEXT:
	case L_PGUP:
	case L_PGDN:
	case L_TOP:
	case L_BOTTOM:
		// These events occur in bottom-up architectures (Windows, OS/2, Motif).
		parent->Event(event);
		break;

	default:
		ccode = UIW_STRING::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

VIRTUAL_LIST::VIRTUAL_LIST(const char *fileName, int _recordLength) :
	UIW_WINDOW(0, 0, 0, 0, WOF_NON_FIELD_REGION), recordLength(_recordLength),
	topRecordShowing(0), numberShowing(0)
{
	// Open the database, get the total number of records.
	file = fopen(fileName, "rb");
	fseek(file, 0L, SEEK_END);
	numberOfRecords = (int)(ftell(file) / recordLength);
}

VIRTUAL_LIST::~VIRTUAL_LIST(void)
{
	// Close the database.
	if (file)
		fclose(file);
}

EVENT_TYPE VIRTUAL_LIST::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW);

	switch (ccode)
	{
	case S_INITIALIZE:
		eventMapTable = _eventMapTable;
		ccode = UIW_WINDOW::Event(event);
		break;

	case S_CREATE:
	case S_SIZE:
		{
		ccode = UIW_WINDOW::Event(event);

		// Calculate the number of elements that will fit in the list.
//		int lineHeight = display->TextHeight("Mxq", screenID, font)
//			+ display->preSpace + display->postSpace;
		int lineHeight = display->cellHeight - display->preSpace -
			display->postSpace;												// BUG.cant_use_pre_and_postSpace_this_way.

		int newNumberShowing = (true.bottom - true.top + display->preSpace + display->postSpace);
		newNumberShowing /= lineHeight;
		if (display->isText)
			newNumberShowing++;

		// Make sure the window is full of records.
		numberShowing = newNumberShowing;
		Destroy();
		int right = true.right - true.left + 1;
		for (int line = 0; line < numberShowing; line++)
		{
			VIRTUAL_ELEMENT *element = new VIRTUAL_ELEMENT(0,
				line * lineHeight, right,
				lineHeight - 1, recordLength + 10);
			element->woStatus |= WOS_GRAPHICS;
			LoadRecord(element, topRecordShowing + line);
			Add(element);
		}
		Event(UI_EVENT(S_REDISPLAY, 0));
		}
		break;

	case L_PREVIOUS:
	case L_UP:
		if (Current() && Current()->Previous())
			Add(Current()->Previous());
		else if (topRecordShowing != 0 && First())
		{
			for (VIRTUAL_ELEMENT *object = Last(); object && object->Previous(); object = object->Previous())
				object->DataSet(object->Previous());
			LoadRecord(First(), --topRecordShowing);
		}
		break;

	case L_NEXT:
	case L_DOWN:
		if (Current() && Current()->Next())
			Add(Current()->Next());
		else if (topRecordShowing < numberOfRecords - 1 && First())
		{
			for (VIRTUAL_ELEMENT *object = First(); object && object->Next(); object = object->Next())
				object->DataSet(object->Next());
			LoadRecord(Last(), ++topRecordShowing + numberShowing - 1);
		}
		break;

	case L_PGUP:
		if (First() != Current())
			Add(First());
		else
		{
			topRecordShowing -= numberShowing;
			if (topRecordShowing < 0)
				topRecordShowing = 0;
			int recordNumber = topRecordShowing;
			for (VIRTUAL_ELEMENT *object = First(); object; object = object->Next())
				LoadRecord(object, recordNumber++);
		}
		break;

	case L_PGDN:
		if (Last() != Current())
			Add(Last());
		else
		{
			topRecordShowing += numberShowing;
			if (topRecordShowing > numberOfRecords - 1)
				topRecordShowing = numberOfRecords - 1;
			int recordNumber = topRecordShowing;
			for (VIRTUAL_ELEMENT *object = First(); object; object = object->Next())
				LoadRecord(object, recordNumber++);
		}
		break;

	case L_TOP:
		{
		topRecordShowing = 0;
		int recordNumber = topRecordShowing;
		for (VIRTUAL_ELEMENT *object = First(); object; object = object->Next())
			LoadRecord(object, recordNumber++);
		Add(First());
		}
		break;

	case L_BOTTOM:
		{
		topRecordShowing = numberOfRecords - 1;
		int recordNumber = topRecordShowing;
		for (VIRTUAL_ELEMENT *object = First(); object; object = object->Next())
			LoadRecord(object, recordNumber++);
		Add(Last());
		}
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void VIRTUAL_LIST::LoadRecord(VIRTUAL_ELEMENT *element, int recordNumber)
{
	// Load the record from the file.
	if (recordNumber > numberOfRecords - 1)
		element->DataSet(-1, "");
	else
	{
		long offset = recordLength * recordNumber;
		fseek(file, offset, SEEK_SET);
		char *text = element->DataGet();
		fgets(text, recordLength - 1, file);
		text[recordLength - 1] = '\0';
		element->DataSet(recordNumber, text);
	}
}

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	// Create a window containing the virtual list and attach it to the window manager.
	UIW_WINDOW *window = UIW_WINDOW::Generic(5, 2, 75, 12, "Dictionary");
	*window + new VIRTUAL_LIST("vlist.txt", 78+EOL_LENGTH);
	windowManager->Center(window);

	*windowManager + window;

	// Process user responses.
	UI_APPLICATION::Control();

	return (0);
}
