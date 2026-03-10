//	Zinc Interface Library - UI_DSP.HPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if !defined(UI_DSP_HPP)
#	define UI_DSP_HPP
#	if !defined(UI_GEN_HPP)
#		include <ui_gen.hpp>
#	endif

// --- Version 2.0 and 1.0 compatibility ---
#define _backgroundPalette		UI_DISPLAY::backgroundPalette
#define _xorPalette				UI_DISPLAY::xorPalette
#define _colorMap				UI_DISPLAY::colorMap

// ----- Compiler/Environment Dependencies ----------------------------------

#if defined(ZIL_MSDOS)
	typedef long SCREENID;
	typedef void *HBITMAP;
	struct HICON
	{
		HBITMAP colorBitmap;
		HBITMAP monoBitmap;
	};
	const SCREENID ID_DIRECT			= 0xFFFF;	// special screen values.
	const SCREENID ID_SCREEN			= 0x0001;
#elif defined(ZIL_MSWINDOWS) && defined(WIN32)
	typedef HWND SCREENID;
	const SCREENID ID_DIRECT			= 0x0000;	// special screen values.
	const SCREENID ID_SCREEN			= 0x0000;
#elif defined(ZIL_MSWINDOWS)
	typedef HWND SCREENID;
	const SCREENID ID_DIRECT			= 0xFFFF;	// special screen values.
	const SCREENID ID_SCREEN			= 0x0000;
#elif defined(ZIL_OS2)
	typedef HWND SCREENID;
	typedef HPOINTER HICON;
	const SCREENID ID_DIRECT			= 0xFFFF;	// special screen values.
	const SCREENID ID_SCREEN			= 0x0000;
#elif defined(ZIL_MOTIF)
	typedef Widget SCREENID;
	typedef Pixmap HBITMAP;
	typedef Pixmap HICON;
	const SCREENID ID_DIRECT			= 0x0000;	// special screen values.
	const SCREENID ID_SCREEN			= 0x0000;
#endif

// --- Version 2.0 and 1.0 compatibility ---
#define UI_DOS_TEXT_DISPLAY		UI_TEXT_DISPLAY
#define UI_DOS_BGI_DISPLAY		UI_BGI_DISPLAY
#define UI_DOS_FG_DISPLAY 		UI_FG_DISPLAY

// ----- UI_POSITION --------------------------------------------------------
// ----- member functions are all inline ------------------------------------

struct EXPORT UI_POSITION
{
	int column, line;

#if defined(ZIL_MSWINDOWS)
	void Assign(const POINT &point) { column = point.x, line = point.y; }
#elif defined(ZIL_OS2)
	void Assign(const POINTL &point) { column = point.x, line = point.y; }
#endif
	int operator==(const UI_POSITION &position) const
		{ return (position.column == column && position.line == line); }
	int operator!=(const UI_POSITION &position) const
		{ return (position.column != column || position.line != line); }
	int operator<(const UI_POSITION &position) const
		{ return (position.column < column || position.line < line); }
	int operator>(const UI_POSITION &position) const
		{ return (position.column > column || position.line > line); }
	int operator>=(const UI_POSITION &position) const
		{ return (position.column >= column || position.line >= line); }
	int operator<=(const UI_POSITION &position) const
		{ return (position.column <= column || position.line <= line); }
	UI_POSITION &operator++(void) { column++, line++; return (*this); }
	UI_POSITION &operator--(void) { column--, line--; return (*this); }
	UI_POSITION &operator+=(int offset) { column += offset, line += offset; return (*this); }
	UI_POSITION &operator-=(int offset) { column -= offset, line -= offset; return (*this); }
};

// ----- UI_REGION, UI_REGION_ELEMENT, UI_REGION_LIST -----------------------

struct EXPORT UI_REGION
{
public:
	int left, top, right, bottom;

#if defined(ZIL_MSWINDOWS)
	void Assign(const RECT &rect) { left = rect.left, top = rect.top, right = rect.right - 1, bottom = rect.bottom - 1; }
#elif defined(ZIL_OS2)
	void Assign(const RECTL &rect) { left = rect.xLeft, top = rect.yBottom, right = rect.xRight, bottom = rect.yTop; }
#endif
	int Encompassed(const UI_REGION &region) const
		{ return (left >= region.left && top >= region.top &&
		  right <= region.right && bottom <= region.bottom); }
	int Height(void) const { return (bottom - top + 1); }
	int Overlap(const UI_REGION &region) const
		{ return (Max(region.left, left) <= Min(region.right, right) &&
		  Max(region.top, top) <= Min(region.bottom, bottom)); }
	int Overlap(const UI_POSITION &position) const
		{ return (position.column >= left && position.column <= right &&
		  position.line >= top && position.line <= bottom); }
	int Touching(const UI_POSITION &position) const
		{ return (((position.column == left || position.column == right) && position.line >= top && position.line <= bottom) ||
			((position.line == top || position.line == bottom) && position.column >= left && position.column <= right)); }
	int Overlap(const UI_REGION &region, UI_REGION &result) const
		{ result.left = Max(left, region.left);
		  result.top = Max(top, region.top);
		  result.right = Min(right, region.right);
		  result.bottom = Min(bottom, region.bottom);
		  return (result.left <= result.right && result.top <= result.bottom);
		}
	int Width(void) const { return (right - left + 1); }

	int operator==(const UI_REGION &region) const
		{ return (region.left == left && region.top == top &&
			region.right == right && region.bottom == bottom); }
	int operator!=(const UI_REGION &region) const
		{ return (region.left != left || region.top != top ||
			region.right != right || region.bottom != bottom); }
	UI_REGION &operator++(void) { left--, top--, right++, bottom++; return (*this); }
	UI_REGION &operator--(void) { left++, top++, right--, bottom--; return (*this); }
	UI_REGION &operator+=(int offset) { left -= offset, top -= offset, right += offset, bottom += offset; return (*this); }
	UI_REGION &operator-=(int offset) { left += offset, top += offset, right -= offset, bottom -= offset; return (*this); }
};

class EXPORT UI_REGION_ELEMENT : public UI_ELEMENT
{
public:
	SCREENID screenID;
	UI_REGION region;

	UI_REGION_ELEMENT(SCREENID _screenID, const UI_REGION &_region);
	UI_REGION_ELEMENT(SCREENID _screenID, int _left, int _top, int _right, int _bottom);
	~UI_REGION_ELEMENT(void);

	// Element members.
	UI_REGION_ELEMENT *Next(void) { return((UI_REGION_ELEMENT *)next); }
	UI_REGION_ELEMENT *Previous(void) { return((UI_REGION_ELEMENT *)previous); }
};

class EXPORT UI_REGION_LIST : public UI_LIST
{
public:
	UI_REGION_LIST(void);
	~UI_REGION_LIST(void);
	void Split(SCREENID screenID, const UI_REGION &region, int allocateBelow = FALSE);

	// List members.
	UI_REGION_ELEMENT *Current(void) { return((UI_REGION_ELEMENT *)current); }
	UI_REGION_ELEMENT *First(void) { return((UI_REGION_ELEMENT *)first); }
	UI_REGION_ELEMENT *Last(void) { return((UI_REGION_ELEMENT *)last); }
};

// ----- UI_PALETTE ---------------------------------------------------------
// ----- member functions are all inline ------------------------------------

// --- palette macro ---
#define attrib(foreground, background) (((background) << 4) + (foreground))

#if defined(ZIL_MSDOS)
typedef int COLOR;
#elif defined(ZIL_MSWINDOWS)
typedef DWORD COLOR;

// --- rgb colors ---
const COLOR RGB_BLACK					= 0x00000000L;
const COLOR RGB_BLUE					= 0x00800000L;
const COLOR RGB_GREEN					= 0x00008000L;
const COLOR RGB_CYAN					= 0x00808000L;
const COLOR RGB_RED						= 0x00000080L;
const COLOR RGB_MAGENTA					= 0x00800080L;
const COLOR RGB_BROWN					= 0x00008080L;
const COLOR RGB_LIGHTGRAY				= 0x00C0C0C0L;
const COLOR RGB_DARKGRAY				= 0x00808080L;
const COLOR RGB_LIGHTBLUE				= 0x00FF0000L;
const COLOR RGB_LIGHTGREEN				= 0x0000FF00L;
const COLOR RGB_LIGHTCYAN				= 0x00FFFF00L;
const COLOR RGB_LIGHTRED				= 0x000000FFL;
const COLOR RGB_LIGHTMAGENTA			= 0x00FF00FFL;
const COLOR RGB_YELLOW					= 0x0000FFFFL;
const COLOR RGB_WHITE					= 0x00FFFFFFL;
#elif defined(ZIL_MOTIF)
typedef unsigned long COLOR;

// --- rgb colors ---
const COLOR RGB_BLACK					= 0x000000;
const COLOR RGB_BLUE					= 0x0000B0;
const COLOR RGB_GREEN					= 0x00B000;
const COLOR RGB_CYAN					= 0x00B0B0;
const COLOR RGB_RED						= 0xB00000;
const COLOR RGB_MAGENTA					= 0xB000B0;
const COLOR RGB_BROWN					= 0xA52A2A;
const COLOR RGB_LIGHTGRAY				= 0xC0C0C0;
const COLOR RGB_DARKGRAY				= 0x606060;
const COLOR RGB_LIGHTBLUE				= 0x0000FF;
const COLOR RGB_LIGHTGREEN				= 0x00FF00;
const COLOR RGB_LIGHTCYAN				= 0x00FFFF;
const COLOR RGB_LIGHTRED				= 0xFF0000;
const COLOR RGB_LIGHTMAGENTA			= 0xFF00FF;
const COLOR RGB_YELLOW					= 0xFFFF00;
const COLOR RGB_WHITE					= 0xFFFFFF;
#endif

// --- monochrome ---
const COLOR MONO_BLACK					= 0x00;
const COLOR MONO_DIM					= 0x08;
const COLOR MONO_NORMAL					= 0x07;
const COLOR MONO_HIGH					= 0x0F;

// --- black & white ---
const COLOR BW_BLACK					= 0x00;
const COLOR BW_WHITE					= 0x01;

// --- gray scale ---
const COLOR GS_BLACK					= 0x00;
const COLOR GS_GRAY						= 0x01;
const COLOR GS_BLINKING					= 0x02;
const COLOR GS_WHITE					= 0x03;

// --- colors ---
const COLOR BACKGROUND					= 0xFF;
const COLOR MAX_COLORMAP_INDEX			= 0x0F;

#if !defined(__COLORS) && !defined(_BGI_COLORS)
#define __COLORS
#define _BGI_COLORS
const COLOR BLACK						= 0x00;
const COLOR BLUE						= 0x01;
const COLOR GREEN						= 0x02;
const COLOR CYAN						= 0x03;
const COLOR RED							= 0x04;
const COLOR MAGENTA						= 0x05;
const COLOR BROWN						= 0x06;
const COLOR LIGHTGRAY					= 0x07;
const COLOR DARKGRAY					= 0x08;
const COLOR LIGHTBLUE					= 0x09;
const COLOR LIGHTGREEN					= 0x0A;
const COLOR LIGHTCYAN					= 0x0B;
const COLOR LIGHTRED					= 0x0C;
const COLOR LIGHTMAGENTA				= 0x0D;
const COLOR YELLOW						= 0x0E;
const COLOR WHITE						= 0x0F;
#endif

// --- LOGICAL_PATTERN ---
#define MAX_LOGICAL_PATTERNS				15
typedef int LOGICAL_PATTERN;
const LOGICAL_PATTERN PTN_SOLID_FILL		= 0x0001;		// BGI match SOLID_FILL
const LOGICAL_PATTERN PTN_INTERLEAVE_FILL	= 0x0009;		// BGI match INTERLEAVE_FILL
const LOGICAL_PATTERN PTN_BACKGROUND_FILL	= 0x000C;		// BGI match BACKGROUND_FILL
#if defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(ZIL_MOTIF)
const LOGICAL_PATTERN PTN_SYSTEM_COLOR		= 0x00F0;
const LOGICAL_PATTERN PTN_RGB_COLOR			= 0x00F1;
#endif

struct EXPORT UI_PALETTE
{
	// --- Text mode ---
	UCHAR fillCharacter;				// Fill character.
	COLOR colorAttribute;				// Color attribute.
	COLOR monoAttribute;				// Mono attribute.

	// --- Graphics mode ---
	LOGICAL_PATTERN fillPattern;		// Fill pattern.
	COLOR colorForeground;				// EGA/VGA colors.
	COLOR colorBackground;
	COLOR bwForeground;					// Black & White colors (2 color).
	COLOR bwBackground;
	COLOR grayScaleForeground;			// Monochrome colors (3+ color).
	COLOR grayScaleBackground;
};

// ----- UI_DISPLAY ---------------------------------------------------------

// --- LOGICAL_FONT ---
#define MAX_LOGICAL_FONTS				10
typedef int LOGICAL_FONT;
const LOGICAL_FONT FNT_SMALL_FONT		= 0x0000;	// Base logical fonts.
const LOGICAL_FONT FNT_DIALOG_FONT		= 0x0001;
const LOGICAL_FONT FNT_SYSTEM_FONT		= 0x0002;

// Ignore the & character on the Text() function (used with fill).
const LOGICAL_FONT IGNORE_UNDERSCORE	= 0x1000;

// --- IMAGE_TYPE ---
#define MAX_DISPLAY_IMAGES				2
typedef unsigned IMAGE_TYPE;
const IMAGE_TYPE MOUSE_IMAGE			= 0;
const IMAGE_TYPE CURSOR_IMAGE			= 1;

class EXPORT UI_DISPLAY
{
public:
	int installed;
	int isText;
	int isMono;
	int columns, lines;
	int cellWidth, cellHeight;
	int preSpace, postSpace;
	long miniNumeratorX, miniDenominatorX;
	long miniNumeratorY, miniDenominatorY;
	char *operatingSystem;
	char *windowingSystem;

	static UI_PALETTE *backgroundPalette;
	static UI_PALETTE *xorPalette;
	static UI_PALETTE *colorMap;

#if defined(ZIL_MSWINDOWS)
	HANDLE hInstance;
	HANDLE hPrevInstance;
	int nCmdShow;
#elif defined(ZIL_OS2)
	HAB hab;
#elif defined(ZIL_MOTIF)
	XtAppContext appContext;
	Widget topShell;
	Display *xDisplay;
	Screen *xScreen;
	int xScreenNumber;
	GC xGc;
	char *appClass;
	Pixmap interleaveStipple;
#endif

	virtual ~UI_DISPLAY(void);
	virtual void Bitmap(SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const UCHAR *bitmapArray,
		const UI_PALETTE *palette = NULL, const UI_REGION *clipRegion = NULL,
		HBITMAP *colorBitmap = NULL, HBITMAP *monoBitmap = NULL);
	virtual void BitmapArrayToHandle(SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const UCHAR *bitmapArray, const UI_PALETTE *palette,
		HBITMAP *colorBitmap, HBITMAP *monoBitmap);
	virtual void BitmapHandleToArray(SCREENID screenID, HBITMAP colorBitmap,
		HBITMAP monoBitmap, int *bitmapWidth, int *bitmapHeight,
		UCHAR **bitmapArray);
	virtual void Ellipse(SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual void IconArrayToHandle(SCREENID screenID, int iconWidth,
		int iconHeight, const UCHAR *iconArray, const UI_PALETTE *palette,
		HICON *icon);
	virtual void IconHandleToArray(SCREENID screenID, HICON icon,
		int *iconWidth, int *iconHeight, UCHAR **iconArray);
	virtual void Line(SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void Polygon(SCREENID screenID, int numPoints, const int *polygonPoints,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	void Rectangle(SCREENID screenID, const UI_REGION &region,
		const UI_PALETTE *palette, int width = 1, int fill = FALSE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL)
		{ Rectangle(screenID, region.left, region.top, region.right,
		  region.bottom, palette, width, fill, _xor, clipRegion); }
	virtual void Rectangle(SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1, int fill = FALSE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL);
	virtual void RectangleXORDiff(const UI_REGION &oldRegion, const UI_REGION &newRegion, SCREENID screenID = ID_SCREEN);
	void RegionDefine(SCREENID screenID, const UI_REGION &region)
		{ RegionDefine(screenID, region.left, region.top, region.right, region.bottom); }
	virtual void RegionDefine(SCREENID screenID, int left, int top, int right, int bottom);
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn, int newLine,
		SCREENID oldScreenID = ID_SCREEN, SCREENID newScreenID = ID_SCREEN);
	virtual void Text(SCREENID screenID, int left, int top, const char *text,
		const UI_PALETTE *palette, int length = -1, int fill = TRUE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const char *string, SCREENID screenID = ID_SCREEN,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const char *string, SCREENID screenID = ID_SCREEN,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	int VirtualGet(SCREENID screenID, const UI_REGION &region)
		{ return (VirtualGet(screenID, region.left, region.top, region.right, region.bottom)); }
	virtual int VirtualGet(SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(SCREENID screenID);

	// ADVANCED functions for mouse and cursor --- DO NOT USE! ---
	virtual int DeviceMove(IMAGE_TYPE imageType, int newColumn, int newLine);
	virtual int DeviceSet(IMAGE_TYPE imageType, int column, int line, int width, int height, UCHAR *image);

protected:
	struct EXPORT UI_DISPLAY_IMAGE
	{
		UI_REGION region;
		UCHAR *image;
		UCHAR *screen;
		UCHAR *backup;
	};
	
	UI_DISPLAY_IMAGE displayImage[MAX_DISPLAY_IMAGES];

	UI_DISPLAY(int isText, const char *operatingSystem = NULL,
		const char *windowingSystem = NULL);
	int RegionInitialize(UI_REGION &region, const UI_REGION *clipRegion,
		int left, int top, int right, int bottom);
};

// ----- UI_TEXT_DISPLAY ----------------------------------------------------

// --- TDM_MODE ---
typedef int TDM_MODE;
const TDM_MODE TDM_NONE					= 0xFF;
const TDM_MODE TDM_AUTO					= -1;
const TDM_MODE TDM_BW_25x40 			= 0;
const TDM_MODE TDM_25x40 				= 1;
const TDM_MODE TDM_BW_25x80 			= 2;
const TDM_MODE TDM_25x80				= 3;
const TDM_MODE TDM_MONO_25x80			= 7;
const TDM_MODE TDM_43x80				= 64;

#if defined(ZIL_MSDOS)
class EXPORT UI_TEXT_DISPLAY : public UI_DISPLAY, public UI_REGION_LIST
{
public:
	TDM_MODE mode;

	UI_TEXT_DISPLAY(TDM_MODE _mode = TDM_AUTO);
	virtual ~UI_TEXT_DISPLAY(void);
	virtual void Line(SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual COLOR MapColor(const UI_PALETTE *palette, int isForeground = 1);
	virtual void Rectangle(SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1, int fill = FALSE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL);
	virtual void RectangleXORDiff(const UI_REGION &oldRegion, const UI_REGION &newRegion, SCREENID screenID = ID_SCREEN);
	virtual void RegionDefine(SCREENID screenID, int left, int top, int right, int bottom);
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn, int newLine,
		SCREENID oldScreenID = ID_SCREEN, SCREENID newScreenID = ID_SCREEN);
	virtual void Text(SCREENID screenID, int left, int top, const char *text,
		const UI_PALETTE *palette, int length = -1, int fill = TRUE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const char *string, SCREENID screenID = ID_SCREEN,
 		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const char *string, SCREENID screenID = ID_SCREEN,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(SCREENID screenID);

	// ADVANCED functions for mouse and cursor --- DO NOT USE! ---
	virtual int DeviceMove(IMAGE_TYPE imageType, int newColumn, int newLine);
	virtual int DeviceSet(IMAGE_TYPE imageType, int column, int line, int width, int height, UCHAR *image);

protected:
	USHORT *_screen;
	USHORT *_moveBuffer;
	int _virtualCount;
	UI_REGION _virtualRegion;
	char _stopDevice;
};
#endif

// ----- UI_GRAPHICS_DISPLAY --------------------------------------------------

#if defined(ZIL_MSDOS)
class UI_GRAPHICS_DISPLAY : public UI_DISPLAY, public UI_REGION_LIST
{
public:
	struct GRAPHICSFONT
	{
		int font;
		int maxWidth, maxHeight;
	};
	typedef unsigned char GRAPHICSPATTERN[10];

	static GRAPHICSFONT fontTable[MAX_LOGICAL_FONTS];
	static GRAPHICSPATTERN patternTable[MAX_LOGICAL_PATTERNS];

	UI_GRAPHICS_DISPLAY(int mode = 4);
	virtual ~UI_GRAPHICS_DISPLAY(void);
	virtual void Bitmap(SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const UCHAR *bitmapArray,
		const UI_PALETTE *palette = NULL, const UI_REGION *clipRegion = NULL,
		HBITMAP *colorBitmap = NULL, HBITMAP *monoBitmap = NULL);
	virtual void BitmapArrayToHandle(SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const UCHAR *bitmapArray, const UI_PALETTE *palette,
		HBITMAP *colorBitmap, HBITMAP *monoBitmap);
	virtual void BitmapHandleToArray(SCREENID screenID, HBITMAP colorBitmap,
		HBITMAP monoBitmap, int *bitmapWidth, int *bitmapHeight,
		UCHAR **bitmapArray);
	virtual void Ellipse(SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual void IconArrayToHandle(SCREENID screenID, int iconWidth,
		int iconHeight, const UCHAR *iconArray, const UI_PALETTE *palette,
		HICON *icon);
	virtual void IconHandleToArray(SCREENID screenID, HICON icon,
		int *iconWidth, int *iconHeight, UCHAR **iconArray);
	virtual void Line(SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void Polygon(SCREENID screenID, int numPoints, const int *polygonPoints,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual void Rectangle(SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1, int fill = FALSE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL);
	virtual void RectangleXORDiff(const UI_REGION &oldRegion, const UI_REGION &newRegion, SCREENID screenID = ID_SCREEN);
	virtual void RegionDefine(SCREENID screenID, int left, int top, int right, int bottom);
   	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn, int newLine,
		SCREENID oldScreenID = ID_SCREEN, SCREENID newScreenID = ID_SCREEN);
	virtual void Text(SCREENID screenID, int left, int top, const char *text,
		const UI_PALETTE *palette, int length = -1, int fill = TRUE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const char *string, SCREENID screenID = ID_SCREEN,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const char *string, SCREENID screenID = ID_SCREEN,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(SCREENID screenID);

	// ADVANCED functions for mouse and cursor --- DO NOT USE! ---
	virtual int DeviceMove(IMAGE_TYPE imageType, int newColumn, int newLine);
	virtual int DeviceSet(IMAGE_TYPE imageType, int column, int line, int width, int height, UCHAR *image);

protected:
	int maxColors;
	int _fillPattern;
	int _backgroundColor;
	int _foregroundColor;
	int _fillAttributes;
	int _outlineAttributes;
	signed char _virtualCount;
	UI_REGION _virtualRegion;
	char _stopDevice;

	void SetFont(LOGICAL_FONT logicalFont);
	void SetPattern(const UI_PALETTE *palette, int _xor);
};
#endif

// ----- UI_BGI_DISPLAY -----------------------------------------------------

#if defined(ZIL_MSDOS) && (defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__))
class EXPORT UI_BGI_DISPLAY : public UI_DISPLAY, public UI_REGION_LIST
{
public:
	struct BGIFONT
	{
		int font;
		int charSize;
		int multX, divX;
		int multY, divY;
		int maxWidth, maxHeight;
	};
	typedef char BGIPATTERN[8];

	static UI_PATH *searchPath;
	static BGIFONT fontTable[MAX_LOGICAL_FONTS];
	static BGIPATTERN patternTable[MAX_LOGICAL_PATTERNS];

	UI_BGI_DISPLAY(int driver = 0, int mode = 0);
	virtual ~UI_BGI_DISPLAY(void);
	virtual void Bitmap(SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const UCHAR *bitmapArray,
		const UI_PALETTE *palette = NULL, const UI_REGION *clipRegion = NULL,
		HBITMAP *colorBitmap = NULL, HBITMAP *monoBitmap = NULL);
	virtual void BitmapArrayToHandle(SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const UCHAR *bitmapArray, const UI_PALETTE *palette,
		HBITMAP *colorBitmap, HBITMAP *monoBitmap);
	virtual void BitmapHandleToArray(SCREENID screenID, HBITMAP colorBitmap,
		HBITMAP monoBitmap, int *bitmapWidth, int *bitmapHeight,
		UCHAR **bitmapArray);
	virtual void Ellipse(SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual void IconArrayToHandle(SCREENID screenID, int iconWidth,
		int iconHeight, const UCHAR *iconArray, const UI_PALETTE *palette,
		HICON *icon);
	virtual void IconHandleToArray(SCREENID screenID, HICON icon,
		int *iconWidth, int *iconHeight, UCHAR **iconArray);
	virtual void Line(SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void Polygon(SCREENID screenID, int numPoints, const int *polygonPoints,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual void Rectangle(SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1, int fill = FALSE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL);
	virtual void RectangleXORDiff(const UI_REGION &oldRegion, const UI_REGION &newRegion, SCREENID screenID = ID_SCREEN);
	virtual void RegionDefine(SCREENID screenID, int left, int top, int right, int bottom);
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn, int newLine,
		SCREENID oldScreenID = ID_SCREEN, SCREENID newScreenID = ID_SCREEN);
	virtual void Text(SCREENID screenID, int left, int top, const char *text,
		const UI_PALETTE *palette, int length = -1, int fill = TRUE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const char *string, SCREENID screenID = ID_SCREEN,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const char *string, SCREENID screenID = ID_SCREEN,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(SCREENID screenID);

	// ADVANCED functions for mouse and cursor --- DO NOT USE! ---
	virtual int DeviceMove(IMAGE_TYPE imageType, int newColumn, int newLine);
	virtual int DeviceSet(IMAGE_TYPE imageType, int column, int line, int width, int height, UCHAR *image);

protected:
	int maxColors;
	signed char _virtualCount;
	UI_REGION _virtualRegion;
	char _stopDevice;

	void SetFont(LOGICAL_FONT logicalFont);
	void SetPattern(const UI_PALETTE *palette, int _xor);
};
#endif

// ----- UI_MSC_DISPLAY -------------------------------------------------

#if defined(ZIL_MSDOS) && defined(_MSC_VER)
class EXPORT UI_MSC_DISPLAY : public UI_DISPLAY, public UI_REGION_LIST
{
public:
	struct EXPORT MSCFONT
	{
		char *typeFace;
		char *options;
	};
	typedef unsigned char MSCPATTERN[8];

	static UI_PATH *searchPath;
	static MSCFONT fontTable[MAX_LOGICAL_FONTS];
	static MSCPATTERN patternTable[MAX_LOGICAL_PATTERNS];

	UI_MSC_DISPLAY(int mode = 0);
	virtual ~UI_MSC_DISPLAY(void);
	virtual void Bitmap(SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const UCHAR *bitmapArray,
		const UI_PALETTE *palette = NULL, const UI_REGION *clipRegion = NULL,
		HBITMAP *colorBitmap = NULL, HBITMAP *monoBitmap = NULL);
	virtual void BitmapArrayToHandle(SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const UCHAR *bitmapArray, const UI_PALETTE *palette,
		HBITMAP *colorBitmap, HBITMAP *monoBitmap);
	virtual void BitmapHandleToArray(SCREENID screenID, HBITMAP colorBitmap,
		HBITMAP monoBitmap, int *bitmapWidth, int *bitmapHeight,
		UCHAR **bitmapArray);
	virtual void Ellipse(SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual void IconArrayToHandle(SCREENID screenID, int iconWidth,
		int iconHeight, const UCHAR *iconArray, const UI_PALETTE *palette,
		HICON *icon);
	virtual void IconHandleToArray(SCREENID screenID, HICON icon,
		int *iconWidth, int *iconHeight, UCHAR **iconArray);
	virtual void Line(SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void Polygon(SCREENID screenID, int numPoints, const int *polygonPoints,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual void Rectangle(SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1, int fill = FALSE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL);
	virtual void RectangleXORDiff(const UI_REGION &oldRegion, const UI_REGION &newRegion, SCREENID screenID = ID_SCREEN);
	virtual void RegionDefine(SCREENID screenID, int left, int top, int right, int bottom);
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn, int newLine,
		SCREENID oldScreenID = ID_SCREEN, SCREENID newScreenID = ID_SCREEN);
	virtual void Text(SCREENID screenID, int left, int top, const char *text,
		const UI_PALETTE *palette, int length = -1, int fill = TRUE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const char *string, SCREENID screenID = ID_SCREEN,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const char *string, SCREENID screenID = ID_SCREEN,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(SCREENID screenID);

	// ADVANCED functions for mouse and cursor --- DO NOT USE! ---
	virtual int DeviceMove(IMAGE_TYPE imageType, int newColumn, int newLine);
	virtual int DeviceSet(IMAGE_TYPE imageType, int column, int line, int width, int height, UCHAR *image);

protected:
	int maxColors;
	signed char _virtualCount;
	UI_REGION _virtualRegion;
	char _stopDevice;
	int _fillPattern;
	int _backgroundColor;
	int _foregroundColor;

	void SetFont(LOGICAL_FONT logicalFont);
	void SetPattern(const UI_PALETTE *palette, int _xor);
};
#endif

// ----- UI_FG_DISPLAY -------------------------------------------------

#if defined(ZIL_MSDOS) && defined(__ZTC__)
#	if defined(DOS386)
#		define _far
#	endif
class EXPORT UI_FG_DISPLAY : public UI_DISPLAY, public UI_REGION_LIST
{
public:
	struct FGFONT
	{
		char _far *fontptr;
		char _far *fontptr2;
		int maxWidth, maxHeight;
	};
	typedef unsigned short FGPATTERN[16];

	static UI_PATH *searchPath;
	static FGFONT fontTable[MAX_LOGICAL_FONTS];
	static FGPATTERN patternTable[MAX_LOGICAL_PATTERNS];

	UI_FG_DISPLAY(int mode = 0);
	virtual ~UI_FG_DISPLAY(void);
	virtual void Bitmap(SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const UCHAR *bitmapArray,
		const UI_PALETTE *palette = NULL, const UI_REGION *clipRegion = NULL,
		HBITMAP *colorBitmap = NULL, HBITMAP *monoBitmap = NULL);
	virtual void BitmapArrayToHandle(SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const UCHAR *bitmapArray, const UI_PALETTE *palette,
		HBITMAP *colorBitmap, HBITMAP *monoBitmap);
	virtual void BitmapHandleToArray(SCREENID screenID, HBITMAP colorBitmap,
		HBITMAP monoBitmap, int *bitmapWidth, int *bitmapHeight,
		UCHAR **bitmapArray);
	virtual void Ellipse(SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual void IconArrayToHandle(SCREENID screenID, int iconWidth,
		int iconHeight, const UCHAR *iconArray, const UI_PALETTE *palette,
		HICON *icon);
	virtual void IconHandleToArray(SCREENID screenID, HICON icon,
		int *iconWidth, int *iconHeight, UCHAR **iconArray);
	virtual void Line(SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void Polygon(SCREENID screenID, int numPoints, const int *polygonPoints,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual void Rectangle(SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1, int fill = FALSE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL);
	virtual void RectangleXORDiff(const UI_REGION &oldRegion, const UI_REGION &newRegion, SCREENID screenID = ID_SCREEN);
	virtual void RegionDefine(SCREENID screenID, int left, int top, int right, int bottom);
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn, int newLine,
		SCREENID oldScreenID = ID_SCREEN, SCREENID newScreenID = ID_SCREEN);
	virtual void Text(SCREENID screenID, int left, int top, const char *text,
		const UI_PALETTE *palette, int length = -1, int fill = TRUE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const char *string, SCREENID screenID = ID_SCREEN,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const char *string, SCREENID screenID = ID_SCREEN,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(SCREENID screenID);

	// ADVANCED functions for mouse and cursor --- DO NOT USE! ---
	virtual int DeviceMove(IMAGE_TYPE imageType, int newColumn, int newLine);
	virtual int DeviceSet(IMAGE_TYPE imageType, int column, int line, int width, int height, UCHAR *image);

protected:
	int maxColors;
	int _virtualCount;
	UI_REGION _virtualRegion;
	char _stopDevice;
	int _fillPattern;
	int _backgroundColor;
	int _foregroundColor;
	int _xor;

	void SetFont(LOGICAL_FONT logicalFont);
	void SetPattern(const UI_PALETTE *palette, int _xor);
};
#endif

// ----- UI_WCC_DISPLAY -------------------------------------------------

#if defined(ZIL_MSDOS) && defined(__WATCOMC__)
class EXPORT UI_WCC_DISPLAY : public UI_DISPLAY, public UI_REGION_LIST
{
public:
	struct EXPORT WCCFONT
	{
		char *typeFace;
		char *options;
	};
	typedef unsigned char WCCPATTERN[8];

	static UI_PATH *searchPath;
	static WCCFONT fontTable[MAX_LOGICAL_FONTS];
	static WCCPATTERN patternTable[MAX_LOGICAL_PATTERNS];

	UI_WCC_DISPLAY(int mode = 0);
	virtual ~UI_WCC_DISPLAY(void);
	virtual void Bitmap(SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const UCHAR *bitmapArray,
		const UI_PALETTE *palette = NULL, const UI_REGION *clipRegion = NULL,
		HBITMAP *colorBitmap = NULL, HBITMAP *monoBitmap = NULL);
	virtual void BitmapArrayToHandle(SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const UCHAR *bitmapArray, const UI_PALETTE *palette,
		HBITMAP *colorBitmap, HBITMAP *monoBitmap);
	virtual void BitmapHandleToArray(SCREENID screenID, HBITMAP colorBitmap,
		HBITMAP monoBitmap, int *bitmapWidth, int *bitmapHeight,
		UCHAR **bitmapArray);
	virtual void Ellipse(SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual void IconArrayToHandle(SCREENID screenID, int iconWidth,
		int iconHeight, const UCHAR *iconArray, const UI_PALETTE *palette,
		HICON *icon);
	virtual void IconHandleToArray(SCREENID screenID, HICON icon,
		int *iconWidth, int *iconHeight, UCHAR **iconArray);
	virtual void Line(SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void Polygon(SCREENID screenID, int numPoints, const int *polygonPoints,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual void Rectangle(SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1, int fill = FALSE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL);
	virtual void RectangleXORDiff(const UI_REGION &oldRegion, const UI_REGION &newRegion, SCREENID screenID = ID_SCREEN);
	virtual void RegionDefine(SCREENID screenID, int left, int top, int right, int bottom);
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn, int newLine,
		SCREENID oldScreenID = ID_SCREEN, SCREENID newScreenID = ID_SCREEN);
	virtual void Text(SCREENID screenID, int left, int top, const char *text,
		const UI_PALETTE *palette, int length = -1, int fill = TRUE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const char *string, SCREENID screenID = ID_SCREEN,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const char *string, SCREENID screenID = ID_SCREEN,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(SCREENID screenID);

	// ADVANCED functions for mouse and cursor --- DO NOT USE! ---
	virtual int DeviceMove(IMAGE_TYPE imageType, int newColumn, int newLine);
	virtual int DeviceSet(IMAGE_TYPE imageType, int column, int line, int width, int height, UCHAR *image);

protected:
	int maxColors;
	signed char _virtualCount;
	UI_REGION _virtualRegion;
	char _stopDevice;
	int _fillPattern;
	int _backgroundColor;
	int _foregroundColor;

	void SetFont(LOGICAL_FONT logicalFont);
	void SetPattern(const UI_PALETTE *palette, int _xor);
};
#endif

// ----- UI_MSWINDOWS_DISPLAY -----------------------------------------------

#if defined(ZIL_MSWINDOWS)
class EXPORT UI_MSWINDOWS_DISPLAY : public UI_DISPLAY
{
public:
	static HDC hDC;
	static HFONT fontTable[MAX_LOGICAL_FONTS];
	static WORD patternTable[MAX_LOGICAL_PATTERNS][8];

	UI_MSWINDOWS_DISPLAY(HANDLE hInstance, HANDLE hPrevInstance, int nCmdShow);
	virtual ~UI_MSWINDOWS_DISPLAY(void);
	virtual void Bitmap(SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const UCHAR *bitmapArray,
		const UI_PALETTE *palette = NULL, const UI_REGION *clipRegion = NULL,
		HBITMAP *colorBitmap = NULL, HBITMAP *monoBitmap = NULL);
	virtual void BitmapArrayToHandle(SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const UCHAR *bitmapArray, const UI_PALETTE *palette,
		HBITMAP *colorBitmap, HBITMAP *monoBitmap);
	virtual void BitmapHandleToArray(SCREENID screenID, HBITMAP colorBitmap,
		HBITMAP monoBitmap, int *bitmapWidth, int *bitmapHeight,
		UCHAR **bitmapArray);
	virtual void Ellipse(SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual void IconArrayToHandle(SCREENID screenID, int iconWidth,
		int iconHeight, const UCHAR *iconArray, const UI_PALETTE *palette,
		HICON *icon);
	virtual void IconHandleToArray(SCREENID screenID, HICON icon,
		int *iconWidth, int *iconHeight, UCHAR **iconArray);
	virtual void Line(SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void Polygon(SCREENID screenID, int numPoints, const int *polygonPoints,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual void Rectangle(SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1, int fill = FALSE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL);
	virtual void RectangleXORDiff(const UI_REGION &oldRegion, const UI_REGION &newRegion, SCREENID screenID = ID_SCREEN);
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn, int newLine,
		SCREENID oldScreenID = ID_SCREEN, SCREENID newScreenID = ID_SCREEN);
	virtual void Text(SCREENID screenID, int left, int top, const char *text,
		const UI_PALETTE *palette, int length = -1, int fill = TRUE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const char *string, SCREENID screenID = ID_SCREEN,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const char *string, SCREENID screenID = ID_SCREEN,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(SCREENID screenID);

protected:
	int maxColors;
};
#endif

// ----- UI_OS2_DISPLAY -----------------------------------------------------

#if defined(ZIL_OS2)
class EXPORT UI_OS2_DISPLAY : public UI_DISPLAY
{
public:
	static HPS hps;
	static FONTMETRICS fontTable[MAX_LOGICAL_FONTS];

	UI_OS2_DISPLAY(void);
	virtual ~UI_OS2_DISPLAY(void);
	virtual void Bitmap(SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const UCHAR *bitmapArray,
		const UI_PALETTE *palette = NULL, const UI_REGION *clipRegion = NULL,
		HBITMAP *colorBitmap = NULL, HBITMAP *monoBitmap = NULL);
	virtual void BitmapArrayToHandle(SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const UCHAR *bitmapArray, const UI_PALETTE *palette,
		HBITMAP *colorBitmap, HBITMAP *monoBitmap);
	virtual void BitmapHandleToArray(SCREENID screenID, HBITMAP colorBitmap,
		HBITMAP monoBitmap, int *bitmapWidth, int *bitmapHeight,
		UCHAR **bitmapArray);
	virtual void Ellipse(SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual void IconArrayToHandle(SCREENID screenID, int iconWidth,
		int iconHeight, const UCHAR *iconArray, const UI_PALETTE *palette,
		HICON *icon);
	virtual void IconHandleToArray(SCREENID screenID, HICON icon,
		int *iconWidth, int *iconHeight, UCHAR **iconArray);
	virtual void Line(SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void Polygon(SCREENID screenID, int numPoints, const int *polygonPoints,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual void Rectangle(SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1, int fill = FALSE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL);
	virtual void RectangleXORDiff(const UI_REGION &oldRegion, const UI_REGION &newRegion, SCREENID screenID = ID_SCREEN);
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn, int newLine,
		SCREENID oldScreenID = ID_SCREEN, SCREENID newScreenID = ID_SCREEN);
	virtual void Text(SCREENID screenID, int left, int top, const char *text,
		const UI_PALETTE *palette, int length = -1, int fill = TRUE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const char *string, SCREENID screenID = ID_SCREEN,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const char *string, SCREENID screenID = ID_SCREEN,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(SCREENID screenID);

protected:
	int maxColors;
};
#endif

// ----- UI_MOTIF_DISPLAY -----------------------------------------------------

#if defined(ZIL_MOTIF)
class EXPORT UI_MOTIF_DISPLAY : public UI_DISPLAY
{
public:
	struct MOTIFFONT {
		XFontStruct *fontStruct;
		XmFontList fontList;
	};

	static MOTIFFONT fontTable[MAX_LOGICAL_FONTS];

	UI_MOTIF_DISPLAY(int *argc = NULL, char **argv = NULL,
		char *appClass = "ZincApp", XrmOptionDescList options = NULL,
		Cardinal numOptions = 0, String *fallbackResources = NULL);
	virtual ~UI_MOTIF_DISPLAY(void);
	virtual void Bitmap(SCREENID screenID, int column, int line,
		int bitmapWidth, int bitmapHeight, const UCHAR *bitmapArray,
		const UI_PALETTE *palette = NULL, const UI_REGION *clipRegion = NULL,
		HBITMAP *colorBitmap = NULL, HBITMAP *monoBitmap = NULL);
	virtual void BitmapArrayToHandle(SCREENID screenID, int bitmapWidth,
		int bitmapHeight, const UCHAR *bitmapArray, const UI_PALETTE *palette,
		HBITMAP *colorBitmap, HBITMAP *monoBitmap);
	virtual void BitmapHandleToArray(SCREENID screenID, HBITMAP colorBitmap,
		HBITMAP monoBitmap, int *bitmapWidth, int *bitmapHeight,
		UCHAR **bitmapArray);
	virtual void Ellipse(SCREENID screenID, int column, int line,
		int startAngle, int endAngle, int xRadius, int yRadius,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual void IconArrayToHandle(SCREENID screenID, int iconWidth,
		int iconHeight, const UCHAR *iconArray, const UI_PALETTE *palette,
		HICON *icon);
	virtual void IconHandleToArray(SCREENID screenID, HICON icon,
		int *iconWidth, int *iconHeight, UCHAR **iconArray);
	virtual void Line(SCREENID screenID, int column1, int line1, int column2,
		int line2, const UI_PALETTE *palette, int width = 1, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual COLOR MapColor(const UI_PALETTE *palette, int isForeground);
	virtual void Polygon(SCREENID screenID, int numPoints, const int *polygonPoints,
		const UI_PALETTE *palette, int fill = FALSE, int _xor = FALSE,
		const UI_REGION *clipRegion = NULL);
	virtual void Rectangle(SCREENID screenID, int left, int top, int right,
		int bottom, const UI_PALETTE *palette, int width = 1, int fill = FALSE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL);
	virtual void RectangleXORDiff(const UI_REGION &oldRegion, const UI_REGION &newRegion, SCREENID screenID = ID_SCREEN);
	virtual void RegionDefine(SCREENID screenID, int left, int top, int right, int bottom);
	virtual void RegionMove(const UI_REGION &oldRegion, int newColumn, int newLine,
		SCREENID oldScreenID = ID_SCREEN, SCREENID newScreenID = ID_SCREEN);
	virtual void Text(SCREENID screenID, int left, int top, const char *text,
		const UI_PALETTE *palette, int length = -1, int fill = TRUE,
		int _xor = FALSE, const UI_REGION *clipRegion = NULL,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextHeight(const char *string, SCREENID screenID = ID_SCREEN,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int TextWidth(const char *string, SCREENID screenID = ID_SCREEN,
		LOGICAL_FONT font = FNT_DIALOG_FONT);
	virtual int VirtualGet(SCREENID screenID, int left, int top, int right, int bottom);
	virtual int VirtualPut(SCREENID screenID);

	void InstallPalette(UI_PALETTE *palette);

protected:
};
#endif

#endif // UI_DSP_HPP

