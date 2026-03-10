//	Zinc Interface Library - D_ERROR.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <dos.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#if defined(__ZTC__)
#include <sound.h>
#elif defined(_MSC_VER)
#include <conio.h>
#endif
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UI_ERROR_SYSTEM ----------------------------------------------------

void UI_ERROR_SYSTEM::Beep(void)
{
#if defined(__ZTC__)
	sound_beep(1331);
#elif defined(_MSC_VER)
	// Code taken from example for _outp in MSC manual.
	int frequency = 200;
	clock_t duration = 100;
	_outp(0x43, 0xB6);
	frequency = (unsigned)(1193180L / frequency);
	_outp(0x42, (char)frequency);
	_outp(0x42, (char)(frequency >> 8));
	int control = _inp(0x61);
	_outp(0x61, control | 0x3);
	clock_t goal = duration + clock();
	while(goal > clock())
		;
	_outp(0x61, control);
#elif defined(__BCPLUSPLUS__) || defined(__TCPLUSPLUS__)
	sound(200);
	delay(100);
	nosound();
#endif
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

	const ERROR_OK = 10000;
	const ERROR_CANCEL = 10001;

	UIW_BORDER *border = new UIW_BORDER;
	UIW_SYSTEM_BUTTON *sysButton = new UIW_SYSTEM_BUTTON;
	sysButton->Add(new UIW_POP_UP_ITEM(errorPrompts[1], MNIF_MOVE));
	sysButton->Add(new UIW_POP_UP_ITEM(errorPrompts[2], MNIF_CLOSE));
	UIW_TITLE *title = new UIW_TITLE(errorPrompts[0]);
	extern UI_PALETTE_MAP *_errorPaletteMapTable;
	sysButton->paletteMapTable =
		title->paletteMapTable = _errorPaletteMapTable;

	UI_DISPLAY *display = windowManager->display;
	int width = 43;
	int height = 6;
	char *text = message;
	int textWidth = display->isText ? 38 : 30 * display->cellWidth;
	int textHeight = 0;
	extern int NextLine(char **text, int totalWidth);
//	while (NextLine(&text, textWidth))
//		textHeight++;
	for (;;)								// BUG.2310
	{
		char *oldText = text;
		NextLine(&text, textWidth);
		if (text == oldText)
			break;
		textHeight++;
	}
	if (display->isText)
		height += textHeight;
	else
		height += (textHeight * (display->TextHeight(message) + display->preSpace + display->postSpace) / display->cellHeight) + 1;
	int left = (display->columns / display->cellWidth - width) / 2;
	int top = (display->lines / display->cellHeight - height) / 2;
	UIW_WINDOW *window = new UIW_WINDOW(left, top, width, height, WOF_NO_FLAGS, WOAF_MODAL | WOAF_NO_SIZE | WOAF_NO_DESTROY);
	*window + border + sysButton + title;
	if (display->isText)
		*window
			+ new UIW_TEXT(2, 1, 38, height - 6, message, -1, WNF_NO_FLAGS, WOF_NON_SELECTABLE);
	else
		*window
			+ new UIW_ICON(2, 1, "ASTERISK")
			+ new UIW_TEXT(8, 1, 30, height - 5, message, -1, WNF_NO_FLAGS, WOF_NON_SELECTABLE);
	if (FlagSet(errorStatus, WOS_INVALID))
		*window
			+ new UIW_BUTTON(10, height - 3, 10, statusPrompts[0], BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE,
				WOF_JUSTIFY_CENTER, NULL, ERROR_OK)
			+ new UIW_BUTTON(22, height - 3, 10, statusPrompts[1], BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE,
				WOF_JUSTIFY_CENTER, NULL, ERROR_CANCEL);
	else
		*window
			+ new UIW_BUTTON(16, height - 3, 10, statusPrompts[0], BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE,
				WOF_JUSTIFY_CENTER, NULL, ERROR_OK);
	*windowManager + window;
	delete message;

	// Make sure that all VPuts are flushed before going modal
	display->VirtualPut(0);							// BUG.1674

	// Run the window in a modal fashion.
	UI_EVENT_MANAGER *eventManager = windowManager->eventManager;
	UI_EVENT event;
	EVENT_TYPE ccode = 0;
	do
	{
		eventManager->Get(event, Q_NORMAL);
		ccode = event.type;
		if (ccode == ERROR_OK || ccode == ERROR_CANCEL)
			*windowManager - window;
//		else if (event.type > 9999)											// BUG.1334
		else
			ccode = windowManager->Event(event);
	} while (ccode != ERROR_OK && windowManager->Index(window) != -1);
	*windowManager - window;
	delete window;
	if (FlagSet(errorStatus, WOS_INVALID))
		return ((ccode == ERROR_OK) ? WOS_INVALID : WOS_NO_STATUS);
	else
		return (WOS_NO_STATUS);												// BUG.1027
}

