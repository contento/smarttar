//	ANALOG.CPP (ANALOG) - Analog clock example.
//	COPYRIGHT (C) 1990-1992.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <ui_win.hpp>
#if defined(ZIL_MSDOS)
#	include <stdio.h>
#endif

// Color palette definitions for analog clock face.
static UI_PALETTE grayPalette = { ' ', attrib(DARKGRAY, DARKGRAY),
	attrib(MONO_BLACK, MONO_BLACK), PTN_SOLID_FILL, DARKGRAY, DARKGRAY,
	BW_WHITE, BW_WHITE, GS_GRAY, GS_GRAY };

static UI_PALETTE blackPalette = { ' ', attrib(BLACK, BLACK),
	attrib(MONO_BLACK, MONO_BLACK), PTN_SOLID_FILL, BLACK, BLACK,
	BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK };

static UI_PALETTE cyanPalette = { ' ', attrib(CYAN, CYAN),
	attrib(MONO_BLACK, MONO_BLACK), PTN_SOLID_FILL, CYAN, CYAN,
	BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK };

static UI_PALETTE yellowPalette = { ' ', attrib(YELLOW, YELLOW),
	attrib(MONO_HIGH, MONO_HIGH), PTN_SOLID_FILL, YELLOW, YELLOW,
	BW_WHITE, BW_WHITE, GS_WHITE, GS_WHITE };

// Define the clock device event type.
const DEVICE_TYPE E_CLOCK = 80;

// Definition of psuedoSin (used so that the floating point library is not needed).
const int psuedoSin[] = {0, 1045, 2079, 3090, 4067, 5000, 5878, 6691,
	7431, 8090,	8660, 9135, 9510, 9781, 9945, 10000, 9945, 9781, 9510,
	9135, 8660, 8090, 7431, 6691, 5878, 5000, 4067, 3090, 2079, 1045,
	0, -1045, -2079, -3090, -4067, -5000, -5878, -6691, -7431, -8090,
	-8660, -9135, -9510, -9781, -9945, -10000, -9945, -9781, -9510,
	-9135, -8660, -8090, -7431, -6691, -5878, -5000, -4067, -3090,
	-2079, -1045, 0 };

class ANALOG_FACE : public UI_WINDOW_OBJECT
{
public:
	ANALOG_FACE(void) :
		UI_WINDOW_OBJECT(0, 0, 0, 0, WOF_NON_SELECTABLE | WOF_NON_FIELD_REGION, WOAF_NO_FLAGS) { }
	~ANALOG_FACE(void) { }

	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	void Face(void);
	void UpdateMinutes(int hour, int minute, int handsXOR);
	void UpdateSeconds(int second, int handsXOR);

private:
	int xCenter;
	int yCenter;
	int xRadius;
	int yRadius;
	int xLength;
	int yLength;
	int xSecondHand;
	int ySecondHand;
	int minuteHand[10];
	int hourHand[10];
};

class CLOCK : public UIW_WINDOW, public UI_DEVICE
{
public:
	CLOCK(int left, int top, int width, int height);
	~CLOCK(void) { }

	virtual EVENT_TYPE Event(const UI_EVENT &event);
	void Poll(void);

	UI_TIME time;
	ANALOG_FACE *analogFace;

private:
	int installed;
};

EVENT_TYPE ANALOG_FACE::DrawItem(const UI_EVENT &, EVENT_TYPE )
{
	// Redisplay the clock face and second hand.
	int hour, minute, second;
	((CLOCK *)parent)->time.Export(&hour, &minute, &second);

	display->VirtualGet(screenID, true);
	Face();
	UpdateMinutes(hour, minute, FALSE);
	UpdateSeconds(second, FALSE);
	display->VirtualPut(screenID);

	return (TRUE);
}

EVENT_TYPE ANALOG_FACE::Event(const UI_EVENT &event)
{
	// Translate the event into a logical event.
	EVENT_TYPE ccode = UI_WINDOW_OBJECT::LogicalEvent(event, ID_WINDOW_OBJECT);

	// Switch on the event.
	switch (ccode)
	{
	case S_INITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
		break;

	case S_CREATE:
	case S_SIZE:
		// Set clock object size to be maximum size allowed (uses WOF_NON_FIELD_REGION flag).
		UI_WINDOW_OBJECT::Event(event);

		// Set up center coords., clock hand lengths, and circle radius.
		xCenter = (true.left + true.right) / 2;
		yCenter = (true.top + true.bottom) / 2;
		xRadius = (true.right - true.left) / 15;
		yRadius = (true.bottom - true.top) / 15;
		xLength = (true.right - true.left) / 2 - xRadius;
		yLength = (true.bottom - true.top) / 2 - yRadius;

		woStatus |= WOS_OWNERDRAW;
		RegisterObject("ANALOG_FACE");
		break;

	default:
		// Let the base class process all other events.
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}
 	return (ccode);
}

void ANALOG_FACE::Face(void)
{
	// Return if no display area.
	if (true.left >= true.right || true.top >= true.bottom)
		return;

	// Fill in clock background.
	// Note that the coordinates are calculated based on the clocks
	// coordinates found in UI_REGION true.
	display->VirtualGet(screenID, true);
	// Unoptimized initialization because of HP compiler bug.
	int poly1[8];
	poly1[0] = true.left; poly1[1] = true.top; poly1[2] = true.right;
	poly1[3] = true.top; poly1[4] = true.left; poly1[5] = true.bottom + 1;
	poly1[6] = true.left; poly1[7] = true.top;

	int poly2[8];
	poly2[0] = true.left; poly2[1] = true.bottom + 1; poly2[2] = true.right;
	poly2[3] = true.top; poly2[4] = true.right; poly2[5] = true.bottom + 1;
	poly2[6] = true.left; poly2[7] = true.bottom + 1;

	display->Polygon(screenID, 4, poly1, &grayPalette, TRUE);
	display->Polygon(screenID, 4, poly2, &blackPalette, TRUE);
	display->Ellipse(screenID, xCenter, yCenter, 0, 360, xRadius, yRadius, &cyanPalette, TRUE);
	int x = xRadius / 2;
	int y = yRadius / 2;
	int poly3[8];
	poly3[0] = xCenter - x; poly3[1] = yCenter - y; poly3[2] = xCenter + x;
	poly3[3] = yCenter - y; poly3[4] = xCenter - x; poly3[5] = yCenter + y;
	poly3[6] = xCenter + x; poly3[7] = yCenter + y;

	display->Polygon(screenID, 4, poly3, &yellowPalette);

	// Add tick marks.
	x /= 2;
	y /= 2;
	display->Ellipse(screenID, xCenter, yCenter - yLength + y, 0, 360, x, y, &yellowPalette, TRUE);
	display->Ellipse(screenID, xCenter + xLength - x, yCenter, 0, 360, x, y, &yellowPalette, TRUE);
	display->Ellipse(screenID, xCenter, yCenter + yLength - y, 0, 360, x, y, &yellowPalette, TRUE);
	display->Ellipse(screenID, xCenter - xLength + x, yCenter, 0, 360, x, y, &yellowPalette, TRUE);
	display->VirtualPut(screenID);
}

void ANALOG_FACE::UpdateMinutes(int hour, int minute, int handsXOR)
{
	// Return if no display area.
	if (true.left >= true.right || true.top >= true.bottom)
		return;

	// Remove old hands if they exist.
	display->VirtualGet(screenID, true);
	if (handsXOR)
	{
		display->Polygon(screenID, 5, minuteHand, &cyanPalette, FALSE, TRUE);
		display->Polygon(screenID, 5, hourHand, &cyanPalette, FALSE, TRUE);
	}

	// Compute and draw the minute hand.
	long x1, y1;
	long poly4[10];
	poly4[0] = 0; poly4[1] = 0; poly4[2] = -3; poly4[3] = 20; poly4[4] = 0;
	poly4[5] = 100; poly4[6] = 3; poly4[7] = 20; poly4[8] = 0; poly4[9] = 0;

	int cosRad = (15 + minute) % 60;
	for (int i = 0; i < 10; i+=2)
	{
		x1 = (poly4[i] * psuedoSin[ cosRad ] +
			poly4[i+1] * psuedoSin[ minute ]) * xLength /  1000000L;
		minuteHand[i] = xCenter + (int)x1;
		y1 = (poly4[i] * psuedoSin[ minute ] -
			poly4[i+1] * psuedoSin[ cosRad ] ) * yLength / 1000000L;
		minuteHand[i+1] = yCenter + (int)y1;
	}
	display->Polygon(screenID, 5, minuteHand, &cyanPalette, FALSE, TRUE);

	// Compute and draw the hour hand.
	int sinRad = (hour * 5) % 60 + minute / 12;
	cosRad = (15 + sinRad) % 60;
	long poly5[10];
	poly5[0] = 0; poly5[1] = 0; poly5[2] = -3; poly5[3] = 20; poly5[4] = 0;
	poly5[5] = 50; poly5[6] = 3; poly5[7] = 20; poly5[8] = 0; poly5[9] = 0;

	for (i = 0; i < 10; i+=2)
	{
		x1 = (poly5[i] * psuedoSin[ cosRad ] +
			poly5[i+1] * psuedoSin[ sinRad ]) * xLength /  1000000L;
		hourHand[i] = xCenter + (int)x1;
		y1 = (poly5[i] * psuedoSin[ sinRad ] -
			poly5[i+1] * psuedoSin[ cosRad ] ) * yLength / 1000000L;
		hourHand[i+1] = yCenter + (int)y1;
	}
	display->Polygon(screenID, 5, hourHand, &cyanPalette, FALSE, TRUE);
	display->VirtualPut(screenID);
}

void ANALOG_FACE::UpdateSeconds(int second, int handsXOR)
{
	// Return if no display area.
	if (true.left >= true.right || true.top >= true.bottom)
		return;

	// Remove old hands if they exist.
	display->VirtualGet(screenID, true);
	if (handsXOR)
		display->Line(screenID, xCenter, yCenter, xSecondHand, ySecondHand, &cyanPalette, 1, TRUE);

	// Compute and draw the second hand.
	xSecondHand = (int)(xCenter + 1L * psuedoSin[ second ] * xLength /  10000L);
	ySecondHand = (int)(yCenter - 1L * psuedoSin[ (second + 15) % 60 ] * yLength / 10000L);
	display->Line(screenID, xCenter, yCenter, xSecondHand, ySecondHand, &cyanPalette, 1, TRUE);
	display->VirtualPut(screenID);
}

CLOCK::CLOCK(int left, int top, int width, int height) :
	UIW_WINDOW(left, top, width, height, WOF_NO_FLAGS, WOAF_NO_DESTROY),
	UI_DEVICE(E_CLOCK, D_ON), installed(0)
{
	// Create analog clock object and add to the window.
	analogFace = new ANALOG_FACE();

	// Add other window objects to the window.
	*this
		+ new UIW_BORDER
		+ new UIW_MAXIMIZE_BUTTON
		+ new UIW_MINIMIZE_BUTTON
		+ UIW_SYSTEM_BUTTON::Generic()
		+ new UIW_TITLE("Zinc Application Framework Time", WOF_JUSTIFY_CENTER)
		+ analogFace;
}

EVENT_TYPE CLOCK::Event(const UI_EVENT &event)
{
	EVENT_TYPE returnValue;

	// Switch on the event type.
	// Note: This Event member function is used by both the UIW_WINDOW and
	// UI_DEVICE base classes of CLOCK.
	switch (event.type)
	{
	case E_DEVICE:
	case E_CLOCK:
		// Turn the clock on or off.
		switch (event.rawCode)
		{
		case D_OFF:
		case D_ON:
			state = event.rawCode;
			break;
		}
		returnValue = state;
		break;

	case S_CREATE:
		// Add the clock to the event manager.
		if (!installed)
		{
			installed = TRUE;
			*UI_WINDOW_OBJECT::eventManager + (UI_DEVICE *)this;
		}
		returnValue = UIW_WINDOW::Event(event);
		break;

	case S_DEINITIALIZE:
		// Add the clock to the event manager.
		if (installed)
		{
			installed = FALSE;
			*UI_WINDOW_OBJECT::eventManager - (UI_DEVICE *)this;
		}

		returnValue = UIW_WINDOW::Event(event);
		break;

	default:
		// Process window messages.
		returnValue = UIW_WINDOW::Event(event);
	}

	return(returnValue);
}

void CLOCK::Poll(void)
{
	int hour, minute, second;
	int oldHour, oldMinute, oldSecond;

	// Check to see if the clock is on.
	UI_EVENT event;
	if (state != D_ON)
		return;
#if defined (ZIL_MSWINDOWS)
	// If the face has invalid regions then don't do anything to it.
	RECT tRect;
	if (GetUpdateRect(analogFace->screenID, &tRect, FALSE))
		return;
#endif

	// Get the current time.
	UI_TIME newTime;
	newTime.Export(&hour, &minute, &second);

	// Check to see if the time has changed.
	time.Export(&oldHour, &oldMinute, &oldSecond);
	if (oldSecond != second)
	{
		// Update the second hand (and minute hand if needed).
		if (oldMinute != minute || oldHour != hour)
			analogFace->UpdateMinutes(hour, minute, TRUE);
		analogFace->UpdateSeconds(second, TRUE);

		time.Import();
	}
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

	// Add a clock to the window manager.
	*windowManager + new CLOCK(0, 0, 50, 15);

	// Wait for user response.
	UI_EVENT event;
	EVENT_TYPE ccode;
	do
	{
		// Get input from the user.
		if (!eventManager->Get(event, Q_NO_BLOCK | Q_BEGIN | Q_DESTROY | Q_POLL))
			ccode = windowManager->Event(event);
	} while (ccode != L_EXIT && ccode != S_NO_OBJECT);

	return (0);
}

