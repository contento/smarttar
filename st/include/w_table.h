#ifndef __W_TABLE_H
#define __W_TABLE_H

const EVENT_TYPE L_PGLEFT        = 15001;
const EVENT_TYPE L_PGRIGHT       = 15002;
const EVENT_TYPE L_LEFTMOST      = 15003;
const EVENT_TYPE L_RIGHTMOST     = 15004;
const EVENT_TYPE L_SCROLL_LEFT   = 15005;
const EVENT_TYPE L_SCROLL_RIGHT  = 15006;
const EVENT_TYPE L_SCROLL_UP     = 15007;
const EVENT_TYPE L_SCROLL_DOWN   = 15008;
const EVENT_TYPE L_SCROLL_PGUP   = 15009;
const EVENT_TYPE L_SCROLL_PGDN   = 15010;
const EVENT_TYPE L_SCROLL_HOME   = 15011;
const EVENT_TYPE L_SCROLL_END    = 15012;
const EVENT_TYPE REMOVE_SCROLL   = 15013;

class EXPORT UIW_TABLE : public UIW_WINDOW
{
public:
    UIW_TABLE(int left, int top, int width, int height,
              WOF_FLAGS woFlags = WOF_NO_FLAGS, WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS,
              UI_HELP_CONTEXT helpContext = NO_HELP_CONTEXT, UI_WINDOW_OBJECT *minObject = NULL);
    virtual ~UIW_TABLE(void)
    {}
    //


    UI_REGION clientArea;
    //
    virtual EVENT_TYPE Event(const UI_EVENT &event);
    static UIW_TABLE *Generic(int left, int top, int width, int height,
                              char *title, UI_WINDOW_OBJECT *minObject = NULL, WOF_FLAGS woFlags = WOF_NO_FLAGS,
                              WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS, UI_HELP_CONTEXT helpContext = NO_HELP_CONTEXT);
    void ScrollWindow(int vDelta, int hDelta); // Use Windows ScrollWindow function.
    void ScrollCompute(int reset);
    virtual void *Information(INFO_REQUEST request, void *data, OBJECTID objectID = 0);
private:
    UI_WINDOW_OBJECT *corner;
    int vPosition, hPosition;
    int vMax, hMax, vOffset, hOffset;
    int getvMin, gethMin, getvMax, gethMax;
#if defined(ZIL_PERSISTENCE)
public:
    static UI_WINDOW_OBJECT *New(const char *aName, UI_STORAGE *aFile, UI_STORAGE_OBJECT *anObject)
    {
        return (new UIW_TABLE(aName, aFile, anObject));
    }
    UIW_TABLE(const char *aName, UI_STORAGE *aFile, UI_STORAGE_OBJECT *anObject);
    virtual void Load(const char *aName, UI_STORAGE *aFile, UI_STORAGE_OBJECT *anObject);
    virtual void Store(const char *aName, UI_STORAGE *aFile, UI_STORAGE_OBJECT *anObject);
#endif
};

#endif // __W_TABLE_H
