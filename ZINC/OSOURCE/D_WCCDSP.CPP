//	Zinc Interface Library - D_WCCDSP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <malloc.h>
#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <graph.h>
#include "ui_dsp.hpp"

#if defined(OEM)
#	define FONTOUT
#else
extern UCHAR mapISOto850(UCHAR val);
#	define FONTOUT	mapISOto850
#endif

// ----- UI_WCC_DISPLAY -----------------------------------------------------

static int _originalMode = -1;
static int _originalCursor;

#pragma check_stack (off)
UI_WCC_DISPLAY::UI_WCC_DISPLAY(int mode):
	UI_DISPLAY(FALSE, "DOS", "WCC")
{
#if defined(ZIL_LINKBUG)
	extern void z_wccdsp_dummy(void);		// Bug fix for Zortech & Microsoft linkers.
	z_wccdsp_dummy();
#endif

	// Initialize the basic member variables.
	_virtualCount = 0;
	_stopDevice = FALSE;
	_fillPattern = -1;
	_backgroundColor = -1;
	_foregroundColor = -1;

	// Register and select a font.
	struct _fontinfo fontInfo;

	// Use temporary path if not installed in main().
	int pathInstalled = searchPath ? TRUE : FALSE;
	if (!pathInstalled)
		searchPath = new UI_PATH;
	const char *pathName = searchPath->FirstPathName();
	int registered = 0;
	char path[128];
	while (pathName && registered <= 0)
	{
		strcpy(path, pathName);
		if (path[ui_strlen(path) - 1] != '\\')
			strcat(path, "\\");
		strcat(path, "HELVB.FON");
		registered = _registerfonts(path);
		if (registered > 0)
			break;
		pathName = searchPath->NextPathName();
	}

	if (registered <= 0)
		return;

	// Delete path if it was installed temporarily.
	if (!pathInstalled)
	{
		delete searchPath;
		searchPath = NULL;
	}

	// Set mode to maximum resolution if mode is not specified
	struct videoconfig config;
	if (_originalMode == -1)
	{
		_getvideoconfig(&config);
		_originalMode = config.mode;
		_originalCursor = _gettextcursor();
	}

	if (!mode)
		mode = _MAXRESMODE;
	if (!_setvideomode(mode))
		return;

	// Get specifics about the video setup.
	_getvideoconfig(&config);
	lines = config.numypixels;
	columns = config.numxpixels;
	maxColors = config.numcolors;
	isMono = (maxColors == 2);

	// Reset the font information.
	SetFont(-1);

	// Fill the screen according to the specified palette.
	cellWidth = TextWidth("M", ID_SCREEN, FNT_DIALOG_FONT) - 2;
	cellHeight = TextHeight(NULL, ID_SCREEN, FNT_DIALOG_FONT) +
		preSpace + postSpace + 2 + 3;  // 2 above the text, 2 below the text.
	_setvieworg(0, 0);
	_setviewport(0, 0, columns - 1, lines - 1);
	SetPattern(backgroundPalette, FALSE);
	_setcolor(_backgroundColor);
	_setfillmask(patternTable[_fillPattern]);
	_rectangle(_GFILLINTERIOR, 0, 0, columns - 1, lines - 1);

	// Define the screen display region.
	Add(0, new UI_REGION_ELEMENT(ID_SCREEN, 0, 0, columns - 1, lines - 1));
	installed = TRUE;
}

UI_WCC_DISPLAY::~UI_WCC_DISPLAY(void)
{
	// Restore the display.
	if (installed)
	{
		_settextcursor(_originalCursor);
		_setvideomode(_originalMode);
		_clearscreen(_GCLEARSCREEN);
	}
}

void UI_WCC_DISPLAY::Bitmap(SCREENID screenID, int column, int line,
	int bitmapWidth, int bitmapHeight, const UCHAR *bitmapArray,
	const UI_PALETTE *palette, const UI_REGION *clipRegion, HBITMAP *,
	HBITMAP *)
{
	// Assign the bitmap to the region structure.
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, column, line,
		column + bitmapWidth - 1, line + bitmapHeight - 1))
		return;
	if (!palette)
		palette = colorMap;

	// Draw the rectangle on the display.
	int changedScreen = FALSE;
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			if (!changedScreen)
				changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
			_setviewport(0, 0, columns - 1, lines - 1);
			for (int y = tRegion.top; y <= tRegion.bottom; y++)
			{
				const UCHAR *pixel = bitmapArray + (y - line) * bitmapWidth + tRegion.left - column;
				for (int x = tRegion.left; x <= tRegion.right; x++, pixel++)
					if (*pixel != BACKGROUND)
					{
						_setcolor(MapColor(palette+*pixel, TRUE));
						_setpixel(x, y);
					}
			}
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

void UI_WCC_DISPLAY::Ellipse(SCREENID screenID, int x, int y,
	int startAngle, int endAngle, int xRadius, int yRadius,
	const UI_PALETTE *palette, int fill, int xor, const UI_REGION *clipRegion)
{
	const int pSin[] = { 0, 871, 1736, 2588, 3420, 4226, 5000, 5736, 6428,
		7071, 7660, 8191, 8660, 9063, 9397, 9659, 9848, 9962, 10000 };

	// Assign the ellipse to the region structure.
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, x - xRadius, y - yRadius,
		x + xRadius, y + yRadius))
		return;
	while (startAngle < 0)							// BUG.1616
		startAngle += 360, endAngle += 360;
	if (startAngle > 360)
		startAngle %= 360;
	while (endAngle < 0)
		endAngle += 360;
	if (endAngle > 360)
		endAngle %= 360;

	// Draw the ellipse on the display.
	int startX = 0;
	int startY = 0;
	int endX = 0;
	int endY = 0;
	int changedScreen = FALSE;
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			if (!changedScreen)
			{
				changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
				SetPattern(palette, xor);

				// Compute points to start and stop.
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
			}
			_setviewport(tRegion.left, tRegion.top, tRegion.right, tRegion.bottom);

			if (fill && _fillPattern != PTN_SOLID_FILL)
			{
				_setcolor(_foregroundColor);
				_setfillmask(NULL);
				if (startAngle == 0 && endAngle == 360)
					_ellipse(_GFILLINTERIOR, x - xRadius - tRegion.left, y - yRadius - tRegion.top, x + xRadius - tRegion.left, y + yRadius - tRegion.top);
				else
					_pie(_GFILLINTERIOR, x - xRadius - tRegion.left, y - yRadius - tRegion.top, x + xRadius - tRegion.left, y + yRadius - tRegion.top, startX - tRegion.left, startY - tRegion.top, endX - tRegion.left, endY - tRegion.top);

				_setcolor(_backgroundColor);
				_setfillmask(patternTable[_fillPattern]);
				if (startAngle == 0 && endAngle == 360)
					_ellipse(_GFILLINTERIOR, x - xRadius - tRegion.left, y - yRadius - tRegion.top, x + xRadius - tRegion.left, y + yRadius - tRegion.top);
				else
					_pie(_GFILLINTERIOR, x - xRadius - tRegion.left, y - yRadius - tRegion.top, x + xRadius - tRegion.left, y + yRadius - tRegion.top, startX - tRegion.left, startY - tRegion.top, endX - tRegion.left, endY - tRegion.top);
			}
			else if (fill)
			{
				_setcolor(_backgroundColor);
				_setfillmask(NULL);
				if (startAngle == 0 && endAngle == 360)
					_ellipse(_GFILLINTERIOR, x - xRadius - tRegion.left, y - yRadius - tRegion.top, x + xRadius - tRegion.left, y + yRadius - tRegion.top);
				else
					_pie(_GFILLINTERIOR, x - xRadius - tRegion.left, y - yRadius - tRegion.top, x + xRadius - tRegion.left, y + yRadius - tRegion.top, startX - tRegion.left, startY - tRegion.top, endX - tRegion.left, endY - tRegion.top);
			}
			if (!fill || _backgroundColor != _foregroundColor)
			{
				_setcolor(_foregroundColor);
				if (startAngle == 0 && endAngle == 360)
					_ellipse(_GBORDER, x - xRadius - tRegion.left, y - yRadius - tRegion.top, x + xRadius - tRegion.left, y + yRadius - tRegion.top);
				else
					_pie(_GBORDER, x - xRadius - tRegion.left, y - yRadius - tRegion.top, x + xRadius - tRegion.left, y + yRadius - tRegion.top, startX - tRegion.left, startY - tRegion.top, endX - tRegion.left, endY - tRegion.top);
			}
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		SetPattern(palette, FALSE);
}

int UI_WCC_DISPLAY::DeviceMove(IMAGE_TYPE imageType, int newColumn, int newLine)
{
	// Make sure the image type is valid.
	int x, y;
	UI_DISPLAY_IMAGE *view = &displayImage[imageType];
	if (!view->image || _stopDevice || _virtualCount != 0 ||
		(view->region.left == newColumn && view->region.top == newLine))
		return (FALSE);
	_stopDevice = TRUE;

	int width = view->region.right - view->region.left + 1;
	int height = view->region.bottom - view->region.top + 1;

	int overlap = FALSE;
	if (newColumn < 0 || newLine < 0)					// BUG.1066
		newColumn = newLine = width = height = 0;
	else
	{
		int deltaX = newColumn - view->region.left;
		int deltaY = newLine - view->region.top;

		UI_REGION newRegion;
		newRegion.left = view->region.left + deltaX;
		newRegion.right = view->region.right + deltaX;
		newRegion.top = view->region.top + deltaY;
		newRegion.bottom = view->region.bottom + deltaY;

		for (int i = 0; i < MAX_DISPLAY_IMAGES; i++)
			if (i != imageType && (view->region.Overlap(displayImage[i].region) ||
					       newRegion.Overlap(displayImage[i].region)))
			{
				overlap = TRUE;
				break;
			}

		if (overlap)
		{
			UI_REGION virtualRegion;
			virtualRegion.left = Min(view->region.left, newRegion.left);
			virtualRegion.top = Min(view->region.top, newRegion.top);
			virtualRegion.right = Max(view->region.right, newRegion.right);
			virtualRegion.bottom = Max(view->region.bottom, newRegion.bottom);

			VirtualGet(0, virtualRegion.left, virtualRegion.top,
				   virtualRegion.right, virtualRegion.bottom);

			view->region.left += deltaX;
			view->region.right += deltaX;
			view->region.top += deltaY;
			view->region.bottom += deltaY;

			VirtualPut(0);
		}
		else
		{
			// Compute the update coordinates.
			_setviewport(0, 0, columns - 1, lines - 1);
			UCHAR *image = view->image;
			UCHAR *screen = view->screen;
			UCHAR *backup = view->backup;

			int oldY = view->region.top;
			int newY = newLine;
			for (y = 0; y < height; y++, oldY++, newY++)
			{
				UCHAR *tScreen = &screen[deltaY * width + deltaX];
				int oldX = view->region.left;
				int newX = newColumn;
				for (x = 0; x < width; x++, oldX++, newX++, image++, screen++, backup++, tScreen++)
				{
					if (*image == BACKGROUND)
						*backup = BACKGROUND;
					else if (*image != BACKGROUND &&
						 x + deltaX >= 0 && x + deltaX < width &&
						 y + deltaY >= 0 && y + deltaY < height &&
						 *tScreen != BACKGROUND)
					{
						*backup = *tScreen;
						*tScreen = BACKGROUND;
					}
					else
						*backup = _getpixel(newX, newY);
					if (*image != BACKGROUND)
					{
						_setcolor(*image);
						_setpixel(newX, newY);
					}
				}
			}
		}
	}
	if (!overlap)
	{
		UCHAR *screen = view->screen;
		for (y = view->region.top; y <= view->region.bottom; y++)
			for (x = view->region.left; x <= view->region.right; x++, screen++)
				if (*screen != BACKGROUND)
				{
					_setcolor(*screen);
					_setpixel(x, y);
				}
		memcpy(view->screen, view->backup, width * height);
		view->region.left = newColumn;
		view->region.top = newLine;
		view->region.right = newColumn + width - 1;
		view->region.bottom = newLine + height - 1;
	}

	_stopDevice = FALSE;
	return (TRUE);
}

int UI_WCC_DISPLAY::DeviceSet(IMAGE_TYPE imageType, int column, int line,
	int width, int height, UCHAR *image)
{
	if (_stopDevice)
		return (FALSE);
	_stopDevice = TRUE;

	UI_DISPLAY_IMAGE *view = &displayImage[imageType];
	UI_REGION virtualRegion;
	if (view->image)
	{
		virtualRegion.left = Min(view->region.left, column);
		virtualRegion.top = Min(view->region.top, line);
		virtualRegion.right = Max(view->region.right, column + width - 1);
		virtualRegion.bottom = Max(view->region.bottom, line + height - 1);
	}
	else
	{
		virtualRegion.left = column;
		virtualRegion.top = line;
		virtualRegion.right = column + width - 1;
		virtualRegion.bottom = line + height - 1;
	}

	VirtualGet(0, virtualRegion.left, virtualRegion.top, virtualRegion.right,
		virtualRegion.bottom);

	view->image = image;
	view->region.left = column;
	view->region.top = line;
	view->region.right = column + width - 1;
	view->region.bottom = line + height - 1;
	if (view->image)
	{
		if (view->screen)
		{
			delete view->screen;
			delete view->backup;
		}
		view->screen = new UCHAR[width * height];
		view->backup = new UCHAR[width * height];
		memset(view->screen, BACKGROUND, width * height);
	}

	VirtualPut(0);
	_stopDevice = FALSE;
	return (TRUE);
}

void UI_WCC_DISPLAY::Line(SCREENID screenID, int x1, int y1,
	int x2, int y2, const UI_PALETTE *palette, int width, int xor,
	const UI_REGION *clipRegion)
{
	// Assign the line to a region structure.
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, Min(x1, x2), Min(y1, y2),
		Max(x1, x2) + width, Max(y1, y2) + width))
		return;

	// Draw the line on the display.
	int changedScreen = FALSE;
	int slope;
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			if (!changedScreen)
			{
				changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
				SetPattern(palette, xor);
				_setcolor(_foregroundColor);
				slope = x1 == x2 ? 1 : (y1 - y2) / (x1 - x2);
			}
			_setviewport(tRegion.left, tRegion.top, tRegion.right, tRegion.bottom);
			for (int i = 0; i < width; i++)
				if (slope)
				{
					_moveto(x1 - tRegion.left + i, y1 - tRegion.top);
					_lineto(x2 - tRegion.left + i, y2 - tRegion.top);
				}
				else
				{
					_moveto(x1 - tRegion.left, y1 - tRegion.top + i);
					_lineto(x2 - tRegion.left, y2 - tRegion.top + i);
				}
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

COLOR UI_WCC_DISPLAY::MapColor(const UI_PALETTE *palette, int foreground)
{
	// Match the color request based on the type of display.
	if (maxColors == 2)
		return (foreground ? palette->bwForeground : palette->bwBackground);
	else if (maxColors < 16)
		return (foreground ? palette->grayScaleForeground : palette->grayScaleBackground);
	return (foreground ? palette->colorForeground : palette->colorBackground);
}

void UI_WCC_DISPLAY::Polygon(SCREENID screenID, int numPoints,
	const int *polygonPoints, const UI_PALETTE *palette, int fill, int xor,
	const UI_REGION *clipRegion)
{
	// Assign the polygon to the region structure.
	int left = 0x0FFF, top = 0x0FFF, right = 0, bottom = 0;
	for (int i = 0; i < numPoints * 2; i += 2)
	{
		left = Min(left, polygonPoints[i]);
		top = Min(top, polygonPoints[i+1]);
		right = Max(right, polygonPoints[i]);
		bottom = Max(bottom, polygonPoints[i+1]);
	}
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, left, top, right, bottom))
		return;

	// Draw the polygon on the display.
	int changedScreen = FALSE;
	int *tPolygon = new int[numPoints * 2];
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			if (!changedScreen)
			{
				changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
				SetPattern(palette, xor);
			}
			_setviewport(tRegion.left, tRegion.top, tRegion.right, tRegion.bottom);
			for (i = 0; i < numPoints * 2; i += 2)
			{
				tPolygon[i] = polygonPoints[i] - tRegion.left;
				tPolygon[i+1] = polygonPoints[i+1] - tRegion.top;
			}
			if (fill && _fillPattern != PTN_SOLID_FILL)
			{
				_setcolor(_foregroundColor);
				_setfillmask(NULL);
				_polygon(_GFILLINTERIOR, numPoints, (struct xycoord *)tPolygon);

				_setcolor(_backgroundColor);
				_setfillmask(patternTable[_fillPattern]);
				_polygon(_GFILLINTERIOR, numPoints, (struct xycoord *)tPolygon);
			}
			else if (fill)
			{
				_setcolor(_backgroundColor);
				_setfillmask(NULL);
				_polygon(_GFILLINTERIOR, numPoints, (struct xycoord *)tPolygon);
			}
			if (!fill || _backgroundColor != _foregroundColor)
			{
				_setcolor(_foregroundColor);
				for (i = 2; i < numPoints * 2; i += 2)
				{
					_moveto(tPolygon[i-2], tPolygon[i-1]);
					_lineto(tPolygon[i], tPolygon[i+1]);
				}
			}
			if (screenID == ID_DIRECT)
				break;
		}
	delete tPolygon;

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

void UI_WCC_DISPLAY::Rectangle(SCREENID screenID, int left, int top,
	int right, int bottom, const UI_PALETTE *palette, int width, int fill,
	int xor, const UI_REGION *clipRegion)
{
	// Assign the rectangle to the region structure.
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, left, top, right, bottom))
		return;

	// Draw the rectangle on the display.
	int changedScreen = FALSE;
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			if (!changedScreen)
			{
				changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
				SetPattern(palette, xor);
			}
			_setviewport(tRegion.left, tRegion.top, tRegion.right, tRegion.bottom);
			if (fill && _fillPattern != PTN_SOLID_FILL)
			{
				_setcolor(_foregroundColor);
				_setfillmask(NULL);
				_rectangle(_GFILLINTERIOR, left - tRegion.left, top - tRegion.top, right - tRegion.left, bottom - tRegion.top);

				_setcolor(_backgroundColor);
				_setfillmask(patternTable[_fillPattern]);
				_rectangle(_GFILLINTERIOR, left - tRegion.left, top - tRegion.top, right - tRegion.left, bottom - tRegion.top);
			}
			else if (fill)
			{
				_setcolor(_backgroundColor);
				_setfillmask(NULL);
				_rectangle(_GFILLINTERIOR, left - tRegion.left, top - tRegion.top, right - tRegion.left, bottom - tRegion.top);
			}
			if (!fill || _backgroundColor != _foregroundColor)
			{
				_setcolor(_foregroundColor);
				for (int i = 0; i < width; i++)
				{
					_rectangle(_GBORDER, left - (tRegion.left - i), top - (tRegion.top - i),
						right - (tRegion.left + i), bottom - (tRegion.top + i));
				}
			}
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

void UI_WCC_DISPLAY::RectangleXORDiff(const UI_REGION &oldRegion,
	const UI_REGION &newRegion, SCREENID)
{
	// See if the regions match.
	if (oldRegion.left == newRegion.left && oldRegion.top == newRegion.top &&
		oldRegion.right == newRegion.right && oldRegion.bottom == newRegion.bottom)
		return;

	// Hide the screen devices.
	UI_REGION region;
	if (!RegionInitialize(region, NULL,
		Min(oldRegion.left, newRegion.left), Min(oldRegion.top, newRegion.top),
		Max(oldRegion.right, newRegion.right), Max(oldRegion.bottom, newRegion.bottom)))
		return;
	VirtualGet(ID_DIRECT, region.left, region.top, region.right, region.bottom);

	// Draw the XOR rectangles on the display.
	SetPattern(xorPalette, TRUE);
	_setcolor(_foregroundColor);
	_setviewport(0, 0, columns - 1, lines - 1);
	_rectangle(_GBORDER, oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.bottom);
	_rectangle(_GBORDER, newRegion.left, newRegion.top, newRegion.right, newRegion.bottom);
	SetPattern(xorPalette, FALSE);

	// Update the screen.
	VirtualPut(ID_DIRECT);
}

void UI_WCC_DISPLAY::RegionDefine(SCREENID screenID, int left, int top,
	int right, int bottom)
{
	// See if it is a full screen definition.
	UI_REGION region = { left, top, right, bottom };
	if (region.left <= 0 && region.top <= 0 && region.right >= columns - 1 && region.bottom >= lines - 1)
	{
		UI_REGION_LIST::Destroy();
		Add(0, new UI_REGION_ELEMENT(screenID, 0, 0, columns - 1, lines - 1));
		return;
	}

	// Throw away regions with a negative width or height.
	if (region.right < region.left || region.bottom < region.top)
		return;
	
	// Throw away regions that are completely off the screen.
	if (region.left >= columns || region.right < 0 ||
		region.top >= lines || region.bottom < 0)
		return;

	// Clip regions partially off the screen to fit on the screen.
	if (region.left < 0)
		region.left = 0;
	if (region.right >= columns)
		region.right = columns - 1;
	if (region.top < 0)
		region.top = 0;
	if (region.bottom >= lines)
		region.bottom = lines - 1;

	// Split any overlapping regions.
	Split(screenID, region, FALSE);

	// Define the new display region.
	UI_REGION_LIST::Add(0, new UI_REGION_ELEMENT(screenID, region));
}

void UI_WCC_DISPLAY::RegionMove(const UI_REGION &oldRegion,
	int newColumn, int newLine, SCREENID oldScreenID, SCREENID)
{
	VirtualGet(oldScreenID, Min(oldRegion.left, newColumn), Min(oldRegion.top, newLine),
		Max(oldRegion.right, newColumn + (oldRegion.right - oldRegion.left)),
		Max(oldRegion.bottom, newLine + (oldRegion.bottom - oldRegion.top)));

	char *buffer = NULL;
	_setviewport(0, 0, columns - 1, lines - 1);
	long size = _imagesize(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.bottom);
	buffer = (char *)malloc(size);
	if (buffer)
	{
		_getimage(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.bottom, buffer);
		_putimage(newColumn, newLine, buffer, _GPSET);
	}
	else if (oldRegion.top < newLine)
	{
		size = _imagesize(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.top);
		buffer = (char *)malloc(size);
		if (!buffer)
			return;
		int height = oldRegion.bottom - oldRegion.top;
		for (int i = height; i >= 0; i--)
			if (oldRegion.top + i > 0 && oldRegion.top + i < lines)
			{
				_getimage(oldRegion.left, oldRegion.top + i, oldRegion.right, oldRegion.top + i, buffer);
				_putimage(newColumn, newLine + i, buffer, _GPSET);
			}
	}
	else
	{
		size = _imagesize(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.top);
		buffer = (char *)malloc(size);
		if (!buffer)
			return;
		int height = oldRegion.bottom - oldRegion.top;
		for (int i = 0; i <= height; i++)
			if (oldRegion.top + i > 0 && oldRegion.top + i < lines)
			{
				_getimage(oldRegion.left, oldRegion.top + i, oldRegion.right, oldRegion.top + i, buffer);
				_putimage(newColumn, newLine + i, buffer, _GPSET);
			}
	}
	free(buffer);

	VirtualPut(oldScreenID);
}

void UI_WCC_DISPLAY::Text(SCREENID screenID, int left, int top,
	const char *text, const UI_PALETTE *palette, int length, int fill,
	int xor, const UI_REGION *clipRegion, LOGICAL_FONT logicalFont)
{
	// Make sure we have a valid string.
	if (!text || !text[0] || !palette)
		return;

	// Set up the fill line.
	static char fillLine[256];
	if (length < 0)
		length = ui_strlen(text);
	else if (length > 255)
		length = 255;
	strncpy(fillLine, text, length);
	fillLine[length] = '\0';

	// Determine the hot-key values and shrink the string to remove '&'.
	char *hotKey = NULL;
	int hotKeyLeft, hotKeyRight;
	if (!FlagSet(logicalFont, IGNORE_UNDERSCORE))
	{
		for (char *hotChar = strchr(fillLine, '&'); hotChar;
			hotChar = strchr(++hotChar, '&'))
		{
			if (hotChar[1] == '&')
				memcpy(hotChar, hotChar + 1, ui_strlen(hotChar));
			else
			{
				*hotChar = '\0';
				hotKeyLeft = left + TextWidth(fillLine, screenID, logicalFont | IGNORE_UNDERSCORE);
				strcpy(hotChar, hotChar + 1);
				hotKey = hotChar;
				extern char _dialogSize[];
				hotKeyRight = hotKeyLeft + _dialogSize[*hotKey] - 1;
			}
		}
	}

	// Check for special characters.
	for (char *string = strchr(fillLine, '\t'); string; string = strchr(string, '\t'))
		*string = ' ';
	for (string = strchr(fillLine, '\r'); string; string = strchr(string, '\r'))
		*string = ' ';
	for (string = strchr(fillLine, '\n'); string; string = strchr(string, '\n'))
		*string = ' ';

	// Assign the rectangle to the region structure.
	SetFont(logicalFont);
	UI_REGION region, tRegion;
	int width = TextWidth(fillLine, screenID, logicalFont | IGNORE_UNDERSCORE);
	int height = TextHeight(fillLine, screenID, logicalFont);
	int right = left + width;
	int bottom = top + height;
	if (!RegionInitialize(region, clipRegion, left, top, right, bottom))
		return;

	// Draw the rectangle on the display.
	int changedScreen = FALSE;
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			if (!changedScreen)
			{
				changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
				SetPattern(palette, xor);
			}
			_setviewport(tRegion.left, tRegion.top, tRegion.right, tRegion.bottom);
			if (fill && _fillPattern != PTN_SOLID_FILL)
			{
				_setcolor(_foregroundColor);
				_setfillmask(NULL);
				_rectangle(_GFILLINTERIOR, region.left - tRegion.left, region.top - tRegion.top,
					region.right - tRegion.left, region.bottom - tRegion.top);

				_setcolor(_backgroundColor);
				_setfillmask(patternTable[_fillPattern]);
				_rectangle(_GFILLINTERIOR, region.left - tRegion.left, region.top - tRegion.top,
					region.right - tRegion.left, region.bottom - tRegion.top);
			}
			else if (fill)
			{
				_setcolor(_backgroundColor);
				_setfillmask(NULL);
				_rectangle(_GFILLINTERIOR, region.left - tRegion.left, region.top - tRegion.top,
					region.right - tRegion.left, region.bottom - tRegion.top);
			}
			if (!fill || _backgroundColor != _foregroundColor)
			{
				_setcolor(_foregroundColor);
				_moveto(left - tRegion.left, top - tRegion.top);
#if !defined(OEM)
				for (int i=0; fillLine[i]; i++)
					fillLine[i] = FONTOUT(fillLine[i]);
				fillLine[i] = '\0';
#endif

				_outgtext(fillLine);
			}
			if (hotKey)
			{
				_moveto(hotKeyLeft - tRegion.left, bottom - tRegion.top);
				_lineto(hotKeyRight - tRegion.left, bottom - tRegion.top);
			}
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

void UI_WCC_DISPLAY::SetFont(LOGICAL_FONT logicalFont)
{
	static int _logicalFont = -1;
	static WCCFONT _fontInfo;
	WCCFONT fontInfo;
	char fontString[32];

	// Check for invalid or new font set.
	if (_logicalFont == -1 || logicalFont == -1)
	{
		logicalFont = FNT_DIALOG_FONT;
		fontInfo = fontTable[FNT_DIALOG_FONT];
		::sprintf(fontString, "t'%s'%s", fontInfo.typeFace, fontInfo.options);
		_setfont(fontString);

		// Set up the size tables.
		char text[2];
		text[1] = '\0';
		extern char _dialogSize[];
		for (int i = 0; i < 256; i++)
		{
			text[0] = FONTOUT(i);
			_dialogSize[i] = _getgtextextent(text);
		}
		_logicalFont = logicalFont;
		_fontInfo = fontInfo;
	}
	else
	{
		logicalFont &= 0x0FFF;
		if (logicalFont == _logicalFont)
			return;
	}

	// Update the font information.
	fontInfo = fontTable[logicalFont];
	if (fontInfo.typeFace != _fontInfo.typeFace ||
		fontInfo.options != _fontInfo.options)
	{
		::sprintf(fontString, "t'%s'%s", fontInfo.typeFace, fontInfo.options);
		_setfont(fontString);
	}
	_fontInfo = fontInfo;
	_logicalFont = logicalFont;
}

void UI_WCC_DISPLAY::SetPattern(const UI_PALETTE *palette, int xor)
{
	static int _xor = -1;

	if (palette)
	{
		_fillPattern = palette->fillPattern;
		_backgroundColor = MapColor(palette, FALSE);
		_foregroundColor = MapColor(palette, TRUE);
	}

	if (xor != _xor)
	{
		_setwritemode(xor ? _GXOR : _GPSET);
		_xor = xor;
	}
}

int UI_WCC_DISPLAY::TextHeight(const char *string, SCREENID screenID,
	LOGICAL_FONT logicalFont)
{
	logicalFont &= 0x0FFF;
	SetFont(logicalFont);
	struct _fontinfo fontInfo;
	_getfontinfo(&fontInfo);
	return (fontInfo.pixheight);
}

int UI_WCC_DISPLAY::TextWidth(const char *string, SCREENID screenID,
	LOGICAL_FONT logicalFont)
{
	if (!string || !(*string))
		return (0);
	SetFont(logicalFont & 0x0FFF);
#if defined(OEM)
	int length = _getgtextextent(string);
#else
	char trString[256];
	for (int i = 0; string[i]; i++)
		trString[i] = FONTOUT(string[i]);
	trString[i] = 0;
	int length = _getgtextextent(trString);
#endif

	if (!FlagSet(logicalFont, IGNORE_UNDERSCORE))
	{
		for (char *hotChar = strchr(string, '&'); hotChar;
			hotChar = strchr(++hotChar, '&'))
		{
			length -= _getgtextextent("&");
			if (hotChar[1] == '&')
				++hotChar;
		}
	}
	return (length);
}

int UI_WCC_DISPLAY::VirtualGet(SCREENID screenID, int left, int top,
	int right, int bottom)
{
	if (--_virtualCount == -1)
	{
 		_stopDevice = TRUE;
		_virtualRegion.left = Max(0, left);
		_virtualRegion.top = Max(0, top);
		_virtualRegion.right = Min(columns - 1, right);
		_virtualRegion.bottom = Min(lines - 1, bottom);
		SetPattern(NULL, FALSE);
		for (int i = MAX_DISPLAY_IMAGES - 1; i >= 0; i--)
			if (_virtualRegion.Overlap(displayImage[i].region) && 
				displayImage[i].image && displayImage[i].screen)
			{
				int x, y;
				UI_DISPLAY_IMAGE *view = &displayImage[i];
				UCHAR *screen = view->screen;
				_setviewport(0, 0, columns - 1, lines - 1);
				for (y = view->region.top; y <= view->region.bottom; y++)
					for (x = view->region.left; x <= view->region.right; x++, screen++)
						if (*screen != BACKGROUND)
						{
							_setcolor(*screen);
							_setpixel(x, y);
						}
			}
	}
	return (TRUE);
}

int UI_WCC_DISPLAY::VirtualPut(SCREENID screenID)
{
	if (++_virtualCount == 0)
	{
		SetPattern(NULL, FALSE);
		for (int i = 0; i < MAX_DISPLAY_IMAGES; i++)
			if (_virtualRegion.Overlap(displayImage[i].region) &&
				displayImage[i].image && displayImage[i].screen)
			{
				int x, y;
				UI_DISPLAY_IMAGE *view = &displayImage[i];
				UCHAR *screen = view->screen;
				UCHAR *image = view->image;
				_setviewport(0, 0, columns - 1, lines - 1);
				for (y = view->region.top; y <= view->region.bottom; y++)
					for (x = view->region.left; x <= view->region.right; x++, screen++, image++)
						if (*image != BACKGROUND)
						{
							*screen = _getpixel(x, y);
							_setcolor(*image);
							_setpixel(x, y);
						}
			}
		_stopDevice = FALSE;
	}
	else if (_virtualCount > 0)						// BUG.1674
		_virtualCount = 0;
	return (TRUE);
}

void UI_WCC_DISPLAY::BitmapArrayToHandle(SCREENID , int ,
	int , const UCHAR *, const UI_PALETTE *,
	HBITMAP *, HBITMAP *)
{
}

void UI_WCC_DISPLAY::BitmapHandleToArray(SCREENID , HBITMAP ,
		HBITMAP , int *, int *,
		UCHAR **)
{
}

void UI_WCC_DISPLAY::IconArrayToHandle(SCREENID , int ,
	int , const UCHAR *, const UI_PALETTE *,
	HICON *)
{
}

void UI_WCC_DISPLAY::IconHandleToArray(SCREENID , HICON ,
	int *, int *, UCHAR **)
{
}

