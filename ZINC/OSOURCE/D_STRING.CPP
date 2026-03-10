//	Zinc Interface Library - D_STRING.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <ctype.h>
#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

const int STRING_OFFSET = 4;

static int IsPrint(int c)
{
	if (c > 127 && c < 255)
		return (TRUE);
	else
		return (isprint(c));
}

// ----- UIW_STRING ---------------------------------------------------------

extern char _dialogSize[];

char UIW_STRING::CharacterConvert(char key)
{
	if (FlagSet(stFlags, STF_LOWER_CASE))
		key = tolower(key);
	if (FlagSet(stFlags, STF_UPPER_CASE))
		key = toupper(key);
	if (FlagSet(stFlags, STF_VARIABLE_NAME) && key == ' ')
		key = '_';
	return (key);
}

int UIW_STRING::CursorUpdate(int newOffset, int scrollWidth, int showCursor)
{
#if defined(ZIL_OPTIMIZE)
	SCREENID screenID = this->screenID;
	UI_DISPLAY *display = this->display;
	UI_REGION clip = this->clip;
	UI_REGION active = this->active;
#endif
	int oldLeftClip = leftClip;

	// Check for valid region.
	if (active.right <= active.left)
	{
		leftClip = cursor = newOffset;
		return (scrollWidth);
	}

	// Check for cursor moving to the left of the field.
	while (leftClip > 0 && newOffset < leftClip)
		leftClip = (leftClip > scrollCount) ? leftClip - scrollCount : 0;
	char saveChar = text[newOffset];
	text[newOffset] = '\0';
	int count = text[leftClip] ?
		display->TextWidth(&text[leftClip], screenID, font) : 0;

	// Check for cursor moving to the right of the field.
	while (active.left + count > active.right)
	{
		int width = newOffset - leftClip;
		leftClip = (width > scrollCount) ? leftClip + scrollCount : newOffset;
		count = display->TextWidth(&text[leftClip], screenID, font);
	}
	if (showCursor && FlagSet(parent->woStatus, WOS_CURRENT))
	{
		if (active.left + count >= clip.left && active.top >= clip.top &&
			active.left + count <= clip.right && active.top <= clip.bottom)
		{
			eventManager->DeviceState(E_CURSOR, insertMode ? DC_INSERT : DC_OVERSTRIKE);
			eventManager->DevicePosition(E_CURSOR, active.left + count, active.top);
		}
		else
			eventManager->DeviceState(E_CURSOR, D_OFF);
	}
	text[newOffset] = saveChar;

	// Reset the string flag and cursor position.
	cursor = newOffset;

	// Determine and return the actual scroll offset.
	if (oldLeftClip < leftClip)
	{
		saveChar = text[leftClip];
		text[leftClip] = '\0';
		scrollWidth = -display->TextWidth(&text[oldLeftClip], screenID, font);
		text[leftClip] = saveChar;
	}
	else if (oldLeftClip > leftClip)
	{
		saveChar = text[oldLeftClip];
		text[oldLeftClip] = '\0';
		scrollWidth = display->TextWidth(&text[leftClip], screenID, font);
		text[oldLeftClip] = saveChar;
	}

	// Return the scroll width.
	return (scrollWidth);
}

EVENT_TYPE UIW_STRING::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	// Draw the string item.
	UI_REGION region = true;
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
	if (FlagSet(stFlags, STF_PASSWORD))	// Check for a password.
	{
		woStatus &= ~WOS_REDISPLAY;
		return (ccode);
	}
	else if (!display->isText)
	{
		region.left += STRING_OFFSET;
		region.right -= STRING_OFFSET;
	}
	if (parent && parent->Inherited(ID_LIST) &&								// BUG.7127
		FlagSet(((UIW_WINDOW *)parent)->wnFlags, WNF_BITMAP_CHILDREN))
			font &= ~IGNORE_UNDERSCORE;
	DrawText(screenID, region, text, lastPalette, FALSE, ccode);
	woStatus &= ~WOS_REDISPLAY;
	return (ccode);
}

EVENT_TYPE UIW_STRING::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode;
	int insertCount = 0;
	int deleteCount = 0;
	int scrollWidth = 0;
	int tCursor = cursor;
	int tBeginMark = beginMark;
	int tEndMark = endMark;
//	int noEditCursor = FlagSet(woFlags, WOF_VIEW_ONLY);			// BUG.1308
//#define noEditCursor	TRUE							// BUG.internal
	int noEditCursor = FlagSet(woFlags, WOF_VIEW_ONLY) && (parent->Inherited(ID_LIST) ||
		FlagSet(((UIW_WINDOW *)parent)->wnFlags, WNF_BITMAP_CHILDREN));

	// Switch on the event type.
	ccode = LogicalEvent(event, ID_STRING);
	if (ccode == L_CUT_PASTE)
		ccode = (beginMark != -1) ? L_CUT : L_PASTE;
	switch (ccode)
	{
	case S_INITIALIZE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (!display->isText)
			relative.bottom = relative.top + display->cellHeight - display->preSpace - display->postSpace - 1;
		break;

	case S_SIZE:
	case S_CREATE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		active = true;
		if (active.Width() < display->cellWidth * 5)
			scrollCount = 1;
		if (parent->Inherited(ID_LIST))										// Bug.1271
		{
//			woFlags &= ~WOF_AUTO_CLEAR;				// BUG.1270
			tCursor = cursor = 0;
		}
		if (ccode == S_CREATE && FlagSet(woStatus, WOS_UNANSWERED))
			strcpy(text, "");
		break;

	case S_REDISPLAY:
		return (UI_WINDOW_OBJECT::Event(event));

	case S_CURRENT:
	case S_NON_CURRENT:
		if (FlagSet(stFlags, STF_SUBSTRING))
			;
		else if (ccode != S_CURRENT)
		{
			woStatus &= ~WOS_AUTO_MARK;
			beginMark = endMark = -1;
		}
		else if (FlagSet(woFlags, WOF_AUTO_CLEAR) && !FlagSet(woFlags, WOF_VIEW_ONLY | WOF_NON_SELECTABLE))
		{
			woStatus |= WOS_AUTO_MARK;
			tCursor = length;
			beginMark = 0, endMark = length;
		}
		// Continue to S_DISPLAY_ACTIVE.
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		if (!screenID)
			break;
		{
		EVENT_TYPE returnCode = UI_WINDOW_OBJECT::Event(event);
//		if (ccode != S_CURRENT && parent->Inherited(ID_LIST))				// BUG.General
		if (ccode != S_CURRENT && parent->Inherited(ID_LIST) &&
			!parent->Inherited(ID_COMBO_BOX))
			lastPalette = LogicalPalette(ccode, ID_LIST);
		if (FlagSet(woStatus, WOS_REDISPLAY) && ccode != S_CURRENT &&
		    FlagSet(woFlags, WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT | WOF_NON_SELECTABLE))
		{
			if (!FlagSet(parent->woStatus, WOS_MINIMIZED))		// BUG.1735
				DrawItem(event, ccode);
			return (returnCode);
		}
		else if (FlagSet(woStatus, WOS_REDISPLAY))
		{
			// Compute the active region.
			if (!FlagSet(stFlags, STF_SUBSTRING))
				length = ui_strlen(text);
			active = true;
			if (FlagSet(woFlags, WOF_BORDER))
				DrawBorder(screenID, active, FALSE, ccode);
			display->Rectangle(screenID, active, lastPalette, 0, TRUE, FALSE, &clip);
			if (!display->isText)	// Center the text vertically.
			{
				active.left += STRING_OFFSET;
				active.right -= STRING_OFFSET;
				int height = display->TextHeight(text, screenID, font);
				active.top += (active.Height() - height) / 2;
				active.bottom = active.top + height;
			}

			// Update the string information.
			scrollWidth = CursorUpdate(tCursor, scrollWidth, (ccode == S_CURRENT &&
				(!noEditCursor || FlagSet(stFlags, STF_SUBSTRING))) ? TRUE : FALSE);
			StringUpdate(insertCount, deleteCount, scrollWidth);
			return (returnCode);
		}
		else if (ccode == S_CURRENT && FlagSet(stFlags, STF_SUBSTRING))
			CursorUpdate(cursor, 0, TRUE);
		return (returnCode);
		}

	case L_UP:
	case L_LEFT:
		if (tCursor > 0)
			tCursor--;
		else
			ccode = S_UNKNOWN;
		break;

	case L_DOWN:
	case L_RIGHT:
		if (tCursor < length)
		{
			tCursor++;
			if (FlagSet(stFlags, STF_SUBSTRING) && tCursor == length && text[tCursor - 1] == ' ')
				ccode = S_UNKNOWN;
		}
		else
			ccode = S_UNKNOWN;
		if (FlagSet(woStatus, WOS_AUTO_MARK))
		{
			beginMark = endMark = -1;
			woStatus &= ~WOS_AUTO_MARK;
		}
		break;

	case L_BOL:
		tCursor = 0;
		break;

	case L_EOL:
		tCursor = length;
		if (FlagSet(woStatus, WOS_AUTO_MARK))
		{
			beginMark = endMark = -1;
			woStatus &= ~WOS_AUTO_MARK;
		}
		break;

	case L_WORD_LEFT:
		if (tCursor > 0)
			tCursor--;
		else
			ccode = S_UNKNOWN;
		while (tCursor > 0 && text[tCursor] == ' ')
			tCursor--;
		while (tCursor > 0 && text[tCursor-1] != ' ')
			tCursor--;
		break;

	case L_WORD_RIGHT:
		while (tCursor < length && text[tCursor] != ' ')
			tCursor++;
		while (tCursor < length && text[tCursor] == ' ')
			tCursor++;
		if (tCursor == length)
			tCursor = cursor;
		if (tCursor == cursor)
			ccode = S_UNKNOWN;
		else if (FlagSet(woStatus, WOS_AUTO_MARK))
		{
			beginMark = endMark = -1;
			woStatus &= ~WOS_AUTO_MARK;
		}
		break;

	case L_INSERT_TOGGLE:
		if (!FlagSet(woFlags, WOF_VIEW_ONLY))
			insertMode = !insertMode;
		eventManager->DeviceState(E_CURSOR, insertMode ? DC_INSERT : DC_OVERSTRIKE);	// BUG.1308
		break;

	case L_MARK:
		if (noEditCursor)
			break;
		if (beginMark == -1)
			beginMark = endMark = cursor;
		else
			beginMark = endMark = -1;
		break;

	case S_HSCROLL:
		scrollWidth = event.scroll.delta;
		leftClip += scrollWidth;
		if (leftClip < 0)
			leftClip = 0;
		cursor = tCursor = leftClip;
		break;

	case L_VIEW:
#if defined(ZIL_EDIT)
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
#endif
		if (!noEditCursor && true.Overlap(event.position))
			eventManager->DeviceState(E_MOUSE, DM_EDIT);
		else
			ccode = UI_WINDOW_OBJECT::Event(event);
		return (ccode);

	case L_BEGIN_MARK:
	case L_CONTINUE_MARK:
	case L_END_MARK:
#if defined(ZIL_EDIT)
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
#endif
		if (ccode == L_END_MARK && true.Overlap(event.position) &&
			userFunction && parent->Inherited(ID_LIST))
		{
			UI_EVENT uEvent;
			uEvent = event;
			ccode = (*userFunction)(this, uEvent, L_SELECT);
		}
		else if (!noEditCursor && true.Overlap(event.position))
		{
			tCursor = leftClip - 1;
			int left = active.left;
			do
			{
				left += _dialogSize[(UCHAR)text[++tCursor]];	// BUG.2248
			} while (tCursor < length && left <= event.position.column);
			if (FlagSet(woStatus, WOS_AUTO_MARK))
			{
				woStatus &= ~WOS_AUTO_MARK;
				beginMark = endMark = -1;
			}
			if (ccode == L_BEGIN_MARK || (ccode == L_CONTINUE_MARK && beginMark == -1))
			{
				beginMark = endMark = tCursor;
				tBeginMark = tEndMark = -1;
			}
			else if (ccode == L_END_MARK)
			{
				if (beginMark == endMark)
				{
					beginMark = endMark = -1;
					// Send Activate message to PenDOS, if installed.
					UI_EVENT pendosEvent(E_MOUSE, D_ACTIVATE);
					pendosEvent.data = this;
					eventManager->Event(pendosEvent, E_MOUSE);
				}
				woStatus |= WOS_AUTO_MARK;
			}
		}
		else
			ccode = S_UNKNOWN;
		break;

	case L_BACKSPACE:
		if (UIW_STRING::Event(L_LEFT) != S_UNKNOWN)
		{
			UIW_STRING::Event(L_DELETE);
			return ccode;
		}
		break;

	case L_CUT:
	case L_DELETE:
	case L_DELETE_EOL:
	case L_DELETE_WORD:
		if (FlagSet(woFlags, WOF_VIEW_ONLY))
			break;
		else if (beginMark != -1)
		{
			deleteCount = endMark - beginMark;
			if (ccode == L_CUT)
			{
				if (pasteBuffer)
					delete pasteBuffer;
				pasteLength = deleteCount;
				if (pasteLength)
				{
					pasteBuffer = new char[pasteLength];
					memcpy(pasteBuffer, &text[beginMark], pasteLength);
				}
				else
					pasteBuffer = NULL;
			}
			tCursor = beginMark;
			beginMark = endMark = -1;
			length -= deleteCount;
			memmove(&text[tCursor], &text[tCursor+deleteCount], length - tCursor + 1);
			break;
		}
		else if (tCursor == length && FlagSet(stFlags, STF_SUBSTRING))
			return (S_UNKNOWN);
		{
		int emptyQueue;
		UI_EVENT tEvent = event;
		do
		{
			if (ccode == L_DELETE)
			{
				if (tCursor < length)
					++deleteCount;
			}
			else if (ccode == L_DELETE_EOL)
				deleteCount = length - tCursor;
			else if (ccode == L_DELETE_WORD)
			{
				while (tCursor > 0 && text[tCursor-1] != ' ')
					tCursor--;
				deleteCount = tCursor;
				while (deleteCount < length && text[deleteCount] != ' ')
					deleteCount++;
				while (deleteCount < length && text[deleteCount] == ' ')
					deleteCount++;
				deleteCount -= tCursor;
			}
			emptyQueue = eventManager->Get(tEvent, Q_NO_BLOCK | Q_NO_DESTROY);
			if (!emptyQueue && tEvent.type == event.type && tEvent.rawCode == event.rawCode)
				eventManager->Get(tEvent);
			else
				emptyQueue = TRUE;
		} while (!emptyQueue);
		if (deleteCount > 0)
		{
			length -= deleteCount;
			memmove(&text[tCursor], &text[tCursor+deleteCount], length - tCursor + 1);
		}
		}
		break;

	case L_COPY_MARK:
		if (beginMark != -1)
		{
			pasteLength = endMark - beginMark;
			if (pasteBuffer)
				delete pasteBuffer;
			if (pasteLength)
				pasteBuffer = new char[pasteLength];
			else
				pasteBuffer = NULL;
			memcpy(pasteBuffer, &text[beginMark], pasteLength);
			beginMark = endMark = -1;
		}
		break;

	case L_PASTE:
		if (!FlagSet(woFlags, WOF_VIEW_ONLY) &&
		    pasteLength < maxLength - length)				// BUG.1280
		{
			memmove(&text[tCursor+pasteLength], &text[tCursor], length - tCursor);
			length += pasteLength;
			text[length] = 0;					// BUG.1545c
			for (int i = 0; i < pasteLength; i++)
			{
				char key = CharacterConvert(pasteBuffer[i]);
				text[tCursor++] = key;
			}
			insertCount = pasteLength;
		}
		break;

	case L_SELECT:
		if (cursor > length)
			cursor = length;
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case L_PGUP:									// BUG.1229
	case L_PGDN:									// BUG.1229
		break;									// BUG.1229

	case E_KEY:
		if (beginMark != -1)
		{
			UIW_STRING::Event(UI_EVENT(L_CUT));
			tCursor = cursor;
			tBeginMark = beginMark, tEndMark = endMark;
		}
		if (FlagSet(woFlags, WOF_VIEW_ONLY) || !IsPrint(event.key.value))
			break;
		{
		insertCount = 0;
		int emptyQueue;
		UI_EVENT tEvent = event;
		do
		{
			char key = CharacterConvert(tEvent.key.value);
			if (length < maxLength - 1 && (insertMode || tCursor == length ||
				text[tCursor] == '\r' || text[tCursor] == '\n'))
			{
				length++;
				memmove(&text[tCursor+1], &text[tCursor], length - tCursor);
				text[tCursor++] = key;
				insertCount++;
			}
			else if (!insertMode && tCursor < length)
			{
				text[tCursor++] = key;
				insertCount++;
			}
			emptyQueue = eventManager->Get(tEvent, Q_NO_BLOCK | Q_NO_DESTROY);
			if (!emptyQueue && tEvent.type == event.type && IsPrint(tEvent.key.value) &&
				tEvent.key.value != '\n')
				eventManager->Get(tEvent);
			else
				emptyQueue = TRUE;
		} while (!emptyQueue);
		}
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Check the mark information.
	if (beginMark != -1)
	{
		if (cursor == beginMark)
			beginMark = tCursor;
		else
			endMark = tCursor;
		if (beginMark > endMark)	// Make sure the mark order is correct.
		{
			int temp = beginMark;
			beginMark = endMark;
			endMark = temp;
		}
	}
	if (cursor != tCursor && beginMark != -1 && ccode != S_CURRENT)
	{
		// Reset the mark region if the mark status is set.
		if (FlagSet(woStatus, WOS_AUTO_MARK))
		{
			beginMark = endMark = -1;
			woStatus &= ~WOS_AUTO_MARK;
		}
		else
			tBeginMark = tEndMark = -1;
	}

	// Recompute the cursor position and redraw the string.
	if (!screenID)
	{
		if (insertCount || deleteCount)
			woStatus |= WOS_CHANGED;
		cursor = tCursor;
		return (ccode);
	}
	if (cursor != tCursor)
		scrollWidth = CursorUpdate(tCursor, scrollWidth, (!noEditCursor
			|| FlagSet(stFlags, STF_SUBSTRING)) ? TRUE : FALSE);
	if (insertCount || deleteCount || scrollWidth ||
		beginMark != tBeginMark || endMark != tEndMark)
		StringUpdate(insertCount, deleteCount, scrollWidth);

	// Return the control code.
	return (ccode);
}

void UIW_STRING::StringUpdate(int insertCount, int deleteCount, int scrollWidth)
{
	// Check for a valid screen region and password.
	if (insertCount || deleteCount)
		woStatus |= WOS_CHANGED;
	if (FlagSet(stFlags, STF_PASSWORD) || active.right <= active.left)
	{
		woStatus &= ~WOS_REDISPLAY;
		return;
	}

	// Compute the actual clip region and save the display.
	UI_REGION clipRegion = clip;
	if (clipRegion.right > active.right)
		clipRegion.right = active.right;
	if (clipRegion.bottom > active.bottom)
		clipRegion.bottom = active.bottom;

	// Draw the string information.
	int left = active.left;
	int scrollLeft = leftClip;
	if (!scrollWidth && (insertCount || deleteCount))
	{
		scrollLeft = cursor - insertCount;
		char saveChar = text[scrollLeft];
		text[scrollLeft] = '\0';
		left += display->TextWidth(&text[leftClip], screenID, font);
		text[scrollLeft] = saveChar;
	}
	int top = active.top;
	int beginOffset;
	int endOffset = 0;
	for (int i = 0; endOffset < length; i++)
	{
		beginOffset = (endOffset > scrollLeft) ? endOffset : scrollLeft;
		if (i == 0)			// Begin mark region.
			endOffset = (beginMark != -1) ? beginMark : length;
		else if (i == 1)	// End mark region.
			endOffset = endMark;
		else				// End of string.
			endOffset = length;
		if (beginOffset != endOffset && endOffset > scrollLeft && left < clipRegion.right)
		{
			char saveChar = text[endOffset];
			text[endOffset] = '\0';
			if (parent && parent->Inherited(ID_LIST) &&						// BUG.7127
				FlagSet(((UIW_WINDOW *)parent)->wnFlags, WNF_BITMAP_CHILDREN))
					font &= ~IGNORE_UNDERSCORE;
			display->Text(screenID, left, top, &text[beginOffset],
				(i == 1) ? display->xorPalette : lastPalette, 
				endOffset - beginOffset, TRUE, FALSE, &clipRegion,
				font);
			left += display->TextWidth(&text[beginOffset], screenID, font);
			text[endOffset] = saveChar;
		}
	}
	// Update the end of the field.
	if (left <= clipRegion.right)
		display->Rectangle(screenID, left, active.top, active.right,
			active.bottom, lastPalette, 0, TRUE, FALSE, &clip);
	lineWidth = left - scrollWidth - active.left + 1;

	// Restore the display and text.
	woStatus &= ~WOS_REDISPLAY;
}


