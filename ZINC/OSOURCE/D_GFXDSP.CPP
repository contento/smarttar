// D_GFXDSP.CPP - Graphics display class for use with Zinc Interface Library 3.0.
// COPYRIGHT (C) 1990-1993.  All Rights Reserved.
// Zinc Software Incorporated.  Pleasant Grove, Utah  USA

extern "C"
{
#	include "gfx.h"
#	include "gfx_pro.h"
}
#include <string.h>
#include "ui_dsp.hpp"

// Include graphics defines.
extern "C" void AFdecl _gfx_set_w2_ega_mode(void);
extern "C" void AFdecl _gfx_set_ega_modify_type(int);
extern "C" void AFdecl _gfx_clean_up_ega(void);

#define E_RES_MONO			0x4
#define TurnOnEGA()			if (_gfx.gfx_mode & E_RES_MONO) (_gfx.color_flags & XOR_PEL) ? _gfx_set_ega_modify_type(3) : _gfx_set_w2_ega_mode();
#define TurnOffEGA()		if (_gfx.gfx_mode & E_RES_MONO) _gfx_clean_up_ega();

#if defined(OEM)
#define GFX_SMALL_FONT	GFX_oem_smal_font
#define GFX_DIALOG_FONT	GFX_oem_dial_font
#define GFX_SYSTEM_FONT	GFX_oem_syst_font
#else
#define GFX_SMALL_FONT	GFX_iso_smal_font
#define GFX_DIALOG_FONT	GFX_iso_dial_font
#define GFX_SYSTEM_FONT	GFX_iso_syst_font
#endif

extern FONT *GFX_SMALL_FONT(void);
extern FONT *GFX_DIALOG_FONT(void);
extern FONT *GFX_SYSTEM_FONT(void);

UI_GRAPHICS_DISPLAY::UI_GRAPHICS_DISPLAY(int mode) :
	UI_DISPLAY(FALSE, "DOS", "GFX")
{
	_virtualCount = 0;
	_stopDevice = FALSE;
	_fillPattern = -1;
	_backgroundColor = -1;
	_foregroundColor = -1;
	_fillAttributes = FILL_SOLID + NO_FRAME;
	_outlineAttributes = 0;

	// Try to initialize the graphics mode.
	AllocGFXWorkspace(16);

#if defined(DOSX286) || defined(DOS386) || defined(DOS16RM)			// BUG.internal
	if (Screen(mode) == 0)							// BUG.7153
		return;
#else
	if (mode >= 0x100 && IdentifySuperVGA())							// BUG.1570
	{
		int *svga_modes;
		GetSuperVGAInfo(&svga_modes, 0);
		for (int i=0x107; i >= 0x100; i--)
			if (svga_modes[i - 0x100] && i <= mode)
			{
				SetSuperVGAMode(i);
				break;
			}
		if (i < 0x100)
			if (Screen(4) == 0)
				return;
	}
	else if (Screen(mode) == 0)						// BUG.7153
		return;
#endif

	// Get the system, dialog, and small fonts that were linked in.
	GRAPHICSFONT font = {0, 0, 0 };
	FONT *tmp;
	font.font = OpenMemFont(tmp = GFX_SMALL_FONT());
	if (font.font >= 0)
	{
		font.maxWidth = tmp->widest_cell - 1;
		font.maxHeight = tmp->height - 1;
		UI_GRAPHICS_DISPLAY::fontTable[FNT_SMALL_FONT] = font;
	}
	font.font = OpenMemFont(tmp = GFX_DIALOG_FONT());
	if (font.font >= 0)
	{
		font.maxWidth = tmp->widest_cell - 1;
		font.maxHeight = tmp->height - 1;
		UI_GRAPHICS_DISPLAY::fontTable[FNT_DIALOG_FONT] = font;
	}
	font.font = OpenMemFont(tmp = GFX_SYSTEM_FONT());
	if (font.font >= 0)
	{
		font.maxWidth = tmp->widest_cell - 1;
		font.maxHeight = tmp->height - 1;
		UI_GRAPHICS_DISPLAY::fontTable[FNT_SYSTEM_FONT] = font;
	}

	maxColors = _gfx.n_colors;
	isMono = (maxColors == 2);
	columns = _gfx.screen_x_res;
	lines = _gfx.screen_y_res;

	// Reset the font information.
	SetFont(-1);

	// Fill the screen according to the specified palette.
	cellWidth = (fontTable[FNT_DIALOG_FONT].font == ROM_8x8) ?
		TextWidth("M", ID_SCREEN, FNT_DIALOG_FONT) : 	// Bitmap font.
		TextWidth("M", ID_SCREEN, FNT_DIALOG_FONT) - 2; // Stroked font.
	cellHeight = TextHeight(NULL, ID_SCREEN, FNT_DIALOG_FONT) +
		preSpace + postSpace + 4 + 4;  // 4 above the text, 4 below the text.
//	cellWidth = TextWidth("M", ID_SCREEN, FNT_DIALOG_FONT);
//	cellHeight = TextHeight(NULL, ID_SCREEN, FNT_DIALOG_FONT) +
//		preSpace + postSpace + 2 + 2;  // 2 above the text, 2 below the text.
	SetPattern(backgroundPalette, FALSE);
	Box(-1, -1, columns, lines, _fillAttributes | NO_FRAME);

	// Define the screen display region.
	Add(0,new UI_REGION_ELEMENT(ID_SCREEN, 0, 0, columns - 1, lines - 1));
	installed = TRUE;
}

UI_GRAPHICS_DISPLAY::~UI_GRAPHICS_DISPLAY(void)
{
	// Restore the display.
	if (installed)
	{
		Screen(0);
		FreeGFXWorkspace();
		Cls();
	}
}

void UI_GRAPHICS_DISPLAY::Bitmap(SCREENID screenID, int column, int line,
	int bitmapWidth, int bitmapHeight, const UCHAR *bitmapArray,
	const UI_PALETTE *palette, const UI_REGION *clipRegion, HBITMAP *_colorBitmap,
	HBITMAP *_monoBitmap)
{
	// Make sure there is a valid image
	HBITMAP colorBitmap = 0, monoBitmap = 0;
	if (_colorBitmap)
		colorBitmap = *_colorBitmap;
	if (_monoBitmap)
		monoBitmap = *_monoBitmap;
	if (!bitmapArray && !colorBitmap)
		return;

	// Assign the bitmap to the region structure.
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, column, line,
		column + bitmapWidth - 1, line + bitmapHeight - 1))
		return;
	if (!palette)
		palette = colorMap;

	// Prepare the display for output.
	VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
//	if (!colorBitmap)
//		BitmapArrayToHandle(screenID, bitmapWidth, bitmapHeight, bitmapArray,
//			palette, &colorBitmap, &monoBitmap);

	// Draw the bitmap on the display.
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			_gfx.min_x = tRegion.left;
			_gfx.min_y = tRegion.top;
			_gfx.max_x = tRegion.right;
			_gfx.max_y = tRegion.bottom;

			if (monoBitmap)
			{
				PutPic(column, line, (PIC *)monoBitmap, 'A');
				PutPic(column, line, (PIC *)colorBitmap, 'X');
			}
			else if (colorBitmap)
				PutPic(column, line, (PIC *)colorBitmap, 'P');
			else
			{
				TurnOnEGA();
				for (int y = tRegion.top; y <= tRegion.bottom; y++)
				{
					const UCHAR *pixel = bitmapArray + (y - line) * bitmapWidth + tRegion.left - column;
					for (int x = tRegion.left; x <= tRegion.right; x++, pixel++)
						if (*pixel != BACKGROUND)
							Pset(x, y, *pixel);
				}
				TurnOffEGA();
			}

			if (screenID == ID_DIRECT)
				break;
		}

//	if (monoBitmap)
//	{
//		if (!_monoBitmap)
//			delete monoBitmap;
//		else
//			*_monoBitmap = monoBitmap;
//	}
//	if (colorBitmap)
//	{
//		if (!_colorBitmap)
//			delete colorBitmap;
//		else
//			*_colorBitmap = colorBitmap;
//	}

	// Update the screen.
	VirtualPut(screenID);
}

#define BITS_PER_BYTE 8
void UI_GRAPHICS_DISPLAY::BitmapArrayToHandle(SCREENID, int bitmapWidth,
	int bitmapHeight, const UCHAR *bitmapArray, const UI_PALETTE *,
	HBITMAP *colorBitmap, HBITMAP *monoBitmap)
{
#if !defined(DOSX286) && !defined(DOS386)
	if (!bitmapArray || *colorBitmap)
		return;

	int n_planes = 0;
	while(!(((maxColors) >> n_planes) & 0x01))
		n_planes++;

	// Calculate size of image.
	//	planewidth = number of per bytes to contain one bit of each pixel on one line.
	//	xbytes = number of bytes for one line
	//	imageSize = number of bytes in image
	int x_bytes_per_line = bitmapWidth  / BITS_PER_BYTE + ((bitmapWidth / BITS_PER_BYTE) % 2 ? 1 : 2);
	int lineWidth = x_bytes_per_line * n_planes;
	int imageSize = lineWidth * bitmapHeight;

	// Allocate the buffers.
	*colorBitmap = new UCHAR[sizeof(PIC) + imageSize];
	if (!*colorBitmap)
		return;
	*monoBitmap = new UCHAR[sizeof(PIC) + imageSize];
	if (!*monoBitmap)
		return;

	// For convenience get typecasted pointers to important portions of buffers
	PIC *pic = (PIC *)*colorBitmap;
	UCHAR *image = (UCHAR *)*colorBitmap + sizeof(PIC);
	memset(pic, 0, sizeof(PIC) + imageSize);
	PIC *maskPic = (PIC *)*monoBitmap;
	UCHAR *mask = (UCHAR *)*monoBitmap + sizeof(PIC);
	memset(maskPic, 0, sizeof(PIC) + imageSize);

	// Set up the PIC portion of buffers
	InitPicStruct(maskPic, IMAGE_RAM);
	InitPicStruct(pic, IMAGE_RAM);
	maskPic->encoding = pic->encoding = 0;
	maskPic->image_size = pic->image_size = imageSize;
	maskPic->n_orig_colors = pic->n_orig_colors = maxColors;
	maskPic->n_planes = pic->n_planes = n_planes;
	maskPic->x_pels = pic->x_pels = bitmapWidth;
	maskPic->y_rows = pic->y_rows = bitmapHeight;
	maskPic->x_bytes_per_line = pic->x_bytes_per_line = x_bytes_per_line;
	pic->ram_buf = image;
	maskPic->ram_buf = mask;

	// Now fill the image
	for(int y = 0; y < bitmapHeight; y++)
		for (int x = 0; x < bitmapWidth; x++)
		{
			// Do as much calcualation here as possible.
			int shift = BITS_PER_BYTE - x % BITS_PER_BYTE - 1;
			UCHAR pixel = bitmapArray[y * bitmapWidth + x];
			UCHAR imagePixel = (pixel == BACKGROUND) ? 0x00 : pixel;
			UCHAR maskPixel = (pixel == BACKGROUND) ? 0xFF : 0x00;
			int offset = y * lineWidth + x / BITS_PER_BYTE;
			for (int bit = 0; bit < n_planes; bit++)
			{
				image[offset + x_bytes_per_line * bit] |=
					(((imagePixel >> bit) & 0x01) << shift);
				mask[offset + x_bytes_per_line * bit] |=
					(((maskPixel >> bit) & 0x01) << shift);
			}
		}
#endif
}

void UI_GRAPHICS_DISPLAY::BitmapHandleToArray(SCREENID , HBITMAP colorBitmap,
	HBITMAP monoBitmap, int *bitmapWidth, int *bitmapHeight,
	UCHAR **bitmapArray)
{
#if !defined(DOSX286) && !defined(DOS386)
	// Make sure there is a valid Handle.
	if (!colorBitmap)
		return;

	// For convenience get typecasted pointers to important portions of buffers
	PIC *pic = (PIC *)colorBitmap;
	UCHAR *image = (UCHAR *)pic->ram_buf;
	PIC *maskPic = (PIC *)monoBitmap;
	UCHAR *mask = maskPic ? (UCHAR *)maskPic->ram_buf : NULL;

	// Set the width and height up.
	*bitmapWidth = pic->x_pels;
	*bitmapHeight = pic->y_rows;
	int lineWidth = pic->x_bytes_per_line * pic->n_planes;

	// Allocate buffer if necessary and then clear it.
	int imageSize = *bitmapWidth * *bitmapHeight;
	if (!*bitmapArray)
		*bitmapArray = new UCHAR[imageSize];
	memset(*bitmapArray, 0, imageSize);

	// Now convert the handle.
	for (int y = 0; y < *bitmapHeight; y++)
		for (int x = 0; x < *bitmapWidth; x++)
		{
			// Do as much calculation here as possible
			int index = y * *bitmapWidth + x;
			int offset = y * lineWidth + x / BITS_PER_BYTE;
			int shift = BITS_PER_BYTE - x % BITS_PER_BYTE - 1;
			if (mask && mask[offset + pic->x_bytes_per_line] & (0x01 << shift))
				(*bitmapArray)[index] = BACKGROUND;
			else
				for(int bit = 0; bit < pic->n_planes; bit++)
					(*bitmapArray)[index] |=
						((image[offset + (pic->x_bytes_per_line * bit)] >> shift) & 0x01) << bit;
		}
#endif
}

void UI_GRAPHICS_DISPLAY::Ellipse(SCREENID screenID, int x, int y, int startAngle,
	int endAngle, int xRadius, int yRadius, const UI_PALETTE *palette,
	int fill, int xor, const UI_REGION *clipRegion)
{
	// Assign the ellipse to the region structure.
	UI_REGION region, tRegion;
	if (!xRadius || !yRadius)												// BUG.2048
		return;
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

	// Prepare the display for output
	VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
	SetPattern(palette, xor);
	int angleWidth = (startAngle > endAngle) ?
		(360 - startAngle) + endAngle : endAngle - startAngle;

	// Draw the ellipse on the display.
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			_gfx.min_x = tRegion.left;
			_gfx.min_y = tRegion.top;
			_gfx.max_x = tRegion.right;
			_gfx.max_y = tRegion.bottom;

			// Draw an ellipse.
			if (startAngle == 0 && endAngle == 360)
			{
				Oval(x, y, xRadius, yRadius, fill ? _fillAttributes : _outlineAttributes);
			}
			// Draw an arc or sector.
			else
			{
				if (fill)
					OvalPieSlice(x, y, xRadius, yRadius, startAngle * 10, angleWidth * 10, _fillAttributes);
				else
					OvalArc(x, y, xRadius, yRadius, startAngle * 10, angleWidth * 10, _outlineAttributes);
			}
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	VirtualPut(screenID);
}

int UI_GRAPHICS_DISPLAY::DeviceMove(IMAGE_TYPE imageType, int newColumn, int newLine)
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
//	if (newColumn < 0 || newLine < 0)				// BUG.1066 BUG.1525
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
		_gfx.min_x = 0;
		_gfx.min_y = 0;
		_gfx.max_x = columns - 1;
		_gfx.max_y = lines - 1;
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
					*backup = Point(newX, newY);
				if (*image != BACKGROUND)
					Pset(newX, newY, *image);
			}
		}
		screen = view->screen;
		for (y = view->region.top; y <= view->region.bottom; y++)
			for (x = view->region.left; x <= view->region.right; x++, screen++)
				if (*screen != BACKGROUND)
					Pset(x, y, *screen);
		memcpy(view->screen, view->backup, width * height);
		view->region.left = newColumn;
		view->region.top = newLine;
		view->region.right = newColumn + width - 1;
		view->region.bottom = newLine + height - 1;
	}

	_stopDevice = FALSE;
	return (TRUE);
}

int UI_GRAPHICS_DISPLAY::DeviceSet(IMAGE_TYPE imageType, int column, int line,
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

void UI_GRAPHICS_DISPLAY::IconArrayToHandle(SCREENID screenID, int iconWidth,
	int iconHeight, const UCHAR *iconArray, const UI_PALETTE *palette,
	HICON *hIcon)
{
	BitmapArrayToHandle(screenID, iconWidth, iconHeight, iconArray, palette,
		&hIcon->colorBitmap, &hIcon->monoBitmap);
}

void UI_GRAPHICS_DISPLAY::IconHandleToArray(SCREENID screenID, HICON hIcon,
	int *iconWidth, int *iconHeight, UCHAR **iconArray)
{
	BitmapHandleToArray(screenID, hIcon.colorBitmap, hIcon.monoBitmap, iconWidth,
		iconHeight, iconArray);
}

void UI_GRAPHICS_DISPLAY::Line(SCREENID screenID, int x1, int y1, int x2, int y2,
	const UI_PALETTE *palette, int width, int xor, const UI_REGION *clipRegion)
{
	// Assign the line to a region structure.
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, Min(x1, x2), Min(y1, y2),
		Max(x1, x2) + width, Max(y1, y2) + width))
		return;

	// Prepare the display for output.
	int slope = x1 == x2 ? 1 : (y1 - y2) / (x1 - x2);
	VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
	SetPattern(palette, xor);
	
	// Draw the line on the display.
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			_gfx.min_x = tRegion.left;
			_gfx.min_y = tRegion.top;
			_gfx.max_x = tRegion.right;
			_gfx.max_y = tRegion.bottom;
			for (int i = 0; i < width; i++)
				if (slope)
					DrawLine(x1 + i, y1, x2 + i, y2, _outlineAttributes);
				else
					DrawLine(x1, y1 + i, x2, y2 + i, _outlineAttributes);
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	VirtualPut(screenID);
}

int UI_GRAPHICS_DISPLAY::MapColor(const UI_PALETTE *palette,int foreground)
{
	// Match the color request based on the type of display.
	if (maxColors == 2)
		return (foreground ? palette->bwForeground : palette->bwBackground);
	else if (maxColors < 16)
		return (foreground ? palette->grayScaleForeground : palette->grayScaleBackground);
	return (foreground ? palette->colorForeground : palette->colorBackground);
}

void UI_GRAPHICS_DISPLAY::Polygon(SCREENID screenID, int numPoints,
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

	// Prepare the display for output.
	VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
	SetPattern(palette, xor);

	// Draw the polygon on the display.
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			_gfx.min_x = tRegion.left;
			_gfx.min_y = tRegion.top;
			_gfx.max_x = tRegion.right;
			_gfx.max_y = tRegion.bottom;
#if defined(DOS386)
			PolyLine(0, 0, (int *)polygonPoints, numPoints,
				 fill ? _fillAttributes : _outlineAttributes);
#else
			PolyLine(0, 0, (int far *)polygonPoints, numPoints,
				 fill ? _fillAttributes : _outlineAttributes);
#endif
			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	VirtualPut(screenID);
}

void UI_GRAPHICS_DISPLAY::Rectangle(SCREENID screenID, int left, int top, int right,
	int bottom, const UI_PALETTE *palette, int width, int fill, int xor,
	const UI_REGION *clipRegion)
{
	// Assign the rectangle to the region structure.
	UI_REGION region, tRegion;
	if (!RegionInitialize(region, clipRegion, left, top, right, bottom))
		return;

	// Prepare the display for output.
	VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
	SetPattern(palette, xor);

	// Draw the rectangle on the display.
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			_gfx.min_x = tRegion.left;
			_gfx.min_y = tRegion.top;
			_gfx.max_x = tRegion.right;
			_gfx.max_y = tRegion.bottom;

			if (fill)
				Box(left - 1, top - 1, right + 1, bottom + 1, _fillAttributes | NO_FRAME);
			if (!fill || _backgroundColor != _foregroundColor)
				for (int i = 0; i < width; i++)
					Box(left + i, top + i, right - i, bottom - i, _outlineAttributes);

			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	VirtualPut(screenID);
}

void UI_GRAPHICS_DISPLAY::RectangleXORDiff(const UI_REGION &oldRegion,
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
	_gfx.min_x = 0;
	_gfx.min_y = 0;
	_gfx.max_x = columns - 1;
	_gfx.max_y = lines - 1;
	Box(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.bottom, _outlineAttributes);
	Box(newRegion.left, newRegion.top, newRegion.right, newRegion.bottom, _outlineAttributes);

	// Update the screen.
	VirtualPut(ID_DIRECT);
}

void UI_GRAPHICS_DISPLAY::RegionDefine(SCREENID screenID, int left, int top,
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

void UI_GRAPHICS_DISPLAY::RegionMove(const UI_REGION &oldRegion,
	int newColumn, int newLine, SCREENID oldScreenID, SCREENID)
{
	VirtualGet(oldScreenID, Min(oldRegion.left, newColumn), Min(oldRegion.top, newLine),
		Max(oldRegion.right, newColumn + (oldRegion.right - oldRegion.left)),
		Max(oldRegion.bottom, newLine + (oldRegion.bottom - oldRegion.top)));

	_gfx.min_x = 0;
	_gfx.min_y = 0;
	_gfx.max_x = columns - 1;
	_gfx.max_y = lines - 1;

	PIC *image;
	if ((image = GetPic(oldRegion.left, oldRegion.top, oldRegion.right, oldRegion.bottom)) != NULL)
	{
		PutPic(newColumn, newLine, image, 'P');
		FreePic(image);
	}
	else
	{
		if (oldRegion.top < newLine)
		{
			for (int i = oldRegion.bottom - oldRegion.top; i >= 0; i--)
				if (oldRegion.top + i > 0 && oldRegion.top + i < lines)
				{
					image = GetPic(oldRegion.left, oldRegion.top + i, oldRegion.right, oldRegion.top + i);
					PutPic(newColumn, newLine + i, image, 'P');
					FreePic(image);
				}
		}
		else
		{
			int height = oldRegion.bottom - oldRegion.top;
			for (int i = 0; i <= height; i++)
				if (oldRegion.top + i > 0 && oldRegion.top + i < lines)
				{
					image = GetPic(oldRegion.left, oldRegion.top + i, oldRegion.right, oldRegion.top + i);
					PutPic(newColumn, newLine + i, image, 'P');
					FreePic(image);
				}
		}
	}

	VirtualPut(oldScreenID);
}

void UI_GRAPHICS_DISPLAY::Text(SCREENID screenID, int left, int top,
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

	// Determine the hot-key values and shrink the string to remove ZIL_HOTMARK.
	char *hotKey = NULL;
	int hotKeyLeft, hotKeyRight;
	if (!FlagSet(logicalFont, IGNORE_UNDERSCORE))
	{
		for (char *hotChar = strchr(fillLine, ZIL_HOTMARK); hotChar;
			hotChar = strchr(++hotChar, ZIL_HOTMARK))
		{
			if (hotChar[1] == ZIL_HOTMARK)
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

	// Prepare the display for output.
	VirtualGet(screenID, region.left, region.top, region.right, region.bottom);
	SetPattern(palette, xor);
	FontColor(CurrFont(DFLT),						// BUG.1515
		  (xor ? WHITE | XOR_PEL : _foregroundColor),
		  (fill ? _backgroundColor : XPARENT));
	
	// Draw the rectangle on the display.
	for (UI_REGION_ELEMENT *dRegion = First(); dRegion; dRegion = dRegion->Next())
		if (screenID == ID_DIRECT ||
			(screenID == dRegion->screenID && dRegion->region.Overlap(region, tRegion)))
		{
			if (screenID == ID_DIRECT)
				tRegion = region;
			_gfx.min_x = tRegion.left;
			_gfx.min_y = tRegion.top;
			_gfx.max_x = tRegion.right;
			_gfx.max_y = tRegion.bottom;

			PrintFont(left, top, fillLine, length);
			if (hotKey)
				DrawLine(hotKeyLeft, bottom, hotKeyRight, bottom, _outlineAttributes);

			if (screenID == ID_DIRECT)
				break;
		}

	// Update the screen.
	VirtualPut(screenID);
}

void UI_GRAPHICS_DISPLAY::SetFont(LOGICAL_FONT logicalFont)
{
	static int _logicalFont = -1;
	static GRAPHICSFONT _fontInfo;
	GRAPHICSFONT fontInfo;

	// Check for invalid or new font set.
	if (_logicalFont == -1 || logicalFont == -1)
	{
		logicalFont = FNT_DIALOG_FONT;
		fontInfo = fontTable[FNT_DIALOG_FONT];
		FontAlign(fontInfo.font, TOP_LINE);
		if (fontInfo.font)
			CurrFont(fontInfo.font);
		LineJustify(JUSTIFY_START);
		LineDirection(TEXT_L_TO_R);

		// Set up the size tables.
		char text[2];
		text[1] = '\0';
		extern char _dialogSize[];
		for (int i = 0; i < 256; i++)
		{
			text[0] = i;
			_dialogSize[i] = GetLineLen(fontInfo.font, text, 1);
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
	if (fontInfo.font != _fontInfo.font)
	{
		FontAlign(fontInfo.font, TOP_LINE);
		if (fontInfo.font)
			CurrFont(fontInfo.font);
	}
	_fontInfo = fontInfo;
	_logicalFont = logicalFont;
}

void UI_GRAPHICS_DISPLAY::SetPattern(const UI_PALETTE *palette, int xor)
{
	int fillPattern = palette->fillPattern;
	int backgroundColor = MapColor(palette, FALSE);
	int foregroundColor = MapColor(palette, TRUE);

	if (fillPattern != _fillPattern || backgroundColor != _backgroundColor)
	{
		_fillPattern = fillPattern;
		_backgroundColor = backgroundColor;
		if (fillPattern == PTN_SOLID_FILL)
		{
			_fillAttributes = FILL_SOLID + foregroundColor;
			SetFillSolid(backgroundColor);
		}
		else
		{
			_fillAttributes = FILL_PAT + foregroundColor;
			SetFillPat(patternTable[fillPattern], foregroundColor, backgroundColor);
		}
	}
	if (foregroundColor != _foregroundColor)
	{
		_foregroundColor = foregroundColor;
		_outlineAttributes = (_outlineAttributes & COLOR_FLAGS) + foregroundColor;
		_fillAttributes = (_fillAttributes & COLOR_FLAGS) + foregroundColor;
	}

	if (xor)
	{
		_fillAttributes |= XOR_PEL;
		_outlineAttributes |= XOR_PEL;
	}
	else
	{
		_fillAttributes &= ~XOR_PEL;
		_outlineAttributes &= ~XOR_PEL;
	}
}

int UI_GRAPHICS_DISPLAY::TextHeight(const char *, SCREENID, LOGICAL_FONT logicalFont)
{
	logicalFont &= 0x0FFF;
	SetFont(logicalFont);
	if (fontTable[logicalFont].maxHeight)
		return (fontTable[logicalFont].maxHeight);
	else
		return (GetFontHeight(DFLT));
}

int UI_GRAPHICS_DISPLAY::TextWidth(const char *string, SCREENID, LOGICAL_FONT logicalFont)
{
	if (!string || !(*string))
		return (0);
	SetFont(logicalFont & 0x0FFF);
	int length =  GetLineLen(CurrFont(DFLT), (char *)string, strlen(string));

	char *tabPtr = strchr((char *)string, '\t');
	while (tabPtr)
	{
		length += GetLineLen(CurrFont(DFLT), " ", 1);
		tabPtr = strchr(tabPtr + 1, '\t');
	}

	if (!FlagSet(logicalFont, IGNORE_UNDERSCORE))
	{
		for (char *hotChar = strchr(string, ZIL_HOTMARK); hotChar;
			hotChar = strchr(++hotChar, ZIL_HOTMARK))
		{
			length -= GetLineLen(CurrFont(DFLT), ZIL_HOTMARKSTR, 1);
			if (hotChar[1] == ZIL_HOTMARK)
				++hotChar;
		}
	}
	return (length);
}

int UI_GRAPHICS_DISPLAY::VirtualGet(SCREENID, int left, int top, int right, int bottom)
{
	if (--_virtualCount == -1)
	{
		_gfx.min_x = 0;
		_gfx.min_y = 0;
		_gfx.max_x = columns - 1;
		_gfx.max_y = lines - 1;
		_gfx.color_flags &= ~XOR_PEL;

		_stopDevice = TRUE;
		_virtualRegion.left = Max(0, left);
		_virtualRegion.top = Max(0, top);
		_virtualRegion.right = Min(columns - 1, right);
		_virtualRegion.bottom = Min(lines - 1, bottom);

		TurnOnEGA();
		int x, y;
		for (int i = MAX_DISPLAY_IMAGES - 1; i >= 0; i--)
			if (_virtualRegion.Overlap(displayImage[i].region) && 
				displayImage[i].image && displayImage[i].screen)
			{
				UI_DISPLAY_IMAGE *view = &displayImage[i];
				UCHAR *screen = view->screen;
				for (y = view->region.top; y <= view->region.bottom; y++)
					for (x = view->region.left; x <= view->region.right; x++, screen++)
						if (*screen != BACKGROUND)
							Pset (x, y, *screen);
			}
		TurnOffEGA();
	}
	return (TRUE);
}

int UI_GRAPHICS_DISPLAY::VirtualPut(SCREENID)
{
	if (++_virtualCount == 0)
	{
		_gfx.min_x = 0;
		_gfx.min_y = 0;
		_gfx.max_x = columns - 1;
		_gfx.max_y = lines - 1;
		_gfx.color_flags &= ~XOR_PEL;

		TurnOnEGA();
		int x, y;
		for (int i = 0; i < MAX_DISPLAY_IMAGES; i++)
			if (_virtualRegion.Overlap(displayImage[i].region) &&
				displayImage[i].image && displayImage[i].screen)
			{
				UI_DISPLAY_IMAGE *view = &displayImage[i];
				UCHAR *screen = view->screen;
				UCHAR *image = view->image;
				for (y = view->region.top; y <= view->region.bottom; y++)
					for (x = view->region.left; x <= view->region.right; x++, screen++, image++)
						if (*image != BACKGROUND)
						{
							*screen = Point(x, y);
							Pset (x, y, *image);
						}
			}
		_stopDevice = FALSE;
		TurnOffEGA();
	}
	else if (_virtualCount > 0)						// BUG.1674
		_virtualCount = 0;
	return (TRUE);
}

#if (defined(__BCPLUSPLUS__) || defined(__TCPLUSPLUS__)) && (__BORLANDC__ < 0x0300)
GRAPHICSFONT UI_GRAPHICS_DISPLAY::fontTable[MAX_LOGICAL_FONTS] =
#elif defined(__ZTC__) && __ZTC__ < 0x310
GRAPHICSFONT UI_GRAPHICS_DISPLAY::fontTable[MAX_LOGICAL_FONTS] =
#else
UI_GRAPHICS_DISPLAY::GRAPHICSFONT UI_GRAPHICS_DISPLAY::fontTable[MAX_LOGICAL_FONTS] =
#endif
{
	{ ROM_8x8, 8, 11 },	// FNT_SMALL_FONT
	{ ROM_8x8, 8, 11 },	// FNT_DIALOG_FONT
	{ ROM_8x8, 8, 11 },	// FNT_SYSTEM_FONT
	{ ROM_8x8, 8, 11 },
	{ ROM_8x8, 8, 11 },
	{ ROM_8x8, 8, 11 },
	{ ROM_8x8, 8, 11 },
	{ ROM_8x8, 8, 11 },
	{ ROM_8x8, 8, 11 },
	{ ROM_8x8, 8, 11 }
};

#if (defined(__BCPLUSPLUS__) || defined(__TCPLUSPLUS__)) && (__BORLANDC__ < 0x0300)
GRAPHICSPATTERN UI_GRAPHICS_DISPLAY::patternTable[MAX_LOGICAL_PATTERNS] =
#elif defined(__ZTC__) && __ZTC__ < 0x310
GRAPHICSPATTERN UI_GRAPHICS_DISPLAY::patternTable[MAX_LOGICAL_PATTERNS] =
#else
UI_GRAPHICS_DISPLAY::GRAPHICSPATTERN UI_GRAPHICS_DISPLAY::patternTable[MAX_LOGICAL_PATTERNS] =
#endif
{
	// The first two bytes indicate the number of bytes per row and number of rows.
	{ 1, 8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },	// EMPTY_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// SOLID_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// LINE_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// LTSLASH_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// SLASH_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// BKSLASH_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// LTBKSLASH_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// HATCH_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// XHATCH_FILL
	{ 1, 8, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 },	// INTERLEAVE_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// WIDE_DOT_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },	// CLOSE_DOT_FILL

	{ 1, 8, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 },	// PTN_BACKGROUND_FILL
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
	{ 1, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
};
