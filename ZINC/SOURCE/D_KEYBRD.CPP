//	Zinc Interface Library - D_KEYBRD.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define USE_RAW_KEYS
#include <dos.h>
#include <stdlib.h>
#include "ui_evt.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

#if !defined(OEM)
extern UCHAR map850toISO(UCHAR val);
#endif

// ----- UID_KEYBOARD -------------------------------------------------------

UID_KEYBOARD::UID_KEYBOARD(DEVICE_STATE _state) :
	UI_DEVICE(E_KEY, _state)
{
#if defined(ZIL_LINKBUG)
	extern void z_device_dummy(void);	// Bug fix for broken linkers.
	z_device_dummy();
#endif
	if (installed)
		return;
	// Initialize the keyboard device.
	installed = TRUE;
	I_KeyboardOpen();
}

UID_KEYBOARD::~UID_KEYBOARD(void)
{
	if (!installed)
		return;
	I_KeyboardClose();
	installed = FALSE;
}

void UID_KEYBOARD::Poll(void)
{
	// Make sure the keyboard is valid.
	if (!installed)
		return;
	unsigned shiftState;
	if (!I_KeyboardQuery(&shiftState))
	{
		// Put L_ALT_KEY on queue if pressed and released alone.
		if (FlagSet(shiftState, S_ALT) && altState == ALT_NOT_PRESSED)
			altState = ALT_PRESSED_NO_EVENTS;
		else if (!FlagSet(shiftState, S_ALT))
		{
			if (altState == ALT_PRESSED_NO_EVENTS)
			{
				UI_EVENT event;
				event.rawCode = 0;
				event.key.value = 0;
				event.key.shiftState = 0;
				event.type = L_ALT_KEY;
				if (state != D_OFF)
					eventManager->Put(event, Q_END);
			}
			altState = ALT_NOT_PRESSED;
		}
		return;
	}

	// Set the key information and call the event manager.
	if (altState == ALT_PRESSED_NO_EVENTS)
		altState = ALT_PRESSED_EVENTS;

	// Get the key from the low level driver.
	UI_EVENT event;
	event.type = E_KEY;
	I_KeyboardRead(&event.rawCode, &event.key.shiftState, &event.key.value);
#if !defined(OEM)
	event.key.value = map850toISO(event.key.value);
#endif
	// Place event on the queue.
	if (state != D_OFF && eventManager)
	{
		if (event.rawCode == CTRL_C || event.rawCode == CTRL_BREAK)
		{
			event.type = breakHandlerSet;
			eventManager->Put(event, Q_BEGIN);
		}
		else
			eventManager->Put(event, Q_END);
	}
}

EVENT_TYPE UID_KEYBOARD::Event(const UI_EVENT &event)
{
	// Switch on the rawCode.
	switch (event.type)
	{
	case D_OFF:
	case D_ON:
		state = event.type;
		break;
	}

	// Return the keyboard state.
	return (state);
}
