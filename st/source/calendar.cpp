//
// [ CALENDAR.CPP ]
//

#include "stdst.h"

#include <calendar.h>
#include <events.h>

extern CFG *g_cfg;

// ---------------------------------------------------------------------------
// 								CALENDAR
// ---------------------------------------------------------------------------

static const char *CAL_TITLE_FMT = "Días Festivos [%s %d]";

const OBJECTID ID_CALENDAR = 3504;

UI_EVENT_MAP CALENDAR::calendarEvent[] =
    {
        { ID_CALENDAR,	UE_PREVIOUS,	E_KEY, WHITE_PGUP },
        { ID_CALENDAR,	UE_PREVIOUS,	E_KEY, GRAY_PGUP },
        { ID_CALENDAR,	UE_NEXT,  		E_KEY, GRAY_PGDN },
        { ID_CALENDAR,	UE_NEXT,	  	E_KEY, WHITE_PGDN },
        // End of array.
        { ID_END, 0, 0, 0 }
    };

CALENDAR::CALENDAR(WORD left, WORD top, WORD width, WORD height)
        : UIW_WINDOW(left, top, width, height)
{
    // make a local copy of hollydays
	memcpy(Hollydays, g_cfg->Hollydays, sizeof(CALENDAR_ENTRY)*MAX_HOLLY_YEARS);
    // Create and set the days of month object.
    char title[0x30];

    Date.Export((int *)&Year, (int *)&Month, (int *)&Day);
    CurrentYear = Year;

    sprintf(title, CAL_TITLE_FMT, UI_DATE::monthTable[(Month-1)*2+1], Year);
    // Create the calendar title.
    CalendarTitle = new UIW_TITLE(title, WOF_JUSTIFY_CENTER);
    *this
    + new UIW_BORDER
    + new UIW_SYSTEM_BUTTON(SYF_GENERIC)
    + CalendarTitle
    ;
    *this
    + &(* new UIW_TOOL_BAR(0, 0, width, 1)
        + new UIW_PROMPT(0, 0, 5, " &Día ")
        + (WDay = new UIW_INTEGER(0, 0, 4, (int *)&Day))
        + new UIW_BUTTON(0, 0, 10, "Ad&icionar", BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE, WOF_JUSTIFY_CENTER, NULL, UE_ADD_SDAY)
        + new UIW_BUTTON(0, 0, 10, "&Eliminar", BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE, WOF_JUSTIFY_CENTER, NULL, UE_DEL_SDAY)
        + new UIW_BUTTON(0, 0, 10, "Mes &Sig.", BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE, WOF_JUSTIFY_CENTER, NULL, UE_NEXT)
        + new UIW_BUTTON(0, 0, 10, "Mes A&nt.", BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE, WOF_JUSTIFY_CENTER, NULL, UE_PREVIOUS)
        + new UIW_BUTTON(0, 0, 10, "&Aceptar", BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE, WOF_JUSTIFY_CENTER, NULL, UE_ACCEPT)
        + new UIW_BUTTON(0, 0, 10, "&Cancelar", BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE, WOF_JUSTIFY_CENTER, NULL, UE_CANCEL)
       )
    + (DaysOfMonth = new DAYS_OF_MONTH())
    ;
    WORD tInt = 3;
    WDay->Information(SET_TEXT_LENGTH, &tInt);
    // Sets the CALENDAR class Date -- defaults to the system Date.

    DaysOfMonth->Date = Date;

    this->woAdvancedFlags |= WOAF_NO_SIZE | WOAF_MODAL | WOAF_DIALOG_OBJECT;
    windowManager->Center(this);
}

EVENT_TYPE CALENDAR::Event(const UI_EVENT &event)
{
    // Switch on the event type.
    EVENT_TYPE ccode = UI_EVENT_MAP::MapEvent(calendarEvent, event, ID_CALENDAR);
    switch (ccode)
    {
    case UE_ACCEPT:
        // accept table as valid
		memcpy(g_cfg->Hollydays, Hollydays, sizeof(CALENDAR_ENTRY)*MAX_HOLLY_YEARS);
		g_cfg->Save(NULL, FALSE);
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        //
        UI_EVENT tmpEvent;
        tmpEvent.type = E_CONTROLLER;
        // use the raw code to put service
        tmpEvent.rawCode = UE_REFRESH_STBAR;
        eventManager->Put(tmpEvent);
        //
        break;
    case UE_CANCEL:
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    case UE_ADD_SDAY:
        if (AddHollyday(Year, Month, WDay->DataGet()))
            DaysOfMonth->Event(UI_EVENT(S_REDISPLAY, 0));
        break;
    case UE_DEL_SDAY:
        if (SubtractHollyday(Year, Month, WDay->DataGet()))
            DaysOfMonth->Event(UI_EVENT(S_REDISPLAY, 0));
        break;
    case L_PGUP:
    case L_PGDN:
    case UE_NEXT:
    case UE_PREVIOUS:
        if (ccode == UE_NEXT)
        {
            // avoid to go too many years ahead !!!
            if (Month==12 && Year==(CurrentYear+MAX_HOLLY_YEARS-1))
                break;
            if (++Month > 12)
            {
                Month = 1;
                ++Year;
            }
        }
        else
        { // UE_PREVIOUS
            // avoid to go before current Year !!!
            if (Month == 1 && Year == CurrentYear)
                break;
            if (--Month < 1)
            {
                Month = 12;
                --Year;
            }
        }
        Date.Import(Year, Month, 1);
        Date.Export((int *)&Year, (int *)&Month, NULL, 0);
        char title[0x30];
        // Setup the new Date with only the Month and the Year.
        // This is done to setup the title string.
        Date.Import(Year, Month, 1);
        // Get the Month and Year string for the title.
        sprintf(title, CAL_TITLE_FMT, UI_DATE::monthTable[(Month-1)*2+1], Year);
        // Reset the Date to the current Date.
        Date.Import(Year, Month, Day);
        // Change the title to show new Month.
        CalendarTitle->DataSet(title);
        DaysOfMonth->Date = Date;
        DaysOfMonth->Event(UI_EVENT(S_REDISPLAY, 0));
        ccode = UIW_WINDOW::Event(event);
        break;
    default:
        // Call the window event to process other events.
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    // Return the control code.
    return (ccode);
}

BOOL CALENDAR::AddHollyday(WORD year, WORD month, WORD day)
{
    for (WORD i=0; i<MAX_HOLLY_YEARS; i++)
        if (Hollydays[i].Year == year)
        {
            for (WORD j=0; j<MAX_HOLLYDAYS; j++)
            {
                if (Hollydays[i].Table[month-1][j] == day)
                    return FALSE; // avoid repeated days
                if (!Hollydays[i].Table[month-1][j])
                { // empty slot
                    Hollydays[i].Table[month-1][j] = day;
                    return TRUE;
                }
            }
            // the year exist but there is no space for more days !!!
            break;
        }
    // the year was not found.  Recycle the older year.
    // classical FindMin
    WORD older = 0;
    for (i=0; i<MAX_HOLLY_YEARS; i++)
        if (Hollydays[i].Year < Hollydays[older].Year)
            older = i;
    //
    memset(&Hollydays[older], 0, sizeof(CALENDAR_ENTRY));
    Hollydays[older].Year = year;
    Hollydays[older].Table[month-1][0] = day;
    return TRUE; // still here !!!
}



BOOL CALENDAR::SubtractHollyday(WORD year, WORD month, WORD day)
{
    for (WORD i=0; i<MAX_HOLLY_YEARS; i++)
        if (Hollydays[i].Year == year)
        {
            for (WORD j=0; j<MAX_HOLLYDAYS; j++)
                if (Hollydays[i].Table[month-1][j] == day)
                {
                    Hollydays[i].Table[month-1][j] = 0;
                    return TRUE;
                }
            break;
        }
    return FALSE; // still here !!!
}



BOOL CALENDAR::IsHollyday(WORD year, WORD month, WORD day)
{
    for (WORD i=0; i<MAX_HOLLY_YEARS; i++)
        if (Hollydays[i].Year == year)
        {
            for (WORD j=0; j<MAX_HOLLYDAYS; j++)
                if (Hollydays[i].Table[month-1][j] == day)
                    return TRUE;
            break;
        }
    return FALSE; // still here !!!
}

// ---------------------------------------------------------------------------
// 		DAYS_OF_MONTH
// ---------------------------------------------------------------------------



EVENT_TYPE DAYS_OF_MONTH::Event(const UI_EVENT &event)
{
    // Switch on the event type.
    EVENT_TYPE ccode = UI_WINDOW_OBJECT::Event(event);
    switch (ccode)
    {
    case S_CREATE:
    case S_SIZE:
        ccode = UI_WINDOW_OBJECT::Event(event);
        true   -= display->cellWidth;
        Width  = true.Width();
        Height = true.Height() - display->cellHeight;
        // Register the object with Windows API.
        woStatus |= WOS_OWNERDRAW;
        RegisterObject("DAYS_OF_MONTH");
        break;
    }
    // Return the control code.
    return (ccode);
}

// Special palettes
static UI_PALETTE dayPalette = {
    ' ', attrib(BLACK, WHITE), attrib(MONO_BLACK, MONO_BLACK), PTN_SOLID_FILL,
    BLACK, WHITE, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK
};

UI_PALETTE _hollydayPalette = {
    ' ', attrib(BLUE, LIGHTGRAY), attrib(MONO_BLACK, MONO_BLACK), PTN_SOLID_FILL,
    WHITE, LIGHTGRAY, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK
};

UI_PALETTE _weekendPalette = {
    ' ', attrib(YELLOW, LIGHTGRAY), attrib(MONO_BLACK, MONO_BLACK), PTN_SOLID_FILL,
    YELLOW, LIGHTGRAY, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK
};

static UI_PALETTE linePalette = {
    ' ', attrib(BLACK, LIGHTGRAY), attrib(MONO_BLACK, MONO_BLACK), PTN_SOLID_FILL,
    BLACK, LIGHTGRAY, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK
};

static UI_PALETTE backPalette = {
    ' ', attrib(LIGHTGRAY, LIGHTGRAY), attrib(MONO_HIGH, MONO_HIGH), PTN_SOLID_FILL,
    LIGHTGRAY, LIGHTGRAY, BW_WHITE, BW_WHITE, GS_WHITE, GS_WHITE
};

EVENT_TYPE DAYS_OF_MONTH::DrawItem(const UI_EVENT &, EVENT_TYPE )
{
    // Get the Current Day
    WORD currentDay;
    WORD year, month;
    Date.Export((int *)&year, (int *)&month, (int *)&currentDay);
    // Find the starting Day.
    Date.Import(year, month, 1);
    WORD startingDay = Date.DayOfWeek();
    // Now set Date information.
    Date.Import(year, month, currentDay);
    WORD daysInMonth = Date.DaysInMonth();
    WORD weeksinMonth = (startingDay + daysInMonth-1) / 7;
    if ((startingDay + daysInMonth-1) % 7)
        weeksinMonth++;
    // Change cellWidth and cellHeights
    WORD cellWidth = Width / 7;
    WORD cellHeight = Height / weeksinMonth;
    if (cellHeight == 0)
        cellHeight++;
    WORD i, top = 1;
    WORD left = startingDay;
    WORD middle = cellWidth - cellWidth / 2;
    // Draw the calendar in the middle of the window.
    UI_REGION region = true;
    WORD offset = (region.right - region.left) % 7;
    region.left += offset / 2;
    region.right -= offset / 2;
    region.top += display->cellHeight;
    if (!display->isText)
    {
        offset = (region.bottom - region.top) % 7;
        region.top += offset / 2;
        region.bottom -= offset / 2;
    }
    display->VirtualGet(screenID, true);
    display->Rectangle(screenID, true, &backPalette, 1, TRUE);
    if (Height > weeksinMonth && Width >= 28)
        display->Rectangle(screenID, region, &linePalette);
    for (i=0; i<7; i++)
    {
        char *s = UI_DATE::dayTable[i*2];
        WORD column = region.left + i*cellWidth + middle - display->TextWidth(s)/2+1;
        WORD line = region.top - display->TextHeight(s);
        display->Text(screenID, column, line, s, &linePalette, -1, FALSE, FALSE, &clip);
    }
    if (!display->isText)
    {
        for (i = 0;  i < weeksinMonth; i++)
            display->Line(screenID, region.left, region.top+i*cellHeight, region.right,
                          region.top+i*cellHeight, &linePalette);
        for (i = 1;  i <= 6; i++)
            display->Line(screenID, region.left+i*cellWidth, region.top,
                          region.left+i*cellWidth, region.bottom, &linePalette);
    }
    else if (Height <= weeksinMonth)
        region.top --;
    // Now print the days of the month;
    char dayStr[3];
    UI_DATE tmpDate(year, month, 1);
    UI_PALETTE *palette;
    for (i = 1;  i <= daysInMonth; i++)
    {
        ui_itoa(i, dayStr, 10);
        palette = &dayPalette;
        if (((CALENDAR *)parent)->IsHollyday(year, month, i))
            palette = &_hollydayPalette;
        else if (_IsWeekend(year, month, i))
            palette = &_weekendPalette;
        display->Text(screenID, region.left+left*cellWidth-middle-display->TextWidth(dayStr)/2,
                      region.top+top*cellHeight-cellHeight/2-display->TextHeight(dayStr)/2, dayStr, palette, -1, FALSE);
        //
        if (left % 7 == 0)
        {
            left = 1;
            top++;
        }
        else
            left++;
        ++tmpDate;
    }
    display->VirtualPut(screenID);
    return (TRUE);
}
