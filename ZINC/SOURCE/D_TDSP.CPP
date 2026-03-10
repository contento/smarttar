//	Zinc Interface Library - D_TDSP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>
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

#define CellXOR(cell) ((cell & 0x00FF) | (~(cell & 0xFF00) & 0xFF00))

// ----- UI_TEXT_DISPLAY ----------------------------------------------------

UI_TEXT_DISPLAY::UI_TEXT_DISPLAY(int _mode) :
	UI_DISPLAY(TRUE)
{
	_virtualCount = 0;
	_stopDevice = FALSE;

	mode = _mode;
	I_ScreenOpen(&mode, &lines, &columns);
	isMono = (mode == 0 || mode == 2 || mode == 5 || mode == 6 || mode == 7);

	cellWidth = cellHeight = 1;	
	preSpace = postSpace = 0;

	// Fill the screen according to the specified palette.
	USHORT color = MapColor(backgroundPalette);
	color = (color << 8) | FONTOUT(backgroundPalette->fillCharacter);
	int screenSize = columns * lines;
	_screen = new USHORT[screenSize];
	_moveBuffer = new USHORT[screenSize];
	for (int i = 0; i < screenSize; i++)
		_screen[i] = color;
	int offset = 0;
	for (i = 1; i <= lines; i++)
	{
		I_ScreenPut(1, i, columns, i, &_screen[offset]);
		offset += columns;
	}

	extern char _dialogSize[];
	memset(_dialogSize, 1, 256);

	// Define the screen display region.
	Add(NULL, new UI_REGION_ELEMENT(ID_SCREEN, 0, 0, columns - 1, lines - 1));
	installed = TRUE;
}

UI_TEXT_DISPLAY::~UI_TEXT_DISPLAY(void)
{
	// Restore the display.
	if (installed)
	{
		delete _screen;
		I_ScreenClose();
	}
	if (_moveBuffer)
		delete _moveBuffer;
}

int UI_TEXT_DISPLAY::DeviceMove(IMAGE_TYPE imageType, int newColumn, int newLine)
{
	if (imageType != MOUSE_IMAGE || _stopDevice)
		return (FALSE);
	_stopDevice = TRUE;

	UI_DISPLAY_IMAGE *view = &displayImage[MOUSE_IMAGE];
	int column = view->region.left;
	int line = view->region.top;
	view->region.left = view->region.right = newColumn;
	view->region.top = view->region.bottom = newLine;

	USHORT screen;
	if (column >= 0 && column < columns && line >= 0 && line < lines)
	{
		screen = _screen[line * columns + column];
		I_ScreenPut(column + 1, line + 1, column + 1, line + 1, &screen);
	}
	if (newColumn >= 0 && newColumn < columns && newLine >= 0 && newLine < lines)
	{
		screen = CellXOR(_screen[newLine * columns + newColumn]);
		I_ScreenPut(newColumn + 1, newLine + 1, newColumn + 1, newLine + 1, &screen);
	}

	_stopDevice = FALSE;
	return (TRUE);
}

int UI_TEXT_DISPLAY::DeviceSet(IMAGE_TYPE imageType, int column, int line,
	int, int, UCHAR *)
{
	DeviceMove(imageType, column, line);
	return (TRUE);
}

void UI_TEXT_DISPLAY::Line(SCREENID screenID, int column1, int line1,
	int column2, int line2, const UI_PALETTE *palette, int width, int xor,
	const UI_REGION *clipRegion)
{
	// Ensure a left-top to right-bottom line.
	if (column2 < column1)
	{
		int column = column1;
		column1 = column2;
		column2 = column;
	}
	if (line2 < line1)
	{
		int line = line1;
		line1 = line2;
		line2 = line;
	}

	// Set up the line region.
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, column1, line1, column2, line2))
		return;

	// Draw the line on the display.
	int changedScreen = FALSE;
	USHORT color = MapColor(palette);
	if (line1 == line2 && width >= 1)
		color = (color << 8) | FONTOUT(_iHorizontal[width - 1] & 0xFF);
	else if (column1 == column2 && width >= 1)
		color = (color << 8) | FONTOUT(_iVertical[width - 1] & 0xFF);
	else
		color = (color << 8) | FONTOUT(palette->fillCharacter);
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			if (!changedScreen)
				changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
			for (int row = tRegion.top; row <= tRegion.bottom; row++)
			{
				int offset = columns * row + tRegion.left;
				for (int column = tRegion.left; column <= tRegion.right; column++, offset++)
					_screen[offset] = xor ? CellXOR(_screen[offset]) : color;	// BUG.1491
			}
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

COLOR UI_TEXT_DISPLAY::MapColor(const UI_PALETTE *palette, int)
{
	// Match the color request based on the type of display.
	return (isMono ? palette->monoAttribute :
		(I_ScreenColor(palette->colorAttribute >> 4) << 4) |
		I_ScreenColor(palette->colorAttribute));
}

void UI_TEXT_DISPLAY::Rectangle(SCREENID screenID, int left, int top, int right,
	int bottom, const UI_PALETTE *palette, int width, int fill, int xor,
	const UI_REGION *clipRegion)
{
	// Assign the rectangle to the region structure.
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, left, top, right, bottom))
		return;

	// Check for the text shadow ``feature''.
	if (!palette)
	{
		VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
		UI_REGION clip;
		if (clipRegion)				// Un-optimized for Zortech bug.
			clip = *clipRegion;
		else
			clip = region;
		int offset = columns * region.top + region.right;
		if (top == region.top && right == region.right)
		{
			_screen[offset] = (_screen[offset] & 0xF000) | FONTOUT(_iLowerBox & 0xFF);
			region.top++;
			offset += columns;
		}
		if (right == region.right)
		{
			for (int line = region.top; line < region.bottom; line++, offset += columns)
				if (line >= clip.top && line <= clip.bottom)
					_screen[offset] = (_screen[offset] & 0xF000) | FONTOUT(_iFullBox & 0xFF);
			if (bottom != region.bottom && region.bottom >= clip.top && region.bottom <= clip.bottom)
				_screen[offset] = (_screen[offset] & 0xF000) | FONTOUT(_iFullBox & 0xFF);
		}
		if (left == region.left)
			region.left++;
		offset = columns * region.bottom + region.left;
		if (bottom == region.bottom)
		{
			for (int column = region.left; column <= region.right; column++, offset++)
				if (column >= clip.left && column <= clip.right)
					_screen[offset] = (_screen[offset] & 0xF000) | FONTOUT(_iUpperBox & 0xFF);
		}
		VirtualPut(screenID);
		return;
	}

	// Check for a no fill-pattern.
	if (!fill && top != bottom)
	{
		VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
		if (left < right - 1)
		{
			// Optimization of these lines doesn't work because
			// Line() can't tell which direction the line is going.
//			Line(screenID, left + 1, top, right - 1, top, palette, width, xor, clipRegion);	// BUG.????, BUG.1482
//			Line(screenID, left + 1, bottom, right - 1, bottom, palette, width, xor, clipRegion);
			Line(screenID, left, top, right, top, palette, width, xor, clipRegion);	// BUG.????, BUG.1482, BUG.1989
			Line(screenID, left, bottom, right, bottom, palette, width, xor, clipRegion);
		}
//		if (top < bottom - 1)
		if (top < bottom)
		{
			// Optimization of these lines doesn't work because
			// Line() can't tell which direction the line is going.
//			Line(screenID, left, top + 1, left, bottom - 1, palette, width, xor, clipRegion);	// BUG.????, BUG.1482
//			Line(screenID, right, top + 1, right, bottom - 1, palette, width, xor, clipRegion);
			Line(screenID, left, top, left, bottom, palette, width, xor, clipRegion);	// BUG.????, BUG.1482, BUG.1989
			Line(screenID, right, top, right, bottom, palette, width, xor, clipRegion);
		}
		Text(screenID, left, top, &_iCornerUL[width - 1], palette, 1, TRUE, xor, clipRegion);	// BUG.1482
		Text(screenID, right, top, &_iCornerUR[width - 1], palette, 1, TRUE, xor, clipRegion);
		Text(screenID, left, bottom, &_iCornerLL[width - 1], palette, 1, TRUE, xor, clipRegion);
		Text(screenID, right, bottom, &_iCornerLR[width - 1], palette, 1, TRUE, xor, clipRegion);
		VirtualPut(screenID);
		return;
	}

	// Draw the rectangle on the display.
	int changedScreen = FALSE;
	USHORT color = MapColor(palette);
	color = (color << 8) | FONTOUT(palette->fillCharacter);
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			if (!changedScreen)
				changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
			for (int row = tRegion.top; row <= tRegion.bottom; row++)
			{
				int offset = columns * row + tRegion.left;
				for (int column = tRegion.left; column <= tRegion.right; column++, offset++)
					_screen[offset] = xor ? CellXOR(_screen[offset]) : color;
			}
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

void UI_TEXT_DISPLAY::RectangleXORDiff(const UI_REGION &oldRegion,
	const UI_REGION &newRegion, SCREENID)
{
	// Set up the hide region.
	if (oldRegion.left == newRegion.left && oldRegion.top == newRegion.top &&
		oldRegion.right == newRegion.right && oldRegion.bottom == newRegion.bottom)
		return;
	UI_REGION region;
	region.left = Max(Min(oldRegion.left, newRegion.left), 0);
	region.top = Max(Min(oldRegion.top, newRegion.top), 0);
	region.right = Min(Max(oldRegion.right, newRegion.right), columns - 1);
	region.bottom = Min(Max(oldRegion.bottom, newRegion.bottom), lines - 1);

	// XOR the screen region.
	int row, column, offset;
	VirtualGet(ID_SCREEN, region.left, region.top, region.right, region.bottom);
	for (row = Max(oldRegion.top, region.top);
		row <= Min(oldRegion.bottom, region.bottom); row++)
	{
		offset = columns * row + Max(oldRegion.left, region.left);
		for (column = Max(oldRegion.left, region.left);
			column <= Min(oldRegion.right, region.right); column++, offset++)
			if (row == oldRegion.top || row == oldRegion.bottom ||
				column == oldRegion.left || column == oldRegion.right)
				_screen[offset] = CellXOR(_screen[offset]);
	}
	for (row = Max(newRegion.top, region.top);
		row <= Min(newRegion.bottom, region.bottom); row++)
	{
		offset = columns * row + Max(newRegion.left, region.left);
		for (column = Max(newRegion.left, region.left);
			column <= Min(newRegion.right, region.right); column++, offset++)
			if (row == newRegion.top || row == newRegion.bottom ||
				column == newRegion.left || column == newRegion.right)
			_screen[offset] = CellXOR(_screen[offset]);
	}

	// Update the screen.
	VirtualPut(ID_SCREEN);
}

void UI_TEXT_DISPLAY::RegionDefine(SCREENID screenID, int left, int top,
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

void UI_TEXT_DISPLAY::RegionMove(const UI_REGION &oldRegion,
	int newColumn, int newLine, SCREENID oldScreenID, SCREENID)
{
	UI_REGION region;
	if (!RegionInitialize(region, NULL, Min(oldRegion.left, newColumn),
		Min(oldRegion.top, newLine),
		Max(oldRegion.right, newColumn + (oldRegion.right - oldRegion.left)),
		Max(oldRegion.bottom, newLine + (oldRegion.bottom - oldRegion.top))))
		return;
	VirtualGet(oldScreenID, region.left, region.top, region.right, region.bottom);
	for (int row = region.top; row <= region.bottom; row++)
	{
		int offset = row * columns + region.left;
		I_ScreenPut(region.left + 1, row + 1, region.right + 1, row + 1, &_screen[offset]);
	}
	if (oldRegion.right >= oldRegion.left &&
	    oldRegion.bottom >= oldRegion.top)
	{
		I_ScreenGet(oldRegion.left+1, oldRegion.top+1,
			    oldRegion.right+1, oldRegion.bottom+1,
			    _moveBuffer);
		I_ScreenPut(newColumn+1, newLine+1,
			    newColumn+1 + oldRegion.right - oldRegion.left,
			    newLine+1 + oldRegion.bottom - oldRegion.top,
			    _moveBuffer);
	}
	for (row = region.top; row <= region.bottom; row++)
	{
		int offset = row * columns + region.left;
		I_ScreenGet(region.left + 1, row + 1, region.right + 1, row + 1, &_screen[offset]);
	}
	VirtualPut(oldScreenID);
}

void UI_TEXT_DISPLAY::Text(SCREENID screenID, int left, int top,
	const char *text, const UI_PALETTE *palette, int length, int fill,
	int xor, const UI_REGION *clipRegion, LOGICAL_FONT logicalFont)
{
	// Make sure we have a valid string.
	if (!text || !text[0])
		return;

	// Set up the fill line.
	static char fillLine[256];
	if (length < 0)
		length = ui_strlen(text);
	else if (length > 255)
		length = 255;
	strncpy(fillLine, text, length);
	fillLine[length] = '\0';

	if (!FlagSet(logicalFont, IGNORE_UNDERSCORE))
	{
		for (char *hotChar = strchr(fillLine, ZIL_HOTMARK); hotChar;
			hotChar = strchr(++hotChar, ZIL_HOTMARK))
		{
			memcpy(hotChar, hotChar + 1, ui_strlen(hotChar));
			if (*hotChar == ZIL_HOTMARK)
				++hotChar;
		}
	}

	// Check for special characters.
	for (char *string = strchr(fillLine, '\t'); string; string = strchr(string, '\t'))
		*string = ' ';
	for (string = strchr(fillLine, '\r'); string; string = strchr(string, '\r'))
		*string = ' ';
	for (string = strchr(fillLine, '\n'); string; string = strchr(string, '\n'))
		*string = ' ';

	// Convert the cell coordinates.
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, left, top,
		left + TextWidth(fillLine, screenID, logicalFont | IGNORE_UNDERSCORE) - 1,
		top + TextHeight(fillLine, screenID, logicalFont) - 1))
		return;

	// Draw the rectangle on the display.
	int changedScreen = FALSE;
	USHORT color = MapColor(palette);
	color = (color << 8);
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			if (!changedScreen)
				changedScreen = VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
			int sOffset = columns * tRegion.top + tRegion.left;
			int tOffset = tRegion.left - left;
			for (int column = tRegion.left; column <= tRegion.right; column++, sOffset++, tOffset++)
				_screen[sOffset] = xor ? CellXOR(_screen[sOffset]) :
//					(fill ? color : (_screen[sOffset] & 0xFF00)) | FONTOUT(fillLine[tOffset]);  // BUG.1491
					(fill ? color : ((_screen[sOffset] & 0xF000) | (color & 0x0F00))) | FONTOUT(fillLine[tOffset]);	// BUG.1574
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	if (changedScreen)
		VirtualPut(screenID);
}

int UI_TEXT_DISPLAY::TextHeight(const char *, SCREENID, LOGICAL_FONT)
{
	return (1);
}

int UI_TEXT_DISPLAY::TextWidth(const char *string, SCREENID, LOGICAL_FONT logicalFont)
{
	if (!string || !string[0])
		return (0);
	int length = ui_strlen(string);
	if (!FlagSet(logicalFont, IGNORE_UNDERSCORE))
	{
		for (char *hotChar = strchr(string, ZIL_HOTMARK); hotChar;
			hotChar = strchr(++hotChar, ZIL_HOTMARK))
		{
			--length;
			if (hotChar[1] == ZIL_HOTMARK)
				++hotChar;
		}
	}
	return (length);
}

int UI_TEXT_DISPLAY::VirtualGet(SCREENID, int left, int top, int right, int bottom)
{
#ifndef ZINC_DEBUG
	if (--_virtualCount == -1)
	{
#endif
 		_stopDevice = TRUE;
		_virtualRegion.left = Max(0, left);
		_virtualRegion.top = Max(0, top);
		_virtualRegion.right = Min(columns - 1, right);
		_virtualRegion.bottom = Min(lines - 1, bottom);
		if (_virtualRegion.Overlap(displayImage[MOUSE_IMAGE].region))
		{
			UI_DISPLAY_IMAGE *view = &displayImage[MOUSE_IMAGE];
			int column = view->region.left;
			int line = view->region.top;
			USHORT screen = _screen[line * columns + column];
			I_ScreenPut(column + 1, line + 1, column + 1, line + 1, &screen);
		}
#ifndef ZINC_DEBUG
	}
	else
	{
		_virtualRegion.left = Max(0, Min(_virtualRegion.left, left));
		_virtualRegion.top = Max(0, Min(_virtualRegion.top, top));
		_virtualRegion.right = Min(columns - 1, Max(_virtualRegion.right, right));
		_virtualRegion.bottom = Min(lines - 1, Max(_virtualRegion.bottom, bottom));
	}
#endif
	return (TRUE);
}

int UI_TEXT_DISPLAY::VirtualPut(SCREENID)
{
#ifndef ZINC_DEBUG
	if (++_virtualCount == 0)
	{
#endif
		for (int row = _virtualRegion.top; row <= _virtualRegion.bottom; row++)
		{
			int offset = row * columns + _virtualRegion.left;
			I_ScreenPut(_virtualRegion.left + 1, row + 1,
				_virtualRegion.right + 1, row + 1, &_screen[offset]);
		}
		if (_virtualRegion.Overlap(displayImage[MOUSE_IMAGE].region))
		{
			UI_DISPLAY_IMAGE *view = &displayImage[MOUSE_IMAGE];
			int column = view->region.left;
			int line = view->region.top;
			USHORT screen = CellXOR(_screen[line * columns + column]);
			I_ScreenPut(column + 1, line + 1, column + 1, line + 1, &screen);
		}

		_stopDevice = FALSE;
#ifndef ZINC_DEBUG
	}
	else if (_virtualCount > 0)						// BUG.1674
		_virtualCount = 0;
#endif
	return (TRUE);
}
