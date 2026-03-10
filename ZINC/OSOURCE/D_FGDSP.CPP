//	Zinc Interface Library - D_FGDSP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <string.h>
#include <fg.h>
#include "ui_dsp.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

#if defined(OEM)
#	define FONTOUT
#else
extern UCHAR mapISOto850(UCHAR val);
#	define FONTOUT	mapISOto850
#endif

// ----- UI_FG_DISPLAY -----------------------------------------------------
UI_FG_DISPLAY::UI_FG_DISPLAY(int mode) :
	UI_DISPLAY(FALSE, "DOS", "FG")
{
	// Find the type of display.
	if (fg_init() == FG_NULL)
		return;

	// Initialize the basic member variables
	_virtualCount = 0;
	_stopDevice = FALSE;
	_fillPattern = -1;
	_backgroundColor = -1;
	_foregroundColor = -1;
	_xor = -1;

// The following checks for SVGA screen and sets it to VGA so that the mouse
// can work properly (uncomment these lines for this option).
//	if (fg_get_type() >= 0x13)
//	{
//		fg_term();
//		fg_init_vga12();
//	}

	columns = fg.displaybox[FG_X2] + 1;
	lines = fg.displaybox[FG_Y2] + 1;
	maxColors = fg.nsimulcolor;
	isMono = (maxColors == 2);

	// Fix for reverse text/graphics order.
	int saveColor = FG_YELLOW;
	FG_YELLOW = FG_BROWN;
	FG_BROWN = saveColor;

	extern void z_fgdsp_dummy(void);
	z_fgdsp_dummy();

	// Reset the font information.
	SetFont(-1);

	// Fill the screen according to the specified palette.
	cellWidth = TextWidth("M", ID_SCREEN, FNT_DIALOG_FONT);
	cellHeight = TextHeight(NULL, ID_SCREEN, FNT_DIALOG_FONT) +
		preSpace + postSpace + 2 + 2;  // 2 above the text, 2 below the text.
	SetPattern(backgroundPalette, FALSE);
	if (_fillPattern != PTN_SOLID_FILL)
	{
		fg_fillbox(fg.color[_foregroundColor], _xor, ~0, fg.displaybox);
		fg_fillboxpattern(fg.color[_backgroundColor], _xor, ~0, fg.displaybox, FG_FILL_USER_DEFINED);
	}
	else
		fg_fillbox(fg.color[_foregroundColor], _xor, ~0, fg.displaybox);
	fg_flush();

	// Define the screen display region.
	Add(NULL, new UI_REGION_ELEMENT(ID_SCREEN, 0, 0, columns - 1, lines - 1));
	installed = TRUE;
}

UI_FG_DISPLAY::~UI_FG_DISPLAY(void)
{
	// Restore the display.
	if (installed)
		fg_term();
}

void UI_FG_DISPLAY::Bitmap(SCREENID screenID, int column, int line,
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
			for (int y = tRegion.top; y <= tRegion.bottom; y++)
			{
				const UCHAR *pixel = bitmapArray + (y - line) * bitmapWidth + tRegion.left - column;
				for (int x = tRegion.left; x <= tRegion.right; x++, pixel++)
					if (*pixel != BACKGROUND && x >= 0)
						fg_drawdot(fg.color[MapColor(palette+*pixel, TRUE)], FG_MODE_SET, ~0, x, lines - y - 1);
			}
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

void UI_FG_DISPLAY::Ellipse(SCREENID screenID, int x, int y,
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
			fg_box_t clipBox;
			clipBox[FG_X1] = tRegion.left;
			clipBox[FG_X2] = tRegion.right;
			clipBox[FG_Y1] = lines - tRegion.bottom - 1;
			clipBox[FG_Y2] = lines - tRegion.top - 1;

			if (fill && xRadius > yRadius)
			{
				int shrinkRadius = xRadius;
				while (shrinkRadius > 0)
				{
					shrinkRadius--;
					fg_drawellipse(fg.color[_backgroundColor], _xor, ~0, x, lines - y - 1,
						shrinkRadius, yRadius, startAngle * 10, endAngle * 10,
						(fg_const_pbox_t)&clipBox);
				}
			}
			else if (fill)
			{
				int shrinkRadius = yRadius;
				while (shrinkRadius > 0)
				{
					shrinkRadius--;
					fg_drawellipse(fg.color[_backgroundColor], _xor, ~0, x, lines - y - 1,
						xRadius, shrinkRadius, startAngle * 10, endAngle * 10,
						(fg_const_pbox_t)&clipBox);
				}
			}
			fg_drawellipse(fg.color[_foregroundColor], _xor, ~0, x, lines - y - 1,
				xRadius, yRadius, startAngle * 10, endAngle * 10,
				(fg_const_pbox_t)&clipBox);
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

int UI_FG_DISPLAY::DeviceMove(IMAGE_TYPE imageType, int newColumn, int newLine)
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

//	if (newColumn < 0 || newLine < 0)				// BUG.1066, BUG.1526
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

	_stopDevice = FALSE;
	return (TRUE);
}

int UI_FG_DISPLAY::DeviceSet(IMAGE_TYPE imageType, int column, int line,
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
		}
		view->screen = new UCHAR[width * height];
	}

	VirtualPut(0);
	_stopDevice = FALSE;
	return (TRUE);
}

void UI_FG_DISPLAY::Line(SCREENID screenID, int x1, int y1,
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
			fg_box_t clipBox;
			clipBox[FG_X1] = tRegion.left;
			clipBox[FG_X2] = tRegion.right;
			clipBox[FG_Y1] = lines - tRegion.bottom - 1;
			clipBox[FG_Y2] = lines - tRegion.top - 1;

			for (int i = 0; i < width; i++)
				if (slope)
				{
					fg_line_t line;
					line[FG_X1] = x1 + i;
					line[FG_X2] = x2 + i;
					line[FG_Y1] = lines - y1 - 1;
					line[FG_Y2] = lines - y2 - 1;

					fg_drawlineclip(fg.color[_foregroundColor], _xor, ~0, FG_LINE_SOLID,
						(fg_const_pline_t)&line, (fg_const_pbox_t)&clipBox);
				}
				else
				{
					fg_line_t line;
					line[FG_X1] = x1;
					line[FG_X2] = x2;
					line[FG_Y1] = lines - y1 - 1 + i;
					line[FG_Y2] = lines - y2 - 1 + i;

					fg_drawlineclip(fg.color[_foregroundColor], _xor, ~0, FG_LINE_SOLID,
						(fg_const_pline_t)&line, (fg_const_pbox_t)&clipBox);
				}
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

COLOR UI_FG_DISPLAY::MapColor(const UI_PALETTE *palette, int foreground)
{
	// Match the color request based on the type of display.
	if (maxColors == 2)
	{
		int color = foreground ? palette->bwForeground : palette->bwBackground;
		return ((color == BW_BLACK) ? BLACK : WHITE);
	}
	else if (maxColors < 16)
		return (foreground ? palette->grayScaleForeground : palette->grayScaleBackground);
	return (foreground ? palette->colorForeground : palette->colorBackground);
}

void UI_FG_DISPLAY::Polygon(SCREENID screenID, int numPoints,
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
	fg_coord_t *tPolygon = new fg_coord_t[numPoints * 2];
	for (i = 0; i < numPoints * 2; i += 2)
	{
		tPolygon[i] = polygonPoints[i];
		tPolygon[i+1] = lines - polygonPoints[i+1] - 1;
	}
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
			fg_box_t clipBox;
			clipBox[FG_X1] = tRegion.left;
			clipBox[FG_X2] = tRegion.right;
			clipBox[FG_Y1] = lines - tRegion.bottom - 1;
			clipBox[FG_Y2] = lines - tRegion.top - 1;

			if (fill && _fillPattern != PTN_SOLID_FILL)
			{
				fg_fillpolygon(fg.color[_foregroundColor], _xor, ~0, numPoints - 1,
					tPolygon, clipBox);
				fg_fillpolygonpattern(fg.color[_backgroundColor], _xor, ~0, numPoints - 1,
					tPolygon, clipBox, FG_FILL_USER_DEFINED);
			}
			else if (fill)
				fg_fillpolygon(fg.color[_backgroundColor], _xor, ~0, numPoints - 1,
					tPolygon, clipBox);
			if (!fill || _backgroundColor != _foregroundColor)
				fg_drawpolygon(fg.color[_foregroundColor], _xor, ~0, FG_LINE_SOLID,
					numPoints - 1, tPolygon, clipBox);
			if (screenID == ID_DIRECT)
				break;
		}
	delete tPolygon;

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

void UI_FG_DISPLAY::Rectangle(SCREENID screenID, int left, int top,
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
			fg_box_t clipBox;
			clipBox[FG_X1] = tRegion.left;
			clipBox[FG_X2] = tRegion.right;
			clipBox[FG_Y1] = lines - tRegion.bottom - 1;
			clipBox[FG_Y2] = lines - tRegion.top - 1;
			if (fill && _fillPattern != PTN_SOLID_FILL)
			{
				fg_fillbox(fg.color[_foregroundColor], _xor, ~0, (fg_const_pbox_t)&clipBox);
				fg_fillboxpattern(fg.color[_backgroundColor], _xor, ~0, (fg_const_pbox_t)&clipBox, FG_FILL_USER_DEFINED);
			}
			else if (fill)
				fg_fillbox(fg.color[_backgroundColor], _xor, ~0, (fg_const_pbox_t)&clipBox);
			if (!fill || _backgroundColor != _foregroundColor)
			{
				fg_box_t box;
				box[FG_X1] = left;
				box[FG_X2] = right;
				box[FG_Y1] = lines - bottom - 1;
				box[FG_Y2] = lines - top - 1;
				for (int w = 0; w < width; w++)
				{
					fg_drawbox(fg.color[_foregroundColor], _xor, ~0, FG_LINE_SOLID,
						(fg_const_pbox_t)&box, (fg_const_pbox_t)&clipBox);
					box[FG_X1] += 1;
					box[FG_X2] -= 1;
					box[FG_Y1] += 1;
					box[FG_Y2] -= 1;
				}
			}
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

void UI_FG_DISPLAY::RectangleXORDiff(const UI_REGION &oldRegion,
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
	fg_box_t box;
	box[FG_X1] = oldRegion.left;
	box[FG_X2] = oldRegion.right;
	box[FG_Y1] = lines - oldRegion.bottom - 1;
	box[FG_Y2] = lines - oldRegion.top - 1;
	fg_drawbox(fg.color[_foregroundColor], FG_MODE_XOR, ~0, FG_LINE_SOLID,
		(fg_const_pbox_t)&box, fg.displaybox);

	box[FG_X1] = newRegion.left;
	box[FG_X2] = newRegion.right;
	box[FG_Y1] = lines - newRegion.bottom - 1;
	box[FG_Y2] = lines - newRegion.top - 1;
	fg_drawbox(fg.color[_foregroundColor], FG_MODE_XOR, ~0, FG_LINE_SOLID,
		(fg_const_pbox_t)&box, fg.displaybox);

	// Update the screen.
	VirtualPut(ID_DIRECT);
}

void UI_FG_DISPLAY::RegionDefine(SCREENID screenID, int left, int top,
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

void UI_FG_DISPLAY::RegionMove(const UI_REGION &oldRegion,
	int newColumn, int newLine, SCREENID oldScreenID, SCREENID)
{
	VirtualGet(oldScreenID, Min(oldRegion.left, newColumn), Min(oldRegion.top, newLine),
		Max(oldRegion.right, newColumn + (oldRegion.right - oldRegion.left)),
		Max(oldRegion.bottom, newLine + (oldRegion.bottom - oldRegion.top)));

	fg_box_t box;
	box[FG_X1] = oldRegion.left;
	box[FG_X2] = oldRegion.right;
	box[FG_Y1] = lines - oldRegion.bottom - 1;
	box[FG_Y2] = lines - oldRegion.top - 1;
	fg_blit(box, newColumn, lines - (newLine + oldRegion.bottom - oldRegion.top) - 1, 0, 0);

	VirtualPut(oldScreenID);
}

void UI_FG_DISPLAY::Text(SCREENID screenID, int left, int top,
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
#if defined(OEM)
	strncpy(fillLine, text, length);
	fillLine[length] = '\0';
#else
	for (int i=0; text[i] && i < length; i++)
		fillLine[i] = mapISOto850(text[i]);
	fillLine[i] = '\0';
#endif

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
	{
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
			fg_box_t clipBox;
			clipBox[FG_X1] = tRegion.left;
			clipBox[FG_X2] = tRegion.right;
			clipBox[FG_Y1] = lines - tRegion.bottom - 1;
			clipBox[FG_Y2] = lines - tRegion.top - 1;
			if (fill && _fillPattern != PTN_SOLID_FILL)
			{
				fg_fillbox(fg.color[_foregroundColor], _xor, ~0, (fg_const_pbox_t)&clipBox);
				fg_fillboxpattern(fg.color[_backgroundColor], _xor, ~0, (fg_const_pbox_t)&clipBox, FG_FILL_USER_DEFINED);
			}
			else if (fill)
				fg_fillbox(fg.color[_backgroundColor], _xor, ~0, (fg_const_pbox_t)&clipBox);
			fg_puts(fg.color[_foregroundColor], _xor, ~0, FG_ROT0, left,
				lines - bottom - 2, fillLine, (fg_const_pbox_t)&clipBox);
			if (hotKey)
			{
				fg_line_t line;
				line[FG_X1] = hotKeyLeft;
				line[FG_X2] = hotKeyRight;
				line[FG_Y1] = lines - bottom - 1;
				line[FG_Y2] = lines - bottom - 1;

				fg_drawlineclip(fg.color[_foregroundColor], _xor, ~0, FG_LINE_SOLID,
					(fg_const_pline_t)&line, (fg_const_pbox_t)&clipBox);
			}
			if (screenID == ID_DIRECT)
				break;
		}
	}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

void UI_FG_DISPLAY::SetFont(LOGICAL_FONT logicalFont)
{
	static int _logicalFont = -1;
	static FGFONT _fontInfo;
	FGFONT fontInfo;

	// Check for invalid or new font set.
	if (_logicalFont == -1 || logicalFont == -1)
	{
		logicalFont = FNT_DIALOG_FONT;
		fontInfo = fontTable[FNT_DIALOG_FONT];
		if (!fontInfo.fontptr)
			return;
		fg_font_t font;
		font.fontptr = fontInfo.fontptr;
		font.fontptr2 = fontInfo.fontptr2;
		font.charbox[FG_X1] = 0;
		font.charbox[FG_Y1] = 0;
		font.charbox[FG_X2] = fontInfo.maxWidth - 1;
		font.charbox[FG_Y2] = fontInfo.maxHeight - 1;
		fg_set_font(&font);

		// Set up the size tables.
		extern char _dialogSize[];
		memset(_dialogSize, fontInfo.maxWidth, 256);
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
	if (!fontInfo.fontptr)
		return;
	if (fontInfo.fontptr != _fontInfo.fontptr ||
		fontInfo.fontptr2 != _fontInfo.fontptr2)
	{
		fg_font_t font;
		font.fontptr = fontInfo.fontptr;
		font.fontptr2 = fontInfo.fontptr2;
		font.charbox[FG_X1] = 0;
		font.charbox[FG_Y1] = 0;
		font.charbox[FG_X2] = fontInfo.maxWidth - 1;
		font.charbox[FG_Y2] = fontInfo.maxHeight - 1;
		fg_set_font(&font);
	}
	_fontInfo = fontInfo;
	_logicalFont = logicalFont;
}

void UI_FG_DISPLAY::SetPattern(const UI_PALETTE *palette, int xor)
{
	int fillPattern = palette->fillPattern;

	if (fillPattern != _fillPattern)
	{
		if (fillPattern != PTN_SOLID_FILL)
			fg_setfillpattern(FG_FILL_USER_DEFINED, patternTable[fillPattern]);
		_fillPattern = fillPattern;
	}
	_backgroundColor = MapColor(palette, FALSE);
	_foregroundColor = MapColor(palette, TRUE);
	_xor = xor;
}

int UI_FG_DISPLAY::TextHeight(const char *string, SCREENID, LOGICAL_FONT logicalFont)
{
	logicalFont &= 0x0FFF;
	SetFont(logicalFont);
	return (fontTable[logicalFont].maxHeight - 3);
}

int UI_FG_DISPLAY::TextWidth(const char *string, SCREENID, LOGICAL_FONT logicalFont)
{
	if (!string || !(*string))
		return (0);
	SetFont(logicalFont & 0x0FFF);
	int length = ui_strlen(string) * fontTable[logicalFont & 0x0FFF].maxWidth;

	if (!FlagSet(logicalFont, IGNORE_UNDERSCORE))
	{
		for (char *hotChar = strchr(string, '&'); hotChar;
			hotChar = strchr(++hotChar, '&'))
		{
			length -= fontTable[logicalFont & 0x0FFF].maxWidth;
			if (hotChar[1] == '&')
				++hotChar;
		}
	}
	return (length);
}

int UI_FG_DISPLAY::VirtualGet(SCREENID, int left, int top, int right, int bottom)
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

				fg_color_t *screen = (fg_color_t *)view->screen;
				fg_box_t box;
				box[FG_X1] = Max(view->region.left, 0);
				box[FG_X2] = Min(view->region.right, columns - 1);
				box[FG_Y1] = Max(lines - view->region.bottom - 1, 0);
				box[FG_Y2] = Min(lines - view->region.top - 1, lines - 1);
				fg_writebox(box, screen);
/* START BLOCK COMMENT
**					UCHAR *screen = view->screen;
**					for (y = view->region.top; y <= view->region.bottom; y++)
**						for (x = view->region.left; x <= view->region.right; x++, screen++)
**							if (*screen != BACKGROUND)
**								fg_drawdot(*screen, FG_MODE_SET, ~0, x, lines - y - 1);
END BLOCK COMMENT */
			}
	}
	return (TRUE);
}

int UI_FG_DISPLAY::VirtualPut(SCREENID)
{
	if (++_virtualCount == 0)
	{
		for (int i = 0; i < MAX_DISPLAY_IMAGES; i++)
			if (_virtualRegion.Overlap(displayImage[i].region) &&
				displayImage[i].image && displayImage[i].screen)
			{
				int x, y;
				UI_DISPLAY_IMAGE *view = &displayImage[i];
				fg_color_t *screen = (fg_color_t *)view->screen;
/* START BLOCK COMMENT
**					fg_color_t *image = (fg_color_t *)view->image;
END BLOCK COMMENT */
				fg_box_t box;
				box[FG_X1] = Max(view->region.left, 0);
				box[FG_X2] = Min(view->region.right, columns - 1);
				box[FG_Y1] = Max(lines - view->region.bottom - 1, 0);
				box[FG_Y2] = Min(lines - view->region.top - 1, lines - 1);
				fg_readbox(box, screen);
/* START BLOCK COMMENT
**					fg_writebox(box, image);
END BLOCK COMMENT */
/* START BLOCK COMMENT
**					UCHAR *screen = view->screen;
END BLOCK COMMENT */
				UCHAR *image = view->image +
					(lines - view->region.top - 1 - box[FG_Y2]) *
					(view->region.right - view->region.left + 1);
				int MinMaxY = Min(view->region.bottom, lines - 1);
				int MinMaxX = Min(view->region.right, columns - 1);
				int xDiff = view->region.right - MinMaxX;
				for (y = Max(0, view->region.top); y <= MinMaxY;
					y++, image += xDiff)
					for (x = view->region.left; x <= MinMaxX; x++, image++)
						if (*image != BACKGROUND && x >= 0)
						{
/* START BLOCK COMMENT
**								*screen = fg_readdot(x, y);
END BLOCK COMMENT */
							fg_drawdot(*image, FG_MODE_SET, ~0, x, lines - y - 1);
						}
			}
		_stopDevice = FALSE;
	}
	else if (_virtualCount > 0)						// BUG.1674
		_virtualCount = 0;
	return (TRUE);
}

void UI_FG_DISPLAY::BitmapArrayToHandle(SCREENID , int ,
	int , const UCHAR *, const UI_PALETTE *,
	HBITMAP *, HBITMAP *)
{
}

void UI_FG_DISPLAY::BitmapHandleToArray(SCREENID , HBITMAP ,
		HBITMAP , int *, int *,
		UCHAR **)
{
}

void UI_FG_DISPLAY::IconArrayToHandle(SCREENID , int ,
	int , const UCHAR *, const UI_PALETTE *,
	HICON *)
{
}

void UI_FG_DISPLAY::IconHandleToArray(SCREENID , HICON ,
	int *, int *, UCHAR **)
{
}

