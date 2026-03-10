//	Zinc Interface Library - W_KEYBRD.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_evt.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UID_KEYBOARD -------------------------------------------------------

UID_KEYBOARD::UID_KEYBOARD(DEVICE_STATE _state) :
	UI_DEVICE(E_KEY, _state)
{
	installed = TRUE;
}

UID_KEYBOARD::~UID_KEYBOARD(void)
{
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

void UID_KEYBOARD::Poll(void)
{
}

