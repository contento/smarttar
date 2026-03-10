#ifndef __W_STATBR_H
#define __W_STATBR_H

#if !defined(UI_WIN_HPP)
#include <ui_win.hpp>
#endif

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

struct HELP_INFO
{
    UI_HELP_CONTEXT  Context;
    char            *Text;
    int              Foreground;
    int              Background;
};

const OBJECTID ID_STAT_BAR	 = 3005;

class UIW_STAT_BAR : public UI_WINDOW_OBJECT
{
public:
    UIW_STAT_BAR(void);
    //
    static HELP_INFO *HelpInfo;
    //
    virtual EVENT_TYPE DrawItem(const UI_EVENT &anEvent, EVENT_TYPE aCCode);
    //
    EVENT_TYPE Event (const UI_EVENT &anEvent);
    void 			 Update(UI_WINDOW_OBJECT *theHelpObject);
    void       setMsg(char *aMsg, int aForeground = LIGHTGRAY, int aBackground = BLACK, int aMsgCount = 0x05);
    BOOL       PendingMsg(void)
    {
        return (MsgCount);
    }
    // Persistence
    static UI_WINDOW_OBJECT *New(const char *, UI_STORAGE *, UI_STORAGE_OBJECT *)
    {
        return new UIW_STAT_BAR();
    }

#if defined(ZIL_STORE)
    virtual void Store(const ZIL_ICHAR *name,
                       ZIL_STORAGE *file = ZIL_NULLP(ZIL_STORAGE),
                       ZIL_STORAGE_OBJECT *object = ZIL_NULLP(ZIL_STORAGE_OBJECT),
                       UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
                       UI_ITEM *userTable = ZIL_NULLP(UI_ITEM))
    {}
    ;
#endif

protected:
    char 								Msg[0x40];
    char               	RecStr[0x10];
    char                TimeStr[0x0A];
    char                DateStr[0x14];
    UI_PALETTE         *DatePalette;
    UI_HELP_CONTEXT     HelpContext;
    char               *HelpText;
    UI_PALETTE         *HelpPalette;
    UI_PALETTE         *MsgPalette;
    int 								MsgCount;
    //
    void setHelpInfo   (UI_HELP_CONTEXT theHelpContext);
    void refresh       (EVENT_TYPE aCCode, BOOL thefAll = FALSE);
    void drawDipMessage(UI_REGION& theRegion, const char *theText, UI_PALETTE *theTextPalette, EVENT_TYPE& aCCode);
};

#endif // __W_STATBR_H
