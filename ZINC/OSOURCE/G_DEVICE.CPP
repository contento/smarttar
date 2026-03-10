//	Zinc Interface Library - G_DEVICE.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_evt.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_LINKBUG)
void z_device_dummy(void) { }	// Bug fix for broken linkers.
#endif

ALT_STATE UI_DEVICE::altState = ALT_NOT_PRESSED;
UI_DISPLAY *UI_DEVICE::display = NULL;
UI_EVENT_MANAGER *UI_DEVICE::eventManager = NULL;

int UID_CURSOR::blinkRate = 40;

EVENT_TYPE UID_KEYBOARD::breakHandlerSet = L_EXIT;

void (*mouseQueueEvent)(unsigned, int *, int *) = NULL;
