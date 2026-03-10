#ifndef __MENUBAR_H
#define __MENUBAR_H

#if !defined(__STRING_H)
#include <string.h>
#endif

#if !defined(UI_WIN_HPP)
#include <ui_win.hpp>
#endif

#if !defined(__PH_ENG_H)
#include <ph_eng.h>
#endif

#if !defined(__W_PHONE_H)
#include <w_phone.h>
#endif

class EXPORT UIW_TIME_DATE : public UIW_WINDOW
{
public:
    UIW_TIME_DATE(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_TIMES : public UIW_WINDOW
{
public:
    UIW_TIMES(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_LOCK_NUM : public UIW_WINDOW
{
public:
    UIW_LOCK_NUM(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_NAL_TAR : public UIW_WINDOW
{
public:
    UIW_NAL_TAR(BOOL select = TRUE);
    //
    virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    UIW_GROUP   *WTarGroup;
    UIW_PROMPT  *WTarPrompts[MAX_DDN_TARIFF];
    UIW_REAL    *WTars      [MAX_DDN_TARIFF];
    UIW_BUTTON  *WIVAButtons[MAX_DDN_TARIFF];
    UIW_GROUP   *WSchGroup;
    UIW_TIME    *WfromTimes[MAX_DDN_DAY_TYPE][MAX_DDN_SCHEDULE];
    UIW_TIME    *WtoTimes  [MAX_DDN_DAY_TYPE][MAX_DDN_SCHEDULE];
    UIW_INTEGER *WPercents [MAX_DDN_DAY_TYPE][MAX_DDN_SCHEDULE];
    UIW_BIGNUM  *WTax;
    UIW_BUTTON  *WBApply;
    UIW_BUTTON  *WEApply;
    //
    BOOL Select;
};

class EXPORT UIW_INTER_TAR : public UIW_WINDOW
{
public:
    UIW_INTER_TAR(BOOL select = TRUE);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    UIW_GROUP   *WFullGroup;
    UIW_GROUP   *WUSAGroup;
    UIW_GROUP   *WOthersGroup;
    UIW_GROUP   *WBorderGroup;
    //
    UIW_PROMPT  *WTarPrompts[MAX_DDI_TARIFF];
    UIW_REAL    *WTars      [MAX_DDI_TARIFF];
    //
    UIW_TIME    *WfromTimes[MAX_DDI_DAY_TYPE][MAX_DDI_SCHEDULE];
    UIW_TIME    *WtoTimes  [MAX_DDI_DAY_TYPE][MAX_DDI_SCHEDULE];
    UIW_INTEGER *WPercents [MAX_DDI_DAY_TYPE][MAX_DDI_SCHEDULE];
    //
    UIW_BIGNUM  *WTax;
    UIW_BUTTON  *WBApply;
    UIW_BUTTON  *WEApply;
    //
    BOOL Select;
};

class EXPORT UIW_NEW_CITY : public UIW_WINDOW
{
public:
    UIW_NEW_CITY(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_NEW_COUNTRY : public UIW_WINDOW
{
public:
    UIW_NEW_COUNTRY(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_CHANGE_PASSWD : public UIW_WINDOW
{
public:
    UIW_CHANGE_PASSWD(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_SIMULA : public UIW_WINDOW
{
public:
    UIW_SIMULA(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    static EVENT_TYPE ProcessBooth(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
    static EVENT_TYPE ProcessCall (UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
		static void HangCall(UI_WINDOW_OBJECT *object, int bNum);
		static void MakeCall(UI_WINDOW_OBJECT *object, int bNum);
		static void SetPrePaid(UI_WINDOW_OBJECT *object, int bNum);
		static void SetControls(
			UI_WINDOW_OBJECT *object,
			int bNum,
			char *newCaption);
		UIW_PHONE *WPhone;
};

class EXPORT UIW_DUMP : public UIW_WINDOW
{
public:
    UIW_DUMP(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    void Do(void);
};

class EXPORT UIW_SYS_INFO : public UIW_WINDOW
{
public:
    UIW_SYS_INFO(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_SIGNAL : public UIW_WINDOW
{
public:
    UIW_SIGNAL(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    UIW_GROUP   *WSignalGroup;
    UIW_BUTTON  *WSteadyButton;
    UIW_INTEGER *WTCom;
    USER_EVENT  LastEvent;
};

class EXPORT UIW_SACCUM : public UIW_WINDOW
{
public:
    UIW_SACCUM(BOOL fromDBView=FALSE, BOOL fromTurn=TRUE);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_PRINT_AREA : public UIW_WINDOW
{
public:
    UIW_PRINT_AREA(UIW_VT_LIST *wList);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    UIW_VT_LIST *WList;
    UI_WINDOW_OBJECT *WStr;
};

class EXPORT UIW_ZPRINT : public UIW_WINDOW
{
public:
    UIW_ZPRINT(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    static EVENT_TYPE processArea(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
};

class EXPORT UIW_IPRINT : public UIW_WINDOW
{
public:
    UIW_IPRINT(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    static EVENT_TYPE processArea(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
};

class EXPORT UIW_FOOTER : public UIW_WINDOW
{
public:
    UIW_FOOTER(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_ADM_REC : public UIW_WINDOW
{
public:
    UIW_ADM_REC(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_ALIAS : public UIW_WINDOW
{
public:
    UIW_ALIAS(BOOL select = TRUE);
    //
    virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    UIW_GROUP  *WBoothsGroup;
    UIW_PROMPT *WPrompts[MAX_BOOTH];
    UIW_STRING *WNames[MAX_BOOTH];
};

class EXPORT UIW_ALARM : public UIW_WINDOW
{
public:
    UIW_ALARM(void);
    //
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_OP_ID : public UIW_WINDOW
{
public:
    UIW_OP_ID(void);
    //
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_CASH : public UIW_WINDOW
{
public:
    UIW_CASH(void);
    //
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_DISPLAY : public UIW_WINDOW
{
public:
    UIW_DISPLAY(void);
    //
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_ROUND : public UIW_WINDOW
{
public:
    UIW_ROUND(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_S_PORT : public UIW_WINDOW
{
public:
    UIW_S_PORT(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_P_PORT : public UIW_WINDOW
{
public:
    UIW_P_PORT(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_PPORT : public UIW_WINDOW
{
public:
    UIW_PPORT(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_PASSWORD : public UIW_WINDOW
{
public:
    UIW_PASSWORD(BOOL forConfig = TRUE);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

//
// EXTENSIONS
//

class E_ACCOUNT : public UIW_WINDOW
{
public:
    E_ACCOUNT(void);
    ~E_ACCOUNT(void);
    //
    virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    static EVENT_TYPE ProcessExt  (UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
    static EVENT_TYPE ProcessValue(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
    static EVENT_TYPE ProcessFrom(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
    static EVENT_TYPE ProcessTo  (UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
    //
    struct ENTRY
    {
        UIW_STRING *Text;
        UIW_TIME   *Time;
        UIW_DATE   *Date;
        UIW_BIGNUM *Value;
    };
    ENTRY WCredits[3];
    ENTRY WDebits [3];
    ENTRY WOthers [3];
    void PrintData(void);
};

class E_ACCUM : public UIW_WINDOW
{
public:
    E_ACCUM(void);
    ~E_ACCUM(void);
    //
    virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    void PrintData(void);
};

class E_PARAMETERS : public UIW_WINDOW
{
public:
    E_PARAMETERS(void);
    ~E_PARAMETERS(void);
    //
    virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    static EVENT_TYPE Process(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
};

class EXPORT UIW_ABOUT : public UIW_WINDOW
{
public:
    UIW_ABOUT(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

#endif // __MENUBAR_H
