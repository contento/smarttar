//	Zinc Interface Library Designer - Z_BITMAP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ui_dsn.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

static UI_EVENT_MAP _bitmapEventTable[] =
{
#if defined(ZIL_OS2)
	{ ID_WINDOW_OBJECT, 	L_BEGIN_SELECT,		WM_BUTTON1DOWN, 	M_LEFT | M_LEFT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_SELECT,		WM_BUTTON2DOWN, 	M_RIGHT |  M_RIGHT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_SELECT,	WM_MOUSEMOVE, 		M_LEFT },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_SELECT,	WM_MOUSEMOVE, 		M_RIGHT },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT,		WM_BUTTON1UP, 		M_LEFT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT,		WM_BUTTON2UP, 		M_RIGHT_CHANGE },
#elif defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS)
	{ ID_WINDOW_OBJECT, 	L_VIEW,				E_MOUSE, 	0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_SELECT,		E_MOUSE, 	M_LEFT | M_LEFT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_SELECT,		E_MOUSE, 	M_RIGHT |  M_RIGHT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_SELECT,	E_MOUSE, 	M_LEFT },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_SELECT,	E_MOUSE, 	M_RIGHT },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT,		E_MOUSE, 	M_LEFT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT,		E_MOUSE, 	M_RIGHT_CHANGE },
#elif defined(ZIL_MOTIF)
	{ ID_WINDOW_OBJECT,		L_BEGIN_SELECT,		ButtonPress,	M_LEFT | M_LEFT_CHANGE },
	{ ID_WINDOW_OBJECT,		L_BEGIN_SELECT,		ButtonPress,	M_RIGHT | M_RIGHT_CHANGE },
	{ ID_WINDOW_OBJECT,		L_CONTINUE_SELECT,	MotionNotify,	M_LEFT },
	{ ID_WINDOW_OBJECT,		L_CONTINUE_SELECT,	MotionNotify,	M_RIGHT },
	{ ID_WINDOW_OBJECT,		L_END_SELECT,		ButtonRelease,	M_LEFT_CHANGE },
	{ ID_WINDOW_OBJECT,		L_END_SELECT,		ButtonRelease,	M_RIGHT_CHANGE },
#endif

	// End of array.
	{ ID_END, 0, 0, 0 }
};

BITMAP_OBJECT::BITMAP_OBJECT(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object) :
	UI_WINDOW_OBJECT(name, file, object)
{
	eventMapTable = _bitmapEventTable;
}

void BITMAP_OBJECT::Line(int x1, int y1, int x2, int y2, COLOR color)
{
	// Maximized for speed.
	int left = Min(x1, x2);
	int top = Min(y1, y2);
	int right = Max(x1, x2);
	int bottom = Max(y1, y2);
	int deltaX = right - left;
	int deltaY = bottom - top;
	if (deltaX || deltaY)
	{
#if !defined(ZIL_OS2) && !defined(ZIL_MOTIF)
		display->VirtualGet(screenID, true);
#endif
		int sign = (x2 > x1) ? (y2 > y1) : (y1 > y2);
		if (deltaX > deltaY)
		{
			int adjust = (deltaY == 0) ? 0 : deltaX / deltaY / 2;
			if (sign)
			{
				for (int x = 0; x <= deltaX; x++)
				{
					int y = deltaY * (x + adjust) / deltaX;
					Pixel(left + x, top + y, color);
				}
			}
			else
			{
				for (int x = 0; x <= deltaX; x++)
				{
					int y = deltaY * (x + adjust) / deltaX;
					Pixel(left + x, bottom - y, color);
				}
			}
		}
		else
		{
			int adjust = (deltaX == 0) ? 0 : deltaY / deltaX / 2;
			if (sign)
			{
				for (int y = 0; y <= deltaY; y++)
				{
					int x = deltaX * (y + adjust) / deltaY;
					Pixel(left + x, top + y, color);
				}
			}
			else
			{
				for (int y = 0; y <= deltaY; y++)
				{
					int x = deltaX * (y + adjust) / deltaY;
					Pixel(right - x, top + y, color);
				}
			}
		}
#if !defined(ZIL_OS2) && !defined(ZIL_MOTIF)
		display->VirtualPut(screenID);
#endif
	}
	else
		Pixel(x1, y1, color);
}

void BITMAP_OBJECT::Rectangle(int left, int top, int right, int bottom, COLOR color, int fill)
{
	if (fill)
	{
		for (int y = top; y <= bottom; y++)
			for (int x = left; x <= right; x++)
				Pixel(x, y, color);
	}
	else
	{
		Line(left, top, right, top, color);
		Line(left, bottom, right, bottom, color);
		Line(left, top, left, bottom, color);
		Line(right, top, right, bottom, color);
	}
}

void BITMAP_OBJECT::Ellipse(int left, int top, int right, int bottom, COLOR color, int fill)
{
	// 4b^2x^2 + 4a^2y^2 = 4ab^2x + 4a^2by - a^2b^2
	long a = right - left + 1;
	long b = bottom - top + 1;

	for (int x = 1; x <= a / 2; x++)
	{
		int y = 1;
		while (y < b && 4*b*b*x*x + 4*a*a*y*y > 4*a*b*b*x + 4*a*a*b*y - a*a*b*b)
			++y;
		Pixel(left + x, top + y - 1, color);
		Pixel(left + x, bottom - y + 1, color);
		Pixel(right - x, top + y - 1, color);
		Pixel(right - x, bottom - y + 1, color);
	}
	for (int y = 1; y <= b / 2; y++)
	{
		int x = 1;
		while (x < a && 4*b*b*x*x + 4*a*a*y*y > 4*a*b*b*x + 4*a*a*b*y - a*a*b*b)
			++x;
		Pixel(left + x - 1, top + y, color);
		Pixel(right - x + 1, top + y, color);
		Pixel(left + x - 1, bottom - y, color);
		Pixel(right - x + 1, bottom - y, color);
	}
/* START BLOCK COMMENT
**		int x = 0;
**		int y = (int)(b / 2);
**		while (x < a / 2)
**		{
**			Pixel(left + x, top + y, color);
**			Pixel(left + x, bottom - y, color);
**			Pixel(right - x, top + y, color);
**			Pixel(right - x, bottom - y, color);
**			long downError = 4*b*b*x*x + 4*a*a*(y+1)*(y+1) - (4*a*b*b*x + 4*a*a*b*(y+1) - a*a*b*b);
**			if (downError < 0)
**				downError = -downError;
**			long rightError = 4*b*b*(x+1)*(x+1) + 4*a*a*y*y - (4*a*b*b*(x+1) + 4*a*a*b*y - a*a*b*b);
**			if (rightError < 0)
**				rightError = -rightError;
**			long diagError = 4*b*b*(x+1)*(x+1) + 4*a*a*(y+1)*(y+1) - (4*a*b*b*(x+1) + 4*a*a*b*(y+1) - a*a*b*b);
**			if (diagError < 0)
**				diagError = -diagError;
**			if (downError <= rightError)
**			{
**				++y;
**				if (diagError <= downError)
**					++x;
**			}
**			else
**			{
**				++x;
**				if (diagError <= rightError)
**					++y;
**			}
**		}
END BLOCK COMMENT */
	if (fill)
	{
 		for (int y = 0; y <= b / 2; y++)
			for (int x = 0; x <= a / 2; x++)
			{
 				if (4*b*b*x*x + 4*a*a*y*y <= 4*a*b*b*x + 4*a*a*b*y - a*a*b*b)
				{
					Pixel(left + x, top + y, color);
					Pixel(left + x, bottom - y, color);
					Pixel(right - x, top + y, color);
					Pixel(right - x, bottom - y, color);
				}
			}
	}
}

void BITMAP_OBJECT::Fill(int column, int line, COLOR newColor)
{
	int oldColor = bitmapArray[line * width + column];
	if (oldColor != newColor)
	{
#if !defined(ZIL_OS2) && !defined(ZIL_MOTIF)
		display->VirtualGet(screenID, true);
#endif
		Pixel(column, line, newColor);
		UCHAR queue[0x100];
		UCHAR head = 0;
		UCHAR tail = 0;
		queue[head++] = column;
		queue[head++] = line;
		while (tail != head)
		{
			int x = queue[tail++];
			int y = queue[tail++];
			if (x > 0 && bitmapArray[y * width + x - 1] == oldColor)
			{
				Pixel(x - 1, y, newColor);
				queue[head++] = x - 1;
				queue[head++] = y;
			}
			if (y > 0 && bitmapArray[(y - 1) * width + x] == oldColor)
			{
				Pixel(x, y - 1, newColor);
				queue[head++] = x;
				queue[head++] = y - 1;
			}
			if (x < width - 1 && bitmapArray[y * width + x + 1] == oldColor)
			{
				Pixel(x + 1, y, newColor);
				queue[head++] = x + 1;
				queue[head++] = y;
			}
			if (y < height - 1 && bitmapArray[(y + 1) * width + x] == oldColor)
			{
				Pixel(x, y + 1, newColor);
				queue[head++] = x;
				queue[head++] = y + 1;
			}
		}
#if !defined(ZIL_OS2) && !defined(ZIL_MOTIF)
		display->VirtualPut(screenID);
#endif
	}
}

