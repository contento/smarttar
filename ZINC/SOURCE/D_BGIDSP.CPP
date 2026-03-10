//	Zinc Interface Library - BGIDSP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <alloc.h>
#include <string.h>
#include <graphics.h>
#include "ui_dsp.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

extern "C"
{
	void SmallFont(void);
	void DialogFont(void);
	void SystemFont(void);
}

// ----- UI_BGI_DISPLAY -----------------------------------------------------

UI_BGI_DISPLAY::UI_BGI_DISPLAY(int driver, int mode) :
	UI_DISPLAY(FALSE, "DOS", "BGI")
{
#if defined(ZIL_LINKBUG)
	extern void z_bgidsp_dummy(void);		// Bug fix for Zortech & Microsoft linkers.
	z_bgidsp_dummy();
#endif

	// Initialize the basic member variables
	_virtualCount = 0;
	_stopDevice = FALSE;

	// Register the system, dialog, and small fonts that were linked in.
	BGIFONT BGIFont = {0, 0, 1, 1, 1, 1, 0, 0 };
	BGIFont.font = registerfarbgifont(SmallFont);
	if (BGIFont.font >= 0)
	{
		BGIFont.charSize = 0;
		BGIFont.maxWidth = 10;
		BGIFont.maxHeight = 11;
		UI_BGI_DISPLAY::fontTable[FNT_SMALL_FONT] = BGIFont;
	}
	BGIFont.font = registerfarbgifont(DialogFont);
	if (BGIFont.font >= 0)
	{
		BGIFont.charSize = 0;
		BGIFont.maxWidth = 11;
		BGIFont.maxHeight = 11;
		UI_BGI_DISPLAY::fontTable[FNT_DIALOG_FONT] = BGIFont;
	}
	BGIFont.font = registerfarbgifont(SystemFont);
	if (BGIFont.font >= 0)
	{
		BGIFont.charSize = 0;
		BGIFont.maxWidth = 11;
		BGIFont.maxHeight = 13;
		UI_BGI_DISPLAY::fontTable[FNT_SYSTEM_FONT] = BGIFont;
	}

	// Find the type of display and initialize the driver.
	if (driver == DETECT)
		detectgraph(&driver, &mode);
	int tDriver, tMode;

	// Use temporary path if not installed in main().
	int pathInstalled = searchPath ? TRUE : FALSE;
	if (!pathInstalled)
		searchPath = new UI_PATH;
	const char *pathName = searchPath->FirstPathName();
	do
	{
		tDriver = driver;
		tMode = mode;
		initgraph(&tDriver, &tMode, pathName);
		pathName = searchPath->NextPathName();
	} while (tDriver == -3 && pathName);
	if (tDriver < 0)
		return;
	driver = tDriver;
	mode = tMode;

	// Delete path if it was installed temporarily.
	if (!pathInstalled)
	{
		delete searchPath;
		searchPath = NULL;
	}

	columns = getmaxx() + 1;
	lines = getmaxy() + 1;
	maxColors = getmaxcolor() + 1;
	isMono = (maxColors == 2);

	// Reset the font information.
	SetFont(-1);

	// Fill the screen according to the specified palette.
	cellWidth = (fontTable[FNT_DIALOG_FONT].font == DEFAULT_FONT) ?
		TextWidth("M", ID_SCREEN, FNT_DIALOG_FONT) : 	// Bitmap font.
		TextWidth("M", ID_SCREEN, FNT_DIALOG_FONT) - 2; // Stroked font.
	cellHeight = TextHeight(NULL, ID_SCREEN, FNT_DIALOG_FONT) +
		preSpace + postSpace + 4 + 4;  // 4 above the text, 4 below the text.
	SetPattern(backgroundPalette, FALSE);
	setviewport(0, 0, columns - 1, lines - 1, TRUE);
	bar(0, 0, columns - 1, lines - 1);

	// Define the screen display region.
	Add(NULL, new UI_REGION_ELEMENT(ID_SCREEN, 0, 0, columns - 1, lines - 1));
	installed = TRUE;
}

UI_BGI_DISPLAY::~UI_BGI_DISPLAY(void)
{
	// Restore the display.
	if (installed)
		closegraph();
}

void UI_BGI_DISPLAY::Bitmap(SCREENID screenID, int column, int line,
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
			setviewport(0, 0, columns - 1, lines - 1, TRUE);
			for (int y = tRegion.top; y <= tRegion.bottom; y++)
			{
				const UCHAR *pixel = bitmapArray + (y - line) * bitmapWidth + tRegion.left - column;
				for (int x = tRegion.left; x <= tRegion.right; x++, pixel++)
					if (*pixel != BACKGROUND)
						putpixel(x, y, MapColor(palette+*pixel, TRUE));
			}
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

void UI_BGI_DISPLAY::Ellipse(SCREENID screenID, int x, int y,
	int startAngle, int endAngle, int xRadius, int yRadius,
	const UI_PALETTE *palette, int fill, int xor, const UI_REGION *clipRegion)
{
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
			setviewport(tRegion.left, tRegion.top, tRegion.right, tRegion.bottom, TRUE);
			if (fill && startAngle == 0 && endAngle == 360)
				fillellipse(x - tRegion.left, y - tRegion.top, xRadius, yRadius);
			else if (fill)
				sector(x - tRegion.left, y - tRegion.top, startAngle, endAngle, xRadius, yRadius);
			else
				ellipse(x - tRegion.left, y - tRegion.top, startAngle, endAngle, xRadius, yRadius);
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

int UI_BGI_DISPLAY::DeviceMove(IMAGE_TYPE imageType, int newColumn, int newLine)
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
//	if (newColumn < 0 || newLine < 0)				// BUG.1066, BUG.1525
//		newColumn = newLine = width = height = 0;
//	else
//	{
	int deltaX = newColumn - view->region.left;
	int deltaY = newLine - view->region.top;

	UI_REGION newRegion;
	newRegion.left = view->region.left + deltaX;
	newRegion.right = view->region.right + deltaX;
	newRegion.top = view->region.top + deltaY;
	newRegion.bottom = view->region.bottom + deltaY;

	for (int i = 0; i < MAX_DISPLAY_IMAGES && !overlap; i++)
		overlap = (i != imageType &&
			   (view->region.Overlap(displayImage[i].region) ||
			    newRegion.Overlap(displayImage[i].region)));

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
		setviewport(0, 0, columns - 1, lines - 1, TRUE);
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
					*backup = getpixel(newX, newY);
				if (*image != BACKGROUND)
					putpixel(newX, newY, *image);
			}
		}
		screen = view->screen;
		for (y = view->region.top; y <= view->region.bottom; y++)
			for (x = view->region.left; x <= view->region.right; x++, screen++)
				if (*screen != BACKGROUND)
					putpixel(x, y, *screen);
		memcpy(view->screen, view->backup, width * height);
		view->region.left = newColumn;
		view->region.top = newLine;
		view->region.right = newColumn + width - 1;
		view->region.bottom = newLine + height - 1;
	}

	_stopDevice = FALSE;
	return (TRUE);
}

int UI_BGI_DISPLAY::DeviceSet(IMAGE_TYPE imageType, int column, int line,
	int width, int height, UCHAR *image)
{
	if (_stopDevice)
		return (FALSE);
	_stopDevice = TRUE;

	UI_DISPLAY_IMAGE *view = &displayImage[imageType];
	UI_REGION virtualRegion;
	if (view->image && view->region.left < columns && view->region.top < lines)
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

void UI_BGI_DISPLAY::Line(SCREENID screenID, int x1, int y1,
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
				slope = x1 == x2 ? 1 : (y1 - y2) / (x1 - x2);
			}
			setviewport(tRegion.left, tRegion.top, tRegion.right, tRegion.bottom, TRUE);
			for (int i = 0; i < width; i++)
				if (slope)
					line(x1 - tRegion.left + i, y1 - tRegion.top,
						x2 - tRegion.left + i, y2 - tRegion.top);
				else
					line(x1 - tRegion.left, y1 - tRegion.top + i,
						x2 - tRegion.left, y2 - tRegion.top + i);
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

COLOR UI_BGI_DISPLAY::MapColor(const UI_PALETTE *palette, int foreground)
{
	// Match the color request based on the type of display.
	if (maxColors == 2)
		return (foreground ? palette->bwForeground : palette->bwBackground);
	else if (maxColors < 16)
		return (foreground ? palette->grayScaleForeground : palette->grayScaleBackground);
	return (foreground ? palette->colorForeground : palette->colorBackground);
}

void UI_BGI_DISPLAY::Polygon(SCREENID screenID, int numPoints,
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
			setviewport(tRegion.left, tRegion.top, tRegion.right, tRegion.bottom, TRUE);
			for (i = 0; i < numPoints * 2; i += 2)
			{
				tPolygon[i] = polygonPoints[i] - tRegion.left;
				tPolygon[i+1] = polygonPoints[i+1] - tRegion.top;
			}
			if (fill)
				fillpoly(numPoints, tPolygon);
			else
				drawpoly(numPoints, tPolygon);
			if (screenID == ID_DIRECT)
				break;
		}
	delete tPolygon;

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

void UI_BGI_DISPLAY::Rectangle(SCREENID screenID, int left, int top,
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
			setviewport(tRegion.left, tRegion.top, tRegion.right, tRegion.bottom, TRUE);
			if (fill && xor)			// Patch for Borland bar() xor bug.
			{
				for (int i = 0; i < tRegion.right - tRegion.left; i++)
					line(i, top - tRegion.top, i, bottom - tRegion.top);
			}
			else if (fill)
				bar(left - tRegion.left, top - tRegion.top, right - tRegion.left, bottom - tRegion.top);
			for (int i = 0; i < width; i++)
				rectangle(left - (tRegion.left - i), top - (tRegion.top - i),
					right - (tRegion.left + i), bottom - (tRegion.top + i));
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

void UI_BGI_DISPLAY::RectangleXORDiff(const UI_REGION &oldRegion,
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
	setviewport(0, 0, columns - 1, lines - 1, TRUE);
	rectangle(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.bottom);
	rectangle(newRegion.left, newRegion.top, newRegion.right, newRegion.bottom);

	// Update the screen.
	VirtualPut(ID_DIRECT);
}

void UI_BGI_DISPLAY::RegionDefine(SCREENID screenID, int left, int top,
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

void UI_BGI_DISPLAY::RegionMove(const UI_REGION &oldRegion,
	int newColumn, int newLine, SCREENID oldScreenID, SCREENID)
{
	VirtualGet(oldScreenID, Min(oldRegion.left, newColumn), Min(oldRegion.top, newLine),
		Max(oldRegion.right, newColumn + (oldRegion.right - oldRegion.left)),
		Max(oldRegion.bottom, newLine + (oldRegion.bottom - oldRegion.top)));

	void far *buffer = NULL;
	setviewport(0, 0, columns - 1, lines - 1, TRUE);
	unsigned size = imagesize(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.bottom);
	if (size && (size != 0xFFFF) && (buffer = new char[size]) != NULL)
	{
		getimage(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.bottom, buffer);
		putimage(newColumn, newLine, buffer, COPY_PUT);
	}
	else if (oldRegion.top < newLine)
	{
		size = imagesize(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.top);
		buffer = new char[size];
		int height = oldRegion.bottom - oldRegion.top;
		for (int i = height; i >= 0; i--)
			if (oldRegion.top + i > 0 && oldRegion.top + i < lines)
			{
				getimage(oldRegion.left, oldRegion.top + i, oldRegion.right, oldRegion.top + i, buffer);
				putimage(newColumn, newLine + i, buffer, COPY_PUT);
			}
	}
	else
	{
		size = imagesize(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.top);
		buffer = new char[size];
		int height = oldRegion.bottom - oldRegion.top;
		for (int i = 0; i <= height; i++)
			if (oldRegion.top + i > 0 && oldRegion.top + i < lines)
			{
				getimage(oldRegion.left, oldRegion.top + i, oldRegion.right, oldRegion.top + i, buffer);
				putimage(newColumn, newLine + i, buffer, COPY_PUT);
			}
	}
	delete buffer;

	VirtualPut(oldScreenID);
}

void UI_BGI_DISPLAY::Text(SCREENID screenID, int left, int top,
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
			setviewport(tRegion.left, tRegion.top, tRegion.right, tRegion.bottom, TRUE);
			if (fill)
				bar(0, 0, tRegion.right - tRegion.left, tRegion.bottom - tRegion.top);
			outtextxy(left - tRegion.left, top - tRegion.top, fillLine);
			if (hotKey)
				line(hotKeyLeft - tRegion.left, bottom - tRegion.top,
					hotKeyRight - tRegion.left, bottom - tRegion.top);
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

void UI_BGI_DISPLAY::SetFont(LOGICAL_FONT logicalFont)
{
	static int _logicalFont = -1;
	static BGIFONT _fontInfo;
	BGIFONT fontInfo;

	// Check for invalid or new font set.
	if (_logicalFont == -1 || logicalFont == -1)
	{
		logicalFont = FNT_DIALOG_FONT;
		fontInfo = fontTable[FNT_DIALOG_FONT];
		settextstyle(fontInfo.font, HORIZ_DIR, fontInfo.charSize);
		if (!fontInfo.charSize)
			setusercharsize(fontInfo.multX, fontInfo.divX, fontInfo.multY, fontInfo.divY);
		settextjustify(LEFT_TEXT, TOP_TEXT);

		// Set up the size tables.
		char text[2];
		text[1] = '\0';
		extern char _dialogSize[];
		for (int i = 0; i < 256; i++)
		{
			text[0] = i;
			_dialogSize[i] = textwidth(text);
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
	if (fontInfo.font != _fontInfo.font ||
		fontInfo.charSize != _fontInfo.charSize)
	{
		settextstyle(fontInfo.font, HORIZ_DIR, fontInfo.charSize);
		if (!fontInfo.charSize)
			setusercharsize(fontInfo.multX, fontInfo.divX, fontInfo.multY, fontInfo.divY);
	}
	else if (fontInfo.multX != _fontInfo.multX || fontInfo.divX != _fontInfo.divX ||
		fontInfo.multY != _fontInfo.multY || fontInfo.divY != _fontInfo.divY)
		setusercharsize(fontInfo.multX, fontInfo.divX, fontInfo.multY, fontInfo.divY);
	_fontInfo = fontInfo;
	_logicalFont = logicalFont;
}

void UI_BGI_DISPLAY::SetPattern(const UI_PALETTE *palette, int xor)
{
	static int _fillPattern = -1;
	static int _backgroundColor = -1;
	static int _foregroundColor = -1;
	static int _xor = -1;
	int fillPattern = palette->fillPattern;
	int backgroundColor = MapColor(palette, FALSE);
	int foregroundColor = MapColor(palette, TRUE);

	if (fillPattern != _fillPattern || backgroundColor != _backgroundColor)
	{
		if (maxColors == 2)
			setfillstyle(fillPattern, backgroundColor);
		else
			setfillpattern(patternTable[fillPattern], backgroundColor);
		_fillPattern = fillPattern;
		_backgroundColor = backgroundColor;
	}
	if (foregroundColor != _foregroundColor)
	{
		setcolor(foregroundColor);
		_foregroundColor = foregroundColor;
	}
	if (xor != _xor)
	{
		setwritemode(xor ? XOR_PUT : COPY_PUT);
		_xor = xor;
	}
}

int UI_BGI_DISPLAY::TextHeight(const char *string, SCREENID, LOGICAL_FONT logicalFont)
{
	logicalFont &= 0x0FFF;
	SetFont(logicalFont);
	if (fontTable[logicalFont].maxHeight)
		return (fontTable[logicalFont].maxHeight);
	else if (string && *string)
		return (textheight((char *)string));
	else
		return (textheight("Mq"));
}

int UI_BGI_DISPLAY::TextWidth(const char *string, SCREENID, LOGICAL_FONT logicalFont)
{
	if (!string || !(*string))
		return (0);
	SetFont(logicalFont & 0x0FFF);
	int length = textwidth((char *)string);

	char *tabPtr = strchr((char *)string, '\t');
	while (tabPtr)
	{
		length += textwidth(" ");
		tabPtr = strchr(tabPtr + 1, '\t');
	}

	if (!FlagSet(logicalFont, IGNORE_UNDERSCORE))
	{
		for (char *hotChar = strchr(string, '&'); hotChar;
			hotChar = strchr(++hotChar, '&'))
		{
			length -= textwidth("&");
			if (hotChar[1] == '&')
				++hotChar;
		}
	}
	return (length);
}

int UI_BGI_DISPLAY::VirtualGet(SCREENID, int left, int top, int right, int bottom)
{
	if (--_virtualCount == -1)
	{
 		_stopDevice = TRUE;
		_virtualRegion.left = Max(0, left);
		_virtualRegion.top = Max(0, top);
		_virtualRegion.right = Min(columns - 1, right);
		_virtualRegion.bottom = Min(lines - 1, bottom);
		for (int i = MAX_DISPLAY_IMAGES - 1; i >= 0; i--)
			if (_virtualRegion.Overlap(displayImage[i].region) && 
				displayImage[i].image && displayImage[i].screen)
			{
				int x, y;
				UI_DISPLAY_IMAGE *view = &displayImage[i];
				UCHAR *screen = view->screen;
				setviewport(0, 0, columns - 1, lines - 1, TRUE);
				for (y = view->region.top; y <= view->region.bottom; y++)
					for (x = view->region.left; x <= view->region.right; x++, screen++)
						if (*screen != BACKGROUND)
							putpixel(x, y, *screen);
			}
	}
	return (TRUE);
}

int UI_BGI_DISPLAY::VirtualPut(SCREENID)
{
	if (++_virtualCount == 0)
	{
		for (int i = 0; i < MAX_DISPLAY_IMAGES; i++)
			if (_virtualRegion.Overlap(displayImage[i].region) &&
				displayImage[i].image && displayImage[i].screen)
			{
				int x, y;
				UI_DISPLAY_IMAGE *view = &displayImage[i];
				UCHAR *screen = view->screen;
				UCHAR *image = view->image;
				setviewport(0, 0, columns - 1, lines - 1, TRUE);
				for (y = view->region.top; y <= view->region.bottom; y++)
					for (x = view->region.left; x <= view->region.right; x++, screen++, image++)
						if (*image != BACKGROUND)
						{
							*screen = getpixel(x, y);
							putpixel(x, y, *image);
						}
			}
		_stopDevice = FALSE;
	}
	else if (_virtualCount > 0)						// BUG.1674
		_virtualCount = 0;
	return (TRUE);
}

void UI_BGI_DISPLAY::BitmapArrayToHandle(SCREENID , int ,
	int , const UCHAR *, const UI_PALETTE *,
	HBITMAP *, HBITMAP *)
{
}

void UI_BGI_DISPLAY::BitmapHandleToArray(SCREENID , HBITMAP ,
		HBITMAP , int *, int *,
		UCHAR **)
{
}

void UI_BGI_DISPLAY::IconArrayToHandle(SCREENID , int ,
	int , const UCHAR *, const UI_PALETTE *,
	HICON *)
{
}

void UI_BGI_DISPLAY::IconHandleToArray(SCREENID , HICON ,
	int *, int *, UCHAR **)
{
}

