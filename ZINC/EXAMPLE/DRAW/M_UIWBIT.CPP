//	Zinc Interface Library Designer - D_BITMAP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <stdio.h>
#include <string.h>
#include <ui_win.hpp>
#include "sketch.hpp"
//dwr
extern "C"
{
#include <Xm/DrawingA.h>
}
//DWR

COLOR UIW_BITMAP::leftColor = 0;
COLOR UIW_BITMAP::rightColor = 15;
COLOR UIW_BITMAP::screenColor = 7;

UIW_BITMAP::UIW_BITMAP(int left, int top, char *_bitmapName, BITMAP_ID _bitmapID,
	int _ratioWidth, int _ratioHeight) :
	UI_WINDOW_OBJECT(left, top, 0, 0, WOF_BORDER, WOAF_NO_FLAGS),
	ratioWidth(_ratioWidth), ratioHeight(_ratioHeight),	bitmapName(NULL),
	bitmapArray(NULL), bitmapID(_bitmapID), drawStyle(IMAGE_PENCIL),
	bitmapWidth(1), bitmapHeight(1)
{
	bitmapName = ui_strdup(_bitmapName);
	colorBitmap = monoBitmap = 0;
}

UIW_BITMAP::~UIW_BITMAP(void)
{
}

EVENT_TYPE UIW_BITMAP::DrawItem(const UI_EVENT &event, EVENT_TYPE )
{
	UI_REGION region = true;
	display->VirtualGet(screenID, true);
	DrawBorder(screenID, region, FALSE, event.type);
	if (bitmapID == ID_VIEW_BITMAP)
	{
		UCHAR *tempArray = new UCHAR[bitmapWidth * bitmapHeight];
		for (int i = 0; i < bitmapWidth * bitmapHeight; i++)
		{
			tempArray[i] = bitmapArray[i];
			if (tempArray[i] == 0xFF)
				tempArray[i] = (UCHAR)screenColor;
		}
		display->Bitmap(screenID, true.left + 1, true.top + 1, bitmapWidth,
			bitmapHeight, tempArray);
		delete tempArray;
	}
	else if (bitmapArray)
	{
		int k = 0;
		region.top = true.top + 1;
		for (int i = 0; i < bitmapHeight; i++)
		{
			region.bottom = region.top + ratioHeight - 1;
			region.left = true.left + 1;
			for (int j = 0; j < bitmapWidth; j++, k++)
			{
				region.right = region.left + ratioWidth - 1;
				int color = bitmapArray[k];
				if (color == 0xFF)
					color = (UCHAR)screenColor;
				display->Rectangle(screenID, region, &display->colorMap[color], 0, TRUE);
				region.left += ratioWidth;
			}
			region.top += ratioHeight;
		}
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
		display->Rectangle(screenID, region, &display->colorMap[drawColor], 0, TRUE);
	}
	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UIW_BITMAP::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW_OBJECT);
	switch (ccode)
	{
	case S_INITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
		relative.right = relative.left + bitmapWidth * ratioWidth + 1;
		relative.bottom = relative.top + bitmapHeight * ratioHeight + 1;
		break;

	case S_CREATE:
		woStatus |= WOS_OWNERDRAW;
		ccode = UI_WINDOW_OBJECT::Event(event);
		RegisterObject(xmDrawingAreaWidgetClass, NULL, ccode);
		break;

	case IMAGE_PENCIL:
		drawStyle = event.type;
		break;

	case L_BEGIN_SELECT:
	case L_CONTINUE_SELECT:
	case L_BEGIN_ESCAPE:
	case L_CONTINUE_ESCAPE:
		{
		int column = event.position.column / ratioWidth;
		if (column < 0)
			column = 0;
		else if (column > bitmapWidth - 1)
			column = bitmapWidth - 1;
		int line = event.position.line / ratioHeight;
		if (line < 0)
			line = 0;
		else if (line > bitmapHeight - 1)
			line = bitmapHeight - 1;
		if (bitmapID == ID_COLOR_BITMAP || bitmapID == ID_SCREEN_BITMAP)
		{
			COLOR color = (bitmapID == ID_COLOR_BITMAP) ? bitmapArray[2 * line + column] : 0xFF;
			if (FlagSet(event.rawCode, M_LEFT))
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
			COLOR color = FlagSet(event.rawCode, M_LEFT) ? leftColor : rightColor;
			if (drawStyle == IMAGE_PENCIL)
				Pixel(column, line, color);
		}
		else if (bitmapID == ID_VIEW_BITMAP)
		{
			COLOR color = FlagSet(event.rawCode, M_LEFT) ? leftColor : rightColor;
			if (color == 0x00FF)
				color = screenColor;
			if (color != screenColor)
			{
				UI_EVENT tEvent(S_REDISPLAY);
				screenColor = color;
				screenObject->Event(tEvent);
				editObject->Event(tEvent);
			}
		}
		}
		break;

	default:
  		ccode = UI_WINDOW_OBJECT::Event(event);
	}

	// Return the control code.
	return (ccode);
}

void *UIW_BITMAP::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	switch (request)
	{
	case FLOOD_COLOR:
		{
		COLOR color = (bitmapID == ID_LEFT_BITMAP) ? leftColor : rightColor;
		if (color == 0xFF)
			color = screenColor;
		UI_REGION region = true;
		--region;
		display->VirtualGet(screenID, true);
		display->Rectangle(screenID, region, &_colorMap[color], 0, TRUE);
		display->VirtualPut(screenID);
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
		viewObject->Pixel(column, line, color);
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
	display->VirtualGet(screenID, true);
	display->Rectangle(screenID, region, &display->colorMap[color], 0, TRUE);
	display->VirtualPut(screenID);
}

