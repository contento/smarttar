//	Zinc Interface Library - D_VLIST.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_VT_LIST --------------------------------------------------------

EVENT_TYPE UIW_VT_LIST::Event(const UI_EVENT &event)
{
	static initializedTime = FALSE;
	static UI_TIME lastTime;
	if (!initializedTime)
	{
		lastTime.Import();
		initializedTime = TRUE;
	}

//	int callSelection = FALSE;
	UI_WINDOW_OBJECT *object;
	UI_WINDOW_OBJECT *oldCurrent = Current();

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_VT_LIST);
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
		int lineHeight = display->TextHeight(NULL, screenID, font) + display->preSpace + display->postSpace;
		int lineWidth = clipList.First() ?
			clipList.First()->region.right - clipList.First()->region.left :
			true.right - true.left;
		int top = 0;
		if (ccode != S_CREATE && first)
			top = First()->relative.top;
		for (object = First(); object; object = object->Next())
		{
			if (ccode == S_REDISPLAY || ccode == S_CREATE)
				object->Event(UI_EVENT(S_CREATE));
			if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
				lineHeight = object->relative.bottom - object->relative.top + 1;
			object->relative.top = top;
			object->relative.right = lineWidth;
			top += lineHeight;
			object->relative.bottom = top - 1;
			object->woFlags &= ~WOF_BORDER;
			object->woFlags |= WOF_VIEW_ONLY;
		}
		if (parent->Inherited(ID_COMBO_BOX) && ccode == S_CREATE && current)									// Bug.A737, BUG.1695, BUG.1268
		{
			int bot = top;
			object = Current();
			top = object->relative.top;
			if (bot - top < true.bottom - true.top)
			{
				do {
					object = object->Previous();
				} while (object && bot - object->relative.top < true.bottom - true.top);
				top = (object ? object->Next()->relative.top : 0);
			}
			for (object = First(); object; object = object->Next())
			{
				object->relative.top -= top;
				object->relative.bottom -= top;
			}
		}
		for (object = First(); object; object = object->Next())
			object->Event(UI_EVENT(S_SIZE, 0));

		if (ccode == S_REDISPLAY)
			UI_WINDOW_OBJECT::Event(event);

		oldCurrent = NULL;	// Ensure scroll region is updated.
		}
		break;

	case S_ADD_OBJECT:
		object = (UI_WINDOW_OBJECT *)event.data;
		if (UI_LIST::Index(object) == -1)
		{
			if (!parent || FlagSet(parent->woStatus, WOS_EDIT_MODE))		// BUG.General
				object->relative.left = 0;
			object->relative.right = object->relative.left + true.right - true.left;
		}
		UIW_WINDOW::Add(object);
		break;

	case L_PREVIOUS:
	case L_NEXT:
		ccode = S_UNKNOWN;
		break;

	case L_UP:
	case L_DOWN:
	case L_LEFT:
	case L_RIGHT:
		{
		int emptyQueue;
		UI_EVENT tEvent;
		tEvent = event;
		ccode = (ccode == L_LEFT || ccode == L_UP) ? L_PREVIOUS : L_NEXT;
		do
		{
			UIW_WINDOW::Event(UI_EVENT(ccode));
			emptyQueue = eventManager->Get(tEvent, Q_NO_BLOCK | Q_NO_DESTROY);
			if (!emptyQueue && tEvent.type == event.type && tEvent.rawCode == event.rawCode)
				eventManager->Get(tEvent);
			else
				emptyQueue = TRUE;
		} while (!emptyQueue);
//		callSelection = TRUE;
		}
		break;

	case L_PGUP:
	case L_PGDN:
		{
		// Go to bottom of current page.
		UI_WINDOW_OBJECT *nObject = Current();
		if (!nObject)							// BUG.2274
			break;
		UI_REGION region;
		if (clipList.First())		// Un-optimized for Zortech bug.
			region = clipList.First()->region;
		else
			region = true;
		for (object = (ccode == L_PGUP) ? Current()->Previous() : Current()->Next(); object;
			 object = (ccode == L_PGUP) ? object->Previous() : object->Next())
			if ((ccode == L_PGUP && object->true.top >= region.top) ||
				(ccode == L_PGDN && object->true.bottom <= region.bottom))
				nObject = object;
			else
				break;

		// Skip non-selectable objects.										// BUG.429
		while (FlagSet(nObject->woFlags, WOF_NON_SELECTABLE) || FlagSet(nObject->woAdvancedFlags, WOAF_NON_CURRENT))
			nObject = (ccode == L_PGUP) ? nObject->Next() : nObject->Previous();

		// Go to bottom of next page if already at bottom of current page.
		if (nObject == Current())
		{
			int height = region.bottom - region.top + 1;
			for (object = Current(); object && height > 0;
				 object = (ccode == L_PGUP) ? object->Previous() : object->Next())
				{
					height -= (nObject->relative.bottom - nObject->relative.top + 1);
					if (height >= 0)
						nObject = object;
				}
		}

		// Skip non-selectable objects.										// BUG.429
		while (FlagSet(nObject->woFlags, WOF_NON_SELECTABLE) || FlagSet(nObject->woAdvancedFlags, WOAF_NON_CURRENT))
			nObject = (ccode == L_PGUP) ? nObject->Next() : nObject->Previous();

		// Select a new current object.
		UIW_WINDOW::Add(nObject);
//		callSelection = TRUE;
		}
		break;

	case L_FIRST:
	case L_LAST:
		object = (ccode == L_FIRST) ? First() : Last();
		if (!object)							// BUG.2274
			break;
		// Skip non-selectable objects.										// BUG.429
		while (FlagSet(object->woFlags, WOF_NON_SELECTABLE) || FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
			object = (ccode == L_FIRST) ? object->Next() : object->Previous();

		UIW_WINDOW::Add(object);
//		callSelection = TRUE;
		break;

	case L_SELECT:
		ccode = UIW_WINDOW::Event(UI_EVENT(L_SELECT));
		if (FlagSet(woAdvancedFlags, WOAF_TEMPORARY))
			eventManager->Put(UI_EVENT(S_CLOSE_TEMPORARY), Q_BEGIN);
		break;

	case L_END_SELECT:
		{
		ccode = UIW_WINDOW::Event(event);
#if defined(ZIL_EDIT)
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			break;
#endif
		if (FlagSet(woAdvancedFlags, WOAF_TEMPORARY) && ccode != S_UNKNOWN &&
			(!clipList.First() || clipList.First()->region.Overlap(event.position)))
//			eventManager->Put(UI_EVENT(S_CLOSE_TEMPORARY));					// BUG.1475
			eventManager->Put(UI_EVENT(S_CLOSE_TEMPORARY), Q_BEGIN);
		UI_TIME currentTime;
		long elapsedTime = currentTime - lastTime;							// BUG.1228
		object = Current();
		if (ccode == L_END_SELECT && object && object->userFunction &&
			object->true.Overlap(event.position) &&
			Abs(elapsedTime) < doubleClickRate)
		{
			UI_EVENT uEvent;
			uEvent = event;
			ccode = (*object->userFunction)(object, uEvent, L_DOUBLE_CLICK);
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
		if (ccode == L_BEGIN_SELECT)
		{
			eventManager->DeviceState(E_CURSOR, D_OFF);
			eventManager->DeviceState(E_MOUSE, DM_VIEW);
		}
		break;
	}

	// Check the vertical scroll region.
	if (oldCurrent != current && ccode != S_INITIALIZE)
	{
		UI_EVENT vEvent(S_VSCROLL_SET);
		if (vScroll)
		{
			vEvent.scroll.minimum = First()->relative.top;
			vEvent.scroll.maximum = Last()->relative.top;
			vEvent.scroll.current = Current()->relative.top;
			vEvent.scroll.delta = Current()->relative.Height();
//			vEvent.scroll.showing = true.Height() - vEvent.scroll.delta;	// Bug.1439
			vEvent.scroll.showing = true.Height();
			if (FlagSet(woFlags, WOF_BORDER))
				vEvent.scroll.showing -= 2;
			vEvent.scroll.showing /= vEvent.scroll.delta;
			if (vEvent.scroll.showing > 1)
				vEvent.scroll.showing--;
			vEvent.scroll.showing *= vEvent.scroll.delta;
			vScroll->Event(vEvent);
		}
		vEvent.type = S_VSCROLL_CHECK;
		if (ccode != S_CREATE && event.type != E_MOUSE)
			UIW_WINDOW::Event(vEvent);
	}

	// Check for a special combo-box condition.
	if (FlagSet(woAdvancedFlags, WOAF_TEMPORARY) && parent &&
		oldCurrent != current && ccode != S_INITIALIZE)
	{
		parent->Information(RESET_SELECTION, Current());
//		if (callSelection)													// BUG.1259
//			Current()->UI_WINDOW_OBJECT::Event(UI_EVENT(L_SELECT));
	}

	// Return the control code.
	return (ccode);
}

