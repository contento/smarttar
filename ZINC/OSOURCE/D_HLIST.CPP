//	Zinc Interface Library - D_HLIST.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_HZ_LIST --------------------------------------------------------

EVENT_TYPE UIW_HZ_LIST::Event(const UI_EVENT &event)
{
	static initializedTime = FALSE;
	static UI_TIME lastTime;
	if (!initializedTime)
	{
		lastTime.Import();
		initializedTime = TRUE;
	}

	UI_WINDOW_OBJECT *object;
	UI_WINDOW_OBJECT *oldCurrent = Current();

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_HZ_LIST);
	switch (ccode)
	{
	case S_SIZE:
	case S_CREATE:
		// Compute the object region.
		clipList.Destroy();
		UI_WINDOW_OBJECT::Event(event);

		// Compute the support object regions.
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
		{
			object->Event(event);
			if (FlagSet(object->woAdvancedFlags, WOAF_OUTSIDE_REGION))
			{
				UI_REGION region = object->true;
				clipList.Destroy();
				clipList.Add(new UI_REGION_ELEMENT(screenID, --region));
			}
			else if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
				clipList.Split(screenID, object->true, FALSE);
		}
		// Continue to S_REDISPLAY.
	case S_REDISPLAY:
		// Compute the list object regions.
		{
		int top = 0, left = 0;
		int itemWidth = cellWidth * display->cellWidth;
		int itemHeight = cellHeight * (display->cellHeight - display->preSpace - display->postSpace);
		int totalHeight = clipList.First() ?
			clipList.First()->region.bottom - clipList.First()->region.top + 1 :
			true.bottom - true.top + 1;
		for (object = First(); object; object = object->Next())
		{
			if (top + itemHeight > totalHeight)
			{
				top = 0;
				left += itemWidth;
			}
			object->relative.left = left;
			object->relative.top = top;
			object->relative.right = left + itemWidth - 1;
			top += itemHeight;
			object->relative.bottom = top - 1;
			object->woFlags &= ~(WOF_NON_SELECTABLE | WOF_BORDER);
			object->woFlags |= WOF_VIEW_ONLY;
			if (ccode == S_REDISPLAY)
				object->Event(UI_EVENT(S_CREATE));
			else
				object->Event(event);
		}

		if (ccode == S_REDISPLAY)
			UI_WINDOW_OBJECT::Event(event);

		oldCurrent = NULL;	// Ensure scroll region is updated.
		}
		break;

	case L_PREVIOUS:
	case L_NEXT:
		ccode = S_UNKNOWN;
		break;

	case L_LEFT:
	case L_RIGHT:
		{
		int currentTop = Current()->true.top;
		object = (ccode == L_LEFT) ? Current()->Previous() : Current()->Next();
		while (object && object != current)
			if (object->true.top == currentTop)
				UIW_WINDOW::Add(object);
			else
				object = (ccode == L_LEFT) ? object->Previous() : object->Next();
		}
		break;

	case L_UP:
	case L_DOWN:
		{
		int emptyQueue;
		UI_EVENT tEvent;
		tEvent = event;
		ccode = (ccode == L_UP) ? L_PREVIOUS : L_NEXT;
		do
		{
			UIW_WINDOW::Event(UI_EVENT(ccode));
			emptyQueue = eventManager->Get(tEvent, Q_NO_BLOCK | Q_NO_DESTROY);
			if (!emptyQueue && tEvent.type == event.type && tEvent.rawCode == event.rawCode)
				eventManager->Get(tEvent);
			else
				emptyQueue = TRUE;
		} while (!emptyQueue);
		}
		break;

	case L_PGUP:
	case L_PGDN:
		{
		// Go to bottom of current page.
		UI_WINDOW_OBJECT *nObject = Current();
		UI_REGION region;
		if (clipList.First())		// Un-optimized for Zortech bug.
			region = clipList.First()->region;
		else
			region = true;
		for (object = (ccode == L_PGUP) ? Current()->Previous() : Current()->Next(); object;
			 object = (ccode == L_PGUP) ? object->Previous() : object->Next())
			if ((ccode == L_PGUP && object->true.left >= region.left) ||
				(ccode == L_PGDN && object->true.right <= region.right))
				nObject = object;
			else
				break;
		// Go to bottom of next page if already at bottom of current page.
		if (nObject == Current())
		{
			object = (ccode == L_PGUP) ? Current()->Previous() : Current()->Next();
			while (object)
			{
				nObject = object;
				if (nObject->true.top == Current()->true.top)
					break;
				object = (ccode == L_PGUP) ? object->Previous() : object->Next();
			}
		}
		// Select a new current object.
		UIW_WINDOW::Add(nObject);
		}
		break;

	case L_FIRST:
	case L_LAST:
		UIW_WINDOW::Add((ccode == L_FIRST) ? First() : Last());
		break;

	case L_SELECT:
		ccode = UIW_WINDOW::Event(UI_EVENT(L_SELECT));
		if (FlagSet(woAdvancedFlags, WOAF_TEMPORARY))
			eventManager->Put(UI_EVENT(S_CLOSE_TEMPORARY));
		break;

	case L_END_SELECT:
		{
		ccode = UIW_WINDOW::Event(event);
#if defined(ZIL_EDIT)
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			break;
#endif
		UI_TIME currentTime;
		long elapsedTime = currentTime - lastTime;							// BUG.1228
		object = Current();
		if (ccode == L_END_SELECT && object && object->userFunction &&
			object->true.Overlap(event.position) && 
			Abs(elapsedTime) < doubleClickRate)
		{
			UI_EVENT uEvent;
			uEvent = event;
			ccode = (*Current()->userFunction)(Current(), uEvent, L_DOUBLE_CLICK);
		}
		else if (ccode == L_END_SELECT)
			lastTime = currentTime;
		}
		break;

#if defined(ZIL_EDIT)
	case L_VIEW:
	case L_BEGIN_SELECT:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
		// Continue to default.
#endif
	default:
		ccode = UIW_WINDOW::Event(event);
		if (ccode == S_CURRENT || ccode == L_BEGIN_SELECT)
		{
			eventManager->DeviceState(E_CURSOR, D_OFF);
			eventManager->DeviceState(E_MOUSE, DM_VIEW);
		}
		break;
	}

	// Check the horizontal scroll region.
	if (oldCurrent != current && ccode != S_INITIALIZE)
	{
		UI_EVENT hEvent(S_HSCROLL_SET);
		if (hScroll)
		{
			hEvent.scroll.minimum = First()->relative.left;
			hEvent.scroll.maximum = Last()->relative.left;
			hEvent.scroll.current = Current()->relative.left;
			hEvent.scroll.delta = Current()->relative.Width();
			hEvent.scroll.showing = hEvent.scroll.delta;
			hScroll->Event(hEvent);
		}
		hEvent.type = S_HSCROLL_CHECK;
		if (ccode != S_CREATE && event.type != E_MOUSE)
			UIW_WINDOW::Event(hEvent);
	}

	// Return the control code.
	return (ccode);
}

