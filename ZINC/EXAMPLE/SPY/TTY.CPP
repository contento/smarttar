//	TTY.CPP (SPY) - Output window.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

// The TTY_WINDOW is a very useful and simple display window.  Character
// strings can be displayed in the window using a printf style format.  Just
// add the tty window to the window manager and send messages to it.
// For example, if the programmer needs to have continuous output to the
// screen but does not necessarily need to keep the information.

#include <stdio.h>
#include <stdarg.h>

#define USE_RAW_KEYS
#include <ui_win.hpp>
#include "tty.hpp"

TTY_ELEMENT::TTY_ELEMENT(int left, int top, int width, int _height, int length) :
		UIW_STRING(left, top, width, "", length, STF_NO_FLAGS, WOF_NON_SELECTABLE),
		height(_height)
{
}

EVENT_TYPE TTY_ELEMENT::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW);

	switch (ccode)
	{
	case S_INITIALIZE:
		UIW_STRING::Event(event);
		if (display && !display->isText)
			relative.bottom = relative.top + height;
		break;

	default:
		ccode = UIW_STRING::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

EVENT_TYPE TTY_WINDOW::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW);

	switch (ccode)
	{
	case S_CREATE:
	case S_SIZE:
		{
		// Call the default event.
		UIW_WINDOW::Event(event);

		// Calculate the number of elements that will fit in the list.
//		int lineHeight = display->TextHeight("Mxq", screenID, font) + display->preSpace + display->postSpace;
		int lineHeight = display->cellHeight - display->preSpace -
			display->postSpace;												// BUG.'bad use of pre and postSpace'
		int newNumberShowing = (true.bottom - true.top + display->preSpace + display->postSpace);
		
		newNumberShowing /= lineHeight;
		newNumberShowing++;
		if (display->isText)
			newNumberShowing++;

		// Make sure the window is full of lines.
		numberShowing = newNumberShowing;
		Destroy();
		for (int line = 0; line < numberShowing - 1; line++)
		{
			TTY_ELEMENT *element = new TTY_ELEMENT(0,
				line * lineHeight, true.right - true.left + 1,
				lineHeight, MAX_TTY_STRING_LENGTH);
			element->woStatus |= WOS_GRAPHICS;
			Add(element);
		}
		Event(UI_EVENT(S_REDISPLAY, 0));
		}
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
	}

	// Return the control code.
	return (ccode);
}

void TTY_WINDOW::Printf(char *format, ...)
{
	if (Current() && Current()->Next())
		Add(Current()->Next());
	else if (First())
	{
		for (TTY_ELEMENT *object = First(); object && object->Next(); object = object->Next())
			object->DataSet(object->Next());
		SetCurrent(Last());
	}
	else
		return;

	// Format the tty output line.
	static char string[128];
	va_list arguments;
	va_start(arguments, format);
	vsprintf(string, format, arguments);
	Current()->DataSet(string);
}
