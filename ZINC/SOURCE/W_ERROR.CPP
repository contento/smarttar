//	Zinc Interface Library - W_ERROR.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UI_ERROR_SYSTEM ----------------------------------------------------

void UI_ERROR_SYSTEM::Beep(void)
{
	MessageBeep(0);
}

UIS_STATUS UI_ERROR_SYSTEM::ReportError(UI_WINDOW_MANAGER *windowManager,
	UIS_STATUS errorStatus, char *format, ...)
{
	// Beep the system.
	Beep();
	if (!windowManager || !format)
		return (WOS_NO_STATUS);

	// Format the error message then report the error.
	char *message = new char[1024];

	{
		va_list args;
	 	va_start(args, format);
		UI_INTERNATIONAL::vsprintf(message, format, args);
		va_end(args);
	}

	WORD wType = MB_TASKMODAL;
	if (FlagSet(errorStatus, WOS_INVALID))
		wType |= MB_OKCANCEL;
	else
		wType |= MB_OK;
//	SCREENID screenID = windowManager->First() ?							// BUG.7063
//		windowManager->First()->screenID : windowManager->screenID;
//	int response = MessageBox(screenID, message, NULL, wType);
	int response = MessageBox(NULL, message, NULL, wType);
	delete message;
	return ((response == IDOK) ? WOS_INVALID : WOS_NO_STATUS);
}
