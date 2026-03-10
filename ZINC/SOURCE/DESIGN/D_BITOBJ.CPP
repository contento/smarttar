//	Zinc Interface Library Designer - D_BITOBJ.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ui_dsn.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

EVENT_TYPE BITMAP_OBJECT::DrawItem(const UI_EVENT &event, EVENT_TYPE)
{
	UI_REGION region = true;
	DrawBorder(screenID, region, FALSE, event.type);
	if (FlagSet(userFlags, BMF_VIEW_BITMAP))
	{
		int i = 0;
		int j = 0;
		UCHAR *tempArray = new UCHAR[(width + 16) * (height + 16)];
		for (int y = 0; y < height + 16; y++)
		{
			for (int x = 0; x < width + 16; x++)
			{
				if (x < 8 || y < 8 || x > width + 7 || y > height + 7 || bitmapArray[j++] == 0xFF)
					tempArray[i++] = editor->screenColor;
				else
					tempArray[i++] = bitmapArray[j - 1];
			}
		}
//		display->Bitmap(screenID, true.left + 1, true.top + 1, width + 16,	// BUG.2147
//			height + 16, tempArray);
		display->Bitmap(screenID, true.left + 1, true.top + 1, width + 16,
			height + 16, tempArray, NULL, &clip);
		delete tempArray;
	}
	else
	{
		display->VirtualGet(screenID, true);
		int index = 0;
		region.top = true.top + 1;
		for (int y = 0; y < height; y++)
		{
			region.bottom = region.top + ratioHeight - 1;
			region.left = true.left + 1;
			for (int x = 0; x < width; x++, index++)
			{
				region.right = region.left + ratioWidth - 1;
				if (region.Overlap(event.region))
				{
					COLOR color = bitmapArray[index];
					if (color == 0xFF)
						color = editor->screenColor;
					display->Rectangle(screenID, region, &display->colorMap[color], 0, TRUE, FALSE, &clip);
				}
				region.left += ratioWidth;
			}
			region.top += ratioHeight;
		}
		if (FlagSet(userFlags, BMF_GRID))
		{
			UI_PALETTE *outline = LogicalPalette(S_CURRENT, ID_OUTLINE);
			for (int i = 1; i <= width; i++)
				display->Line(screenID, true.left + i * ratioWidth, true.top,
					true.left + i * ratioWidth, true.bottom, outline, 1, FALSE, &clip);
			for (i = 1; i <= height; i++)
				display->Line(screenID, true.left, true.top + i * ratioHeight,
					true.right, true.top + i * ratioHeight, outline, 1, FALSE, &clip);
		}
		display->VirtualPut(screenID);
	}
	return (TRUE);
}

void BITMAP_OBJECT::Pixel(int column, int row, COLOR color)
{
	int index = row * width + column;
	if (bitmapArray[index] != color)
	{
		if (FlagSet(userFlags, BMF_VIEW_BITMAP))
		{
			column += 8;
			row += 8;
		}
		UI_REGION region;
		int grid = FlagSet(userFlags, BMF_GRID) ? 2 : 1;
		region.left = true.left + column * ratioWidth + 1;
		region.top = true.top + row * ratioHeight + 1;
		region.right = region.left + ratioWidth - grid;
		region.bottom = region.top + ratioHeight - grid;
//		display->Rectangle(screenID, region, &display->colorMap[(color == 0xFF) ? editor->screenColor : color], 0, TRUE);	// BUG.2147
		display->Rectangle(screenID, region, &display->colorMap[(color == 0xFF) ? editor->screenColor : color],
			0, TRUE, FALSE, &clip);
		if (FlagSet(userFlags, BMF_EDIT_BITMAP))
			editor->viewBitmap->Pixel(column, row, color);
		else
			bitmapArray[index] = (UCHAR)color;
	}
}

EVENT_TYPE BITMAP_OBJECT::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW_OBJECT);
	switch (ccode)
	{
	case S_INITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
		break;

	case S_CREATE:
		editor = (IMAGE_EDITOR *)parent;
		// Continue to S_SIZE.

	case S_SIZE:
		if (FlagSet(userFlags, BMF_EDIT_BITMAP))
		{
			int tWidth = editor->client.right - editor->client.left - 21 * display->cellWidth - width - 16;
			int tHeight = editor->client.bottom - editor->client.top - display->cellHeight - 10;
			ratioWidth = ratioHeight = Min(tWidth / width, tHeight / height);
			relative.right = relative.left + width * ratioWidth + 1;
			relative.bottom = relative.top + height * ratioHeight + 1;
		}
		else if (FlagSet(userFlags, BMF_VIEW_BITMAP))
		{
			relative.right = editor->client.right - editor->client.left - 3 * display->cellWidth;
			relative.left = relative.right - width - 17;
			relative.bottom = relative.top + height + 17;
		}
		else if (FlagSet(userFlags, BMF_LT_RT_BITMAP))
		{
			relative.right = editor->client.right - editor->client.left - 3 * display->cellWidth;
			relative.left = relative.right - width * ratioWidth - 1;
			relative.bottom = editor->client.bottom - editor->client.top - 2 * display->cellHeight;
			relative.top = relative.bottom - height * ratioHeight - 1;
		}
		else
		{
			relative.right = relative.left + width * ratioWidth + 1;
			relative.bottom = relative.top + height * ratioHeight + 1;
		}
		UI_WINDOW_OBJECT::Event(event);

		// Save editor->client region.
		if (FlagSet(userFlags, BMF_COLOR_BITMAP))
			editor->client = clip;
		break;

	case L_BEGIN_SELECT:
		if (!true.Overlap(event.position))
			break;
		parent->Event(IMAGE_SAVE_UNDO);
		// Continue to L_CONTINUE_SELECT.

	case L_CONTINUE_SELECT:
		{
		if (!true.Overlap(event.position))
			break;
		int x = Min((event.position.column - true.left) / ratioWidth, width - 1);
		int y = Min((event.position.line - true.top) / ratioHeight, height - 1);
		COLOR color = editor->leftRightColors[FlagSet(event.rawCode, M_LEFT) ? 0 : 1];
		if (FlagSet(userFlags, BMF_EDIT_BITMAP))
		{
			switch (editor->drawMode)
			{
			case IMAGE_PENCIL:
				{
				static int lastX, lastY;
				if (ccode == L_BEGIN_SELECT)
					Pixel(x, y, color);
				else
					Line(lastX, lastY, x, y, color);
				lastX = x;
				lastY = y;
				}
				break;

			case IMAGE_BRUSH:
				{
				display->VirtualGet(screenID, true);
				UI_REGION rect;
				int brush = editor->brushSize;
				rect.left = Max(x - brush / 2, 0);
				rect.top = Max(y - brush / 2, 0);
				rect.right = Min(rect.left + brush - 1, width - 1);
				rect.bottom = Min(rect.top + brush - 1, height - 1);
				Rectangle(rect.left, rect.top, rect.right, rect.bottom, color, TRUE);
				display->VirtualPut(screenID);
				}
				break;

			case IMAGE_LINE:
				{
					UI_POSITION p2 = GetRegion(x, y, IMAGE_LINE);
					Line(x, y, p2.column, p2.line, color);
				}
				break;

			case IMAGE_RECTANGLE:
			case IMAGE_FILL_RECTANGLE:
				{
					UI_POSITION p2 = GetRegion(x,y, IMAGE_RECTANGLE);
					int left = Min(x, p2.column);
					int top = Min(y, p2.line);
					int right = Max(x, p2.column);
					int bottom = Max(y, p2.line);
					Rectangle(left, top, right, bottom, color, (editor->drawMode == IMAGE_FILL_RECTANGLE) ? TRUE : FALSE);
				}
				break;

			case IMAGE_ELLIPSE:
			case IMAGE_FILL_ELLIPSE:
				{
					UI_POSITION p2 = GetRegion(x,y, IMAGE_RECTANGLE);
					int left = Min(x, p2.column);
					int top = Min(y, p2.line);
					int right = Max(x, p2.column);
					int bottom = Max(y, p2.line);
					Ellipse(left, top, right, bottom, color, (editor->drawMode == IMAGE_FILL_ELLIPSE) ? TRUE : FALSE);
				}
				break;

			case IMAGE_FILL:
				Fill(x, y, color);
				break;
			}
		}
		else if (FlagSet(userFlags, BMF_COLOR_BITMAP))
		{
			UCHAR color = bitmapArray[y * width + x];
			editor->leftRightBitmap->Pixel(FlagSet(event.rawCode, M_LEFT) ? 0 : 1, 0, color);
		}
		else if (FlagSet(userFlags, BMF_VIEW_BITMAP))
		{
//			editor->screenBitmap->Pixel(0, 0, color);						// BUG.1620
//			editor->leftRightBitmap->Event(S_REDISPLAY);
//			editor->editBitmap->Event(S_REDISPLAY);
//			UI_WINDOW_OBJECT::Event(S_REDISPLAY);
			if (color != 0xFF)
			{
				editor->screenBitmap->Pixel(0, 0, color);
				editor->leftRightBitmap->Event(S_REDISPLAY);
				editor->editBitmap->Event(S_REDISPLAY);
				UI_WINDOW_OBJECT::Event(S_REDISPLAY);
			}
			else
				errorSystem->Beep();
		}
		else if (FlagSet(userFlags, BMF_SCREEN_BITMAP))
			editor->leftRightBitmap->Pixel(FlagSet(event.rawCode, M_LEFT) ? 0 : 1, 0, 0xFF);
		}
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(woStatus, WOS_REDISPLAY))
		{
			DrawItem(event, ccode);
			woStatus &= ~WOS_REDISPLAY;
		}
		break;

	default:
  		ccode = UI_WINDOW_OBJECT::Event(event);

	}

	// Return the control code.
	return (ccode);
}

UI_POSITION BITMAP_OBJECT::GetRegion(int x, int y, EVENT_TYPE drawMode)
{
	UI_POSITION origin;
	UI_POSITION position;
	origin.column = position.column = true.left + x * ratioWidth + ratioWidth / 2 + 1;
	origin.line = position.line = true.top + y * ratioHeight + ratioHeight / 2 + 1;

	if (drawMode == IMAGE_LINE)
		display->Line(screenID, origin.column, origin.line, position.column,
			position.line, display->xorPalette, 1, TRUE, &clip);
	else
//		display->Rectangle(ID_DIRECT, origin.column, origin.line, position.column,
//			position.line, display->xorPalette, 1, FALSE, TRUE);			// BUG.2147
		display->Rectangle(ID_DIRECT, origin.column, origin.line, position.column,
			position.line, display->xorPalette, 1, FALSE, TRUE, &clip);

	EVENT_TYPE ccode = S_UNKNOWN;
	do
	{
		UI_EVENT event;
		eventManager->Get(event);
		ccode = LogicalEvent(event);
		if (ccode == L_CONTINUE_SELECT)
		{
			if (event.position.column < true.left + 1)
				event.position.column = true.left + 1;
			else if (event.position.column > true.right - 1)
				event.position.column = true.right - 1;
			if (event.position.line < true.top + 1)
				event.position.line = true.top + 1;
			else if (event.position.line > true.bottom - 1)
				event.position.line = true.bottom - 1;
			UI_POSITION oldPosition = position;
			position = event.position;
			if (position != oldPosition)
			{
				if (drawMode == IMAGE_LINE)
				{
					display->Line(screenID, origin.column, origin.line, oldPosition.column,
						oldPosition.line, display->xorPalette, 1, TRUE, &clip);
					display->Line(screenID, origin.column, origin.line, position.column, position.line,
						display->xorPalette, 1, TRUE, &clip);
				}
				else
				{
//					display->Rectangle(ID_DIRECT, Min(origin.column, oldPosition.column),	// BUG.2147
//						Min(origin.line, oldPosition.line),	Max(origin.column, oldPosition.column),
//						Max(origin.line, oldPosition.line), display->xorPalette, 1, FALSE, TRUE);
//					display->Rectangle(ID_DIRECT, Min(origin.column, position.column),
//						Min(origin.line, position.line), Max(origin.column, position.column),
//						Max(origin.line, position.line), display->xorPalette, 1, FALSE, TRUE);
					display->Rectangle(ID_DIRECT, Min(origin.column, oldPosition.column),
						Min(origin.line, oldPosition.line),	Max(origin.column, oldPosition.column),
						Max(origin.line, oldPosition.line), display->xorPalette,
						1, FALSE, TRUE, &clip);
					display->Rectangle(ID_DIRECT, Min(origin.column, position.column),
						Min(origin.line, position.line), Max(origin.column, position.column),
						Max(origin.line, position.line), display->xorPalette,
						1, FALSE, TRUE, &clip);
				}
			}
		}
	} while (ccode != L_END_SELECT && ccode != L_SELECT);
	if (drawMode == IMAGE_LINE)
		display->Line(screenID, origin.column, origin.line, position.column,
			position.line, display->xorPalette, 1, TRUE, &clip);
	else
//		display->Rectangle(ID_DIRECT, Min(origin.column, position.column), Min(origin.line, position.line),	// BUG.2147
//			Max(origin.column, position.column), Max(origin.line, position.line), display->xorPalette, 1, FALSE, TRUE);
		display->Rectangle(ID_DIRECT, Min(origin.column, position.column), Min(origin.line, position.line),
			Max(origin.column, position.column), Max(origin.line, position.line),
			display->xorPalette, 1, FALSE, TRUE, &clip);

	UI_POSITION returnPosition;
	returnPosition.column = (position.column - true.left - 1) / ratioWidth;
	returnPosition.line = (position.line - true.top - 1) / ratioHeight;
	return returnPosition;
}

