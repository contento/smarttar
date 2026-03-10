//	Zinc Interface Library - Z_DSP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_dsp.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UI_DISPLAY ---------------------------------------------------------

UI_DISPLAY::UI_DISPLAY(int _isText, const char *_operatingSystem,
	const char *_windowingSystem) : installed(FALSE), isText(_isText),
	isMono(FALSE), columns(0), lines(0), cellWidth(1), cellHeight(1),
	preSpace(2), postSpace(2), miniNumeratorX(1), miniDenominatorX(10),
	miniNumeratorY(1), miniDenominatorY(10)
{
#if defined(ZIL_LINKBUG)
	extern void z_display_dummy(void);		// Bug fix for broken linkers.
	z_display_dummy();
#endif

	operatingSystem = ui_strdup(_operatingSystem);
	windowingSystem = ui_strdup(_windowingSystem);

	UI_INTERNATIONAL::Initialize();

	// Set up the default color maps.
	for (int i = 0; i < MAX_DISPLAY_IMAGES; i++)
	{
		displayImage[i].image = displayImage[i].screen = displayImage[i].backup = NULL;
		displayImage[i].region.left = displayImage[i].region.right = -1;
		displayImage[i].region.top = displayImage[i].region.bottom = -1;
	}
}

UI_DISPLAY::~UI_DISPLAY(void)
{
	if (operatingSystem)
		delete operatingSystem;
	if (windowingSystem)
		delete windowingSystem;

	for (int i = 0; i < MAX_DISPLAY_IMAGES; i++)
	{
		if (displayImage[i].screen)
			delete displayImage[i].screen;
		if (displayImage[i].backup)
			delete displayImage[i].backup;
	}
}

void UI_DISPLAY::Bitmap(SCREENID, int, int, int, int, const UCHAR *,
	const UI_PALETTE *, const UI_REGION *, HBITMAP *, HBITMAP *)
{
}

void UI_DISPLAY::BitmapArrayToHandle(SCREENID, int, int, const UCHAR *, 
	const UI_PALETTE *, HBITMAP *, HBITMAP *)
{
}

void UI_DISPLAY::BitmapHandleToArray(SCREENID, HBITMAP, HBITMAP, int *, 
	int *, UCHAR **)
{
}

void UI_DISPLAY::Ellipse(SCREENID, int, int, int, int, int, int,
	const UI_PALETTE *, int, int, const UI_REGION *)
{
}

void UI_DISPLAY::IconArrayToHandle(SCREENID, int, int, const UCHAR *, 
	const UI_PALETTE *, HICON *)
{
}

void UI_DISPLAY::IconHandleToArray(SCREENID, HICON, int *, int *, UCHAR **)
{
}

void UI_DISPLAY::Line(SCREENID, int, int, int, int, const UI_PALETTE *,
	int, int, const UI_REGION *)
{
}

COLOR UI_DISPLAY::MapColor(const UI_PALETTE *, int)
{
	return (0);
}

void UI_DISPLAY::Polygon(SCREENID, int, const int *, const UI_PALETTE *,
	int, int, const UI_REGION *)
{
}

void UI_DISPLAY::Rectangle(SCREENID, int, int, int, int, const UI_PALETTE *,
	int, int, int, const UI_REGION *)
{
}

void UI_DISPLAY::RectangleXORDiff(const UI_REGION &, const UI_REGION &,
	SCREENID)
{
}

void UI_DISPLAY::RegionDefine(SCREENID, int, int, int, int)
{
}

void UI_DISPLAY::RegionMove(const UI_REGION &, int, int, SCREENID, SCREENID)
{
}

void UI_DISPLAY::Text(SCREENID, int, int, const char *, const UI_PALETTE *,
	int, int, int, const UI_REGION *, LOGICAL_FONT)
{
}

int UI_DISPLAY::TextHeight(const char *, SCREENID, LOGICAL_FONT)
{
	return (1);
}

int UI_DISPLAY::TextWidth(const char *, SCREENID, LOGICAL_FONT)
{
	return (1);
}

int UI_DISPLAY::VirtualGet(SCREENID, int, int, int, int)
{
	return (FALSE);
}

int UI_DISPLAY::VirtualPut(SCREENID)
{
	return (FALSE);
}

int UI_DISPLAY::DeviceMove(IMAGE_TYPE, int, int)
{
	return (TRUE);
}

int UI_DISPLAY::DeviceSet(IMAGE_TYPE, int, int, int, int, UCHAR *)
{
	return (TRUE);
}

int UI_DISPLAY::RegionInitialize(UI_REGION &region,
	const UI_REGION *clipRegion, int left, int top, int right, int bottom)
{
	region.left = Max(0, left);
	region.top = Max(0, top);
	region.right = Min(columns - 1, right);
	region.bottom = Min(lines - 1, bottom);
	if (clipRegion)
	{
		region.left = Max(region.left, clipRegion->left);
		region.top = Max(region.top, clipRegion->top);
		region.right = Min(region.right, clipRegion->right);
		region.bottom = Min(region.bottom, clipRegion->bottom);
	}
	return (region.left <= region.right && region.top <= region.bottom);
}
