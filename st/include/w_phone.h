#ifndef __W_PHONE_H
#define __W_PHONE_H

#if !defined(UI_WIN_HPP)
#include <ui_win.hpp>
#endif

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

class EXPORT UIW_PHONE : public UIW_STRING
{
public:
    UIW_PHONE(int left, int top, int width, char *phone,
              int aMaxLen = sizeof(PHONE),
              WOF_FLAGS woFlags_ = WOF_BORDER | WOF_AUTO_CLEAR,
              USER_FUNCTION userFunction_ = NULL);
    virtual ~UIW_PHONE(void);
    //
    char *DataGet(void);
    void DataSet(char *phone);
    virtual EVENT_TYPE Event(const UI_EVENT &event);
    virtual void      *Information(INFO_REQUEST request, void *data, OBJECTID objectId = 0);
#if defined(ZIL_PERSISTENCE)
    static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
    {
        return (new UIW_PHONE(name, file, object));
    }
    UIW_PHONE(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
    virtual void Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
    virtual void Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object);
#endif
};

#endif // __W_PHONE_H
