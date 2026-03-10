//
// [ W_PHONE.CPP ]
//

#include "stdst.h"

#include <w_phone.h>
#include <res.hpp>

UIW_PHONE::UIW_PHONE(int left, int top, int width, char *phone, int aMaxLen,
                     WOF_FLAGS woFlags_, USER_FUNCTION userFunction_) :
        UIW_STRING(left, top, width, phone, aMaxLen, STF_NO_FLAGS, woFlags_, userFunction_)
{
    if (aMaxLen > sizeof(PHONE))
        aMaxLen = sizeof(PHONE);
    UIW_PHONE::Information(INITIALIZE_CLASS, NULL);
    UIW_PHONE::DataSet(phone);
}

UIW_PHONE::~UIW_PHONE(void)
{}



char *UIW_PHONE::DataGet(void)
{
    return UIW_STRING::DataGet();
}

void UIW_PHONE::DataSet(char *phone)
{
    UIW_STRING::DataSet(phone);
}

EVENT_TYPE UIW_PHONE::Event(const UI_EVENT &event)
{
    EVENT_TYPE tCCode = event.type;

#if defined(ZIL_OS2)
    // Check for OS/2 specific messages.
    if (tCCode == E_OS2 && event.message.msg != WM_CHAR)
        return (UIW_STRING::Event(event));
#endif
    tCCode = LogicalEvent(event, ID_STRING);
    switch (tCCode)
    {
    case L_SELECT:
    case S_NON_CURRENT:
        {
            if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
            {
                PHONE tPhone;
                tPhone[0] = NULL;
                UIW_STRING::DataGet();
                // remove non-digits
                for (int ti=0, tj=0; text[ti]; ti++)
                    if (isdigit(text[ti]))
                    {
                        tPhone[tj] = text[ti];
                        tPhone[++tj] = NULL;
                    }
                tCCode = UIW_STRING::Event(event);	// Call the user or validate function.
                if (tCCode == -1)
                    tPhone[0] = NULL;
                else
                    woStatus &= ~WOS_UNANSWERED;
                UIW_STRING::DataSet(tPhone);
            }
            else
                tCCode = UIW_STRING::Event(event);
            break;
        }
    default:
        tCCode = UIW_STRING::Event(event);
        break;
    }
    return (tCCode);
}

void *UIW_PHONE::Information(INFO_REQUEST request, void *data, OBJECTID objectId)
{
    // Switch on the request.
    if (!objectId) objectId = ID_UIW_PHONE;
    switch (request)
    {
    case INITIALIZE_CLASS:
        // Set the object identification and variables.
        searchID = windowID[0] = ID_UIW_PHONE;
        windowID[1] = ID_NUMBER;
        windowID[2] = ID_STRING;
        break;
    case CHANGED_FLAGS:
        // Check the object and base class flag settings.
        UIW_STRING::Information(CHANGED_FLAGS, data, ID_UIW_PHONE);
        // See if the field needs to be re-computed.
        if (objectId == ID_UIW_PHONE && FlagSet(woStatus, WOS_REDISPLAY))
        {
            UI_EVENT tEvent(S_INITIALIZE, 0);
            Event(tEvent);
            tEvent.type = S_CREATE;
            Event(tEvent);
        }
        break;
    default:
        data = UIW_STRING::Information(request, data, objectId);
        break;
    }
    return (data);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_PERSISTENCE)
UIW_PHONE::UIW_PHONE(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object) :
        UIW_STRING(0, 0, 10, NULL, sizeof(PHONE), STF_NO_FLAGS, WOF_NO_FLAGS)
{
    // Initialize the number information.
    UIW_PHONE::Load(name, file, object);
    UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
    UIW_STRING::Information(INITIALIZE_CLASS, NULL);
    UIW_PHONE::Information(INITIALIZE_CLASS, NULL);
}

void UIW_PHONE::Load(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
{
    // Load the integer information.
    UIW_STRING::Load(name, file, object);
}

void UIW_PHONE::Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
{
    // Store the integer information.
    UIW_STRING::Store(name, file, object);
}
#endif
