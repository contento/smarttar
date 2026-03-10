//	DISPLAY.CPP (DISPLAY) - Example program using UI_DISPLAY.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#define USE_RAW_KEYS
#include <ui_win.hpp>
#if defined(ZIL_MSDOS)
#	include <stdio.h>
#endif

int fill = FALSE;
#define EDGE 70

UI_PALETTE rectanglePalette =
	{ ' ', attrib(BLACK, LIGHTGRAY), attrib(MONO_NORMAL, MONO_BLACK),
	PTN_INTERLEAVE_FILL, BLACK, BLUE, BW_BLACK, BW_WHITE, GS_BLACK, GS_WHITE };
UI_PALETTE ellipsePalette =
	{ ' ', attrib(BLACK, LIGHTGRAY), attrib(MONO_NORMAL, MONO_BLACK),
	PTN_INTERLEAVE_FILL, RED, YELLOW, BW_BLACK, BW_WHITE, GS_BLACK, GS_WHITE };
UI_PALETTE polygonPalette =
	{ ' ', attrib(BLACK, LIGHTGRAY), attrib(MONO_NORMAL, MONO_BLACK),
	PTN_SOLID_FILL, MAGENTA, BROWN, BW_BLACK, BW_WHITE, GS_BLACK, GS_WHITE };
UI_PALETTE textPalette =
	{ ' ', attrib(BLACK, LIGHTGRAY), attrib(MONO_NORMAL, MONO_BLACK),
	PTN_SOLID_FILL, GREEN, LIGHTRED, BW_BLACK, BW_WHITE, GS_BLACK, GS_WHITE };
UI_PALETTE linePalette =
	{ ' ', attrib(BLACK, LIGHTGRAY), attrib(MONO_NORMAL, MONO_BLACK),
	PTN_INTERLEAVE_FILL, LIGHTGRAY, CYAN, BW_BLACK, BW_WHITE, GS_BLACK, GS_WHITE };

const EVENT_TYPE D_MOVE 		= 10000;
const EVENT_TYPE D_REDRAW 		= 10001;
static UI_EVENT_MAP eventMapTable[] =
{
#if defined(ZIL_MSDOS)
	{ ID_WINDOW_OBJECT, D_MOVE,		E_KEY, 		SPACE,		0 },
	{ ID_WINDOW_OBJECT, D_REDRAW,	E_KEY, 		ENTER,		0 },
#elif defined(ZIL_MSWINDOWS)
	{ ID_WINDOW_OBJECT, D_MOVE,		WM_CHAR, 	' ',		0 },
	{ ID_WINDOW_OBJECT, D_REDRAW,	WM_CHAR, 	ENTER,		0 },
#elif defined(ZIL_OS2)
	{ ID_WINDOW_OBJECT, D_MOVE,		WM_CHAR, 	' ',		0 },
	{ ID_WINDOW_OBJECT, D_REDRAW,	WM_CHAR, 	0x000D,		0 },
#elif defined(ZIL_MOTIF)
	{ ID_WINDOW_OBJECT,	D_MOVE,		KeyPress,	XK_space, 	0 },
	{ ID_WINDOW_OBJECT,	D_REDRAW,	KeyPress,	XK_Return, 	0 },
#endif

	// End of array.
	{ ID_END, 0, 0, 0 }
};

void DrawRectangle(UI_DISPLAY *display, SCREENID screenID, UI_REGION &clipRegion, int fill)
{
	// Top, left.
	display->Rectangle(screenID, clipRegion.left - EDGE,
		clipRegion.top - EDGE, clipRegion.left + EDGE,
		clipRegion.top + EDGE, &rectanglePalette, 1, fill, FALSE, &clipRegion);

	// Bottom, left.
	display->Rectangle(screenID, clipRegion.left - EDGE,
		clipRegion.bottom - EDGE, clipRegion.left + EDGE,
		clipRegion.bottom + EDGE, &rectanglePalette, 1, fill, FALSE, &clipRegion);

	// Bottom, right.
	display->Rectangle(screenID, clipRegion.right - EDGE,
		clipRegion.bottom - EDGE, clipRegion.right + EDGE,
		clipRegion.bottom + EDGE, &rectanglePalette, 1, fill, FALSE, &clipRegion);

	// Top, right.
	display->Rectangle(screenID, clipRegion.right - EDGE,
		clipRegion.top - EDGE, clipRegion.right + EDGE,
		clipRegion.top + EDGE, &rectanglePalette, 1, fill, FALSE, &clipRegion);

	// In the middle.
	display->Rectangle(screenID, clipRegion.left + EDGE,
		clipRegion.top + EDGE, clipRegion.right - EDGE,
		clipRegion.bottom - EDGE, &rectanglePalette, 5, fill, FALSE, &clipRegion);
}

void DrawEllipse(UI_DISPLAY *display, SCREENID screenID, UI_REGION &clipRegion, int fill)
{
	// Top, left.
	display->Ellipse(screenID, clipRegion.left, clipRegion.top,
		0, 360, EDGE, EDGE, &ellipsePalette, fill, FALSE, &clipRegion);

	// Bottom, left.
	display->Ellipse(screenID, clipRegion.left, clipRegion.bottom,
		0, 360, EDGE, EDGE, &ellipsePalette, fill, FALSE, &clipRegion);

	// Bottom, right.
	display->Ellipse(screenID, clipRegion.right, clipRegion.bottom,
		0, 360, EDGE, EDGE, &ellipsePalette, fill, FALSE, &clipRegion);

	// Top, right.
	display->Ellipse(screenID, clipRegion.right, clipRegion.top,
		0, 360, EDGE, EDGE, &ellipsePalette, fill, FALSE, &clipRegion);
}

void DrawPolygon(UI_DISPLAY *display, SCREENID screenID, UI_REGION &clipRegion, int fill)
{
	int pointsTL[8];	// Un-optimized for HP bug.
	pointsTL[0] = clipRegion.left + EDGE;
	pointsTL[1] = clipRegion.top + EDGE;
	pointsTL[2] = clipRegion.left - EDGE;
	pointsTL[3] = clipRegion.top;
	pointsTL[4] = clipRegion.left;
	pointsTL[5] = clipRegion.top - EDGE;
	pointsTL[6] = clipRegion.left + EDGE;
	pointsTL[7] = clipRegion.top + EDGE;

	int pointsBL[8];	// Un-optimized for HP bug.
	pointsBL[0] = clipRegion.left + EDGE;
	pointsBL[1] = clipRegion.bottom - EDGE;
	pointsBL[2] = clipRegion.left - EDGE;
	pointsBL[3] = clipRegion.bottom;
	pointsBL[4] = clipRegion.left;
	pointsBL[5] = clipRegion.bottom + EDGE;
	pointsBL[6] = clipRegion.left + EDGE;
	pointsBL[7] = clipRegion.bottom - EDGE;

	int pointsBR[8];	// Un-optimized for HP bug.
	pointsBR[0] = clipRegion.right - EDGE;
	pointsBR[1] = clipRegion.bottom - EDGE;
	pointsBR[2] = clipRegion.right + EDGE;
	pointsBR[3] = clipRegion.bottom;
	pointsBR[4] = clipRegion.right;
	pointsBR[5] = clipRegion.bottom + EDGE;
	pointsBR[6] = clipRegion.right - EDGE;
	pointsBR[7] = clipRegion.bottom - EDGE;

	int pointsTR[8];	// Un-optimized for HP bug.
	pointsTR[0] = clipRegion.right - EDGE;
	pointsTR[1] = clipRegion.top + EDGE;
	pointsTR[2] = clipRegion.right + EDGE;
	pointsTR[3] = clipRegion.top;
	pointsTR[4] = clipRegion.right;
	pointsTR[5] = clipRegion.top - EDGE;
	pointsTR[6] = clipRegion.right - EDGE;
	pointsTR[7] = clipRegion.top + EDGE;

	// Top, left.
	display->Polygon(screenID, 4, pointsTL, &polygonPalette, fill, FALSE, &clipRegion);

	// Bottom, left.
	display->Polygon(screenID, 4, pointsBL, &polygonPalette, fill, FALSE, &clipRegion);

	// Bottom, right.
	display->Polygon(screenID, 4, pointsBR, &polygonPalette, fill, FALSE, &clipRegion);

	// Top, right.
	display->Polygon(screenID, 4, pointsTR, &polygonPalette, fill, FALSE, &clipRegion);
}

void DrawBitmap(UI_DISPLAY *display, SCREENID screenID, UI_REGION &clipRegion)
{
	static UCHAR bitmapArray[] =
	{
#define _ BLACK,
#define O BACKGROUND,
#define I YELLOW,
		O O O O O O O O O O O O I I I I I I I I O O O O O O O O O O O O
		O O O O O O O O O I I I I I I I I I I I I I I O O O O O O O O O
		O O O O O O O I I I I I I I I I I I I I I I I I I O O O O O O O
		O O O O O O I I I I I I I I I I I I I I I I I I I I O O O O O O
		O O O O O I I I I I I I I I I I I I I I I I I I I I I O O O O O
		O O O O I I I I I I I I I I I I I I I I I I I I I I I I O O O O
		O O O I I I I I I I I I I I I I I I I I I I I I I I I I I O O O
		O O I I I I I I I I I I I I I I I I I I I I I I I I I I I I O O
		O O I I I I I I I _ _ _ I I I I I I I I _ _ _ I I I I I I I O O
		O I I I I I I I _ _ _ _ _ I I I I I I _ _ _ _ _ I I I I I I I O
		O I I I I I I I _ _ _ _ _ I I I I I I _ _ _ _ _ I I I I I I I O
		O I I I I I I I _ _ _ _ _ I I I I I I _ _ _ _ _ I I I I I I I O
		I I I I I I I I I _ _ _ I I I I I I I I _ _ _ I I I I I I I I I
		I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
		I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
		I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
		I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
		I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
		I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
		I I I I I _ _ I I I I I I I I I I I I I I I I I I _ _ I I I I I
		O I I I I _ _ I I I I I I I I I I I I I I I I I I _ _ I I I I O
		O I I I I _ _ _ I I I I I I I I I I I I I I I I _ _ _ I I I I O
		O I I I I I _ _ I I I I I I I I I I I I I I I I _ _ I I I I I O
		O O I I I I _ _ _ I I I I I I I I I I I I I I _ _ _ I I I I O O
		O O I I I I I _ _ _ _ I I I I I I I I I I _ _ _ _ I I I I I O O
		O O O I I I I I _ _ _ _ _ _ I I I I I _ _ _ _ _ I I I I I O O O
		O O O O I I I I I I _ _ _ _ _ _ _ _ _ _ _ _ I I I I I I O O O O
		O O O O O I I I I I I I I _ _ _ _ _ _ I I I I I I I I O O O O O
		O O O O O O I I I I I I I I I I I I I I I I I I I I O O O O O O
		O O O O O O O I I I I I I I I I I I I I I I I I I O O O O O O O
		O O O O O O O O O I I I I I I I I I I I I I I O O O O O O O O O
		O O O O O O O O O O O O I I I I I I I I O O O O O O O O O O O O
#undef _
#undef O
#undef I
	};

	// Top, left.
	display->Bitmap(screenID, clipRegion.left - 16,
		clipRegion.top - 16, 32, 32, bitmapArray, NULL, &clipRegion);

	// Bottom, left.
	display->Bitmap(screenID, clipRegion.left - 16,
		clipRegion.bottom - 16, 32, 32, bitmapArray, NULL, &clipRegion);

	// Bottom, right.
	display->Bitmap(screenID, clipRegion.right - 16,
		clipRegion.bottom - 16, 32, 32, bitmapArray, NULL, &clipRegion);

	// Top, right.
	display->Bitmap(screenID, clipRegion.right - 16,
		clipRegion.top - 16, 32, 32, bitmapArray, NULL, &clipRegion);

	// Center.
	display->Bitmap(screenID, clipRegion.left + (clipRegion.right - clipRegion.left) / 2 - 16,
		clipRegion.top + (clipRegion.bottom - clipRegion.top) / 2 - 16, 32, 32, bitmapArray);
}

void DrawText(UI_DISPLAY *display, SCREENID screenID, UI_REGION &clipRegion)
{
	char *string = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	// Top.
	display->Text(screenID, clipRegion.left +
		(clipRegion.right - clipRegion.left - display->TextWidth(string, screenID)) / 2,
		clipRegion.top - (display->TextHeight(string, screenID) / 2), string,
		&textPalette, -1, fill, FALSE, &clipRegion);

	// Left.
	display->Text(screenID, clipRegion.left - (display->TextWidth(string, screenID) / 2),
		clipRegion.top + (clipRegion.bottom - clipRegion.top) / 2 -
		display->TextHeight(string, screenID) / 2, string,
		&textPalette, -1, fill, FALSE, &clipRegion);

	// Right.
	display->Text(screenID, clipRegion.right - (display->TextWidth(string, screenID) / 2),
		clipRegion.top + (clipRegion.bottom - clipRegion.top) / 2 -
		display->TextHeight(string, screenID) / 2, string,
		&textPalette, -1, fill, FALSE, &clipRegion);

	// Bottom.
	display->Text(screenID, clipRegion.left +
		(clipRegion.right - clipRegion.left - display->TextWidth(string, screenID)) / 2,
		clipRegion.bottom - (display->TextHeight(string, screenID) / 2), string,
		&textPalette, -1, fill, FALSE, &clipRegion);
}

void DrawLines(UI_DISPLAY *display, SCREENID screenID, UI_REGION &clipRegion)
{
	// Top, left to bottom, right.
	display->Line(screenID, clipRegion.left - EDGE,
		clipRegion.top, clipRegion.right, clipRegion.bottom + EDGE,
		&linePalette, 1, FALSE, &clipRegion);

	// Bottom, right to top, left.
	display->Line(screenID, clipRegion.right + EDGE,
		clipRegion.bottom, clipRegion.left, clipRegion.top - EDGE,
		&linePalette, 1, FALSE, &clipRegion);

	// Bottom, left to top, right.
	display->Line(screenID, clipRegion.left - EDGE,
		clipRegion.bottom, clipRegion.right, clipRegion.top - EDGE,
		&linePalette, 1, FALSE, &clipRegion);

	// Top, right to bottom, left.
	display->Line(screenID, clipRegion.right + EDGE,
		clipRegion.top, clipRegion.left, clipRegion.bottom + EDGE,
		&linePalette, 1, FALSE, &clipRegion);
}

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	// Check for graphics display.
#if defined(ZIL_MSDOS)
	if (display->isText)
	{
		delete windowManager;
		delete eventManager;
		delete display;
		printf("ERROR: Initialization of the graphics display failed.\n");
		return (0);
	}
#endif

	// Create a window with a NON_FIELD_REGION text field.
//	UIW_WINDOW *mainWindow = UIW_WINDOW::Generic(5, 1, 70, 12, "DISPLAY");	// BUG.2230
	UIW_WINDOW *mainWindow = new UIW_WINDOW(5, 1, 70, 12, WOF_NO_FLAGS, WOAF_NO_SIZE);
	*mainWindow
		+ new UIW_BORDER
		+ new UIW_MINIMIZE_BUTTON
		+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
		+ new UIW_TITLE("DISPLAY");

	UI_WINDOW_OBJECT *clipWindow = new UIW_WINDOW(10, 2, 48, 8, WOF_BORDER);
	*mainWindow
		+ new UIW_PROMPT(1, 0, 66,
			"Press <ENTER> to draw, <SPACE> to shift, <ALT-F4> to exit", WOF_JUSTIFY_CENTER)
		+ clipWindow;

	// Put main window onto display.
	*windowManager
		+ mainWindow;

	// Wait for user response.
	UI_EVENT event;
	EVENT_TYPE ccode;
	do
	{
		eventManager->Get(event, Q_NORMAL);
		ccode = UI_EVENT_MAP::MapEvent(eventMapTable, event);
		if (ccode == D_MOVE)
		{
			UI_REGION moveRegion = clipWindow->true;

			display->RegionMove(moveRegion, moveRegion.left,
				moveRegion.top - display->cellHeight, mainWindow->screenID, mainWindow->screenID);
		}
		else if (ccode == D_REDRAW)
		{
			windowManager->Event(UI_EVENT(S_REDISPLAY, 0));

			UI_REGION clipRegion = clipWindow->true;
			SCREENID drawScreenID = clipWindow->screenID;
			fill = (fill == FALSE);

			display->VirtualGet(drawScreenID, clipRegion);

			// Draw a clipped rectangle.
			DrawRectangle(display, drawScreenID, clipRegion, fill);

			// Draw a clipped ellipse.
			DrawEllipse(display, drawScreenID, clipRegion, fill);

			// Draw a clipped polygon.
			DrawPolygon(display, drawScreenID, clipRegion, fill);

			// Draw clipped bitmaps.
			DrawBitmap(display, drawScreenID, clipRegion);

			// Draw clipped text.
			DrawText(display, drawScreenID, clipRegion);
	
			// Draw clipped lines.
			DrawLines(display, drawScreenID, clipRegion);

			display->VirtualPut(drawScreenID);
		}
		else
			ccode = windowManager->Event(event);
	} while(ccode != L_EXIT && ccode != S_NO_OBJECT);

	return (0);
}
