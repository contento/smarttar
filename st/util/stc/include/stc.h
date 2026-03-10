#ifndef __STC_H
#define __STC_H

#include <ui_win.hpp>

#if !defined(__STRNG_H)
#include <classlib\strng.h>
#endif

#if !defined(__DLISTIMP_H)
#include <classlib\dlistimp.h>
#endif

#ifndef USE_HELP_CONTEXTS
#define USE_HELP_CONTEXTS
#include <help.hpp>
#endif

#if !defined(__EVENTS_H)
#include <events.h>
#endif

#if !defined(__ST_UTIL_H)
#include <st_util.h>
#endif

#if !defined(__CFG_H)
#include <cfg.h>
#endif

#if !defined(__MODEMDEV_H)
#include <modemdev.h>
#endif

class EXPORT CLIENT : public UIW_WINDOW
{
public:
    CLIENT(WORD mode);
    ~CLIENT(void);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
    enum { CONNECTIONMODE, ACTIVATIONMODE } MODE;
private:
    WORD        mode;
    UIW_WINDOW *messageWindow;
    typedef   BI_DoubleListImp<String> Filenames;
    typedef   BI_DoubleListIteratorImp<String> Iterator;
    Filenames *filenames; // to send and receive files
    Iterator  *iterator;
    //
    void disconnect(void);
    BOOL createList(void);
};

class EXPORT SERVER: public UIW_WINDOW
{
public:
    SERVER(WORD mode);
    ~SERVER(void);
    //
    enum { CONNECTIONMODE, ACTIVATIONMODE } MODE;
    //
    virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    WORD mode;
    UIW_WINDOW *messageWindow;
};

class EXPORT STC : public UIW_WINDOW
{
    friend CLIENT;
    friend SERVER;
public:
    STC(void);
    ~STC(void);
    //
    virtual EVENT_TYPE Event(const UI_EVENT &event);
    //
    static MODEM_DEVICE *modemDevice;
};

class EXPORT MODEMCFG: public UIW_WINDOW
{
public:
    MODEMCFG(void);
    ~MODEMCFG(void);
    //
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT CONNECTION: public UIW_WINDOW
{
public:
    CONNECTION(WORD connectionType);
    ~CONNECTION(void);
    //
    enum { ASSERVER, ASCLIENT } CONNECTIONTYPE;
    //
    virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    UIW_WINDOW *messageWindow;
    WORD        connectionType;
    //
    BOOL connect(void);
};

class EXPORT ACTIVATION: public UIW_WINDOW
{
public:
    ACTIVATION(WORD activationType);
    ~ACTIVATION(void);
    //
    enum { ASSERVER, ASCLIENT } ACTIVACTIONTYPE;
    //
    virtual EVENT_TYPE Event(const UI_EVENT &event);
private:
    UIW_WINDOW *messageWindow;
    WORD        activationType;
};

class EXPORT CONSOLE: public UIW_WINDOW
{
public:
    CONSOLE(void);
    ~CONSOLE(void);
    //
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

class EXPORT FTRANSFER: public UIW_WINDOW
{
public:
    FTRANSFER(void);
    ~FTRANSFER(void);
    //
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

#endif //__STC_H
