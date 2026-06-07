//
// [ W_STATBR.CPP ]
//

#include "stdst.h"

#ifdef DOSX286
#include <phapi.h>
#endif

#include <events.h>
#include <w_statbr.h>

extern CFG 	*g_cfg;

// $$$ just compile date and time for DOS, not for others

const int HELP_OFFSET    = 2;
const TMF_FLAGS TM_FLAGS = TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL;
const DTF_FLAGS DT_FLAGS = DTF_SHORT_DAY|DTF_DAY_OF_WEEK|DTF_ALPHA_MONTH|DTF_EUROPEAN_FORMAT|DTF_SHORT_MONTH;

static UI_PALETTE helpPalette = {
    ' ', attrib(BLACK, LIGHTGRAY), attrib(MONO_BLACK, MONO_BLACK), PTN_SOLID_FILL,
    BLACK, LIGHTGRAY, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK
};

static UI_PALETTE msgPalette = {
    ' ', attrib(BLACK, LIGHTGRAY), attrib(MONO_BLACK, MONO_BLACK), PTN_SOLID_FILL,
    BLACK, LIGHTGRAY, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK
};

extern UI_PALETTE _weekendPalette;
extern UI_PALETTE _hollydayPalette;

HELP_INFO *UIW_STAT_BAR::HelpInfo = NULL;

UIW_STAT_BAR::UIW_STAT_BAR(void) :
        UI_WINDOW_OBJECT(0, 0, 0, 0, WOF_NON_SELECTABLE|WOF_NON_FIELD_REGION|WOF_BORDER, WOAF_NO_FLAGS),
        HelpContext(0)
{
    searchID = windowID[0] = ID_STAT_BAR;
    StringID("UIW_STAT_BAR");
    HelpText = NULL;
    HelpPalette = &helpPalette;
    //
    UI_TIME time;
    time.Export(TimeStr, TM_FLAGS);
    UI_DATE date;
    date.Export(DateStr, DT_FLAGS);
    //
    MsgCount = 0;
    Msg[0]   = '\0';
    MsgPalette = &msgPalette;
}

#if defined(ZIL_MSDOS)
EVENT_TYPE UIW_STAT_BAR::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
    return (ccode);
}

EVENT_TYPE UIW_STAT_BAR::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = LogicalEvent(event);
    switch (ccode)
    {
    case S_CREATE:
    case S_SIZE:
        {
            UI_WINDOW_OBJECT::Event(event);
            true.left--;
            true.right++;
            true.top = ++true.bottom - display->cellHeight + 1;
            int packedDate;
            DatePalette = LogicalPalette(ccode, ID_BUTTON);
            packedDate = _GetSysDate();
			if (g_cfg->IsHollyday(packedDate))
                DatePalette = &_hollydayPalette;
            else if (_IsWeekend(packedDate))
                DatePalette = &_weekendPalette;
#if defined(__TEST__)
            strcpy(RecStr, "99999999");
#else
sprintf(RecStr, "%8ld", g_cfg->N_RECEIPT);
#endif
            break;
        }
    case S_DISPLAY_ACTIVE:
    case S_DISPLAY_INACTIVE:
        {
            UI_WINDOW_OBJECT::Event(event);
            if (FlagSet(woStatus, WOS_REDISPLAY))
            {
                refresh(ccode, TRUE);
                woStatus &= ~WOS_REDISPLAY;
            }
            break;
        }
    case UE_REFRESH:
        refresh(ccode);
        break;
    case UE_REFRESH_STBAR:
        refresh(ccode, TRUE);
        woStatus |= WOS_REDISPLAY;
        UI_WINDOW_OBJECT::Event(event);
        break;
    default:
        ccode = UI_WINDOW_OBJECT::Event(event);
        break;
    }
    return (ccode);
}

void UIW_STAT_BAR::Update(UI_WINDOW_OBJECT *helpObject)
{
    if (helpObject && helpObject->helpContext != HelpContext)
    {
        HelpContext = helpObject->helpContext;
        setHelpInfo(HelpContext);
        Event(UI_EVENT(UE_REFRESH,0));
    }
}

void UIW_STAT_BAR::refresh(EVENT_TYPE ccode, BOOL all)
{
    UI_REGION region = true;
    if (FlagSet(woFlags, WOF_BORDER) && all)
        DrawBorder(screenID, region, FALSE, ccode);
    UI_PALETTE *palette = LogicalPalette(ccode, ID_BUTTON);
    if (all)
        display->Rectangle(screenID, region, palette, 0, TRUE, FALSE, &clip);
    const int TIME_LEN =  5*display->cellWidth;
    const int DATE_LEN = 18*display->cellWidth;
    const int REC_LEN  =  8*display->cellWidth;
    // --- help and messages
    region = true;
    region.left += display->cellWidth;
    region.top += HELP_OFFSET;
    region.right -= (REC_LEN+TIME_LEN+DATE_LEN+8*display->cellWidth);
    region.bottom -= (HELP_OFFSET+1);
    if (all)
    {
        if (MsgCount)
        {
            UI_ERROR_SYSTEM::Beep();
            drawDipMessage(region, Msg, MsgPalette, ccode);
            MsgCount--;
        }
        else
            drawDipMessage(region, HelpText, HelpPalette, ccode);
    }
    else
    {
        if (MsgCount)
        {
            UI_ERROR_SYSTEM::Beep();
            drawDipMessage(region, Msg, MsgPalette, ccode);
            MsgCount--;
        }
        else
        {
            static char *helpText = HelpText;
            if (HelpText && (helpText == NULL || strcmp(helpText, HelpText)))
            {
                helpText = HelpText;
                drawDipMessage(region, HelpText, HelpPalette, ccode);
            }
        }
    }
    // --- record info
    region.left = (region.right+display->cellWidth);
    region.right = region.left+REC_LEN+2*display->cellWidth;
    if (all)
        drawDipMessage(region, RecStr, palette, ccode);
    else
    {
#if !defined(__TEST__)
		static long nReceipt = g_cfg->N_RECEIPT;
		if (nReceipt != g_cfg->N_RECEIPT)
        {
			nReceipt = g_cfg->N_RECEIPT;
            sprintf(RecStr, "%8ld", nReceipt); // v.219a
            drawDipMessage(region, RecStr, palette, ccode);
        }
#endif
    }
    // --- time
    region.left = (region.right+display->cellWidth);
    region.right = region.left+TIME_LEN+display->cellWidth;
    if (all)
        drawDipMessage(region, TimeStr, palette, ccode);
    else
    {
        static char timeStr[0x0A] =
            {""
            };
        UI_TIME time;
        time.Export(TimeStr, TM_FLAGS);
        if (strcmp(timeStr, TimeStr))
        {
            strcpy(timeStr, TimeStr);
            //
            // to eval waits
            //
			if (TraceInfo::s_nSemWait)
				itoa(TraceInfo::s_nSemWait, TimeStr, 10);
            //
            // end of eval ...
            //
            drawDipMessage(region, TimeStr, palette, ccode);
        }
    }
    // --- date
    region.left = (region.right+display->cellWidth);
    region.right = true.right-display->cellWidth;
    if (all)
        drawDipMessage(region, DateStr, DatePalette, ccode);
    else
    {
        static UI_DATE lastDate;
        UI_DATE date;
        if (lastDate != date)
        {
            lastDate = date;
            date.Export(DateStr, DT_FLAGS);
            int packedDate;
            date.Export(&packedDate);
            DatePalette = LogicalPalette(ccode, ID_BUTTON);
            if (g_cfg->IsHollyday(packedDate))
                DatePalette = &_hollydayPalette;
            else if (_IsWeekend(packedDate))
                DatePalette = &_weekendPalette;
            drawDipMessage(region, DateStr, DatePalette, ccode);
        }
    }
}

void UIW_STAT_BAR::setMsg(char *msg, int foreground, int background, int msgCount)
{
    strncpy(Msg, msg, sizeof(Msg)-1);
    Msg[sizeof(Msg)-1] = '\0';
    MsgPalette->colorAttribute  = attrib(foreground, background);
    MsgPalette->colorForeground = foreground;
    MsgPalette->colorBackground = background;
    MsgCount = msgCount;
}

void UIW_STAT_BAR::drawDipMessage(UI_REGION& region, const char *msg, UI_PALETTE *palette, EVENT_TYPE& ccode)
{
    UI_PALETTE *dipPalette;
    UI_REGION dipRegion = region;
    dipPalette = LogicalPalette(ccode, ID_DARK_SHADOW);
    display->Line(screenID, dipRegion.left, dipRegion.bottom - 1,
                  dipRegion.left, dipRegion.top, dipPalette, 1, FALSE, &clip);
    dipPalette = LogicalPalette(ccode, ID_DARK_SHADOW);
    display->Line(screenID, dipRegion.left, dipRegion.top,
                  dipRegion.right - 1, dipRegion.top, dipPalette, 1, FALSE, &clip);
    dipPalette = LogicalPalette(ccode, ID_WHITE_SHADOW);
    display->Line(screenID, dipRegion.right, dipRegion.top,
                  dipRegion.right, dipRegion.bottom, dipPalette, 1, FALSE, &clip);
    display->Line(screenID, dipRegion.right, dipRegion.bottom,
                  dipRegion.left, dipRegion.bottom, dipPalette, 1, FALSE, &clip);
    dipRegion.left  += (2*HELP_OFFSET);
    dipRegion.top++;
    dipRegion.right -= (2*HELP_OFFSET);
    dipRegion.bottom--;
    DrawText(screenID, dipRegion, msg, palette, TRUE, ccode);
}

void UIW_STAT_BAR::setHelpInfo(UI_HELP_CONTEXT theHelpContext)
{
    int ti = 0;
    if (!HelpInfo)
        return;
    do
    {
        if (HelpInfo[ti].Context == theHelpContext)
        {
            HelpText = HelpInfo[ti].Text;
            HelpPalette->colorAttribute  = attrib(HelpInfo[ti].Foreground, HelpInfo[ti].Background);
            HelpPalette->colorForeground = HelpInfo[ti].Foreground;
            HelpPalette->colorBackground = HelpInfo[ti].Background;
        }
        ++ti;
    }
    while (HelpInfo[ti].Context);
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
    UIW_STAT_BAR *object = (UIW_STAT_BAR *)GetWindowLong(hWnd, _helpbarOffset);
    return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
#endif

EVENT_TYPE UIW_STAT_BAR::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
    return (ccode);
}

EVENT_TYPE UIW_STAT_BAR::Event(const UI_EVENT &event)
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
        true.left--;
        true.right++;
        true.top = ++true.bottom - display->cellHeight + 1;
        if (ccode == S_CREATE)
#if defined(WIN32)
            RegisterObject("UIW_STAT_BAR", "STATIC", &_helpbarCallback);
#else
RegisterObject("UIW_STAT_BAR", "STATIC", &_helpbarOffset,
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

            // Draw the Text.
            region.left += HELP_OFFSET;
            region.top++;
            region.right -= HELP_OFFSET;
            region.bottom--;
            SetTextColor(hDC, RGB_BLACK);
            SetBkMode(hDC, TRANSPARENT);
            ::DrawText(hDC, (LPSTR)Text, ui_strlen(Text), &region,
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
EVENT_TYPE UIW_STAT_BAR::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
    static UI_PALETTE helpPalette = {
        ' ', 0, 0, PTN_SYSTEM_COLOR, SYSCLR_OUTPUTTEXT,
        SYSCLR_FIELDBACKGROUND, 0, 0, 0, 0
    };

    // Virtualize the display.
    UI_REGION region = true;
    display->VirtualGet(screenID, region);

    // Fill the object region.
    display->Rectangle(screenID, region, &helpPalette, 0, TRUE);

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

    // Draw the Text.
    DrawText(screenID, region, Text, NULL, FALSE, ccode);

    // Update the display.
    display->VirtualPut(screenID);
    return (TRUE);
}

EVENT_TYPE UIW_STAT_BAR::Event(const UI_EVENT &event)
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
        RegisterObject("UIW_STAT_BAR", WC_STATIC, &baseCallback, Text);
        break;
    }

    // Return the control code.
    return (ccode);
}
#elif defined(ZIL_MOTIF)
EVENT_TYPE UIW_STAT_BAR::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
    nargs = 0;
    char *tptr;
    if (!Text)															// BUG.SUN
        Text = "";
    XtSetArg(args[nargs], XmNvalue, &tptr);
    nargs++;
    XtGetValues(screenID, args, nargs);
    if (strcmp(tptr, Text))
    {
        nargs = 0;
        XtSetArg(args[nargs], XmNvalue, Text);
        nargs++;
        XtSetValues(screenID, args, nargs);
    }
    return (ccode);
}

EVENT_TYPE UIW_STAT_BAR::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW_OBJECT);
    switch (ccode)
    {
    case S_SIZE:
    case S_CREATE:
        ccode = UI_WINDOW_OBJECT::Event(event);
        true.top = true.bottom - display->cellHeight;
        woStatus |= WOS_OWNERDRAW;
        font = FNT_DIALOG_FONT;
        XtSetArg(args[nargs], XmNeditable, FALSE);
        nargs++;
        XtSetArg(args[nargs], XmNcursorPositionVisible, FALSE);
        nargs++;
        XtSetArg(args[nargs], XmNmarginHeight, 3);
        nargs++;
        XtSetArg(args[nargs], XmNmarginWidth, 3);
        nargs++;
        XtSetArg(args[nargs], XmNvalue, Text ? Text : "");
        nargs++;			// BUG.SUN
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
