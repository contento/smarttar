//	Zinc Interface Library - D_TEXT.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

const int STRING_OFFSET = 5;

extern char _dialogSize[];

int NextLine(char **tText, int totalWidth)
{
	// Compute the line width.
	int width = 1;
	int length = 0;
	char *text = *tText;
	while (*text && *text != '\r' && *text != '\n' && width < totalWidth)
	{
		width += _dialogSize[(UCHAR)*text];
		length++;
		text++;
	}
	if (width >= totalWidth)
	{
		int tLength = length - 1;
		char *text1 = *tText;
		char *text2 = text;
		for (text2--; text2 > text1 && *text2 != ' '; text2--)
			tLength--;
		if (*text2 == ' ')
		{
			text = text2 + 1;
			for (text2--; text2 > text1 && *text2 == ' '; text2--)
				tLength--;
			length = tLength;
		}
		else
		{
			--length;
			--text;
		}
	}

	// Move past the newline characters and spaces.
	while (*text == ' ')
		text++;
	int rCount = 0, nCount = 0;
	while (*text)
		if ((*text == '\r' && ++rCount == 1) || (*text == '\n' && ++nCount == 1))
			text++;
		else
			break;
	*tText = text;
	return (length);
}

int PreviousLine(char *startText, char **text, int totalWidth)
{
	// Make sure we can find a previous line.
	if (*text <= startText)
		return (0);

	// Find the hard-return previous line.
	char *currentLine = *text;
	int rCount = 0, nCount = 0;
	while (currentLine > startText)
		if ((*(currentLine - 1) == '\r' && ++rCount == 1) ||
			(*(currentLine - 1) == '\n' && ++nCount == 1))
			currentLine--;
		else
			break;
	while (currentLine > startText && *(currentLine - 1) != '\r' &&
		*(currentLine - 1) != '\n')
		--currentLine;

	// Walk through the string to find the last line.
	int length = totalWidth;
	char *nextLine = currentLine;
	while (nextLine < *text)
	{
		currentLine = nextLine;
		length = NextLine(&nextLine, totalWidth);
	}

	*text = currentLine;
	return (length);
}

// ----- UIW_TEXT -----------------------------------------------------------

EVENT_TYPE UIW_TEXT::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	return UIW_WINDOW::DrawItem(event, ccode);				// BUG.internal(1560)
}

EVENT_TYPE UIW_TEXT::Event(const UI_EVENT &event)
{
	// Initialize the default function variables.
	int updateFields = FALSE, updateStrings = FALSE, updateCurrent = FALSE;
	int insertCount = 0, deleteCount = 0, scrollWidth = 0, oldCursor = iString.cursor;
	UIW_STRING *string = Current(), *oldCurrent = Current(), *newCurrent = NULL;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_TEXT);
	switch (ccode)
	{
	case S_CREATE:
	case S_SIZE:
		{
		// Compute the text region.
		UI_LIST::Destroy();
		clipList.Destroy();
		UIW_WINDOW::Event(event);
		if (ccode == S_CREATE && FlagSet(woStatus, WOS_UNANSWERED))			// Bug.1442
			strcpy(text, "");
		if (clipList.First())		// Un-optimized for Zortech bug.
			iString.active = clipList.First()->region;
		else
		{
			iString.active = true;
			if (!display->isText && FlagSet(woFlags, WOF_BORDER))
				--iString.active;
		}
		iString.lineWidth = FlagSet(wnFlags, WNF_NO_WRAP) ?
			0x0FFF : iString.active.right - iString.active.left + 1;
		if (!display->isText)
			iString.lineWidth -= (STRING_OFFSET + STRING_OFFSET);
		int top = 0;
		int bottom = iString.active.bottom - iString.active.top + 1;
		int width = iString.active.right - iString.active.left + 1;
		int lineHeight = display->TextHeight(NULL, screenID, font) + display->preSpace + display->postSpace;

		iString.length = ui_strlen(text);

		// Create the text fields.
		while (top + lineHeight <= bottom)
		{
			WOF_FLAGS flags = WOF_NO_ALLOCATE_DATA;
			flags |= (woFlags & (WOF_VIEW_ONLY | WOF_NON_SELECTABLE));
			string = new UIW_STRING(0, top, width, text, 1024, STF_SUBSTRING, flags);
			string->font = font | IGNORE_UNDERSCORE;
			top += lineHeight;
			UI_LIST::Add(string);
			string->parent = this;
			string->paletteMapTable = paletteMapTable;
			string->woStatus |= (woStatus & WOS_GRAPHICS);
			string->relative.bottom = top - 1;
			string->Event(event);
		}

		// Set up the text fields.
		updateStrings = TRUE;
		}
		break;

	case S_CURRENT:
		iString.insertMode = insertMode;
		for (string = First(); string; string = string->NextString())
			string->insertMode = insertMode;
		if (FlagSet(woFlags, WOF_AUTO_CLEAR) && !FlagSet(woFlags, WOF_VIEW_ONLY | WOF_NON_SELECTABLE))
		{
			iString.woStatus |= WOS_AUTO_MARK;
			for (string = First(); string; string = string->NextString())
				string->woStatus |= WOS_AUTO_MARK;
			iString.beginMark = 0;
			iString.endMark = iString.cursor = iString.length;
			RecomputeFields(TRUE, TRUE);
		}
		return (UIW_WINDOW::Event(event));

	case S_NON_CURRENT:
		updateFields = UIW_TEXT::ResetMark();
		if (updateFields)
			RecomputeFields(TRUE, FALSE);
		return (UIW_WINDOW::Event(event));

	case S_HSCROLL_CHECK:
	case S_VSCROLL_CHECK:
		newCurrent = Current();
		break;

	case S_HSCROLL:
		{
		int delta = event.scroll.delta;
		if (string->leftClip + delta < 0)									// BUG.1262
			delta = -string->leftClip;
		else if (string->leftClip + delta > string->length)
			delta = string->length - string->leftClip;
		if (delta)
		{
			string->leftClip += delta;
			string->cursor += delta;
			if (string->cursor > string->length)
				string->cursor = string->length;
			else if (string->cursor < 0)
				string->cursor = 0;
			updateCurrent = TRUE;
		}
		}
		break;

	case S_VSCROLL:
		// Make sure we can scroll on the text field.
		{
		int delta = event.scroll.delta;
		if (!delta ||
			delta < 0 && oldCurrent->text == text ||
			delta > 0 && !oldCurrent->text[oldCurrent->length])
			break;

		// Scroll up.
		if (delta < 0)
		{
			delta = -delta - (oldCurrent->length + 1);
			if (delta < 0)
				delta = 0;
			while (delta >= 0 && topLine > text)
			{
				char *line = topLine;
				PreviousLine(text, &topLine, iString.lineWidth);
				delta -= (int)(line - topLine);
				iString.cursor -= (int)(line - topLine);
			}
			for (newCurrent = Current(); delta >= 0 && newCurrent != first;
				newCurrent = newCurrent->PreviousString())
			{
				delta -= newCurrent->length + 1;
				iString.cursor -= newCurrent->length + 1;
			}
			if (iString.cursor < 0)
				iString.cursor = 0;
		}

		// Scroll down.
		else if (delta > 0)
		{
			delta = delta - (oldCurrent->length + 1);
			if (delta < 0)
				delta = 0;
			char *lastLine = Last()->text;
			NextLine(&lastLine, iString.lineWidth);
			while (delta >= 0 && *lastLine)
			{
				char *line = topLine;
				NextLine(&topLine, iString.lineWidth);
				delta -= (int)(topLine - line);
				iString.cursor += (int)(topLine - line);
				NextLine(&lastLine, iString.lineWidth);
			}
			for (newCurrent = Current(); delta >= 0 && newCurrent != last;
				newCurrent = newCurrent->NextString())
				if (!FlagSet(newCurrent->Next()->woFlags, WOF_NON_SELECTABLE))
				{
					delta -= newCurrent->length + 1;
					iString.cursor += newCurrent->length + 1;
				}
				else
					break;
			if (iString.cursor > iString.length)
				iString.cursor = iString.length;
		}

		// Recompute the field information.
		RecomputeFields(TRUE, TRUE);
		}
		break;

	case S_SUBTRACT_OBJECT:
	case S_ADD_OBJECT:
		UIW_WINDOW::Event(event);
		break;

	case L_INSERT_TOGGLE:
		if (!FlagSet(woFlags, WOF_VIEW_ONLY))
		{
			insertMode = !insertMode;
			iString.insertMode = insertMode;
			string->Event(event);
			for (string = First(); string; string = string->NextString())
				string->insertMode = insertMode;
		}
		break;

	case L_PREVIOUS:
	case L_NEXT:
		ccode = S_UNKNOWN;
		break;

	case L_TOP:
		updateFields = UIW_TEXT::ResetMark();
		if (iString.beginMark != -1)
		{
			if (iString.cursor == iString.endMark)
				iString.endMark = iString.beginMark;
			iString.beginMark = 0;
			updateFields = TRUE;
		}
		if (oldCurrent->cursor != 0 || oldCurrent->text != text)
		{
			iString.cursor = 0;
			First()->text = Current()->text = NULL;
			RecomputeFields(TRUE, TRUE);
			updateFields = FALSE;
			newCurrent = Current();
		}
		break;

	case L_BOL:
	case L_LEFT:
	case L_WORD_LEFT:
		{
		updateFields = UIW_TEXT::ResetMark();
		EVENT_TYPE tCode = oldCurrent->Event(event);
 		if (oldCurrent->text == text || oldCurrent->cursor != 0)
		{
			ccode = tCode;
			break;
		}
		else if (tCode != S_UNKNOWN)
		{
			char prevChar = text[(int)(oldCurrent->text - text) - 1];
			if ((ccode == L_WORD_LEFT && prevChar != ' ' && prevChar != '\r' && prevChar != '\n') ||
				*oldCurrent->text == ' ')
				eventManager->Put(event);
			ccode = tCode;
			break;
		}
		if (oldCurrent->PreviousString())
			string = newCurrent = oldCurrent->PreviousString();
		else
		{
			oldCurrent->length = PreviousLine(text, &topLine, iString.lineWidth);
			oldCurrent->text = topLine;
			updateFields = updateStrings = updateCurrent = TRUE;
		}
		string->cursor = Min(string->length, iString.cursor - (int)(string->text - text) - 1);
		if (ccode == L_WORD_LEFT)
			eventManager->Put(event, Q_BEGIN);
		if (iString.beginMark != -1)
		{
			string->beginMark = string->endMark = string->cursor;
			updateFields = TRUE;
		}
		}
		break;

	case L_BOTTOM:
		updateFields = UIW_TEXT::ResetMark();
		if (iString.endMark != -1)
		{
			if (iString.cursor == iString.beginMark)
				iString.beginMark = iString.endMark;
			iString.endMark = iString.length;
			updateFields = TRUE;
		}
		if (oldCurrent->cursor != oldCurrent->length || oldCurrent->text[oldCurrent->length])
		{
			iString.cursor = iString.length;
			Last()->text = Current()->text = NULL;
			RecomputeFields(TRUE, TRUE);
			updateFields = FALSE;
			newCurrent = Current();
		}
		break;

	case L_EOL:
	case L_RIGHT:
	case L_WORD_RIGHT:
		{
		updateFields = UIW_TEXT::ResetMark();
		EVENT_TYPE tCode = oldCurrent->Event(event);
		if (tCode != S_UNKNOWN || !oldCurrent->text[oldCurrent->length])
		{
			ccode = tCode;
			break;
		}
		if (oldCurrent->NextString())
			string = newCurrent = oldCurrent->NextString();
		else
		{
			char *lastLine = Last()->text;
			oldCurrent->length = NextLine(&lastLine, iString.lineWidth);
			if (!lastLine[0])
				break;
			NextLine(&topLine, iString.lineWidth);
			oldCurrent->text = lastLine;
			updateFields = updateStrings = updateCurrent = TRUE;
		}
		string->cursor = 0;
		char prevChar = text[(int)(string->text - text) - 1];
		if ((ccode == L_WORD_RIGHT && ((prevChar != ' ' && prevChar != '\r'
			&& prevChar != '\n') || string->length == 0)) || *string->text == ' ')
			eventManager->Put(event);
 		if (iString.beginMark != -1)
		{
			string->beginMark = string->endMark = string->cursor;
			updateFields = TRUE;
		}
		}
		break;

	case L_UP:
	case L_PGUP:
		updateFields = UIW_TEXT::ResetMark();
		if (oldCurrent->text == text)
			break;
		else if (oldCurrent->PreviousString())
			string = newCurrent = (ccode == L_UP) ? oldCurrent->PreviousString() : First();
		else
		{
			int count = (ccode == L_UP) ? 1 : UI_LIST::Count() - 1;
			while (--count >= 0 && topLine > text)
				oldCurrent->length = PreviousLine(text, &topLine, iString.lineWidth);
			oldCurrent->text = topLine;
			updateFields = updateStrings = updateCurrent = TRUE;
			++iString.cursor;
		}
		string->cursor = Min(oldCurrent->cursor, string->length);
		if (iString.beginMark != -1)
		{
			string->beginMark = string->endMark = string->cursor;
			updateFields = TRUE;
		}
		break;

	case L_DOWN:
	case L_PGDN:
		updateFields = UIW_TEXT::ResetMark();
		if (!oldCurrent->text[oldCurrent->length])
			break;
		else if (oldCurrent->NextString())
		{
			for (string = oldCurrent->NextString(); string; string = string->NextString())
			{
				if (!FlagSet(string->woFlags, WOF_NON_SELECTABLE))
					newCurrent = string;
				if (ccode == L_DOWN)
					break;
			}
			string = newCurrent;
		}
		else
		{
			int count = (ccode == L_DOWN) ? 1 : UI_LIST::Count() - 1;
			char *lastLine = Last()->text;
			int lastLength = Last()->length;
			while (--count >= 0 && lastLine[lastLength])
			{
				NextLine(&topLine, iString.lineWidth);
				lastLength = NextLine(&lastLine, iString.lineWidth);
			}
			oldCurrent->text = lastLine;
			oldCurrent->length = NextLine(&lastLine, iString.lineWidth);
			updateFields = updateStrings = updateCurrent = TRUE;
		}
		string->cursor = Min(oldCurrent->cursor, string->length);
		if (iString.beginMark != -1)
		{
			string->beginMark = string->endMark = string->cursor;
			updateFields = TRUE;
		}
		break;

	case L_BACKSPACE:
		if (iString.cursor > 0 && oldCurrent->cursor == 0 &&
			text[(int)(oldCurrent->text - text) - 1] == ' ')
		{
			--iString.cursor;
			Event(L_DELETE);
		}
		else if (Event(L_LEFT) != S_UNKNOWN)
			Event(L_DELETE);
		break;

	case L_DELETE:
	case L_DELETE_WORD:
	case L_DELETE_EOL:
	case L_CUT:
		if (FlagSet(woFlags, WOF_VIEW_ONLY))
			break;
		// Check for a mark above the top line of the text field.
		if (iString.beginMark != -1 && text + iString.beginMark < topLine)
		{
			topLine = text + iString.beginMark;
			PreviousLine(text, &topLine, iString.lineWidth);
		}
		// Process the delete messages.
		if (ccode == L_DELETE_EOL)
		{
			deleteCount = string->length - string->cursor;
			memmove(&text[oldCursor], &text[oldCursor+deleteCount], iString.length - oldCursor + 1);
			iString.length -= deleteCount;
		}
		else
		{
			if (iString.beginMark == -1 && string->cursor == string->length)
			{
				deleteCount = 0;
				char *tText = &string->text[string->cursor];
				if (*tText == ' ')
					++deleteCount;
				else
				{
					// Delete newline characters.
					int rCount = 0, nCount = 0;
					while (*tText)
						if ((*tText == '\r' && ++rCount == 1) || (*tText == '\n' && ++nCount == 1))
						{
							++deleteCount;
							++tText;
						}
						else
							break;
				}
				memmove(&text[oldCursor], &text[oldCursor+deleteCount], iString.length - oldCursor + 1);
				iString.length -= deleteCount;
			}
			else
			{
				deleteCount = iString.length;
				iString.Event(event);
				deleteCount -= iString.length;
			}
		}
		updateFields = UIW_TEXT::ResetMark();
		if (deleteCount)
		{
			string->length -= deleteCount;
			string->maxLength -= deleteCount;
			string->StringUpdate(insertCount, deleteCount, scrollWidth);
			for (string = string->NextString(); string; string = string->NextString())
				string->text -= deleteCount;
			RecomputeFields(TRUE, TRUE);
			newCurrent = Current();
			woStatus |= WOS_CHANGED;
		}
		break;

	case L_COPY_MARK:
		ccode = iString.Event(event);
		woStatus |= (iString.woStatus & WOS_CHANGED);
		iString.woStatus &= ~WOS_CHANGED;
  		RecomputeFields(TRUE, TRUE);
		break;

	case E_KEY:
	case L_PASTE:
		if (FlagSet(woFlags, WOF_VIEW_ONLY))
			break;
		{
		int cursorCount = iString.cursor;
		insertCount = iString.length;
		iString.Event(event);
		woStatus |= (iString.woStatus & WOS_CHANGED);
		iString.woStatus &= ~WOS_CHANGED;
		cursorCount = iString.cursor - cursorCount;
		insertCount = iString.length - insertCount;
		updateFields = UIW_TEXT::ResetMark();
		if (insertCount || !insertMode)
		{
			if (ccode == E_KEY)
			{
				oldCurrent->length += insertCount;
				oldCurrent->maxLength += insertCount;
				for (string = oldCurrent->NextString(); string; string = string->NextString())
					string->text += insertCount;
			}
			char *line = Last()->text;
			if (current == last && Current()->cursor + cursorCount > NextLine(&line, iString.lineWidth))
				NextLine(&topLine, iString.lineWidth);
			else
			{
				scrollWidth = oldCurrent->CursorUpdate(oldCurrent->cursor + cursorCount, 0, TRUE);
				if (oldCurrent->leftClip == 0 || FlagSet(wnFlags, WNF_NO_WRAP))
					oldCurrent->StringUpdate(insertCount, deleteCount, scrollWidth);
			}
			RecomputeFields(TRUE, TRUE);
			newCurrent = Current();
		}
		}
		break;

	case L_SELECT:
#if defined(ZIL_EDIT)
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
#endif
		if (FlagSet(woFlags, WOF_VIEW_ONLY))
			break;
		else if (iString.length + 2 <= iString.maxLength)
		{
			if (event.type == L_SELECT)
				break;
			if (iString.beginMark != -1)
			{
				iString.Event(UI_EVENT(L_COPY_MARK));
				woStatus |= (iString.woStatus & WOS_CHANGED);
				iString.woStatus &= ~WOS_CHANGED;
				RecomputeFields(TRUE, TRUE);
				break;
			}
			updateFields = UIW_TEXT::ResetMark();
			woStatus |= WOS_CHANGED;
			if (iString.cursor != iString.length)
			{
				memmove(&text[iString.cursor+2], &text[iString.cursor], iString.length - iString.cursor + 1);
				text[iString.cursor] = '\r';
				text[iString.cursor+1] = '\n';
			}
			else
			{
				text[iString.cursor] = '\r';
				text[iString.cursor+1] = '\n';
				text[iString.cursor+2] = '\0';
			}
			iString.cursor += 2;
			iString.length += 2;
			oldCurrent->text = NULL;
			if (oldCurrent->NextString())
				current = string = oldCurrent->NextString();
			else
				NextLine(&topLine, iString.lineWidth);
			iString.leftClip = 0;
			RecomputeFields(TRUE, TRUE);
		}
		break;

	case L_MARK:
		updateFields = UIW_TEXT::ResetMark();
		ccode = UIW_WINDOW::Event(event);
		if (FlagSet(woFlags, WOF_VIEW_ONLY))
			break;
		else if (iString.beginMark == -1)
			iString.beginMark = iString.endMark = (int)(oldCurrent->text + oldCurrent->beginMark - text);
		else
		{
			iString.beginMark = iString.endMark = -1;
			updateFields = TRUE;
		}
		break;

	case L_BEGIN_MARK:
	case L_CONTINUE_MARK:
	case L_END_MARK:
		if (FlagSet(woStatus, WOS_EDIT_MODE) || !true.Overlap(event.position))
			return (UI_WINDOW_OBJECT::Event(event));
		else if (ccode == L_BEGIN_MARK && 
			((vScroll && vScroll->true.Overlap(event.position)) ||
			 (hScroll && hScroll->true.Overlap(event.position))))
			return (UIW_WINDOW::Event(event));
		updateFields = UIW_TEXT::ResetMark();
//		if (FlagSet(woFlags, WOF_VIEW_ONLY))				// BUG.internal
//		{
//			ccode = UIW_WINDOW::Event(event);
//			break;
//		}
//		else if (ccode == L_BEGIN_MARK)
		if (ccode == L_BEGIN_MARK)
		{
			if (iString.beginMark != iString.endMark)
				updateFields = TRUE;
			ccode = UIW_WINDOW::Event(event);
			iString.cursor = iString.beginMark = iString.endMark = (int)(Current()->text + Current()->beginMark - text);
			break;
		}
		else if (ccode == L_END_MARK)
		{
			ccode = UIW_WINDOW::Event(event);
			if (iString.beginMark == iString.endMark)
				iString.beginMark = iString.endMark = -1;
			iString.woStatus |= WOS_AUTO_MARK;
			break;
		}
		// L_CONTINUE_MARK needs to check for movement to another field.
		else if (string->true.Overlap(event.position))
		{
			ccode = UIW_WINDOW::Event(event);
			break;
		}
		for (string = First(); string; string = string->NextString())
			if (string->true.Overlap(event.position) && !FlagSet(string->woFlags, WOF_NON_SELECTABLE))
			{
				string->beginMark = string->endMark = -1;
				ccode = UIW_WINDOW::Event(event);
				updateFields = TRUE;
				break;
			}
		break;

	case L_VIEW:
#if defined(ZIL_EDIT)
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
#endif
		if (!FlagSet(woFlags, WOF_VIEW_ONLY) &&
			clipList.First() && clipList.First()->region.Overlap(event.position))
			eventManager->DeviceState(E_MOUSE, DM_EDIT);
		else
			ccode = UIW_WINDOW::Event(event);
		return (ccode);

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Check for a current update.
	if (updateCurrent)
	{
		string = Current();
		scrollWidth = string->CursorUpdate(string->cursor, 0, TRUE);
		string->StringUpdate(0, 0, scrollWidth);
	}

	// Check the current and mark information (see UIW_STRING code).
	if (newCurrent)
		UIW_WINDOW::Add(newCurrent);
	if (current)
	{
		string = Current();
		// Compute the new mark regions.
		int tCursor = (int)(string->text + string->cursor - text);
		if (string->beginMark == -1)
			;
		else if (iString.cursor == iString.beginMark &&
			(iString.cursor != iString.endMark || tCursor < iString.cursor))
			iString.beginMark = tCursor;
		else
			iString.endMark = tCursor;
		// Check for reversed marks (case when moving onto beginMark or endMark lines).
		iString.cursor = tCursor;
		if (iString.beginMark > iString.endMark)
		{
			tCursor = iString.beginMark;
			iString.beginMark = iString.endMark;
			iString.endMark = tCursor;
		}
		// Check the left-clip region of a no-wrap field.
		if (iString.leftClip != string->leftClip)
		{
			if (FlagSet(wnFlags, WNF_NO_WRAP))
				iString.leftClip = string->leftClip;
			updateFields = updateStrings = TRUE;
		}
	}

	if (iString.cursor < 0)													// BUG.1984 & 1999
		iString.cursor = 0;
	else if (iString.cursor > iString.length)
		iString.cursor = iString.length;

	// Update the field information.
	if (updateFields || updateStrings)
		RecomputeFields(updateFields, updateStrings);

	// Recompute the horizontal and vertical scroll regions.
	if (!current)
		return (ccode);
	string = Current();
	if (hScroll && (oldCursor != iString.cursor || ccode == S_CREATE))
	{
		UI_EVENT event(S_HSCROLL_SET);
		event.scroll.minimum = 0;
		event.scroll.delta = 1;
		event.scroll.showing = iString.lineWidth / display->cellWidth;
		if (event.scroll.showing > string->length)
			event.scroll.showing = string->length;
		event.scroll.maximum = string->length;
		event.scroll.current = string->cursor;
		hScroll->Event(event);
	}
	if (vScroll && (newCurrent || updateFields || updateStrings))
	{
		UI_EVENT event(S_VSCROLL_SET);
		event.scroll.minimum = 0;
		event.scroll.maximum = iString.length - string->length;
		event.scroll.current = (int)(string->text - text);
		event.scroll.delta = string->length + 1;
		event.scroll.showing = 0;
		for (string = First(); string; string = string->NextString())
			event.scroll.showing += string->length + 1;
		vScroll->Event(event);
	}

	// Return the control code.
	return (ccode);
}

void UIW_TEXT::RecomputeFields(int updateFields, int updateStrings)
{
	UI_EVENT event;

	// Optimize the screen output.
	if (updateFields && display->isText)
		display->VirtualGet(screenID, true);

	// Check the cursor position within the buffer.
	if (iString.cursor < (int)(topLine - text))
	{
		topLine = text + iString.cursor;
		PreviousLine(text, &topLine, iString.lineWidth);
	}
	else if (Last() && iString.cursor > (int)(Last()->text - text + Last()->length + 1))
	{
		topLine = text + iString.cursor;
		int count = UI_LIST::Count();
		while (--count > 0 && topLine > text)
			PreviousLine(text, &topLine, iString.lineWidth);
	}

	// Recompute the string field text information.
	current = NULL;
	int nonSelectable = FALSE;
	char *bottomLine = topLine;
	for (UIW_STRING *string = First(); string; string = string->NextString())
	{
		// Compute the new string information.
		char *_text = string->text;
		int _length = string->length;
		int _leftClip = string->leftClip;
		int _beginMark = string->beginMark;
		int _endMark = string->endMark;
		int _cursor = string->cursor;
		WOF_FLAGS _woStatus = string->woStatus;

		if (updateStrings)
		{
			string->text = bottomLine;
			if (nonSelectable)
				string->woFlags |= WOF_NON_SELECTABLE;
			else
				string->woFlags &= ~WOF_NON_SELECTABLE;
			string->length = string->maxLength = NextLine(&bottomLine, iString.lineWidth);
			nonSelectable = string->text[string->length] ? FALSE : TRUE;
		}

		// Check the string status and cursor position.
		int offset = (int)(string->text - text);
		if (!FlagSet(string->woFlags, WOF_NON_SELECTABLE) &&
			((!current && iString.cursor < offset + string->length) ||
			 (!current && iString.cursor == offset + string->length &&
			 (text[iString.cursor] == '\r' || text[iString.cursor] == '\n' || text[iString.cursor] == ' ')) ||
			 (!current && (string == last || !string->text[string->length]))))
		{
			current = string;
			string->woStatus |= WOS_CURRENT;
			string->woFlags &= ~WOF_NON_SELECTABLE;
			string->cursor = iString.cursor - offset;
			if (string->cursor < 0)
				string->cursor = 0;
		}
		else
		{
			string->woStatus &= ~WOS_CURRENT;
			string->cursor = iString.leftClip;
		}
		string->leftClip = iString.leftClip;

		// Check the mark region.
		if (iString.beginMark != -1)
		{
			string->beginMark = Max(0, iString.beginMark - (int)(string->text - text));
			string->endMark = Min(string->length, iString.endMark - (int)(string->text - text));
			if (string->beginMark > string->endMark)
				string->beginMark = string->endMark = -1;
		}
		else
			string->beginMark = string->endMark = -1;

		// Check the update status for the string and the cursor.
		if (updateFields && (_text != string->text ||
			_length != string->length || _leftClip != string->leftClip ||
			_beginMark != string->beginMark || _endMark != string->endMark ||
			_cursor != string->cursor || _woStatus != string->woStatus))
		{
			event.type = S_REDISPLAY;
			string->Event(event);
			// Give the event manager time to poll its devices.
			eventManager->Get(event, Q_NO_BLOCK | Q_NO_DESTROY);
		}
	}

	// Optimize the screen output.
	if (updateFields && display->isText)
		display->VirtualPut(screenID);
}

int UIW_TEXT::ResetMark(void)
{
	if (!FlagSet(iString.woStatus, WOS_AUTO_MARK))
		return (FALSE);
	iString.woStatus &= ~WOS_AUTO_MARK;
	for (UIW_STRING *string = First(); string; string = string->NextString())
	{
		string->beginMark = -1;	// This causes to automatically be reset.
		string->woStatus &= ~WOS_AUTO_MARK;
	}
	iString.beginMark = iString.endMark = -1;
	return (TRUE);
}


