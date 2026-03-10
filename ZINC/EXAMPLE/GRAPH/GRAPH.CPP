//	GRAPH.CPP (GRAPH) - Extended graphics example.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <stdlib.h>
#include <ui_win.hpp>
#if defined(ZIL_MSDOS)
#	include <stdio.h>
#endif

// Converts MiniCells to pixels
#define MiniConvertY(y) ((int)((y) * display->cellHeight * display->miniNumeratorY / display->miniDenominatorY))
#define MiniConvertX(x) ((int)((x) * display->cellWidth * display->miniNumeratorX / display->miniDenominatorX))

// Palettes used for drawing graphics.
UI_PALETTE graphPalette[] =
{
	{ '\260', attrib(BLACK, BLACK), attrib(MONO_DIM, MONO_BLACK),
		PTN_INTERLEAVE_FILL, BLACK, RED, BW_BLACK, BW_BLACK, GS_GRAY, GS_GRAY },
	{ '\260',attrib( BLACK, BLACK), attrib(MONO_DIM, MONO_BLACK),
		PTN_INTERLEAVE_FILL, BLACK, YELLOW, BW_BLACK, BW_BLACK, GS_GRAY, GS_GRAY },
	{ '\260', attrib(BLACK, BLACK), attrib(MONO_DIM, MONO_BLACK),
		PTN_INTERLEAVE_FILL, BLACK, GREEN, BW_BLACK, BW_BLACK, GS_GRAY, GS_GRAY },
	{ '\260', attrib(BLACK, BLACK), attrib(MONO_DIM, MONO_BLACK),
		PTN_INTERLEAVE_FILL, BLACK, BLUE, BW_BLACK, BW_BLACK, GS_GRAY, GS_GRAY },
	{ '\260', attrib(BLACK, BLACK), attrib(MONO_DIM, MONO_BLACK),
		PTN_INTERLEAVE_FILL, BLACK, LIGHTRED, BW_BLACK, BW_WHITE,	GS_GRAY, GS_GRAY },
	{ '\260', attrib(BLACK, BLACK), attrib(MONO_DIM, MONO_BLACK),
		PTN_SOLID_FILL, BLACK, LIGHTGREEN, BW_BLACK, BW_WHITE, GS_GRAY, GS_GRAY },
	{ '\260', attrib(BLACK, BLACK), attrib(MONO_DIM, MONO_BLACK),
		PTN_SOLID_FILL, BLACK, DARKGRAY, BW_BLACK, BW_BLACK, GS_GRAY, GS_GRAY },
	{ '\260', attrib(BLACK, BLACK), attrib(MONO_DIM, MONO_BLACK),
		PTN_SOLID_FILL, RED, DARKGRAY, BW_BLACK, BW_BLACK, GS_GRAY, GS_GRAY },
	{ '\260', attrib(BLACK, BLACK), attrib(MONO_DIM, MONO_BLACK),
		PTN_SOLID_FILL, GREEN, DARKGRAY, BW_BLACK, BW_BLACK, GS_GRAY, GS_GRAY }
};

UI_PALETTE textPalette =
{ '\260', attrib(BLACK, BLACK), attrib(MONO_DIM, MONO_BLACK),
	PTN_INTERLEAVE_FILL, BLACK, RED, BW_BLACK, BW_BLACK, GS_GRAY, GS_GRAY
};

class LINE_CHART : public UIW_WINDOW
{
public:
	LINE_CHART(int left, int top, int width, int height, WOF_FLAGS woFlags = WOF_BORDER);

	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
};

LINE_CHART::LINE_CHART(int left, int top, int width, int height, WOF_FLAGS woFlags)
	: UIW_WINDOW(left, top, width, height, woFlags, WOAF_NO_FLAGS)
{
	woStatus |= WOS_OWNERDRAW;
}

EVENT_TYPE LINE_CHART::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	display->VirtualGet(screenID, true);

	UIW_WINDOW::DrawItem(event, ccode);

	// Initialize data for line graph.
	static int redLine[] = { 25, 20, 37, 48, 86, 79, 93, 0 };
	static int greenLine[] = { 20, 30, 35, 58, 50, 65, 80, 0 };
	const int left = true.left,
		bottom = true.bottom,
		width = 4 * display->cellWidth;

	// Create the graph lines for lineWindow.
	for (int x = 0; redLine[x+1]; x++)
		display->Line(screenID, left + x * width, bottom - MiniConvertY(redLine[x]),
			left + (x + 1) * width, bottom - MiniConvertY(redLine[x+1]),
			&graphPalette[7], 1, FALSE, &clip);

	for (x = 0; greenLine[x+1]; x++)
		display->Line(screenID, left + x * width, bottom - MiniConvertY(greenLine[x]),
			left + (x + 1) * width, bottom - MiniConvertY(greenLine[x+1]),
			&graphPalette[8], 1, FALSE, &clip);

	// Draw the associated text.
	static char *month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", NULL };
	for (x = 0; month[x]; x++)
		display->Text(screenID, left + x * width + 1, bottom - 
			display->cellHeight + display->preSpace + display->postSpace,
			month[x], &textPalette, -1, FALSE, FALSE, &clip);

	display->VirtualPut(screenID);
	return (TRUE);
}

class BAR_CHART : public UIW_WINDOW
{
public:
	BAR_CHART(int left, int top, int width, int height, WOF_FLAGS woFlags);

	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
};

BAR_CHART::BAR_CHART(int left, int top, int width, int height, WOF_FLAGS woFlags)
	: UIW_WINDOW(left, top, width, height, woFlags, WOAF_NO_FLAGS)
{
	woStatus |= WOS_OWNERDRAW;
}

EVENT_TYPE BAR_CHART::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	// Set up the bar chart data.
	static const int BAR_COUNT = 6;
	static int height[BAR_COUNT] = { 67, 46, 88, 81, 65, 32 };
	static char *year[BAR_COUNT] = { "1985", "1986", "1987", "1988", "1989", "1990" };

	// Virtualize the display.
	display->VirtualGet(screenID, true);

	// Draw the outer border and clear the region.
	UIW_WINDOW::DrawItem(event, ccode);

	// Compute the draw region then draw the bars.
	int bottom = true.top + MiniConvertY(100);
	int spacing = 2 * display->cellWidth;
	int barWidth = (true.Width() - (BAR_COUNT + 1) * spacing) / BAR_COUNT;
	int left = true.left + spacing;
	for (int i = 0; i < BAR_COUNT; i++)
	{
		display->Rectangle(screenID, left, bottom - MiniConvertY(height[i]),
			left + barWidth - 1, bottom, &graphPalette[i], 1, TRUE, FALSE, &clip);
		display->Text(screenID, left, bottom + display->cellHeight, year[i],
			&graphPalette[6], -1, FALSE, FALSE, &clip);
		left += barWidth + spacing;
	}

	// Un-virtualize the display.
	display->VirtualPut(screenID);
	return (TRUE);
}

class PIE_CHART : public UIW_WINDOW
{
public:
	PIE_CHART(int left, int top, int width, int height,
		WOF_FLAGS flags = WOF_NO_FLAGS);

	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
};

PIE_CHART::PIE_CHART(int left, int top, int width, int height, WOF_FLAGS flags)
	: UIW_WINDOW(left, top, width, height, flags, WOAF_NO_FLAGS)
{
	woStatus |= WOS_OWNERDRAW;
}

EVENT_TYPE PIE_CHART::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	display->VirtualGet(screenID, true);

	UIW_WINDOW::DrawItem(event, ccode);

	static int sectors[] = { 0, 56, 112, 133, 210, 240, 360, 0 };

	// The following values are dependant on the above values.
	//  cosX[i] = 100 * cos((sector[i] + sector[i + 1]) / 2);
	//  cosX[i] = 100 * sin((sector[i] + sector[i + 1]) / 2);
	static int cosX[] = { 88, 10, -54, -99, -71,  50 };
	static int sinY[] = { 47, 99,  84,  15, -71, -87 };

	static char *label[] = { "Rent", "Food", "Clothing", "Auto", "Other", "Entertainment" };
	UI_POSITION center;
	center.column = true.Width() / 2;
	center.line = true.Height() / 2;

	display->Ellipse(screenID, true.left + center.column,
		true.top + center.line + display->cellHeight, 0, 360,
		center.column/2, center.line/2, &graphPalette[6], TRUE, FALSE, &clip);

	for (int sector = 0; sectors[sector+1]; sector++)
	{
		display->Ellipse(screenID, true.left + center.column, true.top + center.line,
			sectors[sector], sectors[sector+1], center.column/2, center.line/2,
			&graphPalette[sector], TRUE, FALSE, &clip);
//		int left = cosX[sector] * center.column / 2 / 100;					// BUG.2025
		long left = (long)cosX[sector] * center.column / 2 / 100;
		left += left > 0 ? display->cellWidth + true.left + center.column:
			-(display->TextWidth(label[sector], screenID, font) + display->cellWidth)
			+ true.left + center.column;
//		int top = sinY[sector] * center.line / 2 / 100;						// BUG.2025
		long top = (long)sinY[sector] * center.line / 2 / 100;
		top = top > 0 ? true.top + center.line - top - display->TextHeight(label[sector], screenID, font) :
			true.top + center.line - top;
		display->Text(screenID, left, top, label[sector],
			&graphPalette[sector], -1, FALSE, FALSE, &clip, font);
	}

	display->VirtualPut(screenID);
	return (TRUE);
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
		delete windowManager; windowManager = NULL;
		delete eventManager; eventManager = NULL;
		delete display; display = NULL;
		printf("ERROR: Initialization of the graphics display failed.\n");
		return (0);
	}
#endif

	// Add the windows to the window manager.
	*windowManager
		+ &(*UIW_WINDOW::Generic(25, 3, 45, 16, "BAR")
			+ new BAR_CHART(20, 10, 380, 100 + display->cellHeight,
				WOF_MINICELL | WOF_BORDER))
		+ &(*UIW_WINDOW::Generic(15, 12, 60, 8, "SIZABLE PIE CHART")
			+ new PIE_CHART(0, 0, 0, 0, WOF_NON_FIELD_REGION))
		+ &(*UIW_WINDOW::Generic(34, 7, 52, 8, "PIE CHART")
			+ new PIE_CHART(0, 0, 50, 7))
		+ &(*UIW_WINDOW::Generic(7, 2, 30, 14, "LINE CHART")
			+ new LINE_CHART(20, 10, 240, 100, WOF_MINICELL | WOF_BORDER));

	// Process user responses.
	UI_APPLICATION::Control();

	return (0);
}
