#ifndef __W_SGROUP_H
#define __W_SGROUP_H

#if !defined(UI_WIN_HPP)
#include <ui_win.hpp>
#endif

class EXPORT UIW_SGROUP : public UIW_WINDOW
{
public:
    UIW_SGROUP(int left, int top, int width, int height,
               WOF_FLAGS woFlags = WOF_NO_FLAGS, WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS,
               UI_HELP_CONTEXT helpContext = NO_HELP_CONTEXT, UI_WINDOW_OBJECT *minObject = NULL)
            : UIW_WINDOW(left, top, width, height, woFlags, woAdvancedFlags, helpContext, minObject)
    {}
    ;
protected:
    int width;
    virtual EVENT_TYPE Event(const UI_EVENT &event);
};

#endif // __W_SGROUP_H