//	Zinc Interface Library Designer - Z_HLPBAR.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_WINSTATICS			// OS/2 object definition.
#include <string.h>
#include "ui_dsn.hpp"
#if defined(ZIL_MOTIF)
#include <Xm/Text.h>
#elif defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

HELP_BAR::HELP_BAR(void) :
	UI_WINDOW_OBJECT(0, 0, 0, 0, WOF_NON_SELECTABLE | WOF_NON_FIELD_REGION | WOF_BORDER, WOAF_NO_FLAGS),
	helpContext(0)
{
	searchID = windowID[0] = ID_HELP_BAR;
	StringID("HELP_BAR");
	text = NULL;
}

#if defined(ZIL_MSDOS)
EVENT_TYPE HELP_BAR::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	return (ccode);
}

EVENT_TYPE HELP_BAR::Event(const UI_EVENT &event)
{
	const int HELP_OFFSET = 2;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event);
	switch (ccode)
	{
	case S_CREATE:
	case S_SIZE:
		UI_WINDOW_OBJECT::Event(event);
		if (display->isText)
			true.top = true.bottom;
		else
		{
			true.left--; true.right++;
			true.top = ++true.bottom - display->cellHeight + 1;
		}
		break;

	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(woStatus, WOS_REDISPLAY) && display->isText)
		{
			UI_REGION region = true;
			UI_PALETTE *palette = LogicalPalette(ccode, ID_BUTTON);
			DrawText(screenID, region, text, palette, TRUE, ccode);
		}
		else if (FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_REGION region = true;
			if (FlagSet(woFlags, WOF_BORDER))
				DrawBorder(screenID, region, FALSE, ccode);
			UI_PALETTE *palette = LogicalPalette(ccode, ID_BUTTON);
			display->Rectangle(screenID, region, palette, 0, TRUE, FALSE, &clip);
			region.left += display->cellWidth;
			region.top += HELP_OFFSET;
			region.right -= display->cellWidth;		
			region.bottom -= (HELP_OFFSET + 1);
			palette = LogicalPalette(ccode, ID_DARK_SHADOW);
			display->Line(screenID, region.left, region.bottom - 1,
				region.left, region.top, palette, 1, FALSE, &clip);
			display->Line(screenID, region.left, region.top,
				region.right - 1, region.top, palette, 1, FALSE, &clip);
			palette = LogicalPalette(ccode, ID_WHITE_SHADOW);
			display->Line(screenID, region.right, region.top,
				region.right, region.bottom, palette, 1, FALSE, &clip);
			display->Line(screenID, region.right, region.bottom,
				region.left, region.bottom, palette, 1, FALSE, &clip);
			region.left += HELP_OFFSET; region.top++;
			region.right -= HELP_OFFSET; region.bottom--;
			palette = LogicalPalette(ccode, ID_BUTTON);
			DrawText(screenID, region, text, palette, TRUE, ccode);
			woStatus &= ~WOS_REDISPLAY;
		}
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}
#elif defined(ZIL_MSWINDOWS)
static int _helpbarOffset = -1;
#if defined(WIN32)
static WNDPROC _helpbarCallback = NULL;
#else
static FARPROC _helpbarCallback = (FARPROC)DefWindowProc;
static FARPROC _helpbarJumpInstance = NULL;

long FAR PASCAL _export HelpbarJumpProcedure(HWND hWnd, WORD wMsg, WORD wParam, LONG lParam)
{
	HELP_BAR *object = (HELP_BAR *)GetWindowLong(hWnd, _helpbarOffset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
#endif

EVENT_TYPE HELP_BAR::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	return (ccode);
}

EVENT_TYPE HELP_BAR::Event(const UI_EVENT &event)
{
	const int HELP_OFFSET = 1;

	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_INITIALIZE:
#if !defined(WIN32)
		if (!_helpbarJumpInstance)
			_helpbarJumpInstance = (FARPROC)HelpbarJumpProcedure;
#endif
		UI_WINDOW_OBJECT::Event(event);
		break;

	case S_SIZE:
	case S_CREATE:
		UI_WINDOW_OBJECT::Event(event);
		true.left--; true.right++;
		true.top = ++true.bottom - display->cellHeight + 1;
		if (ccode == S_CREATE)
#if defined(WIN32)
			RegisterObject("HELP_BAR", "STATIC", &_helpbarCallback);
#else
			RegisterObject("HELP_BAR", "STATIC", &_helpbarOffset,
				&_helpbarJumpInstance, &_helpbarCallback, NULL);
#endif
		break;

	default:
		WORD message = event.message.message;
		if ((ccode == S_REDISPLAY && screenID) ||
			(event.type == E_MSWINDOWS && message == WM_PAINT))
		{
			if (ccode == S_REDISPLAY)
				InvalidateRect(screenID, NULL, FALSE);
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(screenID, &ps);
			RECT region;
			GetClientRect(screenID, &region);

			// Fill the background.
			HBRUSH fillBrush = CreateSolidBrush(RGB_LIGHTGRAY);
			FillRect(hDC, &region, fillBrush);
			DeleteObject(fillBrush);

			// Draw the shadow.
			region.left += display->cellWidth;
			region.top += HELP_OFFSET;
			region.right -= display->cellWidth;		
			region.bottom -= (HELP_OFFSET + 1);
			HPEN darkShadow = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
			HPEN oldPen = SelectObject(hDC, darkShadow);
#if defined(WIN32)
			MoveToEx(hDC, region.left, region.bottom - 1, 0);
#else
			MoveTo(hDC, region.left, region.bottom - 1);
#endif
			LineTo(hDC, region.left, region.top);
			LineTo(hDC, region.right, region.top);
			SelectObject(hDC, oldPen);
			DeleteObject(darkShadow);
			HPEN lightShadow = GetStockObject(WHITE_PEN);
			oldPen = SelectObject(hDC, lightShadow);
			LineTo(hDC, region.right, region.bottom);
			LineTo(hDC, region.left - 1, region.bottom);
			SelectObject(hDC, oldPen);

			// Draw the text.
			region.left += HELP_OFFSET; region.top++;
			region.right -= HELP_OFFSET; region.bottom--;
			SetTextColor(hDC, RGB_BLACK);
			SetBkMode(hDC, TRANSPARENT);
			::DrawText(hDC, (LPSTR)text, ui_strlen(text), &region,
				DT_SINGLELINE | DT_VCENTER | DT_LEFT);
			EndPaint(screenID, &ps);
		}
		else if (event.type == E_MSWINDOWS && message == WM_ERASEBKGND)
			return (TRUE);
		else
			ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}
#elif defined(ZIL_OS2)
EVENT_TYPE HELP_BAR::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	static UI_PALETTE _helpPalette =
	{
		' ', 0, 0, PTN_SYSTEM_COLOR, SYSCLR_OUTPUTTEXT,
		SYSCLR_FIELDBACKGROUND, 0, 0, 0, 0
	};

	// Virtualize the display.
	UI_REGION region = true;
	display->VirtualGet(screenID, region);

	// Fill the object region.
	display->Rectangle(screenID, region, &_helpPalette, 0, TRUE);

	// Draw the outer shadow.
	UI_PALETTE *outline = LogicalPalette(ccode, ID_OUTLINE);
	display->Line(screenID, region.left, region.top, region.right, region.top, outline);
	int xOffset = display->cellWidth;
	int yOffset = display->cellWidth / 2;
	region.left += xOffset + 1;
	region.top += yOffset + 1;
	region.right -= xOffset;
	region.bottom -= yOffset;

	// Draw the inner shadow.
	UI_PALETTE *lightShadow = LogicalPalette(ccode, ID_WHITE_SHADOW);
	UI_PALETTE *darkShadow = LogicalPalette(ccode, ID_DARK_SHADOW);
	display->Line(screenID, region.left, region.top + 1, region.left,
		region.bottom - 1, darkShadow, 1, FALSE);
	display->Line(screenID, region.left, region.top, region.right,
		region.top, darkShadow, 1, FALSE);
	display->Line(screenID, region.right, region.top + 1, region.right,
		region.bottom, lightShadow, 1, FALSE);
	display->Line(screenID, region.left, region.bottom, region.right - 1,
		region.bottom, lightShadow, 1, FALSE);
	--region;
	region.left += xOffset;

	// Draw the text.
	DrawText(screenID, region, text, NULL, FALSE, ccode);

	// Update the display.
	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE HELP_BAR::Event(const UI_EVENT &event)
{
	static PFNWP baseCallback = NULL;

	// Switch on the event type.
	EVENT_TYPE ccode = UI_WINDOW_OBJECT::Event(event);
	switch (ccode)
	{
	case S_SIZE:
	case S_CREATE:
		true.top = true.bottom - display->cellHeight + 1;
		woStatus |= WOS_OWNERDRAW;
		font = FNT_SMALL_FONT;
		RegisterObject("UIW_HELPBAR", WC_STATIC, &baseCallback, text);
		break;
	}

	// Return the control code.
	return (ccode);
}
#elif defined(ZIL_MOTIF)
EVENT_TYPE HELP_BAR::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	nargs = 0;
	char *tptr;
	if (!text)															// BUG.SUN
		text = "";
	XtSetArg(args[nargs], XmNvalue, &tptr); nargs++;
	XtGetValues(screenID, args, nargs);
	if (strcmp(tptr, text))
	{
		nargs = 0;
		XtSetArg(args[nargs], XmNvalue, text); nargs++;
		XtSetValues(screenID, args, nargs);
	}
	return (ccode);
}

EVENT_TYPE HELP_BAR::Event(const UI_EVENT &event)
{

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW_OBJECT);
	switch (ccode)
	{
	case S_SIZE:
	case S_CREATE:
		ccode = UI_WINDOW_OBJECT::Event(event);
		true.top = true.bottom - display->cellHeight;
		woStatus |= WOS_OWNERDRAW;
		font = FNT_DIALOG_FONT;
		XtSetArg(args[nargs], XmNeditable, FALSE); nargs++;
		XtSetArg(args[nargs], XmNcursorPositionVisible, FALSE); nargs++;
		XtSetArg(args[nargs], XmNmarginHeight, 3); nargs++;
		XtSetArg(args[nargs], XmNmarginWidth, 3); nargs++;
		XtSetArg(args[nargs], XmNvalue, text ? text : ""); nargs++;			// BUG.SUN
		RegisterObject(NULL, XmCreateText, ccode, TRUE);
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}
#endif

void HELP_BAR::Update(UI_WINDOW_OBJECT *helpObject)
{
	if (helpObject && helpObject->helpContext != helpContext)
	{
		helpContext = helpObject->helpContext;
		text = HelpBarMessage(helpContext);
		if (screenID)
			HELP_BAR::Event(UI_EVENT(S_REDISPLAY, 0));
	}
}

