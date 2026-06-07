//
// [ TABLE.CPP ]
//

#include <ui_win.hpp>
#include <w_table.h>

const OBJECTID ID_UIW_TABLE = 3500;

UIW_TABLE::UIW_TABLE(int left, int top, int width, int height,
                     USHORT woFlags, USHORT woAdvancedFlags, UI_HELP_CONTEXT helpContext,
                     UI_WINDOW_OBJECT *minObject
                    ) :
        UIW_WINDOW(left, top, width, height,
                   woFlags, woAdvancedFlags, helpContext, minObject),
        vPosition(0), hPosition(0), vMax(0), hMax(0), vOffset(0), hOffset(0)
{}



EVENT_TYPE UIW_TABLE::Event(const UI_EVENT &event)
{
    UI_WINDOW_OBJECT *object = Current();
    EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW);
    switch (ccode)
    {
    case S_CREATE:
    case S_SIZE:
        UIW_WINDOW::Event(event);
        // Recalculate the scroll bar information.
        ScrollCompute(TRUE);
        break;

    case S_VSCROLL:
    case S_HSCROLL:
        //Scroll the window and update scroll bar position.
        if (ccode == S_VSCROLL)
        {
            UI_EVENT vEvent(S_VSCROLL);
            vEvent.scroll.delta = event.scroll.delta;
            if ((!vPosition && vEvent.scroll.delta <= 0) ||
                    (vPosition == vMax && vEvent.scroll.delta >= 0))
                return ccode;
            vPosition += vEvent.scroll.delta;
            if (vPosition < 0)
            {
                vEvent.scroll.delta -= vPosition;
                vPosition = 0;
            }
            else if (vPosition > vMax)
            {
                vEvent.scroll.delta -= (vPosition - vMax);
                vPosition = vMax;
            }
            if (vScroll)
                vScroll->Event(vEvent);
            ScrollWindow(vEvent.scroll.delta, 0);

        }
        else   // S_HSCROLL
        {
            UI_EVENT hEvent(S_HSCROLL);
            hEvent.scroll.delta = event.scroll.delta;
            if ((!hPosition && hEvent.scroll.delta <= 0) ||
                    (hPosition == hMax && hEvent.scroll.delta >= 0))
                return ccode;
            hPosition += hEvent.scroll.delta;

            if (hPosition < 0)
            {
                hEvent.scroll.delta -= hPosition;
                hPosition = 0;
            }
            else if (hPosition > hMax)
            {
                hEvent.scroll.delta -= (hPosition - hMax);
                hPosition = hMax;
            }
            if (hScroll)
                hScroll->Event(hEvent);
            ScrollWindow(0, hEvent.scroll.delta);
        }
        // change current
        if (!object->true.Encompassed(clientArea))
        {
            for (UI_WINDOW_OBJECT *wObject = First(); wObject; wObject = wObject->Next())
            {
                if (wObject->true.Encompassed(clientArea))
                {
                    if (object != wObject)
                    {
                        object->Event(UI_EVENT(S_NON_CURRENT, 0));
                        SetCurrent(wObject);
                        wObject->Event(UI_EVENT(S_CURRENT, 0));
                    }
                    break;
                }
            }
        }
        break;
        //
        // this the unique difference beetwen a UIW_WINDOW and this one
        // its processing of the TAB and F6 key.
        //
    case L_PREVIOUS:
    case L_NEXT:
        ccode = S_UNKNOWN;
        break;

    case L_UP:
    case L_DOWN:
        if (object)
        {
            UI_WINDOW_OBJECT *sObject = NULL;
            UI_EVENT vEvent(S_VSCROLL);
            if (object && object->true.Overlap(clientArea) && ccode == L_UP)
            {
                sObject = object->Previous();
                while (sObject && (FlagSet(sObject->woFlags, WOF_NON_SELECTABLE)
                                   || sObject->relative.left != object->relative.left))
                    sObject = sObject->Previous();
            }
            else if (object && object->true.Overlap(clientArea))
            {
                sObject = object->Next();
                while (sObject && (FlagSet(sObject->woFlags, WOF_NON_SELECTABLE)
                                   || sObject->relative.left != object->relative.left))
                    sObject = sObject->Next();
            }
            if (sObject)
            {
                UIW_WINDOW::Add(sObject);
                if (!sObject->true.Encompassed(clientArea) && vScroll)
                {
                    vEvent.scroll.delta = (ccode == L_UP) ? sObject->true.top - clientArea.top :
                                          sObject->true.bottom - clientArea.bottom;
                    Event(vEvent);
                }
            }
            else if (vScroll)
            {
                vEvent.scroll.delta = (ccode == L_UP) ? -display->cellHeight : display->cellHeight;
                Event(vEvent);
            }
        }
        else if (object)
            return (object->Event(event));
        break;

    case L_LEFT:
    case L_RIGHT:
        if (object)
        {
            UI_WINDOW_OBJECT *sObject = NULL;
            UI_EVENT hEvent(S_HSCROLL);

            if (object && object->true.Overlap(clientArea) && ccode == L_LEFT)
            {
                sObject = object->Previous();
                while (sObject && (FlagSet(sObject->woFlags, WOF_NON_SELECTABLE)
                                   || sObject->relative.top != object->relative.top))
                    sObject = sObject->Previous();
            }
            else if (object && object->true.Overlap(clientArea))
            {
                sObject = object->Next();
                while (sObject && (FlagSet(sObject->woFlags, WOF_NON_SELECTABLE)
                                   || sObject->relative.top != object->relative.top))
                    sObject = sObject->Next();
            }
            if (sObject)
            {
                UIW_WINDOW::Add(sObject);
                if (!sObject->true.Encompassed(clientArea) && hScroll)
                {
                    hEvent.scroll.delta = (ccode == L_LEFT) ? sObject->true.left - clientArea.left :
                                          sObject->true.right - clientArea.right;
                    Event(hEvent);
                }
            }
            else if (hScroll)
            {
                hEvent.scroll.delta = (ccode == L_LEFT) ? -display->cellHeight : display->cellHeight;
                Event(hEvent);
            }
        }
        else if (object)
            return (object->Event(event));
        break;
    case L_SCROLL_PGUP:
    case L_SCROLL_PGDN:
    case L_PGUP:
    case L_PGDN:
        {
            UI_EVENT vEvent(S_VSCROLL);
            vEvent.scroll.delta = (ccode == L_PGUP || ccode == L_SCROLL_PGUP)
                                  ? -(clientArea.bottom - clientArea.top) :	clientArea.bottom - clientArea.top;
            Event(vEvent);
            // change current
            if (!object->true.Encompassed(clientArea))
            {
                for (UI_WINDOW_OBJECT *wObject = First(); wObject; wObject = wObject->Next())
                {
                    if (wObject->true.Encompassed(clientArea))
                    {
                        if (object != wObject)
                        {
                            object->Event(UI_EVENT(S_NON_CURRENT, 0));
                            SetCurrent(wObject);
                            wObject->Event(UI_EVENT(S_CURRENT, 0));
                        }
                        break;
                    }
                }
            }
            break;
        }
    case L_TOP:
    case L_BOTTOM:
        {
            UI_EVENT vEvent(S_VSCROLL);
            vEvent.scroll.delta = (ccode == L_TOP) ? -vMax : vMax;
            Event(vEvent);
            // change current
            object->Event(UI_EVENT(S_CURRENT, 0));
            break;
        }
    case REMOVE_SCROLL:
        {
#ifdef ZIL_MSWINDOWS
            // Remove scroll bar and send repaint message for scroll bar area.
            RECT region;
            if (this != event.data)
                return object->Event(event);

            if (event.rawCode)
            {
                int xRegion = GetSystemMetrics(SM_CXHSCROLL);
                int yRegion = GetSystemMetrics(SM_CYHSCROLL);
                ShowScrollBar(screenID, SB_HORZ, 0);
                GetClientRect(screenID, &region);
                region.right = region.left + xRegion;
                region.top = region.bottom - yRegion;
            }
            else
            {
                int xRegion = GetSystemMetrics(SM_CXVSCROLL);
                int yRegion = GetSystemMetrics(SM_CYVSCROLL);
                ShowScrollBar(screenID, SB_VERT, 0);
                GetClientRect(screenID, &region);
                region.left = region.right - xRegion;
                region.bottom = region.top + yRegion;
            }
            InvalidateRect(screenID, &region, TRUE);
            return 0;
#else
if (vScroll && hScroll && this == event.data)
            {
                int vWidth = display->isText ? 1 : vScroll->true.right - vScroll->true.left;
                int hWidth = display->isText ? 1 : hScroll->true.bottom - hScroll->true.top;
                ScrollCompute(TRUE);
                if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
                    Event(UI_EVENT(S_REGION_DEFINE));

                UI_REGION newRegion = clientArea;
                newRegion.right += vWidth;
                newRegion.bottom += hWidth;
                Event(UI_EVENT(S_CURRENT, 0, newRegion));
            }
            else if (this == event.data)
            {
                ScrollCompute(TRUE);
                if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
                    Event(UI_EVENT(S_REGION_DEFINE));
                Event(UI_EVENT(S_CURRENT, 0, clientArea));
            }
            else return ((UIW_TABLE *)event.data)->Event(event);
#endif
        }

    default:

#ifdef ZIL_MSWINDOWS
        switch(event.message.message)
        {

        case WM_VSCROLL:
            {
                UI_EVENT sEvent(S_VSCROLL);
                switch(event.message.wParam)
                {
                case SB_LINEUP:
                    sEvent.scroll.delta = -display->cellHeight;
                    break;
                case SB_LINEDOWN:
                    sEvent.scroll.delta = display->cellHeight;
                    break;
                case SB_PAGEUP:
                    sEvent.scroll.delta = -(clientArea.bottom - clientArea.top);
                    break;
                case SB_PAGEDOWN:
                    sEvent.scroll.delta = clientArea.bottom - clientArea.top;
                    break;
                case SB_THUMBPOSITION:
                    sEvent.scroll.delta = LOWORD(event.message.lParam) - vPosition;
                    break;
                default:
                    sEvent.scroll.delta = 0;
                    break;
                }
                if (sEvent.scroll.delta)
                    return Event(sEvent);
            }
            break;

        case WM_HSCROLL:
            {
                UI_EVENT sEvent(S_HSCROLL);
                switch(event.message.wParam)
                {
                case SB_LINEUP:
                    sEvent.scroll.delta = -display->cellHeight;
                    break;
                case SB_LINEDOWN:
                    sEvent.scroll.delta = display->cellHeight;
                    break;
                case SB_PAGEUP:
                    sEvent.scroll.delta = -(clientArea.bottom - clientArea.top);
                    break;
                case SB_PAGEDOWN:
                    sEvent.scroll.delta = clientArea.bottom - clientArea.top;
                    break;
                case SB_THUMBPOSITION:
                    sEvent.scroll.delta = LOWORD(event.message.lParam) - hPosition;
                    break;
                default:
                    sEvent.scroll.delta = 0;
                    break;
                }
                if (sEvent.scroll.delta)
                    return Event(sEvent);
            }

        default:
            break;
        }

#endif

        // Call the window event to process other events.
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    // Return the control code.
    return (ccode);
}

void UIW_TABLE::ScrollWindow(int vDelta, int hDelta)
{
#ifdef ZIL_MSWINDOWS
    if (vDelta)
    {
        ::ScrollWindow(screenID, 0, -vDelta, NULL, NULL);
        SetScrollPos(screenID, SB_VERT, vPosition, TRUE);
    }
    if (hDelta)
    {
        ::ScrollWindow(screenID, -hDelta, 0, NULL, NULL);
        SetScrollPos(screenID, SB_HORZ, hPosition, TRUE);
    }

    RECT region;
    GetClientRect(screenID, &region);

    if (vDelta && !vPosition && (getvMax + 2 <= region.bottom - region.top))
    {
        UI_EVENT rEvent(REMOVE_SCROLL, 0);
        rEvent.data = this;
        eventManager->Put(rEvent);
        return;
    }
    if (hDelta && !hPosition && (gethMax + 2 <= region.right - region.left))
    {
        UI_EVENT rEvent(REMOVE_SCROLL, 1);
        rEvent.data = this;
        eventManager->Put(rEvent);
        return;
    }

#else
    UI_WINDOW_OBJECT *object;
    // Move the objects in the window by the scroll delta.
    if (vDelta)
        for (object = First(); object; object = object->Next())
        {
            object->relative.top -= vDelta;
            object->relative.bottom -= vDelta;
            object->Event(UI_EVENT(S_CREATE));
            if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
                object->parent->Event(UI_EVENT(S_REGION_DEFINE));
        }
    if (hDelta)
        for (object = First(); object; object = object->Next())
        {
            object->relative.left -= hDelta;
            object->relative.right -= hDelta;
            object->Event(UI_EVENT(S_CREATE));
            if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
                object->parent->Event(UI_EVENT(S_REGION_DEFINE));
        }

    if ((!vPosition && getvMax <= clientArea.bottom - clientArea.top && vScroll) ||
            (!hPosition && gethMax <= clientArea.right - clientArea.left && hScroll))
    {
        // Send the L_END_SELECT message to the scroll bar to break
        // out of the event loop, then put the REMOVE_SCROLL message
        // on the queue.

        if (vDelta)
            vScroll->Event(UI_EVENT(L_END_SELECT));
        else
            hScroll->Event(UI_EVENT(L_END_SELECT));
        eventManager->Put(UI_EVENT(L_END_SELECT));
        UI_EVENT rEvent(REMOVE_SCROLL);
        rEvent.data = this;
        eventManager->Put(rEvent);
    }
    else
        Event(UI_EVENT(S_CURRENT, 0, clientArea));
#endif

}

UIW_TABLE *UIW_TABLE::Generic(int left, int top, int width, int height,
                              char *title, UI_WINDOW_OBJECT *minObject, WOF_FLAGS woFlags,
                              WOAF_FLAGS woAdvancedFlags, UI_HELP_CONTEXT helpContext)
{
    // Create the window and add default window objects.
    UIW_TABLE *window = new UIW_TABLE(left, top, width, height,
                                      woFlags, woAdvancedFlags, helpContext, minObject);

    *window
    + new UIW_BORDER
    + new UIW_MAXIMIZE_BUTTON
    + new UIW_MINIMIZE_BUTTON
    + new UIW_SYSTEM_BUTTON(SYF_GENERIC)
    + new UIW_TITLE(title);

    if (!minObject)
        *window + new UIW_ICON(0, 0, title, title, ICF_MINIMIZE_OBJECT);

    // Return a pointer to the new window.
    return (window);
}

void UIW_TABLE::ScrollCompute(int reset)
{
    UI_WINDOW_OBJECT *object;
    UI_EVENT sEvent;

#ifdef ZIL_MSWINDOWS
    RECT region;
    GetClientRect(screenID, &region);
    clientArea.Assign(region);
#else
clientArea = clipList.First()->region;
#endif

    // Recalculate the vertical and horizontal scroll region.
    getvMin = 32000;
    gethMin = 32000;
    getvMax = 0;
    gethMax = 0;

    for (object = First(); object; object = object->Next())
    {
        if (object->relative.bottom > getvMax)
            getvMax = object->relative.bottom;
        if (object->relative.top < getvMin)
            getvMin = object->relative.top;
        if (object->relative.right > gethMax)
            gethMax = object->relative.right;
        if (object->relative.left < gethMin)
            gethMin = object->relative.left;
    }

#ifndef ZIL_MSWINDOWS
    int reCreate = FALSE;

    // Save the offset of topmost and leftmost object in window.
    if (getvMax && getvMin > 0 && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
        vOffset = getvMin;
    else if (getvMax && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
        vOffset = display->cellWidth;

    if (gethMax && gethMin > 0 && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
        hOffset = gethMin;
    else if (gethMax && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
        hOffset = display->cellWidth;

    if (!vOffset && getvMax)
        vOffset = getvMin;
    if (!hOffset && gethMax)
        hOffset = gethMin;
    if (getvMax)
        getvMax = getvMax - getvMin + vOffset;
    if (gethMax)
        gethMax = gethMax - gethMin + hOffset;

    // The following code automatically adds or deletes scroll bars.
    if (!vScroll && !hScroll && (getvMax > clientArea.bottom - clientArea.top &&
                                 gethMax > clientArea.right - clientArea.left || getvMin < 0 && gethMin < 0))
    {
        *this + (corner = new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_CORNER))
        + (vScroll = new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL))
        + (hScroll = new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_HORIZONTAL));
        clientArea.right -= display->isText ? 1 : vScroll->true.right - vScroll->true.left;
        clientArea.bottom -= display->isText ? 1 : hScroll->true.bottom - hScroll->true.top;
        reCreate = TRUE;
    }
    else if (vScroll && hScroll && getvMax <= clientArea.bottom - clientArea.top &&
             gethMax <= clientArea.right - clientArea.left && !vPosition && !hPosition)
    {
        clientArea.right += display->isText ? 1 : vScroll->true.right - vScroll->true.left;
        clientArea.bottom += display->isText ? 1 : hScroll->true.bottom - hScroll->true.top;
        *this - corner - vScroll - hScroll;
        delete corner;  corner  = NULL;
        delete vScroll; vScroll = NULL;
        delete hScroll; hScroll = NULL;
        reCreate = TRUE;
    }
    else if (vScroll && getvMax <= clientArea.bottom - clientArea.top && !vPosition)
    {
        clientArea.right += display->isText ? 1 : vScroll->true.right - vScroll->true.left;
        *this - vScroll;
        delete vScroll; vScroll = NULL;
        if (hScroll && corner)
        {
            *this - corner;
            delete corner; corner = NULL;
        }
        reCreate = TRUE;
    }
    else if (hScroll && gethMax <= clientArea.right - clientArea.left && !hPosition)
    {
        clientArea.bottom += display->isText ? 1 : hScroll->true.bottom - hScroll->true.top;
        *this - hScroll;
        delete hScroll; hScroll = NULL;
        if (vScroll && corner)
        {
            *this - corner;
            delete corner; corner = NULL;
        }
        reCreate = TRUE;
    }
    else if (!vScroll && (getvMax > clientArea.bottom - clientArea.top || getvMin < 0))
    {
        if (hScroll)
        {
            *this - hScroll;
            delete hScroll;
            *this + (corner = new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_CORNER))
            + (vScroll = new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL))
            + (hScroll = new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_HORIZONTAL));
        }
        else
            *this + (vScroll = new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL));
        clientArea.right -= display->isText ? 1 : vScroll->true.right - vScroll->true.left;
        reCreate = TRUE;
    }
    else if (!hScroll && (gethMax > clientArea.right - clientArea.left || gethMin < 0))
    {
        if (vScroll)
        {
            *this - vScroll;
            delete vScroll;
            *this + (corner = new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_CORNER))
            + (vScroll = new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL))
            + (hScroll = new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_HORIZONTAL));
        }
        else
            *this + (hScroll = new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_HORIZONTAL));
        clientArea.bottom -= display->isText ? 1 : hScroll->true.bottom - hScroll->true.top;
        reCreate = TRUE;
    }
    // Let the window update itself of the scroll bar changes.
    if (reCreate)
        UIW_WINDOW::Event(UI_EVENT(S_CREATE));

    vMax = Max(vPosition ? vMax : 0, getvMax - (clientArea.bottom - clientArea.top) + display->cellWidth);
    hMax = Max(hPosition ? hMax : 0, gethMax - (clientArea.right - clientArea.left) + display->cellWidth);

    if (vScroll && reset)
    {
        sEvent.type = S_VSCROLL_SET;
        sEvent.scroll.minimum = 0;
        sEvent.scroll.maximum = vMax;
        sEvent.scroll.current = vPosition;
        sEvent.scroll.delta = display->cellHeight;
        sEvent.scroll.showing = (clientArea.bottom - clientArea.top);
        vScroll->Event(sEvent);
    }
    if (hScroll && reset)
    {
        sEvent.type = S_HSCROLL_SET;
        sEvent.scroll.minimum = 0;
        sEvent.scroll.maximum = hMax;
        sEvent.scroll.current = hPosition;
        sEvent.scroll.delta = display->cellHeight;
        sEvent.scroll.showing = (clientArea.right - clientArea.left);
        hScroll->Event(sEvent);
    }
#else
    vMax = Max( 0, getvMax - (clientArea.bottom - clientArea.top) + 2);
    hMax = Max( 0, gethMax - (clientArea.right - clientArea.left) + 2);
    if (reset)
    {
        SetScrollRange(screenID, SB_VERT, 0, vMax, FALSE);
        SetScrollPos(screenID, SB_VERT, vPosition, TRUE);
        SetScrollRange(screenID, SB_HORZ, 0, hMax, FALSE);
        SetScrollPos(screenID, SB_HORZ, hPosition, TRUE);
    }
#endif
}

void *UIW_TABLE::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
    // Switch on the request.
    if (!objectID)
        objectID = ID_UIW_TABLE;
    switch (request)
    {
    case INITIALIZE_CLASS:
        // Set the object identification and variables.
        searchID = windowID[0] = ID_UIW_TABLE;
        windowID[1] = ID_LIST;
        windowID[2] = ID_WINDOW;
        font = FNT_DIALOG_FONT;
        if (objectID == ID_UIW_TABLE && FlagSet(woStatus, WOS_REDISPLAY))
        {
            UI_EVENT event(S_INITIALIZE, 0);
            Event(event);
            event.type = S_CREATE;
            Event(event);
        }
        break;
    default:
        data = UIW_WINDOW::Information(request, data, objectID);
        break;
    }

    // Return the information.
    return (data);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_PERSISTENCE)
UIW_TABLE::UIW_TABLE(const char *aName, UI_STORAGE *aFile, UI_STORAGE_OBJECT *anObject) :
        UIW_WINDOW(0, 0, 20, 6, WOF_NO_FLAGS)
{
    // Initialize the list information.
    Load(aName, aFile, anObject);
    UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
    UIW_WINDOW::Information(INITIALIZE_CLASS, NULL);
    UIW_TABLE::Information(INITIALIZE_CLASS, NULL);
}

void UIW_TABLE::Load(const char *aName, UI_STORAGE *aFile, UI_STORAGE_OBJECT *anObject)
{
    // Load the vertical list information.
    UIW_WINDOW::Load(aName, aFile, anObject);
}

void UIW_TABLE::Store(const char *aName, UI_STORAGE *aFile, UI_STORAGE_OBJECT *anObject)
{
    // Store the vertical list information.
    UIW_WINDOW::Store(aName, aFile, anObject);
}
#endif
