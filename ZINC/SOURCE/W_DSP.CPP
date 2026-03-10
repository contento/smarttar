//	Zinc Interface Library - W_DSP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
#include <mem.h>
#endif
#include <string.h>
#include "ui_dsp.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

static PAINTSTRUCT _winPaint;
static int _virtualCount = 0;
static int _virtualUpdate = 0;
static UI_REGION _virtualRegion = { 0, 0, 0, 0 };

// ----- UI_MSWINDOWS_DISPLAY -----------------------------------------------

UI_MSWINDOWS_DISPLAY::UI_MSWINDOWS_DISPLAY(HANDLE _hInstance,
	HANDLE _hPrevInstance, int _nCmdShow) :
	UI_DISPLAY(FALSE, "Windows", NULL), maxColors(16)
{
#if defined(ZIL_LINKBUG)
	extern void z_windsp_dummy(void);		// Bug fix for Zortech & Microsoft linkers.
	z_windsp_dummy();
#endif

	hInstance = _hInstance;
	hPrevInstance = _hPrevInstance;
	nCmdShow = _nCmdShow;

	// Set up the initial logical fonts.
	LONG baseUnits = GetDialogBaseUnits();
	HFONT systemFont = GetStockObject(SYSTEM_FONT);
	LOGFONT info;
	GetObject(systemFont, sizeof(LOGFONT), (LPSTR)&info);
	HFONT smallFont = CreateFont(HIWORD(baseUnits) * 3 / 4,
		LOWORD(baseUnits) * 3 / 4, info.lfEscapement, info.lfOrientation,
		400, info.lfItalic, info.lfUnderline, info.lfStrikeOut,
		info.lfCharSet, info.lfOutPrecision, info.lfClipPrecision,
		info.lfQuality, info.lfPitchAndFamily, "Helv");
	if (!fontTable[FNT_SMALL_FONT])
		fontTable[FNT_SMALL_FONT] = smallFont;
	HFONT dialogFont = CreateFont(HIWORD(baseUnits) * 3 / 4,
		LOWORD(baseUnits) * 3 / 4, info.lfEscapement, info.lfOrientation,
		info.lfWeight, info.lfItalic, info.lfUnderline, info.lfStrikeOut,
		info.lfCharSet, info.lfOutPrecision, info.lfClipPrecision,
		info.lfQuality, info.lfPitchAndFamily, "Helv");
	if (!fontTable[FNT_DIALOG_FONT])
		fontTable[FNT_DIALOG_FONT] = dialogFont;
	for (int i = FNT_SYSTEM_FONT; i < MAX_LOGICAL_FONTS; i++)
		if (!fontTable[i])
			fontTable[i] = systemFont;

	// Compute the default display values.
	GetObject(dialogFont, sizeof(LOGFONT), (LPSTR)&info);
	columns = GetSystemMetrics(SM_CXSCREEN);
	lines = GetSystemMetrics(SM_CYSCREEN);
	cellWidth = info.lfWidth + 1;
//	cellHeight = GetSystemMetrics(SM_CYMENU) + preSpace + postSpace + 2;
	cellHeight =
		info.lfHeight - 1 +		// average letter height minus a funny fudge factor
		info.lfHeight / 3 +		// blank space above the letter
		info.lfHeight / 4 +		// blank space below the letter
		2 +						// border space
		preSpace +				// preSpace (before the border)
		postSpace;				// postSpace (after the border)
	installed = TRUE;
}

UI_MSWINDOWS_DISPLAY::~UI_MSWINDOWS_DISPLAY(void)
{
	DeleteObject(fontTable[FNT_SMALL_FONT]);
	DeleteObject(fontTable[FNT_DIALOG_FONT]);
}

void UI_MSWINDOWS_DISPLAY::Bitmap(SCREENID screenID, int left, int top,
	int bitmapWidth, int bitmapHeight, const UCHAR *bitmapArray,
	const UI_PALETTE *palette, const UI_REGION *clipRegion,
	HBITMAP *_colorBitmap, HBITMAP *_monoBitmap)
{
	// Make sure there is a valid bitmap.
	HBITMAP colorBitmap = 0, monoBitmap = 0;
	if (_colorBitmap)
		colorBitmap = *_colorBitmap;
	if (_monoBitmap)
		monoBitmap = *_monoBitmap;
	if (!bitmapArray && !colorBitmap)
		return;

	// Virtualize the display. (This call sets up hDC.)
	VirtualGet(screenID, left, top, left + bitmapWidth - 1, top + bitmapHeight - 1);
	left -= _virtualRegion.left;
	top -= _virtualRegion.top;

	// Set the clipRegion. (Relative to hDC)
	HRGN cRegion = 0;
	if (clipRegion)
	{
		cRegion = CreateRectRgn(clipRegion->left - _virtualRegion.left,
			clipRegion->top - _virtualRegion.top,
			clipRegion->right - _virtualRegion.left + 1,
			clipRegion->bottom - _virtualRegion.top + 1);
		SelectClipRgn(hDC, cRegion);
	}

	// Convert the bitmap array then draw the bitmap.
	int oldROP2 = SetROP2(hDC, R2_COPYPEN);
	if (!colorBitmap)
		BitmapArrayToHandle(screenID, bitmapWidth, bitmapHeight, bitmapArray,
			palette, &colorBitmap, &monoBitmap);
	if (monoBitmap)
	{
		HDC hMemDC = CreateCompatibleDC(hDC);
		SelectObject(hMemDC, monoBitmap);
		BitBlt(hDC, left, top, bitmapWidth, bitmapHeight, hMemDC, 0, 0, SRCAND);
		SelectObject(hMemDC, colorBitmap);
		BitBlt(hDC, left, top, bitmapWidth, bitmapHeight, hMemDC, 0, 0, SRCINVERT);
		DeleteDC(hMemDC);
	}
	else if (colorBitmap)
	{
		HDC hMemDC = CreateCompatibleDC(hDC);
		SelectObject(hMemDC, colorBitmap);
		BitBlt(hDC, left, top, bitmapWidth, bitmapHeight, hMemDC, 0, 0, SRCCOPY);
		DeleteDC(hMemDC);
	}
	if (monoBitmap)
	{
		if (!_monoBitmap)
			DeleteObject(monoBitmap);
		else
			*_monoBitmap = monoBitmap;
	}
	if (colorBitmap)
	{
		if (!_colorBitmap)
			DeleteObject(colorBitmap);
		else
			*_colorBitmap = colorBitmap;
	}
	SetROP2(hDC, oldROP2);

	// Un-virtualize the display.
	if (cRegion)
		DeleteObject(cRegion);
	VirtualPut(screenID);
}

void UI_MSWINDOWS_DISPLAY::BitmapArrayToHandle(SCREENID ,
	int bitmapWidth, int bitmapHeight, const UCHAR *bitmapArray,
	const UI_PALETTE *palette, HBITMAP *colorBitmap, HBITMAP *monoBitmap)
{
	HBITMAP hBitmap;
	HBITMAP hOldBitmap;
	HDC hDC = GetDC(0);
	HDC hMemDC = CreateCompatibleDC(hDC);
	const UCHAR *pixel;
	int column, row;

	if (colorBitmap)
	{
		// Create a bitmap compatable with the display.
		hBitmap = CreateCompatibleBitmap(hDC, bitmapWidth, bitmapHeight);

		// Select the bitmap into the memory DC.
		hOldBitmap = SelectObject(hMemDC, hBitmap);

		if (bitmapArray)
		{
			// Draw the Zinc bitmap onto the memory DC.
			pixel = bitmapArray;
			for (row = 0; row < bitmapHeight; row++)
			{
				for (column = 0; column < bitmapWidth; column++)
				{
					if (palette)
						SetPixel(hMemDC, column, row, MapColor(&palette[*pixel], TRUE));
					else if (*pixel <= (UCHAR)MAX_COLORMAP_INDEX)
//					else if (*pixel <= MAX_COLORMAP_INDEX)
						SetPixel(hMemDC, column, row, MapColor(&colorMap[*pixel], TRUE));
					else
						SetPixel(hMemDC, column, row, RGB_BLACK);
					pixel++;
				}
			}
		}
		else
			// Create a white icon if no bitmap array was passed.
			BitBlt(hMemDC, 0, 0, bitmapWidth, bitmapHeight, NULL, 0, 0, WHITENESS);

		// Get the bitmap.
		hBitmap = SelectObject(hMemDC, hOldBitmap);
		*colorBitmap = hBitmap;
	}

	if (monoBitmap)
	{
		hBitmap = CreateBitmap(bitmapWidth, bitmapHeight, 1, 1, NULL);
		hOldBitmap = SelectObject(hMemDC, hBitmap);
		BitBlt(hMemDC, 0, 0, bitmapWidth, bitmapHeight, NULL, 0, 0, BLACKNESS);

		if (bitmapArray)
		{
			// Draw the Zinc bitmap onto the memory DC.
			pixel = bitmapArray;
			for (row = 0; row < bitmapHeight; row++)
			{
				for (column = 0; column < bitmapWidth; column++)
				{
					if (*pixel == BACKGROUND)
						SetPixel(hMemDC, column, row, RGB_WHITE);
					pixel++;
				}
			}
		}
		else
			// Create a white icon if no bitmap array was passed.
			BitBlt(hMemDC, 0, 0, bitmapWidth, bitmapHeight, NULL, 0, 0, WHITENESS);

		// Get the bitmap.
		hBitmap = SelectObject(hMemDC, hOldBitmap);
		*monoBitmap = hBitmap;
		
		BITMAP bitmap;
		GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitmap);
	}
	
	// Clean up.
	DeleteDC(hMemDC);
	ReleaseDC(0, hDC);
}

void UI_MSWINDOWS_DISPLAY::BitmapHandleToArray(SCREENID , HBITMAP colorBitmap,
		HBITMAP monoBitmap, int *bitmapWidth, int *bitmapHeight,
		UCHAR **bitmapArray)
{
	BITMAP bitmap;
	HBITMAP hBitmap;
	HBITMAP hOldBitmap;
	HDC hDC = GetDC(0);
	HDC hMemDC = CreateCompatibleDC(hDC);
	UCHAR *pixel;
	int column, row;

	if (colorBitmap)
	{
		hBitmap = colorBitmap;
		hOldBitmap = SelectObject(hMemDC, hBitmap);
		GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitmap);
		*bitmapWidth = bitmap.bmWidth;
		*bitmapHeight = bitmap.bmHeight;
		if (!(*bitmapArray))
			*bitmapArray = new UCHAR[bitmap.bmWidth * bitmap.bmHeight];

		pixel = *bitmapArray;
		for (row = 0; row < *bitmapHeight; row++)
			for (column = 0; column < *bitmapWidth; column++)
			{
				COLOR color = GetPixel(hMemDC, column, row);
				*pixel = 0;

				for (UCHAR i = 0; i <= MAX_COLORMAP_INDEX; i++)
					if (color == MapColor(&colorMap[i], TRUE))
					{
						*pixel = i;
						break;
					}
				pixel++;
			}

		hBitmap = SelectObject(hMemDC, hOldBitmap);
	}

	if (monoBitmap)
	{
		hBitmap = monoBitmap;
		hOldBitmap = SelectObject(hMemDC, hBitmap);
		if (!(*bitmapArray))
			*bitmapArray = new UCHAR[bitmap.bmWidth * bitmap.bmHeight];

		pixel = *bitmapArray;
		for (row = 0; row < *bitmapHeight; row++)
			for (column = 0; column < *bitmapWidth; column++)
			{
				if (GetPixel(hMemDC, column, row) != RGB_BLACK)
					*pixel = BACKGROUND;
				pixel++;
			}

		hBitmap = SelectObject(hMemDC, hOldBitmap);
	}
	
	// Clean up.
	ReleaseDC(0, hDC);
	DeleteDC(hMemDC);
}

void UI_MSWINDOWS_DISPLAY::Ellipse(SCREENID screenID, int x, int y,
	int startAngle, int endAngle, int xRadius, int yRadius,
	const UI_PALETTE *palette, int fill, int xor, const UI_REGION *clipRegion)
{
	const int pSin[] = { 0, 871, 1736, 2588, 3420, 4226, 5000, 5736, 6428,
		7071, 7660, 8191, 8660, 9063, 9397, 9659, 9848, 9962, 10000 };

	// Virtualize the display. (This call sets up hDC.)
	int left = x - xRadius + 1;
	int top = y - yRadius + 1;
	int right = x + xRadius - 1;
	int bottom = y + yRadius - 1;
	VirtualGet(screenID, left, top, right, bottom);
	x -= _virtualRegion.left;
	y -= _virtualRegion.top;

	// Set the clipRegion (Relative to hDC).
	HRGN cRegion = 0;
	if (clipRegion)
	{
		cRegion = CreateRectRgn(clipRegion->left - _virtualRegion.left,
			clipRegion->top - _virtualRegion.top,
			clipRegion->right - _virtualRegion.left + 1,
			clipRegion->bottom - _virtualRegion.top + 1);
		SelectClipRgn(hDC, cRegion);
	}
	while (startAngle < 0)							// BUG.1616
		startAngle += 360, endAngle += 360;
	if (startAngle > 360)
		startAngle %= 360;
	while (endAngle < 0)
		endAngle += 360;
	if (endAngle > 360)
		endAngle %= 360;

	// Compute points to start and stop.
	int startX = 0;
	int startY = 0;
	int endX = 0;
	int endY = 0;
	if (startAngle <= 90)
	{
		startX = x + (int)(1L * pSin[(90 - startAngle) / 5] * xRadius / 10000L);
		startY = y - (int)(1L * pSin[startAngle / 5] * yRadius / 10000L);
	}
	else if (startAngle <= 180)
	{
		startX = x - (int)(1L * pSin[(startAngle - 90) / 5] * xRadius / 10000L);
		startY = y - (int)(1L * pSin[(180 - startAngle) / 5] * yRadius / 10000L);
	}
	else if (startAngle <= 270)
	{
		startX = x - (int)(1L * pSin[(270 - startAngle) / 5] * xRadius / 10000L);
		startY = y + (int)(1L * pSin[(startAngle - 180) / 5] * yRadius / 10000L);
	}
	else if (startAngle <= 360)
	{
		startX = x + (int)(1L * pSin[(startAngle - 270) / 5] * xRadius / 10000L);
		startY = y + (int)(1L * pSin[(360 - startAngle) / 5] * yRadius / 10000L);
	}

	if (endAngle <= 90)
	{
		endX = x + (int)(1L * pSin[(90 - endAngle) / 5] * xRadius / 10000L);
		endY = y - (int)(1L * pSin[endAngle / 5] * yRadius / 10000L);
	}
	else if (endAngle <= 180)
	{
		endX = x - (int)(1L * pSin[(endAngle - 90) / 5] * xRadius / 10000L);
		endY = y - (int)(1L * pSin[(180 - endAngle) / 5] * yRadius / 10000L);
	}
	else if (endAngle <= 270)
	{
		endX = x - (int)(1L * pSin[(270 - endAngle) / 5] * xRadius / 10000L);
		endY = y + (int)(1L * pSin[(endAngle - 180) / 5] * yRadius / 10000L);
	}
	else if (endAngle <= 360)
	{
		endX = x + (int)(1L * pSin[(endAngle - 270) / 5] * xRadius / 10000L);
		endY = y + (int)(1L * pSin[(360 - endAngle) / 5] * yRadius / 10000L);
	}

	int oldROP2 = SetROP2(hDC, (xor == TRUE) ? R2_XORPEN : R2_COPYPEN);
	HANDLE oldPen;
	if (palette)
		oldPen = SelectObject(hDC, CreatePen(PS_SOLID, 1, MapColor(palette, TRUE)));

	if (fill)
	{
		HBITMAP hBitmap;
		HBRUSH oldBrush;
		COLORREF oldForeground, oldBackground;
		if (palette)
		{
			hBitmap = CreateBitmap(8, 8, 1, 1,
				(palette->fillPattern & 0x00F0) ?
				(LPSTR)patternTable[PTN_SOLID_FILL] :
				(LPSTR)patternTable[palette->fillPattern]);
			oldBrush = SelectObject(hDC, CreatePatternBrush(hBitmap));
			oldForeground = SetTextColor(hDC, MapColor(palette, TRUE));
			oldBackground = SetBkColor(hDC, MapColor(palette, FALSE));
		}
		int oldMode = SetBkMode(hDC, OPAQUE);

		if (startAngle == 0 && endAngle == 360)
			::Ellipse(hDC, x - xRadius, y - yRadius, x + xRadius, y + yRadius);
		else
//			Pie(hDC, x - xRadius, y - yRadius, x + xRadius, y + yRadius,	// BUG.7215
//				startX, startY, endX, endY);
			Pie(hDC, x - xRadius, y - yRadius, x + xRadius, y + yRadius,
				startX - 1, startY - 1, endX - 1, endY - 1);

		SetBkMode(hDC, oldMode);
		if (palette)
		{
			SetBkColor(hDC, oldBackground);
			SetTextColor(hDC, oldForeground);
			DeleteObject(SelectObject(hDC, oldBrush));
			DeleteObject(hBitmap);
		}
	}
	else
		// Draw a line ellipse.
		Arc(hDC, x - xRadius, y - yRadius, x + xRadius, y + yRadius,
			startX, startY, endX, endY);
	SetROP2(hDC, oldROP2);

	if (palette)
		DeleteObject(SelectObject(hDC, oldPen));

	// Un-virtualize the display.
	if (cRegion)
		DeleteObject(cRegion);
	VirtualPut(screenID);
}

void UI_MSWINDOWS_DISPLAY::IconArrayToHandle(SCREENID screenID, int iconWidth,
	int iconHeight, const UCHAR *iconArray, const UI_PALETTE *palette,
	HICON *icon)
{
// TEMPORARY
#if !defined(WIN32)
if (UI_INTERNATIONAL::countryCode == 81)
	return;
#endif

	HBITMAP colorBitmap = 0, monoBitmap = 0;
	BitmapArrayToHandle(screenID, iconWidth, iconHeight, iconArray,
		palette, &colorBitmap, &monoBitmap);
	extern void WindowsBitmapToIcon(UI_DISPLAY *display,
		HBITMAP colorBitmap, HBITMAP monoBitmap, HICON *icon);
	WindowsBitmapToIcon(this, colorBitmap, monoBitmap, icon);
	DeleteObject(colorBitmap);												// BUG.General
	DeleteObject(monoBitmap);
}

void UI_MSWINDOWS_DISPLAY::IconHandleToArray(SCREENID screenID, HICON icon,
	int *iconWidth, int *iconHeight, UCHAR **iconArray)
{
	HBITMAP colorBitmap = 0, monoBitmap = 0;
	extern void WindowsIconToBitmap(UI_DISPLAY *display, HICON icon,
		HBITMAP *colorBitmap, HBITMAP *monoBitmap);
	WindowsIconToBitmap(this, icon, &colorBitmap, &monoBitmap);
	BitmapHandleToArray(screenID, colorBitmap, monoBitmap, iconWidth,
		iconHeight, iconArray);
}

void UI_MSWINDOWS_DISPLAY::Line(SCREENID screenID, int x1, int y1,
	int x2, int y2, const UI_PALETTE *palette, int width, int xor,
	const UI_REGION *clipRegion)
{
	// Virtualize the display. (This call sets up hDC.)
	VirtualGet(screenID, Min(x1, x2), Min(y1, y2), Max(x1, x2), Max(y1, y2));
	x1 -= _virtualRegion.left;
	y1 -= _virtualRegion.top;
	x2 -= _virtualRegion.left;
	y2 -= _virtualRegion.top;

	// Set the clipRegion (Relative to hDC).
	HRGN cRegion = 0;
	if (clipRegion)
	{
		cRegion = CreateRectRgn(clipRegion->left - _virtualRegion.left,
			clipRegion->top - _virtualRegion.top,
			clipRegion->right - _virtualRegion.left + 1,
			clipRegion->bottom - _virtualRegion.top + 1);
		SelectClipRgn(hDC, cRegion);
	}

	// Draw the line on the window.
	COLORREF color;
	HPEN foreground, oldForeground;
	int oldROP2 = SetROP2(hDC, xor ? R2_XORPEN : R2_COPYPEN);
	if (palette)
	{
		color = MapColor(palette, TRUE);
		foreground = CreatePen(PS_SOLID, width, color);
		oldForeground = SelectObject(hDC, foreground);
	}

#if defined(WIN32)
	MoveToEx(hDC, x1, y1, NULL);
#else
	MoveTo(hDC, x1, y1);
#endif
	LineTo(hDC, x2, y2);

	if (palette)
	{
		SetPixel(hDC, x2, y2, color);
		DeleteObject(SelectObject(hDC, oldForeground));
	}
	SetROP2(hDC, oldROP2);

	// Un-virtualize the display.
	if (cRegion)
		DeleteObject(cRegion);
	VirtualPut(screenID);
}

COLOR UI_MSWINDOWS_DISPLAY::MapColor(const UI_PALETTE *palette, int foreground)
{
	// Check for a valid palette.
	if (!palette)
	{
		if (foreground)
			return (GetTextColor(hDC));
		return (GetBkColor(hDC));
	}

	// Match the color request based on the COLOR type in fillPattern.
	COLOR color = foreground ? palette->colorForeground : palette->colorBackground;
	if (palette->fillPattern == PTN_SYSTEM_COLOR)
		return (GetSysColor((int)color));
	else if (palette->fillPattern == PTN_RGB_COLOR)
		return (color);
	else if (color <= MAX_COLORMAP_INDEX)
		return (colorMap[(int)color].colorForeground);
	else
		return (RGB_BLACK);
}

void UI_MSWINDOWS_DISPLAY::Polygon(SCREENID screenID, int numPoints,
	const int *polygonPoints, const UI_PALETTE *palette, int fill, int xor,
	const UI_REGION *clipRegion)
{
	// Determine the maximum region that bounds the polygon.
	int left = 0xFFFF, top = 0xFFFF;
	int right = 0, bottom = 0;
	POINT *winPolygon = new POINT[numPoints];
	for (int i = 0; i < numPoints; i++)
	{
		int column = polygonPoints[i*2];
		int line = polygonPoints[i*2+1];
		if (column < left)
			left = column;
		if (column > right)
			right = column;
		if (line < top)
			top = line;
		if (line > bottom)
			bottom = line;
	}

	// Virtualize the display. (This call sets up hDC.)
	VirtualGet(screenID, left, top, right, bottom);

	// Set up the polygon points.
	for (i = 0; i < numPoints; i++)
	{
		winPolygon[i].x = polygonPoints[i*2] - _virtualRegion.left;
		winPolygon[i].y = polygonPoints[i*2+1] - _virtualRegion.top;
	}

	// Set the clipRegion.
	HRGN cRegion = 0;
	if (clipRegion)
	{
		cRegion = CreateRectRgn(clipRegion->left - _virtualRegion.left,
			clipRegion->top - _virtualRegion.top,
			clipRegion->right - _virtualRegion.left + 1,
			clipRegion->bottom - _virtualRegion.top + 1);
		SelectClipRgn(hDC, cRegion);
	}

	HANDLE oldPen;
	int oldROP2 = SetROP2(hDC, xor ? R2_XORPEN : R2_COPYPEN);
	if (palette)
		oldPen = SelectObject(hDC, CreatePen(PS_SOLID, 1, MapColor(palette, TRUE)));
	int oldFillMode = SetPolyFillMode(hDC, WINDING);

	if (fill)
	{
		// Set the fill pattern.
		HBITMAP hBitmap;
		HBRUSH oldBrush;
		COLORREF oldForeground, oldBackground;
		if (palette)
		{
			hBitmap = CreateBitmap(8, 8, 1, 1,
				(palette->fillPattern & 0x00F0) ?
				(LPSTR)patternTable[PTN_SOLID_FILL] :
				(LPSTR)patternTable[palette->fillPattern]);
			oldBrush = SelectObject(hDC, CreatePatternBrush(hBitmap));
			oldForeground = SetTextColor(hDC, MapColor(palette, TRUE));
			oldBackground = SetBkColor(hDC, MapColor(palette, FALSE));
		}
		int oldMode = SetBkMode(hDC, OPAQUE);

		// Draw an filled polygon.
		::Polygon(hDC, winPolygon, numPoints);

		SetBkMode(hDC, oldMode);
		if (palette)
		{
			SetBkColor(hDC, oldBackground);
			SetTextColor(hDC, oldForeground);
			DeleteObject(SelectObject(hDC, oldBrush));
			DeleteObject(hBitmap);
		}
	}
	else
		// Draw a line polygon.
		Polyline(hDC, winPolygon, numPoints);
	delete winPolygon;
	SetPolyFillMode(hDC, oldFillMode);
	if (palette)
		DeleteObject(SelectObject(hDC, oldPen));
	SetROP2(hDC, oldROP2);

	// Un-virtualize the display.
	if (cRegion)
		DeleteObject(cRegion);
	VirtualPut(screenID);
}

void UI_MSWINDOWS_DISPLAY::Rectangle(SCREENID screenID, int left, int top,
	int right, int bottom, const UI_PALETTE *palette, int width, int fill,
	int xor, const UI_REGION *clipRegion)
{
	// Virtualize the display. (This call sets up hDC.)
	VirtualGet(screenID, left, top, right, bottom);

	// Change coordinates to be relative to hDC.
	RECT rect;
	rect.left = left - _virtualRegion.left;
	rect.top = top - _virtualRegion.top;
	rect.right = right - _virtualRegion.left + 1;
	rect.bottom = bottom - _virtualRegion.top + 1;

	// Set the clipRegion.
	HRGN cRegion = 0;
	if (clipRegion)
	{
		cRegion = CreateRectRgn(clipRegion->left - _virtualRegion.left,
			clipRegion->top - _virtualRegion.top,
			clipRegion->right - _virtualRegion.left + 1,
			clipRegion->bottom - _virtualRegion.top + 1);
		SelectClipRgn(hDC, cRegion);
	}

	// Draw the outline.
	HBRUSH borderBrush = CreateSolidBrush(MapColor(palette, TRUE));
	for (int i = 0; i < width; i++)
	{
		if (xor)
			DrawFocusRect(hDC, &rect);
		else
			::FrameRect(hDC, &rect, borderBrush);
		rect.left++;
		rect.top++;
		rect.right--;
		rect.bottom--;
	}
	DeleteObject(borderBrush);

	// Fill the rectangle.
	if (fill)
	{
		// Set the fill pattern.
		HBITMAP hBitmap = CreateBitmap(8, 8, 1, 1,
			(!palette || (palette->fillPattern & 0x00F0)) ?
			(LPSTR)patternTable[PTN_SOLID_FILL] :
			(LPSTR)patternTable[palette->fillPattern]);
		HBRUSH fillBrush = CreatePatternBrush(hBitmap);
		COLORREF oldForeground, oldBackground;
		if (palette)
		{
			oldForeground = SetTextColor(hDC, MapColor(palette, TRUE));
			oldBackground = SetBkColor(hDC, MapColor(palette, FALSE));
		}

		FillRect(hDC, &rect, fillBrush);

		if (palette)
		{
			SetBkColor(hDC, oldBackground);
			SetTextColor(hDC, oldForeground);
		}
		DeleteObject(fillBrush);
		DeleteObject(hBitmap);
	}

	// Un-virtualize the display.
	if (cRegion)
		DeleteObject(cRegion);
	VirtualPut(screenID);
}

void UI_MSWINDOWS_DISPLAY::RectangleXORDiff(const UI_REGION &, const UI_REGION &, SCREENID)
{
}

void UI_MSWINDOWS_DISPLAY::RegionMove(const UI_REGION &oldRegion, int newColumn,
	int newLine, SCREENID oldScreenID, SCREENID newScreenID)
{
	// Get the device context.
	HDC _hDestDC = (newScreenID != ID_DIRECT) ? GetDC(newScreenID) : hDC;
	HDC _hSrcDC = (oldScreenID != ID_DIRECT) ? GetDC(oldScreenID) : hDC;

	BitBlt(_hDestDC, newColumn, newLine, oldRegion.right - oldRegion.left + 1,
		oldRegion.bottom - oldRegion.top + 1, _hSrcDC, oldRegion.left,
		oldRegion.top, SRCCOPY);

	if (newScreenID != ID_DIRECT)
		ReleaseDC(newScreenID, _hDestDC);
	if (oldScreenID != ID_DIRECT)
		ReleaseDC(oldScreenID, _hSrcDC);
}

void UI_MSWINDOWS_DISPLAY::Text(SCREENID screenID, int left, int top,
	const char *text, const UI_PALETTE *palette, int length, int fill,
	int xor, const UI_REGION *clipRegion, LOGICAL_FONT logicalFont)
{
	// Make sure there is a valid string.
	if (!text || text[0] == '\0')
		return;

	logicalFont &= 0x0FFF;

	// Virtualize the display. (This call sets up hDC.)
	int right = left + TextWidth(text, screenID, logicalFont) - 1;
	int bottom = top + TextHeight(text, screenID, logicalFont) - 1;
	VirtualGet(screenID, left, top, right, bottom);
	left -= _virtualRegion.left;
	top -= _virtualRegion.top;
	right -= _virtualRegion.left;
	bottom -= _virtualRegion.top;

	// Set the clipRegion.
	HRGN cRegion = 0;
	if (clipRegion)
	{
		cRegion = CreateRectRgn(clipRegion->left - _virtualRegion.left,
			clipRegion->top - _virtualRegion.top,
			clipRegion->right - _virtualRegion.left + 1,
			clipRegion->bottom - _virtualRegion.top + 1);
		SelectClipRgn(hDC, cRegion);
	}
	if (length < 0)
		length = ui_strlen(text);

	// Set up the colors, fill and font and draw the text.
	int oldDrawMode = SetROP2(hDC, xor ? R2_XORPEN : R2_COPYPEN);
	COLORREF oldForeground, oldBackground;
	if (palette)
	{
		oldForeground = SetTextColor(hDC, MapColor(palette, TRUE));
		oldBackground = SetBkColor(hDC, MapColor(palette, FALSE));
	}
	int oldMode = SetBkMode(hDC, TRANSPARENT);
	HFONT oldFont = SelectObject(hDC, fontTable[logicalFont]);

	if (fill)
		Rectangle(screenID, left + _virtualRegion.left,
			top + _virtualRegion.top,
			left + _virtualRegion.left + TextWidth(text, screenID, logicalFont) - 1,
			top + _virtualRegion.top + TextHeight(text, screenID, logicalFont) - 1,
			palette, 0, TRUE, xor, clipRegion);

	RECT region = { left, top, right + 1, bottom + 1 };
	WORD format = DT_SINGLELINE | DT_VCENTER | DT_LEFT;
	::DrawText(hDC, (LPSTR)text, length, &region, format);

	if (palette)
	{
		SetTextColor(hDC, oldForeground);
		SetBkColor(hDC, oldBackground);
	}
	SetBkMode(hDC, oldMode);
	SelectObject(hDC, oldFont);
	SetROP2(hDC, oldDrawMode);

	// Un-virtualize the display.
	if (cRegion)
		DeleteObject(cRegion);
	VirtualPut(screenID);
}

int UI_MSWINDOWS_DISPLAY::TextHeight(const char *string, SCREENID screenID,
	LOGICAL_FONT logicalFont)
{
	// Make sure there is a valid string.
	if (!string || !string[0])
		return (cellHeight - preSpace - postSpace);

	logicalFont &= 0x0FFF;
	HDC _hDC;
	if (screenID != ID_SCREEN)
		_hDC = GetDC(screenID);
	else
	{
		HDC tHDC = GetDC(GetDesktopWindow());
		_hDC = CreateCompatibleDC(tHDC);
		ReleaseDC(GetDesktopWindow(), tHDC);
	}

	// Get the string height.
	HFONT oldFont = SelectObject(_hDC, fontTable[logicalFont]);
#if defined(WIN32)
	SIZE size;
	GetTextExtentPoint(_hDC, (LPSTR)string, ui_strlen(string), &size);
	int height = size.cy;
#else
	int height = HIWORD(GetTextExtent(_hDC, (LPSTR)string, ui_strlen(string)));
#endif
	SelectObject(_hDC, oldFont);

	if (screenID != ID_SCREEN)
		ReleaseDC(screenID, _hDC);
	else
		DeleteDC(_hDC);

	// Return the text height.
	return (height);
}

int UI_MSWINDOWS_DISPLAY::TextWidth(const char *string, SCREENID screenID,
	LOGICAL_FONT logicalFont)
{
	// Make sure there is a valid string.
	if (!string || !string[0])
		return (0);

	logicalFont &= 0x0FFF;
	HDC _hDC;
	if (screenID != ID_SCREEN)
		_hDC = GetDC(screenID);
	else
	{
		HDC tHDC = GetDC(GetDesktopWindow());
		_hDC = CreateCompatibleDC(tHDC);
		ReleaseDC(GetDesktopWindow(), tHDC);
	}

	// Get the string width.
	HFONT oldFont = SelectObject(_hDC, fontTable[logicalFont]);
#if defined(WIN32)
	SIZE size;
	GetTextExtentPoint(_hDC, (LPSTR)string, ui_strlen(string), &size);
	int width = size.cx;
#else
	int width = LOWORD(GetTextExtent(_hDC, (LPSTR)string, ui_strlen(string)));
#endif
	SelectObject(_hDC, oldFont);

	if (screenID != ID_SCREEN)
		ReleaseDC(screenID, _hDC);
	else
		DeleteDC(_hDC);

	// Return the text width.
	return (width);
}

int UI_MSWINDOWS_DISPLAY::VirtualGet(SCREENID screenID, int left, int top,
	int right, int bottom)
{
	if (--_virtualCount == -1)
	{
		RECT winRegion;
		_virtualUpdate = FALSE;
		if (screenID == ID_DIRECT)
			;
		else if (GetUpdateRect(screenID, &winRegion, FALSE))
			hDC = BeginPaint(screenID, &_winPaint);
		else
		{
			hDC = GetDC(screenID);
			_virtualUpdate = TRUE;
		}
		_virtualRegion.left = left;
		_virtualRegion.top = top;
		_virtualRegion.right = right;
		_virtualRegion.bottom = bottom;
	}
	return (TRUE);
}

int UI_MSWINDOWS_DISPLAY::VirtualPut(SCREENID screenID)
{
	if (++_virtualCount == 0)
	{
		if (!hDC || screenID == ID_DIRECT)
			;
		else if (_virtualUpdate)
			ReleaseDC(screenID, hDC);
		else
			EndPaint(screenID, &_winPaint);
		_virtualRegion.left = 
			_virtualRegion.top =
			_virtualRegion.right =
			_virtualRegion.bottom = 0;
		hDC = 0;
	}
	return (TRUE);
}

void WindowsIconToBitmap(UI_DISPLAY *, HICON icon,
	HBITMAP *colorBitmap, HBITMAP *monoBitmap)
{
	HBITMAP hBitmap;
	HBITMAP hOldBitmap;
	HBITMAP hANDBitmap;
	HBITMAP hOldANDBitmap;
	HDC hDC = GetDC(0);
	HDC hMemDC = CreateCompatibleDC(hDC);
	HDC hANDMemDC = CreateCompatibleDC(hDC);
	int iconWidth = GetSystemMetrics(SM_CXICON);
	int iconHeight = GetSystemMetrics(SM_CYICON);

	if (colorBitmap)
	{
		// Select a bitmap of icon size into the memory DC.
		hBitmap = CreateCompatibleBitmap(hDC, iconWidth, iconHeight);
		hOldBitmap = SelectObject(hMemDC, hBitmap);

		// Draw the icon on to the bitmap.
		DrawIcon(hMemDC, 0, 0, icon);

		// Get the bitmap.
		hBitmap = SelectObject(hMemDC, hOldBitmap);
		*colorBitmap = hBitmap;
	}

	if (monoBitmap)
	{
		// Select a bitmap of icon size into the memory DC.
		hBitmap = CreateBitmap(iconWidth, iconHeight, 1, 1, NULL);
		hANDBitmap = CreateBitmap(iconWidth, iconHeight, 1, 1, NULL);
		hOldBitmap = SelectObject(hMemDC, hBitmap);
		hOldANDBitmap = SelectObject(hANDMemDC, hANDBitmap);

		// Draw the icon on to the bitmap.
		BitBlt(hMemDC, 0, 0, iconWidth, iconHeight, NULL, 0, 0, WHITENESS);
		DrawIcon(hMemDC, 0, 0, icon);
		BitBlt(hANDMemDC, 0, 0, iconWidth, iconHeight, NULL, 0, 0, BLACKNESS);
		DrawIcon(hANDMemDC, 0, 0, icon);
		BitBlt(hMemDC, 0, 0, iconWidth, iconHeight, hANDMemDC, iconWidth, iconHeight, SRCERASE);

		// Get the bitmap.
		hBitmap = SelectObject(hMemDC, hOldBitmap);
		hANDBitmap = SelectObject(hMemDC, hOldANDBitmap);
		*monoBitmap = hBitmap;


		BITMAP bitmap;
		GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitmap);
		BitBlt(hDC, 0, 0, iconWidth, iconHeight, hMemDC, iconWidth, iconHeight, SRCCOPY);
	}

	// Clean up.
	DeleteDC(hMemDC);
	DeleteDC(hANDMemDC);
	ReleaseDC(0, hDC);
}

void WindowsBitmapToIcon(UI_DISPLAY *display, HBITMAP colorBitmap,
	HBITMAP monoBitmap, HICON *icon)
{
	if (!colorBitmap)
		return;

	BITMAP colorInfo;
	BITMAP monoInfo;

	// Find size of the icon.
	GetObject(colorBitmap, sizeof(BITMAP), (LPSTR)&colorInfo);
	DWORD size = colorInfo.bmWidth * colorInfo.bmHeight * colorInfo.bmPlanes * colorInfo.bmBitsPixel;
	UCHAR *colorBits = new UCHAR[(int)size + 1];
	UCHAR *monoBits = new UCHAR[(int)size + 1];

	// Get the color bits.
	size = GetBitmapBits(colorBitmap, size, (LPSTR)colorBits);

	// If monochrome bitmap then use bits, else set bits to black.
	if (monoBitmap)
		GetObject(monoBitmap, sizeof(BITMAP), (LPSTR)&monoInfo);
	if (!monoBitmap || monoInfo.bmPlanes != 1 || monoInfo.bmBitsPixel != 1)
		memset(monoBits, 0, (size_t)size);
	else
		size = GetBitmapBits(monoBitmap, size, (LPSTR)monoBits);

	// Create the icon.
#if defined(WIN32)
	*icon = CreateIcon(display->hInstance, colorInfo.bmWidth, colorInfo.bmHeight,
		(BYTE)colorInfo.bmPlanes, (BYTE)colorInfo.bmBitsPixel, monoBits, colorBits);
#else
	*icon = CreateIcon(display->hInstance, colorInfo.bmWidth, colorInfo.bmHeight,
		colorInfo.bmPlanes, colorInfo.bmBitsPixel, (LPSTR)monoBits, (LPSTR)colorBits);
#endif

	// Clean up.
	delete colorBits;
	delete monoBits;
}

