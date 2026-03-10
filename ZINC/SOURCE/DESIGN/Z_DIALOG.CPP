//	Zinc Interface Library Designer - Z_DIALOG.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include <stdarg.h>
#include "ui_dsn.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

DIALOG_WINDOW::DIALOG_WINDOW(char *title, char *icon, DIF_FLAGS diFlags, char *format, ...) :
	UIW_WINDOW(0, 0, 0, 0, WOF_NO_FLAGS, WOAF_MODAL | WOAF_DIALOG_OBJECT |	WOAF_NO_SIZE | WOAF_NO_DESTROY)
{
	// Get formatted text.
	char text[512];
	va_list arguments;
	va_start(arguments, format);
	vsprintf(text, format, arguments);
	va_end(arguments);

	// Calculate text size.
	int textWidth = 0;
	int textHeight = 0;
	char *linePtr = text;
	int index = 0;
	do
	{
		if (text[index] == '\n' || text[index] == '\0')
		{
			++textHeight;
			char saveChar = text[index];
			text[index] = '\0';
			int tempWidth = display->TextWidth(linePtr);
			text[index] = saveChar;
			if (tempWidth > textWidth)
				textWidth = tempWidth;
			linePtr = text + index + 1;
		}
	} while (text[index++] != '\0');

	// Adjust text size.
	if (!display->isText)
	{
		textWidth += display->cellWidth;
		textHeight = textHeight * (display->TextHeight("W") + 4);
	}

	// Calculate number of buttons.
	int buttons = 0;
	DIF_FLAGS testFlags = 1;
	do
	{
		if (FlagSet(diFlags, testFlags))
			++buttons;
		testFlags <<= 1;
	} while (testFlags);

	// Set window size.
	int iconOffset = (icon && !display->isText) ? 6 : 0;
	int windowWidth = textWidth / display->cellWidth + iconOffset + 4;
	if (windowWidth < buttons * 12 + 1)
		windowWidth = buttons * 12 + 1;
	int windowHeight = textHeight / display->cellHeight + 5;
#if defined(ZIL_MSWINDOWS)
	if (textHeight > display->cellHeight)
		--windowHeight;
#elif defined(ZIL_MOTIF)
	textHeight += 4;
#endif
	relative.left = (display->columns / display->cellWidth - windowWidth) / 2;
	relative.top = (display->lines / display->cellHeight - windowHeight) / 2;
	relative.right = relative.left + windowWidth;
	relative.bottom = relative.top + windowHeight;

	// Add objects to window.
	UIW_TEXT *textField;
	(void *)&(*this
		+ new UIW_BORDER
		+ new UIW_SYSTEM_BUTTON
		+ new UIW_TITLE(title)
		+ (textField = new UIW_TEXT((2 + iconOffset) * display->cellWidth, display->cellHeight,
			textWidth, textHeight, text, 512, WNF_NO_WRAP, WOF_NON_SELECTABLE)));
	if (icon && !display->isText)
		(void *)&(*this + new UIW_ICON(2, 1, icon));

	textField->woStatus |= WOS_GRAPHICS;
#if defined(ZIL_MSDOS)
	textField->paletteMapTable = _textPaletteTable;
#endif

	// Add buttons to window.
	int offset = (windowWidth - buttons * 12) / 2 + 1;
	if (FlagSet(diFlags, DIF_OK))
	{
		(void *)&(*this + new UIW_BUTTON (offset, windowHeight - 2, 10, "~OK", BTF_NO_TOGGLE | BTF_AUTO_SIZE |
			BTF_SEND_MESSAGE, WOF_JUSTIFY_CENTER, NULL,	DIALOG_OK));
		offset += 12;
	}
	if (FlagSet(diFlags, DIF_YES))
	{
		(void *)&(*this + new UIW_BUTTON (offset, windowHeight - 2, 10, "~Yes", BTF_NO_TOGGLE | BTF_AUTO_SIZE |
			BTF_SEND_MESSAGE, WOF_JUSTIFY_CENTER, NULL, DIALOG_YES));
		offset += 12;
	}
	if (FlagSet(diFlags, DIF_NO))
	{
		(void *)&(*this + new UIW_BUTTON (offset, windowHeight - 2, 10, "~No", BTF_NO_TOGGLE | BTF_AUTO_SIZE |
			BTF_SEND_MESSAGE, WOF_JUSTIFY_CENTER, NULL, DIALOG_NO));
		offset += 12;
	}
	if (FlagSet(diFlags, DIF_CANCEL))
	{
		(void *)&(*this + new UIW_BUTTON (offset, windowHeight - 2, 10, "~Cancel", BTF_NO_TOGGLE | BTF_AUTO_SIZE |
			BTF_SEND_MESSAGE, WOF_JUSTIFY_CENTER, NULL, DIALOG_CANCEL));
		offset += 12;
	}
	if (FlagSet(diFlags, DIF_ABORT))
	{
		(void *)&(*this + new UIW_BUTTON (offset, windowHeight - 2, 10, "~Abort", BTF_NO_TOGGLE | BTF_AUTO_SIZE |
			BTF_SEND_MESSAGE, WOF_JUSTIFY_CENTER, NULL, DIALOG_ABORT));
		offset += 12;
	}
	if (FlagSet(diFlags, DIF_RETRY))
	{
		(void *)&(*this + new UIW_BUTTON (offset, windowHeight - 2, 10, "~Retry", BTF_NO_TOGGLE | BTF_AUTO_SIZE |
			BTF_SEND_MESSAGE, WOF_JUSTIFY_CENTER, NULL, DIALOG_RETRY));
		offset += 12;
	}
}

EVENT_TYPE DIALOG_WINDOW::Responce(void)
{
	errorSystem->Beep();
	(void *)&(*windowManager + this);

	// Wait for user responce.
	EVENT_TYPE returnValue = 0;
	do
	{
		UI_EVENT event;
		eventManager->Get(event);
		if (event.type >= DIALOG_FIRST && event.type <= DIALOG_LAST)
			returnValue = event.type;
		else
			windowManager->Event(event);
	} while (!returnValue && windowManager->Index(this) != -1);
	(void *)&(*windowManager - this);
	return returnValue;
}
