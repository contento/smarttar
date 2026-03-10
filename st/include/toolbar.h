#ifndef __TOOLBAR_H
#define __TOOLBAR_H

#if !defined(__LISTIMP_H)
#include <classlib\listimp.h>
#endif

#if !defined(UI_WIN_HPP)
#include <ui_win.hpp>
#endif

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#if !defined(__DB_ENG_H)
#include <db_eng.h>
#endif

#if !defined( __W_TABLE_H)
#include <w_table.h>
#endif

#if !defined (__W_STATBR_H)
#include <w_statbr.h>
#endif

#if !defined( __W_PHONE_H)
#include <w_phone.h>
#endif

class EXPORT UIW_MANUAL : public UIW_WINDOW
{
public:
    UIW_MANUAL(int boothNum);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
    ~UIW_MANUAL(void);
private:
    static WORD BoothNum;
    //
    static EVENT_TYPE processCheck  (UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
    static EVENT_TYPE processPayment(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
};

class EXPORT UIW_SP_SERV : public UIW_WINDOW
{
public:
    UIW_SP_SERV(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    static UIW_GROUP     *WService;
    static UIW_INTEGER   *WInt1, *WInt2;
	static UIW_BIGNUM    *WBig1, *WBig2, *WBig3, *WBig4;
    static UIW_STRING  	 *WStr1, *WStr2;
    static UIW_PHONE     *WPhone;
    static UIW_BUTTON    *WPrint;
    //
    static EVENT_TYPE processTel  (UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
    static EVENT_TYPE processTelex(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
    static EVENT_TYPE processMCard(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
    static EVENT_TYPE processFax  (UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
    static EVENT_TYPE processOther(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
};

class EXPORT UIW_CALC : public UIW_WINDOW
{
public:
    UIW_CALC(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    static EVENT_TYPE compute(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
};

class EXPORT UIW_SPY : public UIW_WINDOW
{
public:
    UIW_SPY(void);
    virtual ~UIW_SPY(void)
    {}


    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_INTER : public UIW_WINDOW
{
public:
    UIW_INTER(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_LOCK : public UIW_WINDOW
{
public:
    UIW_LOCK(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_RECEIPT : public UIW_WINDOW
{
public:
    UIW_RECEIPT(void);
    //
    virtual EVENT_TYPE Event(const UI_EVENT &event);
    // we need this public to let access from DControl
    int          Option;
    UIW_VT_LIST *WList;
    UIW_BUTTON  *WPay;
    UIW_INTEGER *WBooth;
    UIW_BIGNUM  *WNumber;
    static long  Number;
    static int   Booth;
private:
    //
    static EVENT_TYPE processNumber(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
    static EVENT_TYPE processBooth (UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode);
};

class EXPORT UIW_ACCUM : public UIW_WINDOW
{
public:
    UIW_ACCUM(BOOL fromDBView=FALSE, BOOL fromTurn=TRUE);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_OPERATION : public UIW_WINDOW
{
public:
    UIW_OPERATION(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT UIW_FORMS : public UIW_WINDOW
{
public:
    UIW_FORMS(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

#endif // __TOOLBAR_H
