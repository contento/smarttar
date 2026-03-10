//	Zinc Interface Library Designer - D_BITMAP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <stdio.h>
#include <string.h>
#include <ui_win.hpp>
#include "sketch.hpp"

COLOR UIW_BITMAP::leftColor = 0;
COLOR UIW_BITMAP::rightColor = 15;
COLOR UIW_BITMAP::screenColor = 7;

void *UIW_BITMAP::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	switch (request)
	{
	case FLOOD_COLOR:
		{
		COLOR newColor = (bitmapID == ID_LEFT_BITMAP) ?	leftColor : rightColor;
		if (newColor == 0xFF)
			newColor = screenColor;
		UI_REGION region = true;
		++region.left;
		++region.top;
		--region.right;
		--region.bottom;
//		display->Rectangle(screenID, region, &_colorMap[newColor], 0, TRUE);
		display->Rectangle(screenID, region, &_colorMap[newColor], 0, TRUE,	// BUG.2147
			FALSE, &clip);
		}
		break;

	case GET_BITMAP_WIDTH:
		*(int *)data = bitmapWidth;
		break;

	case SET_BITMAP_WIDTH:
		bitmapWidth = *(int *)data;
		if (bitmapID == ID_VIEW_BITMAP)
			bitmapWidth += 16;
		relative.right = relative.left + bitmapWidth * ratioWidth - 1;
		if (FlagSet(woFlags, WOF_BORDER))
			relative.right += 2;
		true = relative;
		break;

	case GET_BITMAP_HEIGHT:
		*(int *)data = bitmapHeight;
		break;

	case SET_BITMAP_HEIGHT:
		bitmapHeight = *(int *)data;
		if (bitmapID == ID_VIEW_BITMAP)
			bitmapHeight += 16;
		relative.bottom = relative.top + bitmapHeight * ratioHeight - 1;
		if (FlagSet(woFlags, WOF_BORDER))
			relative.bottom += 2;
		true = relative;
		break;

	case GET_BITMAP_ARRAY:
		if (!data)
			return (bitmapArray);
		*(UCHAR **)data = bitmapArray;
		break;

	case SET_BITMAP_ARRAY:
		if (bitmapID == ID_VIEW_BITMAP)
		{
			bitmapArray = new UCHAR[bitmapWidth * bitmapHeight];
			memset(bitmapArray, 0xFF, bitmapWidth * bitmapHeight);
			for (int x = 0; x < bitmapWidth - 16; x++)
			{
				for (int y = 0; y < bitmapHeight - 16; y++)
					bitmapArray[(y + 8) * bitmapWidth + x + 8] =
						((UCHAR *)data)[y * (bitmapWidth - 16) + x];
			}
		}
		else
			bitmapArray = (UCHAR *)data;
		break;

	default:
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void UIW_BITMAP::Pixel(int column, int line, UCHAR color)
{
	if (bitmapID == ID_EDIT_BITMAP)
	{
		viewObject->Pixel(column, line, color);
	}
	else
	{
		column += 8;
		line += 8;
	}
	bitmapArray[line * bitmapWidth + column] = color;
	if (color == 0xFF)
		color = screenColor;
	UI_REGION region;
	region.left = true.left + column * ratioWidth + 1;
	region.top = true.top + line * ratioHeight + 1;
	region.right = region.left + ratioWidth - 1;
	region.bottom = region.top + ratioHeight - 1;
//	display->Rectangle(screenID, region, &display->colorMap[color], 0, TRUE);
	display->Rectangle(screenID, region, &display->colorMap[color],	0,		// BUG.2147
		TRUE, FALSE, &clip);
}

EVENT_TYPE UIW_BITMAP::DrawItem(const UI_EVENT &event, EVENT_TYPE )
{
	UI_REGION region = true;
	DrawBorder(screenID, region, FALSE, event.type);
	if (bitmapID == ID_VIEW_BITMAP)
	{
		UCHAR *tempArray = new UCHAR[bitmapWidth * bitmapHeight];
		for (int i = 0; i < bitmapWidth * bitmapHeight; i++)
		{
			tempArray[i] = bitmapArray[i];
			if (tempArray[i] == 0xFF)
				tempArray[i] = screenColor;
		}
//		display->Bitmap(screenID, true.left + 1, true.top + 1, bitmapWidth,	// BUG.2147
//			bitmapHeight, tempArray);
		display->Bitmap(screenID, true.left + 1, true.top + 1, bitmapWidth,
			bitmapHeight, tempArray, NULL, &clip);
		delete tempArray;
	}
	else if (bitmapArray)
	{
		display->VirtualGet(screenID, true);
		int k = 0;
		region.top = true.top + 1;
		for (int i = 0; i < bitmapHeight; i++)
		{
			region.bottom = region.top + ratioHeight - 1;
			region.left = true.left + 1;
			for (int j = 0; j < bitmapWidth; j++, k++)
			{
				region.right = region.left + ratioWidth - 1;
				if (region.Overlap(event.region))
				{
					int color = bitmapArray[k];
					if (color == 0xFF)
						color = screenColor;
//					display->Rectangle(screenID, region,					// BUG.2147
//						&display->colorMap[color], 0, TRUE);
					display->Rectangle(screenID, region,
						&display->colorMap[color], 0, TRUE, FALSE, &clip);
				}
				region.left += ratioWidth;
			}
			region.top += ratioHeight;
		}
		display->VirtualPut(screenID);
	}
	else
	{
		COLOR drawColor;
		if (bitmapID == ID_SCREEN_BITMAP)
			drawColor = 0xFF;
		else
			drawColor = (bitmapID == ID_LEFT_BITMAP) ? leftColor : rightColor;
		if (drawColor == 0xFF)
			drawColor = screenColor;
//		display->Rectangle(screenID, region, &display->colorMap[drawColor], 0, TRUE);
		display->Rectangle(screenID, region, &display->colorMap[drawColor],	// BUG.2147
			0, TRUE, FALSE, &clip);
	}
	return (TRUE);
}

EVENT_TYPE UIW_BITMAP::Event(const UI_EVENT &_event)
{
	UI_EVENT event;
	event = _event;

	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_INITIALIZE:
		UI_WINDOW_OBJECT::Event(event);

		// Read in the bitmap if so specified.
		if (bitmapName && !bitmapArray)
		{
			defaultStorage->ChDir("~UI_BITMAP");
			UI_STORAGE_OBJECT sObject(*defaultStorage, bitmapName, ID_BITMAP_IMAGE, UIS_READ);
			if (!sObject.objectError)
			{
				short _value; sObject.Load(&_value); bitmapWidth = _value;
				sObject.Load(&_value); bitmapHeight = _value;
				bitmapArray = new UCHAR[bitmapWidth * bitmapHeight];
				sObject.Load(bitmapArray, sizeof(UCHAR), bitmapWidth * bitmapHeight);
			}
		}
		relative.right = relative.left + bitmapWidth * ratioWidth + 1;
		relative.bottom = relative.top + bitmapHeight * ratioHeight + 1;
		true = relative;
		break;

	case S_REDISPLAY:
//		relative.right = relative.left + bitmapWidth * ratioWidth + 1;
//		relative.bottom = relative.top + bitmapHeight * ratioHeight + 1;
//		true = relative;
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(woStatus, WOS_REDISPLAY))
		{
			DrawItem(event, event.type);
			woStatus &= ~WOS_REDISPLAY;
		}
		break;

	case IMAGE_PENCIL:
		drawStyle = event.type;
		break;

	case E_MOUSE:
		if (FlagSet(event.rawCode, M_LEFT | M_RIGHT) && true.Overlap(event.position))
		{
			if (!FlagSet(woStatus, WOS_CURRENT))							// BUG.2146
			{
				UI_EVENT tEvent(S_ADD_OBJECT);
				tEvent.data = this;
				UI_WINDOW_OBJECT::Event(tEvent);
			}
			int column = (event.position.column - true.left - 1) / ratioWidth;
			int line = (event.position.line - true.top - 1) / ratioHeight;
			if (column < 0)
				column = 0;
			else if (column > bitmapWidth - 1)
				column = bitmapWidth - 1;
			if (line < 0)
				line = 0;
			else if (line > bitmapHeight - 1)
				line = bitmapHeight - 1;
			if (bitmapID == ID_COLOR_BITMAP || bitmapID == ID_SCREEN_BITMAP)
			{
				UCHAR color;
				if (bitmapID == ID_COLOR_BITMAP)
					color = bitmapArray[2 * line + column];
				else
					color = 0xFF;

				if (FlagsSet(event.rawCode, M_LEFT))
				{
					leftColor = color;
					leftObject->Information(FLOOD_COLOR, NULL);
				}
				else
				{
					rightColor = color;
					rightObject->Information(FLOOD_COLOR, NULL);
				}
			}
			else if (bitmapID == ID_EDIT_BITMAP)
			{
				COLOR color = (FlagsSet(event.rawCode, M_LEFT)) ? leftColor : rightColor;
				if (drawStyle == IMAGE_PENCIL)
					Pixel(column, line, color);
			}
			else if (bitmapID == ID_VIEW_BITMAP)
			{
				int newColor = (FlagsSet(event.rawCode, M_LEFT)) ? (int)leftColor : (int)rightColor;
				if (newColor == 0xFF)
					newColor = (int)screenColor;
				if (newColor != screenColor)
				{
					screenColor = newColor;
					screenObject->Event(UI_EVENT(S_REDISPLAY, 0));
					editObject->Event(UI_EVENT(S_REDISPLAY, 0));
					Event(UI_EVENT(S_REDISPLAY, 0));
				}
			}
		}

	default:
  		ccode = UI_WINDOW_OBJECT::Event(event);

	}

	// Return the control code.
	return (ccode);
}

UIW_BITMAP::UIW_BITMAP(int left, int top, char *_bitmapName, BITMAP_ID _bitmapID,
	int _ratioWidth, int _ratioHeight) :
	UI_WINDOW_OBJECT(left, top, 0, 0, WOF_BORDER, WOAF_NO_FLAGS),
	ratioWidth(_ratioWidth), ratioHeight(_ratioHeight),	bitmapName(NULL),
	bitmapArray(NULL), bitmapID(_bitmapID), drawStyle(IMAGE_PENCIL),
	bitmapWidth(1), bitmapHeight(1)

{
	bitmapName = ui_strdup(_bitmapName);
}

UIW_BITMAP::~UIW_BITMAP(void)
{
}

