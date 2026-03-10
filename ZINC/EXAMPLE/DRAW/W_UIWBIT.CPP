//	Zinc Interface Library Designer - D_BITMAP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <string.h>
#include <ui_win.hpp>
#include "sketch.hpp"
#pragma hdrstop

#include <stdio.h>

COLOR UIW_BITMAP::leftColor = 0;
COLOR UIW_BITMAP::rightColor = 15;
COLOR UIW_BITMAP::screenColor = 7;

// ----- Definition for the Windows UIW_BITMAP callback function -----
static int _bitmapOffset = -1;
#if defined(WIN32)
static WNDPROC _bitmapCallback = NULL;
#else
static FARPROC _bitmapCallback = (FARPROC)DefWindowProc;
static FARPROC _bitmapJumpInstance = NULL;

long FAR PASCAL _export BitmapJumpProcedure(HWND hWnd, WORD wMsg,
	WORD wParam, LONG lParam)
{
	UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)GetWindowLong(hWnd, _bitmapOffset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
#endif

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
		HDC hDC = GetDC(screenID);
		RECT region;
		GetClientRect(screenID, &region);
		HBRUSH fillBrush = CreateSolidBrush(display->MapColor(&display->colorMap[(int)newColor], TRUE));
		FillRect(hDC, &region, fillBrush);
		DeleteObject(fillBrush);
		ReleaseDC(screenID, hDC);
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
 //		if (!bitmapArray)
			display->BitmapHandleToArray(screenID, colorBitmap, monoBitmap,
				&bitmapWidth, &bitmapHeight, &bitmapArray);
		if (!data)
			return (bitmapArray);
		*(UCHAR **)data = bitmapArray;
		break;

	case SET_BITMAP_ARRAY:
		{
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
		if (colorBitmap)
			DeleteObject(colorBitmap);
		if (monoBitmap)
			DeleteObject(monoBitmap);

		UCHAR *tempArray = new UCHAR[bitmapWidth * bitmapHeight];
		for (int x = 0; x < bitmapWidth; x++)
		{
			for (int y = 0; y < bitmapHeight; y++)
			{
				int color = bitmapArray[y * bitmapWidth + x];
				if (color == 0xFF)
					color = (int)screenColor;
				tempArray[y * bitmapWidth + x] = color;
			}
		}
		display->BitmapArrayToHandle(screenID, bitmapWidth, bitmapHeight,
			tempArray, NULL, &colorBitmap, &monoBitmap);
		delete tempArray;
		}
		break;


	default:
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}

	// Call the info function for the edit object.
//	if (editObject)
//		editObject->Information(request, data, objectID);

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
		color = (UCHAR)screenColor;
	COLOR tempColor = display->MapColor(&display->colorMap[color], TRUE);
	HDC hDC = GetDC(screenID);
	HDC hMemDC = CreateCompatibleDC(hDC);
	SelectObject(hMemDC, colorBitmap);
	SetPixel(hMemDC, column, line, tempColor);
	DeleteDC(hMemDC);
	RECT region;
	region.left = column * ratioWidth;
	region.top = line * ratioHeight;
	region.right = region.left + ratioWidth;
	region.bottom = region.top + ratioHeight;
	HBRUSH fillBrush = CreateSolidBrush(tempColor);
	FillRect(hDC, &region, fillBrush);
	DeleteObject(fillBrush);
	ReleaseDC(screenID, hDC);
}

EVENT_TYPE UIW_BITMAP::DrawItem(const UI_EVENT &event, EVENT_TYPE )
{
	DRAWITEMSTRUCT *draw = (DRAWITEMSTRUCT *)event.message.lParam;
	if (bitmapArray)
	{
		HDC hMemDC = CreateCompatibleDC(draw->hDC);
		SelectObject(hMemDC, colorBitmap);
		StretchBlt(draw->hDC, 0, 0, bitmapWidth * ratioWidth,
			bitmapHeight * ratioHeight, hMemDC, 0, 0, bitmapWidth,
			bitmapHeight, SRCCOPY);
		DeleteDC(hMemDC);
	}
	else
	{
		COLOR drawColor;
		if (bitmapID == ID_SCREEN_BITMAP)
			drawColor = screenColor;
		else
			drawColor = (bitmapID == ID_LEFT_BITMAP) ? leftColor : rightColor;
		if (drawColor == 0xFF)
			drawColor = screenColor;
		HBRUSH fillBrush = CreateSolidBrush(display->MapColor(&display->colorMap[(int)drawColor], TRUE));
		FillRect(draw->hDC, &draw->rcItem, fillBrush);
		DeleteObject(fillBrush);
	}
	return (TRUE);
}

EVENT_TYPE UIW_BITMAP::Event(const UI_EVENT &_event)
{
	UI_EVENT event;
	event = _event;

	if (event.type == E_MSWINDOWS)
	{
		if (_event.message.hwnd != screenID)
			return S_UNKNOWN;
		WORD message = _event.message.message;
		WORD wParam = _event.message.wParam;
		if (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN ||
			(message == WM_MOUSEMOVE && (wParam == MK_LBUTTON || wParam == MK_RBUTTON)))
		{
			event.type = E_MOUSE;
			if (message == WM_LBUTTONDOWN || wParam == MK_LBUTTON)
				event.rawCode = M_LEFT;
			else if (message == WM_RBUTTONDOWN || wParam == MK_RBUTTON)
				event.rawCode = M_RIGHT;
			event.position.column += true.left;
			event.position.line += true.top;
		}
		else if (message == WM_PAINT)
		{
			PAINTSTRUCT ps;
			DRAWITEMSTRUCT draw;
			UI_EVENT tEvent(E_MSWINDOWS);
			draw.hDC = BeginPaint(screenID, &ps);
			draw.rcItem.left = draw.rcItem.top = 0;
			draw.rcItem.right = relative.right - relative.left;
			draw.rcItem.bottom = relative.bottom - relative.top;
			draw.CtlType = ODT_BUTTON;
			draw.itemAction = ODA_DRAWENTIRE;
			tEvent.message.lParam = (LONG)&draw;
			DrawItem(tEvent, event.type);
			EndPaint(screenID, &ps);
		}
	}

	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_INITIALIZE:
#if !defined(WIN32)
		if (!_bitmapJumpInstance)
			_bitmapJumpInstance = (FARPROC)BitmapJumpProcedure;
#endif
		UI_WINDOW_OBJECT::Event(event);

		// Read in the bitmap if so specified.
		if (!colorBitmap && bitmapName)
		{
			char tempName[32];
			strcpy(tempName, "_");
			strcat(tempName, bitmapName);
			colorBitmap = LoadBitmap(display->hInstance, bitmapName);
			monoBitmap = LoadBitmap(display->hInstance, tempName);
			dwStyle |= BS_OWNERDRAW;
			if (!bitmapArray)
			{
				display->BitmapHandleToArray(screenID, colorBitmap, monoBitmap,
					&bitmapWidth, &bitmapHeight, &bitmapArray);
			}
		}
		if (colorBitmap)
		{
			BITMAP bm;
			GetObject(colorBitmap, sizeof(BITMAP), (LPSTR)&bm);
			bitmapHeight = bm.bmHeight;
			bitmapWidth = bm.bmWidth;
			if (!bitmapArray)
			{
				display->BitmapHandleToArray(screenID, colorBitmap, monoBitmap,
					&bitmapWidth, &bitmapHeight, &bitmapArray);
			}
		}
		relative.right = relative.left + bitmapWidth * ratioWidth + 1;
		relative.bottom = relative.top + bitmapHeight * ratioHeight + 1;
		true = relative;
		break;

	case S_CREATE:
#if defined(WIN32)
		RegisterObject("UIW_BITMAP", NULL, &_bitmapCallback, NULL);
#else
		RegisterObject("UIW_BITMAP", NULL, &_bitmapOffset,
			&_bitmapJumpInstance, &_bitmapCallback, NULL);
#endif
		break;

	case S_REDISPLAY:
//		relative.right = relative.left + bitmapWidth * ratioWidth + 1;
//		relative.bottom = relative.top + bitmapHeight * ratioHeight + 1;
//		true = relative;
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case IMAGE_PENCIL:
		drawStyle = event.type;
		break;

	case E_MOUSE:
		if (FlagSet(event.rawCode, M_LEFT | M_RIGHT) && true.Overlap(event.position))
		{
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
				COLOR color;
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
					Pixel(column, line, (UCHAR)color);
			}
			else if (bitmapID == ID_VIEW_BITMAP)
			{
				int newColor = (FlagsSet(event.rawCode, M_LEFT)) ? (int)leftColor : (int)rightColor;
				if (newColor == 0xFF)
					newColor = (int)screenColor;
				if (newColor != (int)screenColor)
				{
					screenColor = newColor;
					editObject->Information(SET_BITMAP_ARRAY, editObject->bitmapArray);
					Information(SET_BITMAP_ARRAY, editObject->bitmapArray);
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
	colorBitmap = monoBitmap = 0;
}

UIW_BITMAP::~UIW_BITMAP(void)
{
	if (colorBitmap)														// BUG.2023
		DeleteObject(colorBitmap);
	if (monoBitmap)
		DeleteObject(monoBitmap);
}

