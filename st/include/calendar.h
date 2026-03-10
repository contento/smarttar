#ifndef __CAL_H
#define __CAL_H

#define USE_RAW_KEYS
#if !defined(UI_WIN_HPP)
#include <ui_win.hpp>
#endif

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

class DAYS_OF_MONTH;

class EXPORT CALENDAR : public UIW_WINDOW
{
public:
    CALENDAR(WORD left, WORD top, WORD width, WORD height);
    virtual ~CALENDAR(void)
    {}
    //


    virtual EVENT_TYPE Event(const UI_EVENT &event);
    BOOL IsHollyday(WORD year, WORD month, WORD day);
private:
    static UI_EVENT_MAP calendarEvent[];
    DAYS_OF_MONTH *DaysOfMonth;
    UIW_TITLE     *CalendarTitle;
    UIW_INTEGER   *WDay;
    //
    // local copy to keep integrity !!!
    //
    CALENDAR_ENTRY Hollydays[MAX_HOLLY_YEARS];
    BOOL AddHollyday     (WORD year, WORD month, WORD day);
    BOOL SubtractHollyday(WORD year, WORD month, WORD day);
    //
    UI_DATE Date;
    WORD Day;
    WORD Month;
    WORD Year;
    WORD CurrentYear;
};

class EXPORT DAYS_OF_MONTH : public UI_WINDOW_OBJECT
{
public:
    DAYS_OF_MONTH(void) : UI_WINDOW_OBJECT(0, 0, 0, 0, WOF_NON_FIELD_REGION|WOF_NON_SELECTABLE, WOAF_NO_FLAGS)
    {}


    virtual ~DAYS_OF_MONTH(void)
    {}
    //


    UI_DATE Date;
    virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    WORD Width, Height;
};

#endif // __CAL_H
