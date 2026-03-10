//
// [ W_SGROUP.CPP ]
//

#include "stdst.h"

#include <w_sgroup.h>

EVENT_TYPE UIW_SGROUP::Event(const UI_EVENT &event)
{
    // Switch on the event type.
    EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW);
    switch (ccode)
    {
        //
        // this the unique difference beetwen a UIW_WINDOW and this UIW_SGROUP,
        // its processing of the TAB and F6 key.
        //
    case L_PREVIOUS:
    case L_NEXT:
        ccode = S_UNKNOWN;
        break;
    case L_LEFT:
    case L_RIGHT:
        if (current && Current()->Event(event) != S_UNKNOWN)
            break;
        // Continue to L_UP and L_DOWN.
    case L_UP:
    case L_DOWN:
        ccode = UIW_WINDOW::Event(UI_EVENT((ccode == L_LEFT || ccode == L_UP) ? L_PREVIOUS : L_NEXT));
        break;
    default:
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    // Return the control code.
    return (ccode);
}

