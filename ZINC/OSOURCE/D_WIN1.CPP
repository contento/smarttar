//	Zinc Interface Library - D_WIN1.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <ctype.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_WINDOW ---------------------------------------------------------

UI_WINDOW_OBJECT *UIW_WINDOW::Add(UI_WINDOW_OBJECT *object)
{
	int newObject = FALSE;

	// See if the new object exists or is already current.
	if (!object)
		return (NULL);
	else if (object == current)
	{
		object->woStatus |= WOS_CURRENT;
		return (object);
	}
	// Add the object to the list.
	else if (FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) &&
		support.Index(object) == -1)
	{
		newObject = TRUE;
		support.Add(object);
		object->parent = this;
	}
	else if (!FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) &&
		((!object->Previous() && !object->Next() && object != First()) ||
		 UI_LIST::Index(object) == -1))
	{
		newObject = TRUE;
		UI_LIST::Add(object);
		object->parent = this;

		if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT) || FlagSet(object->woStatus, WOS_CURRENT))
		{
			if (current)
				Current()->woStatus &= ~WOS_CURRENT;
			object->woStatus |= WOS_CURRENT;
			current = object;
		}
	}
	else
	{
		UI_WINDOW_OBJECT *tObject = Current();
		UI_REGION updateRegion;
		if (tObject)
			updateRegion = tObject->true;
		else
			updateRegion = object->true;
		// Place MDI objects at the end of the list.
		if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			UI_LIST::Subtract(object);
			UI_LIST::Add(object);
			object->Event(UI_EVENT(S_REGION_DEFINE));
			updateRegion = object->true;
		}
		// Make the old current field non-current.
		current = object;
		if (tObject && tObject != object)
		{
			if (FlagSet(woStatus, WOS_INTERNAL_ACTION))
				tObject->woStatus &= ~WOS_CURRENT;
			else
				tObject->Event(UI_EVENT(S_NON_CURRENT, 0, object->true));
		}
		// Update the new current field.
		if (tObject != object)
		{
			// Make sure the base window is current on the screen.
			for (UI_WINDOW_OBJECT *root = this; root->parent; root = root->parent)
				if (FlagSet(root->woAdvancedFlags, WOAF_TEMPORARY))
					break;
			if (FlagSet(woStatus, WOS_INTERNAL_ACTION) ||
				root != windowManager->First())
				object->woStatus |= WOS_CURRENT;
			else
			{
				eventManager->DeviceState(E_CURSOR, D_OFF);
				object->Event(UI_EVENT(S_CURRENT, 0, updateRegion));
			}
//			if (FlagSet(wnFlags, WNF_AUTO_SELECT) && !FlagSet(wnFlags, WNF_SELECT_MULTIPLE) &&
//				!Inherited(ID_GROUP))
			if (object->screenID && !FlagSet(woStatus, WOS_INTERNAL_ACTION) &&
				FlagSet(wnFlags, WNF_AUTO_SELECT) && !FlagSet(wnFlags, WNF_SELECT_MULTIPLE)) // BUG.1305
				object->Event(L_SELECT);
		}
	}

	// Initialize the object if its parent is already shown on the screen.
	if (screenID && !object->screenID && newObject)
	{
		object->Event(UI_EVENT(S_INITIALIZE));
		object->Event(UI_EVENT(S_CREATE));
		// Define a screen region for MDI objects.
		if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
			object->Event(UI_EVENT(S_REGION_DEFINE));
		if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
			clipList.Split(screenID, object->true, parent && !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) ? FALSE : TRUE);	// BUG.1249 a-2
		if (!current && !FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT) &&
			!FlagSet(object->woFlags, WOF_NON_SELECTABLE | WOF_SUPPORT_OBJECT))
			current = object;
	}

	// Return a pointer to the object.
	return (object);
}

EVENT_TYPE UIW_WINDOW::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	display->VirtualGet(screenID, true);

	UI_REGION region = true;
	if (FlagSet(woFlags, WOF_BORDER) && true.Overlap(event.region))
		DrawBorder(screenID, region, FALSE, ccode);
	if (!support.First() && !First())
	{
		region = true;
		if (FlagSet(woFlags, WOF_BORDER) && (!display->isText || FlagSet(woAdvancedFlags, WOAF_TEMPORARY)))
			--region;
		clipList.Add(new UI_REGION_ELEMENT(screenID, region));
	}
	UI_REGION tRegion;
	for (UI_REGION_ELEMENT *element = clipList.First(); element; element = element->Next())
		if (element->region.Overlap(event.region, tRegion))
			display->Rectangle(screenID, tRegion, lastPalette, 0, TRUE, FALSE, &clip);

	display->VirtualPut(screenID);
	return (FALSE);	// Draw operation is not complete until children are drawn.
}

EVENT_TYPE UIW_WINDOW::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Check for a minimized object.
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW);
	if (FlagSet(woStatus, WOS_MINIMIZED) && icon &&
		ccode != S_CREATE && ccode != L_MOVE && ccode != L_RESTORE && 
		ccode != L_MAXIMIZE && ccode != L_MINIMIZE && 
		(ccode != E_KEY || !FlagSet(event.key.shiftState, S_ALT)))
		return (icon->Event(event));

	// Switch on the event type.
	switch (ccode)
	{
	case S_INITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			object->Event(event);
		for (object = First(); object; object = object->Next())
		{
			if (FlagSet(object->woStatus, WOS_CURRENT) ||
				(!current && !FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
				 !FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT)))
			{
				if (current)
					Current()->woStatus &= ~WOS_CURRENT;
				if (current && FlagSet(wnFlags, WNF_AUTO_SELECT) && !FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
					Current()->woStatus &= ~WOS_SELECTED;
				current = object;
				object->woStatus |= WOS_CURRENT;
				if (FlagSet(wnFlags, WNF_AUTO_SELECT) && !FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
					object->woStatus |= WOS_SELECTED;
			}
			else
				object->woStatus &= ~WOS_CURRENT;
			object->Event(event);
		}
		break;

	case S_DEINITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			object->Event(event);
		for (object = First(); object; object = object->Next())
			object->Event(event);
		break;

	case S_SIZE:
	case S_CREATE:
		if (FlagSet(woStatus, WOS_MINIMIZED) && icon)
		{
			if (parent)
			{
				UI_WINDOW_OBJECT::Event(event);
				icon->relative = relative;
				icon->clip = clip;
				icon->true = true;
				icon->Event(event);
			}
			else if (FlagSet(woStatus, WOS_MINIMIZED))
			{
				true = clip = relative;
				icon->relative = icon->true = icon->clip = relative;
				icon->Event(event);
			}
			break;
		}

		// Compute the object region.
		{
		clipList.Destroy();
		// Specify an absolute minimum size for non-parent and MDI windows.
		if (!parent || FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			int minWidth = 10 * display->cellWidth;
			if (relative.Width() < minWidth)
				relative.right = relative.left + minWidth - 1;
			int minHeight = display->cellHeight;
			if (display->isText)
				minHeight *= 2;
			else
				minHeight += 2 * UIW_BORDER::width;
			if (relative.Height() < minHeight)
				relative.bottom = relative.top + minHeight - 1;
		}
		UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(woStatus, WOS_MAXIMIZED) && parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
			true = clip;
		UI_REGION region = true;
		if (FlagSet(woFlags, WOF_BORDER) && (!parent || !display->isText ||
			FlagSet(woAdvancedFlags, WOAF_TEMPORARY)))
			--region;
		clipList.Add(new UI_REGION_ELEMENT(screenID, region));

		// Compute the support object regions.
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
		{
			object->Event(event);
			if (FlagSet(object->woAdvancedFlags, WOAF_OUTSIDE_REGION))
			{
				UI_REGION region = object->true;
				clipList.Destroy();
				if (display->isText)
					clipList.Add(new UI_REGION_ELEMENT(screenID, region));
				else
					clipList.Add(new UI_REGION_ELEMENT(screenID, --region));
			}
			else if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
				clipList.Split(screenID, object->true, parent && !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) ? FALSE : TRUE);	// BUG.1249 a-1
		}
		// Compute the remaining object regions.
		for (object = First(); object; object = object->Next())
		{
			object->Event(event);
			if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
				clipList.Split(screenID, object->true, parent && !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) ? FALSE : TRUE);	// BUG.1249 a-2
		}
		}
		break;

	case S_RESET_DISPLAY:
		{
		UI_WINDOW_OBJECT::Event(event);

		// Convert each object's coordinates from graphics.
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			object->Event(event);
		for (object = First(); object; object = object->Next())
			object->Event(event);
		}
		break;

	case S_REGION_DEFINE:
		{
		UI_REGION region = true;
		if (parent)
		{
			region.left = Max(region.left, clip.left);
			region.top = Max(region.top, clip.top);
			region.right = Min(region.right, clip.right);
			region.bottom = Min(region.bottom, clip.bottom);
		}
		if (display->isText)
		{
			for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
				if (FlagSet(object->woAdvancedFlags, WOAF_OUTSIDE_REGION) &&
					!FlagSet(woStatus, WOS_MAXIMIZED))
				{
					if (region.right == true.right)
						region.right--;
					if (region.bottom == true.bottom)
						region.bottom--;
					break;
				}
		}
		if (current)
			Current()->woStatus |= WOS_REDISPLAY;
		display->RegionDefine(screenID, region);
		if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			for (object = First(); object; object = object->Next())
				if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
					object->Event(event);
		}
		}
		break;

	case L_MOVE:
	case L_SIZE:
		if (event.type != E_KEY && !event.rawCode && event.data != this)
			Current()->Event(event);
		else if ((ccode == L_MOVE && !FlagSet(woStatus, WOS_MAXIMIZED) &&
			  (!FlagSet(woAdvancedFlags, WOAF_NO_MOVE) ||
			   FlagSet(woStatus, WOS_MINIMIZED))) ||
			(ccode == L_SIZE && !FlagSet(woStatus, WOS_MAXIMIZED) &&	// BUG.1668
			 !FlagSet(woStatus, WOS_MINIMIZED) &&
			 !FlagSet(woAdvancedFlags, WOAF_NO_SIZE)))
		{
			UI_EVENT sEvent;
			sEvent = event;
			if (ccode != sEvent.type)
			{
				sEvent.type = ccode;
				sEvent.rawCode = 0;
			}
			Modify(sEvent);
#if defined(ZIL_EDIT)
			if (FlagSet(woStatus, WOS_EDIT_MODE))
			{
				UI_EVENT dEvent(D_SET_OBJECT);
				dEvent.rawCode = searchID;
				dEvent.data = this;
				eventManager->Put(dEvent);
			}
#endif
		}
		break;

	case S_MDICHILD_EVENT + S_CLOSE:
		if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && current)
		{
			object = Current();

			// See if menu is current.
			if (FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) && object->userObject) // BUG.1244
				object = (UI_WINDOW_OBJECT *)object->userObject;
			UIW_WINDOW::Subtract(object);

			// Redefine and redisplay the MDI children.
			UI_EVENT tEvent(S_REGION_DEFINE);								// BUG.268
			Event(tEvent);
			tEvent.type = S_CURRENT;
			tEvent.region = object->true;
			Event(tEvent);
			if (!FlagSet(object->woAdvancedFlags, WOAF_NO_DESTROY))
				delete object;
		}
		break;

	case L_MDICHILD_EVENT + L_MOVE:
	case L_MDICHILD_EVENT + L_SIZE:
	case S_MDICHILD_EVENT + S_RESTORE:
	case S_MDICHILD_EVENT + S_MINIMIZE:
	case S_MDICHILD_EVENT + S_MAXIMIZE:
		{
		UI_EVENT tEvent = event;
		tEvent.rawCode = 0;
		if (ccode == L_MDICHILD_EVENT + L_MOVE ||
			ccode == L_MDICHILD_EVENT + L_SIZE)
			tEvent.type = ccode - L_MDICHILD_EVENT;
		else
			tEvent.type = ccode - S_MDICHILD_EVENT;
		if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && current)
		{
			tEvent.data = Current();
			Current()->Event(tEvent);
		}
		else if (parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			tEvent.data = this;
			Event(tEvent);
		}
		else
			ccode = S_UNKNOWN;
		}
		break;

	case L_RESTORE:
	case L_MINIMIZE:
	case L_MAXIMIZE:
		if (event.type != E_KEY && !event.rawCode && event.data != this)
		{
			Current()->Event(event);
			break;
		}
		{
		UI_REGION region = true;
		if ((ccode == L_RESTORE && FlagSet(woStatus, WOS_MAXIMIZED | WOS_MINIMIZED)) ||
			(ccode == L_MINIMIZE && FlagSet(woStatus, WOS_MINIMIZED)) ||
			(ccode == L_MAXIMIZE && FlagSet(woStatus, WOS_MAXIMIZED)))
		{
			relative = restore;
			woStatus &= ~(WOS_MAXIMIZED | WOS_MINIMIZED);
		}
		else if (ccode == L_MINIMIZE && !FlagSet(woStatus, WOS_MAXIMIZED))
		{
			restore = relative;
			if (!icon)
			{
				int minHeight;
				if (!display->isText)
					minHeight = display->cellHeight - 2 -
						display->preSpace - display->postSpace +
						2 * UIW_BORDER::width;
				else
					minHeight = 2;
				int minWidth = 14 * display->cellWidth;						// Bug.General
				if (relative.left < 0)										// Bug.1436
					relative.left = 0;
				else if (relative.left + minWidth - 1 >= display->columns)
					relative.left = display->columns - minWidth;
				if (relative.top < 0)										// Bug.1436
					relative.top = 0;
				else if (relative.top + minHeight - 1 >= display->lines)
					relative.top = display->lines - minHeight;
				relative.right = relative.left + minWidth - 1;
				relative.bottom = relative.top + minHeight - 1;
				if (!parent)
					true = relative;
			}
			else if (parent)
				relative = icon->relative;
			else
				true = relative = icon->true;
			woStatus |= WOS_MINIMIZED;
		}
		else if (ccode == L_MAXIMIZE && !FlagSet(woStatus, WOS_MINIMIZED))
		{
			restore = relative;
			relative.left = relative.top = 0;
			if (parent)
			{
				relative.right = parent->relative.right - parent->relative.left;
				relative.bottom = parent->relative.bottom - parent->relative.top;
			}
			else
			{
				relative.right = display->columns - 1;
				relative.bottom = display->lines - 1;
			}
			woStatus |= WOS_MAXIMIZED;
		}
		else
			return (S_UNKNOWN);
		Event(UI_EVENT(S_CREATE));
		eventManager->DeviceState(E_CURSOR, D_OFF);
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		if (!parent)
		{
			display->RegionDefine(ID_SCREEN, 0, 0, display->columns, display->lines);
			for (UI_WINDOW_OBJECT *object = windowManager->Last(); object; object = object->Previous())
				object->Event(UI_EVENT(S_REGION_DEFINE));
			display->Rectangle(ID_SCREEN, region, display->backgroundPalette, 0, TRUE);
			for (object = windowManager->Last(); object; object = object->Previous())
				if (object == this)
					object->Event(UI_EVENT(S_CURRENT, 0, true));
				else if (object->true.Overlap(region))
					object->Event(UI_EVENT(S_DISPLAY_INACTIVE, 0, region));
		}
		else
		{
			region.left = Min(region.left, true.left);
			region.top = Min(region.top, true.top);
			region.right = Max(region.right, true.right);
			region.bottom = Max(region.bottom, true.bottom);
			if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
				parent->Event(UI_EVENT(S_REGION_DEFINE));
			parent->Event(UI_EVENT(S_CURRENT, 0, region));
		}
		}
		break;

	case S_HSCROLL_CHECK:
	case S_VSCROLL_CHECK:
	case S_HSCROLL:
	case S_VSCROLL:
		{
		object = Current();
		if (!object)
			return (S_UNKNOWN);
		int hDelta = 0, vDelta = 0;
		UI_REGION region;
		if (clipList.First())
			region = clipList.First()->region;
		else
			region = true;
		region.Overlap(clip, region);
		if (ccode == S_VSCROLL_CHECK && object->true.bottom > region.bottom)
		{
			for (; object && object->true.bottom > region.bottom; object = object->Previous())
				vDelta += (object->true.top - object->true.bottom - 1);
		}
		else if (ccode == S_HSCROLL_CHECK && object->true.right > region.right)
		{
			int position = -1;
			for (; object && object->true.right > region.right; object = object->Previous())
				if (object->true.right != position)
				{
					hDelta += (object->true.left - object->true.right - 1);
					position = object->true.right;
				}
		}
		else if (ccode == S_VSCROLL_CHECK && object->true.top < region.top)
		{
			for (; object && object->true.top < region.top; object = object->Next())
				vDelta += (object->true.bottom - object->true.top + 1);
		}
		else if (ccode == S_HSCROLL_CHECK && object->true.left < region.left)
		{
			int position = -1;
			for (; object && object->true.left < region.left; object = object->Next())
				if (object->true.left != position)
				{
					hDelta += (object->true.right - object->true.left + 1);
					position = object->true.left;
				}
		}
		else if (ccode == S_HSCROLL && hScroll)
		{
			hScroll->Event(event);
			hDelta = -event.scroll.delta;
		}
		else if (ccode == S_VSCROLL && vScroll)
		{
			vScroll->Event(event);
			vDelta = -event.scroll.delta;
		}

		// Make sure we need to scroll the window objects.
		if (hDelta && First()->true.left + hDelta > region.left)
			hDelta = region.left - First()->true.left;
		else if (hDelta && Last()->true.left + hDelta < region.left)
			hDelta = region.left - Last()->true.left;
		if (vDelta && First()->true.top + vDelta > region.top)
			vDelta = region.top - First()->true.top;
		else if (vDelta && Last()->true.top + vDelta < region.top)
			vDelta = region.top - Last()->true.top;
		if (vDelta == 0 && hDelta == 0)
			return (S_UNKNOWN);

		// Update the window object positions.
		for (object = First(); object; object = object->Next())
			if (!FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
			{
				object->true.left += hDelta;
				object->true.top += vDelta;
				object->true.right += hDelta;
				object->true.bottom += vDelta;
			}
		if (FlagSet(woStatus, WOS_INTERNAL_ACTION))
			break;

		// Update the screen information.
		if (true.top < 0 || true.left < 0 || true.right >= display->columns || true.bottom >= display->lines)
			region = true;
		else if ((vDelta < 0 && Abs(vDelta) < region.bottom - region.top + 1) ||
			(hDelta < 0 && Abs(hDelta) < region.right - region.left + 1))
		{
			region.left -= hDelta;
			region.top -= vDelta;
			display->RegionMove(region, region.left + hDelta, region.top + vDelta);
			if (hDelta < 0)
				region.left = region.right + hDelta;
			else if (vDelta < 0)
				region.top = region.bottom + vDelta;
		}
		else if ((vDelta > 0 && vDelta < region.bottom - region.top + 1) ||
			(hDelta > 0 && hDelta < region.right - region.left + 1))
		{
			region.right -= hDelta;
			region.bottom -= vDelta;
			display->RegionMove(region, region.left + hDelta, region.top + vDelta);
			if (hDelta > 0)
				region.right = region.left + hDelta;
			else if (vDelta > 0)
				region.bottom = region.top + vDelta;
		}

		// Update the window according to the scrolled region.
		UIW_WINDOW::Event(UI_EVENT(S_CURRENT, 0, region));
		}
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		if (!screenID)
			break;
		{
		UI_EVENT event1 = event;					// BUG.1732
		UI_PALETTE *palette = LogicalPalette(ccode);
		if (palette != lastPalette)
		{
			event1.region = true;
			// Include border...
			++event1.region;
		}

		UI_WINDOW_OBJECT::Event(event1);

		// Check the status of the currently selected object.
		object = Current();
		if (ccode == S_CURRENT)
			eventManager->DeviceState(E_CURSOR, D_OFF);
		else if (ccode == S_NON_CURRENT && !parent && 
			object && FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) &&
			object->userObject && object->Inherited(ID_PULL_DOWN_MENU))
		{
			// Restore the old current field before moving to the next window.
			UIW_WINDOW::Add((UI_WINDOW_OBJECT *)object->userObject);
			object->userObject = NULL;
		}

		int virtualGet = FALSE;
		if (display->isText || (parent && true.Overlap(event1.region)))
		{
			display->VirtualGet(screenID, true);
			virtualGet = TRUE;
		}

		// Draw the window's border and fill the background.
		if (FlagSet(woStatus, WOS_REDISPLAY) && searchID != ID_COMBO_BOX)	// BUG.1455
			DrawItem(event1, ccode);					// BUG.1560

		// Update the children.
		UI_EVENT cEvent, aEvent;
		cEvent = event1;
		aEvent = event1;
		if (ccode == S_CURRENT)
			aEvent.type = S_DISPLAY_ACTIVE;
		else if (ccode == S_NON_CURRENT)
			aEvent.type = S_DISPLAY_INACTIVE;
		else if (ccode == S_DISPLAY_ACTIVE && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && parent)
			cEvent.type = aEvent.type = S_DISPLAY_INACTIVE;
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			if (object == current || FlagSet(object->woStatus, WOS_CURRENT))
				object->Event(cEvent);
			else
				object->Event(aEvent);
		if (!current && first)
		{
//			current = FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) ? last : first;
//			Current()->woStatus |= WOS_CURRENT;								// BUG.1654B
			object = FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) ? Last() : First();
			while (object && !current)
			{
				if (!FlagSet(object->woFlags, WOF_NON_SELECTABLE) && !FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
				{
					current = object;
					Current()->woStatus |= WOS_CURRENT;
				}
				object = FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) ? object->Previous() : object->Next();
			}
		}
		for (object = First(); object; object = object->Next())
		{
			// Check for the current field and prevent multiple calls 
			// for the S_NON_CURRENT message.
			if (object == current)
			{
//				if (!FlagSet(wnFlags, WNF_SELECT_MULTIPLE) && ccode == S_CURRENT)
				if (ccode == S_CURRENT)										// BUG.1178
					object->woStatus |= WOS_CURRENT;
				if (FlagSet(object->woStatus, WOS_CURRENT))
					object->Event(cEvent);
				else
					object->Event(aEvent);
			}
			else if (object->true.Overlap(event1.region))
			{
				if (!FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
					object->woStatus &= ~WOS_CURRENT;
				object->Event(aEvent);
			}
		}
		if (virtualGet)
			display->VirtualPut(screenID);
		woStatus &= ~WOS_REDISPLAY;
		}
		break;

	case S_REDISPLAY:
		UI_WINDOW_OBJECT::Event(event);
		break;

	case S_ADD_OBJECT:
		object = (UI_WINDOW_OBJECT *)event.data;
#if defined(ZIL_EDIT)
		if (UI_LIST::Index(object) == -1 && FlagSet(object->woStatus, WOS_EDIT_MODE))
		{
			UI_REGION region;
			Information(GET_CLIPREGION, &region, NULL);						// BUG.General
/* START BLOCK COMMENT
**				if (clipList.First())
**					region = clipList.First()->region;
**				else
**					region = true;
END BLOCK COMMENT */
			if (!FlagSet(object->woStatus, WOS_GRAPHICS))
			{
				region.left /= display->cellWidth;
				region.top /= display->cellHeight;
			}
			object->relative.left -= region.left;
			object->relative.top -= region.top;
			object->relative.right -= region.left;
			object->relative.bottom -= region.top;
		}
#endif
		UIW_WINDOW::Add(object);
		break;

	case S_SUBTRACT_OBJECT:
		// Subtract the window object from the window.
		Subtract((UI_WINDOW_OBJECT *)event.data);
		break;

	case L_HELP:
		// See if the low level object processes the help.
		if (!FlagSet(woStatus, WOS_MINIMIZED) &&							// Bug.1419
			current && Current()->Event(event) != S_UNKNOWN)
			break;
		else if (parent && helpContext == NO_HELP_CONTEXT)
			ccode = S_UNKNOWN;
		else if (helpSystem)
			helpSystem->DisplayHelp(windowManager, helpContext);
		break;

	case L_FIRST:
		if (FlagSet(woStatus, WOS_MINIMIZED))								// Bug.1419
			return (S_UNKNOWN);
		ccode = current ? Current()->Event(event) : S_UNKNOWN;
		for (object = First(); object && ccode == S_UNKNOWN; object = object->Next())
			if (!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT) &&
				!FlagSet(object->woFlags, WOF_NON_SELECTABLE))
			{
				wnFlags |= WNF_MOVE_REVERSE;
				UIW_WINDOW::Add(object);
				wnFlags &= ~WNF_MOVE_REVERSE;
				ccode = L_FIRST;
			}
		break;

	case L_LAST:
		if (FlagSet(woStatus, WOS_MINIMIZED))								// Bug.1419
			return (S_UNKNOWN);
		ccode = current ? Current()->Event(event) : S_UNKNOWN;
		for (object = Last(); object && ccode == S_UNKNOWN; object = object->Previous())
			if (!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT) &&
				!FlagSet(object->woFlags, WOF_NON_SELECTABLE))
			{
				wnFlags |= WNF_MOVE_FORWARD;
				UIW_WINDOW::Add(object);
				wnFlags &= ~WNF_MOVE_FORWARD;
				ccode = L_LAST;
			}
		break;

	case L_PREVIOUS:
		if (FlagSet(woStatus, WOS_MINIMIZED))								// Bug.1419
			return (S_UNKNOWN);
		{
		EVENT_TYPE tcode = ccode;
		ccode = S_UNKNOWN;
		// Make sure there is a current object.
		if (!current)
			UIW_WINDOW::Add(Last());
		// See if the current object processes the message.
		else if (Current()->Event(event) != S_UNKNOWN)
			ccode = tcode;
		// Don't move if on a support object.
		else if (FlagSet(Current()->woFlags, WOF_SUPPORT_OBJECT))
			UI_ERROR_SYSTEM::Beep();
		// Don't move if on the first field of a non-wrapping window.
		else if (!Current()->Previous() && FlagSet(wnFlags, WNF_NO_WRAP))
			break;
		// Go to the previous field in the window.
		else
		{
			object = Current()->Previous() ? Current()->Previous() : Last();
			while (object && object != current && ccode == S_UNKNOWN)
				if (!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
					!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
				{
					wnFlags |= WNF_MOVE_REVERSE;
					UIW_WINDOW::Add(object);
					wnFlags &= ~WNF_MOVE_REVERSE;
					ccode = tcode;
				}
				else if (object->Previous())
					object = object->Previous();
				else if (FlagSet(wnFlags, WNF_NO_WRAP))
					break;
				else
					object = Last();
			if (FlagSet(Current()->woFlags, WOF_NON_SELECTABLE))	// BUG.1255
				current = NULL;
		}
		}
		break;

	case L_MDICHILD_EVENT + L_NEXT_WINDOW:
		if (parent || !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
			return (S_UNKNOWN);
		// Continue to L_NEXT_WINDOW.
	case L_NEXT:
		if (FlagSet(woStatus, WOS_MINIMIZED))								// Bug.1419
			return (S_UNKNOWN);
		{
		EVENT_TYPE tcode = ccode;
		ccode = S_UNKNOWN;
		// Make sure there is a current object.
		if (!current)
		{
			for (object = First(); object && FlagSet(object->woFlags, WOF_NON_SELECTABLE); object = object->Next())	// BUG.1444
				;
			if (object)
				UIW_WINDOW::Add(object);
		}
		// See if the current object processes the message.
		else if (Current()->Event(event) != S_UNKNOWN)
			ccode = tcode;
		// Don't move if on a support object.
		else if (FlagSet(Current()->woFlags, WOF_SUPPORT_OBJECT))
			UI_ERROR_SYSTEM::Beep();
		// Don't move if on the last field of a non-wrapping window.
		else if (!Current()->Next() && FlagSet(wnFlags, WNF_NO_WRAP))
			;
		// Go to the next field in the window.
		else
		{
			object = Current()->Next() ? Current()->Next() : First();
			while (object && object != current && ccode == S_UNKNOWN)
				if (!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
					!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
				{
					wnFlags |= WNF_MOVE_FORWARD;
					UIW_WINDOW::Add(object);
					wnFlags &= ~WNF_MOVE_FORWARD;
					ccode = tcode;
				}
				else if (object->Next())
					object = object->Next();
				else if (FlagSet(wnFlags, WNF_NO_WRAP))
					break;
				else
					object = First();
			if (FlagSet(Current()->woFlags, WOF_NON_SELECTABLE))	// BUG.1255
				current = NULL;
		}
		}
		break;

	case L_CANCEL:
	case S_ALT_KEY:
		// Bring up or take down the pull-down menu (if any).
		for (object = (UI_WINDOW_OBJECT *)support.Last(); object; object = object->Previous())
			if (object->Inherited(ID_PULL_DOWN_MENU) && !FlagSet(woStatus, WOS_MINIMIZED))
			{
				// Restore the old selected window object.
				if (FlagSet(object->woStatus, WOS_CURRENT))
				{
					if (object->userObject)
						UIW_WINDOW::Add((UI_WINDOW_OBJECT *)object->userObject);
					else
					{
						object->woStatus &= ~WOS_CURRENT;
						current = NULL;
						object->Event(UI_EVENT(S_NON_CURRENT, 0, object->true));
					}
				}
				// Activate the pull-down menu.
				else if (ccode == S_ALT_KEY)
				{
					object->userObject = current;
					UIW_WINDOW::Add(object);
				}
				return (ccode);
			}
			else if (ccode == S_ALT_KEY && object->Inherited(ID_SYSTEM_BUTTON))
			{
				object->Event(UI_EVENT(L_SELECT));
				return (ccode);
			}
		ccode = S_UNKNOWN;
		break;

#if defined(ZIL_EDIT)
	case L_BEGIN_ESCAPE:
	case L_CONTINUE_ESCAPE:
	case L_END_ESCAPE:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
		// Continue to E_MOUSE.
#endif
	case E_MOUSE:
	case L_VIEW:
	case L_BEGIN_SELECT:
	case L_CONTINUE_SELECT:
	case L_END_SELECT:
		// Check for mouse out-of-range.
		if (!true.Overlap(event.position))
		{
			if (current)
				return (Current()->Event(event));
			eventManager->DeviceState(E_MOUSE, DM_VIEW);
			return (S_UNKNOWN);
		}

		// Check for an active support object.
		object = (UI_WINDOW_OBJECT *)support.Current();
		if (object && !object->true.Overlap(event.position))
		{
#if defined(ZIL_EDIT)
			if (!FlagSet(woStatus, WOS_EDIT_MODE))
#endif
				object->Event(event);
			support.SetCurrent(NULL);
		}

		// Let object unactivate its region.
		if ((ccode == L_END_SELECT || ccode == L_END_ESCAPE) &&
			current && !Current()->true.Overlap(event.position))
		{
			Current()->Event(event);
			if (FlagSet(Current()->woAdvancedFlags, WOAF_NON_CURRENT))
			{
				for (object = First(); object; object = object->Next())
					if (FlagSet(object->woStatus, WOS_CURRENT))
					{
						current = object;
						break;
					}
			}
		}

		// See if the message should go to the current object.
		if (ccode == L_CONTINUE_SELECT && FlagSet(wnFlags, WNF_CONTINUE_SELECT))
			;
		else if (current && event.rawCode != 0 &&
			!FlagSet(event.rawCode, M_LEFT_CHANGE | M_MIDDLE_CHANGE | M_RIGHT_CHANGE))
			return (Current()->Event(event));

//		object = support.Last() ? (UI_WINDOW_OBJECT *)support.Last() : Last(); // BUG.1372
		object = support.First() ? (UI_WINDOW_OBJECT *)support.First() : Last();
		while (object)
		{
			if ((!FlagSet(object->woFlags, WOF_NON_SELECTABLE) || FlagSet(woStatus, WOS_EDIT_MODE)) && 
				((FlagSet(object->woAdvancedFlags, WOAF_OUTSIDE_REGION) &&
				  true.Overlap(event.position) && !object->true.Overlap(event.position)) ||
				 (!FlagSet(object->woAdvancedFlags, WOAF_OUTSIDE_REGION) &&
				  object->true.Overlap(event.position))))
			{
				if (FlagSet(woStatus, WOS_MINIMIZED) &&
					(!FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) ||
					!object->Inherited(ID_BUTTON)))
					return (S_UNKNOWN);
				else if (FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
				{
					if (FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
						;
					else if (ccode == L_BEGIN_SELECT)
						current = object;
					else if (ccode == L_END_SELECT)
					{
						for (UI_WINDOW_OBJECT *tObject = First(); tObject; tObject = tObject->Next())
							if (FlagSet(tObject->woStatus, WOS_CURRENT))
						{
							current = tObject;
							break;
						}
					}
				}
				else if (ccode == L_BEGIN_SELECT || (ccode == L_CONTINUE_SELECT &&
					FlagSet(wnFlags, WNF_CONTINUE_SELECT)))
				{
					if (object != current && FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
						object->userObject = current;
					UIW_WINDOW::Add(object);
				}
				if (event.rawCode && FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
				{

					support.SetCurrent(object);
					object->Event(event);
#if defined(ZIL_EDIT)
//					if (FlagSet(woStatus, WOS_EDIT_MODE))					// BUG.731
					if (FlagSet(woStatus, WOS_EDIT_MODE) && !parent)
						UI_WINDOW_OBJECT::Event(event);
#endif
				}
				else
					object->Event(event);
				return (ccode);
			}

//			// Get the next object.											// BUG.1372
//			object = (!object->Previous() && FlagSet(object->woFlags, WOF_SUPPORT_OBJECT)) ?
//				Last() : object->Previous();
			if (FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
				object = object->Next() ? object->Next() : Last();
			else
				object = object->Previous() ? object->Previous() : NULL;
		}

		// Object does not overlap any regions.
		woStatus &= ~WOS_AUTO_MARK;
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		UI_WINDOW_OBJECT::Event(event);
		break;

	default:
		if (ccode == E_KEY && (FlagSet(event.key.shiftState, S_ALT) ||
			FlagSet(woAdvancedFlags, WOAF_NORMAL_HOT_KEYS)))
		{
			UCHAR tHotKey;
			if (FlagSet(woAdvancedFlags, WOAF_NORMAL_HOT_KEYS) &&
				!FlagSet(event.key.shiftState, S_ALT))
				tHotKey = event.key.value;
			else
				tHotKey = UI_EVENT_MAP::MapEvent(hotKeyMapTable, event);
			tHotKey = tolower(tHotKey);

			// Find a matching hotkey.
			for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
				if (object->hotKey == HOT_KEY_SUB_WINDOW &&
					!FlagSet(woStatus, WOS_MINIMIZED) &&					// BUG.1279
					object->Event(event) != S_UNKNOWN)
				{
					if (object != current)
					{
						object->userObject = current;
						UIW_WINDOW::Add(object);
					}
					return (ccode);
				}
				else if (object->hotKey == tHotKey &&
					!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
					(FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) ||		// BUG.1279
					!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT)))
				{
					if (object->Inherited(ID_BUTTON) || object->Inherited(ID_ICON))
						ccode = object->Event(UI_EVENT(L_SELECT));
					return (ccode);
				}
			if (FlagSet(woStatus, WOS_MINIMIZED))							// Bug.1419
				return (S_UNKNOWN);
			for (object = First(); object; object = object->Next())
				if (object->hotKey == HOT_KEY_SUB_WINDOW && object->Event(event) != S_UNKNOWN)
				{
					UIW_WINDOW::Add(object);
					return (ccode);
				}
				else if (object->hotKey == tHotKey &&
					!FlagSet(object->woFlags, WOF_NON_SELECTABLE))	// BUG.7211d
//					!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
//					!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
				{
					if (searchID == ID_PULL_DOWN_MENU && current == object)	// BUG.internal
						current = NULL;			// BUG.1485
					if (!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))	// BUG.7211d
						UIW_WINDOW::Add(object);
					if (object->Inherited(ID_BUTTON) || object->Inherited(ID_ICON))
						ccode = object->Event(UI_EVENT(L_SELECT));
					return (ccode);
				}
			ccode = S_UNKNOWN;
		}
		else if (FlagSet(woStatus, WOS_MINIMIZED))							// Bug.1419
			return (S_UNKNOWN);
		else
			ccode = current ? Current()->Event(event) : S_UNKNOWN;
		break;
	}

	// Return the control code.
	return (ccode);
}

void UIW_WINDOW::RegionMax(UI_WINDOW_OBJECT *object)
{
	// Check to see if it is the top level object.
	if (object == this && (!parent || FlagSet(woAdvancedFlags, WOAF_TEMPORARY)))
	{
		clip = true = relative;
		clipList.Destroy();
		return;
	}

	// Find the appropriate region element.
	UI_REGION_ELEMENT *element = clipList.First();
	UI_REGION region;
	if (element)						// Get first available region.
		region = element->region;
	else								// Determine the update region.
	{
		region = true;
		if (FlagSet(woFlags, WOF_BORDER) && (!display->isText || FlagSet(woAdvancedFlags, WOAF_TEMPORARY)))
			--region;
		clipList.Add(new UI_REGION_ELEMENT(screenID, region));
	}
	int bottomRegion = FlagsSet(object->woFlags, WOF_NON_FIELD_REGION | WOF_JUSTIFY_RIGHT);
	for ( ; element; element = element->Next())
		if (!bottomRegion && region.top > element->region.top)
			region = element->region;
		else if (bottomRegion && region.bottom < element->region.bottom)
			region = element->region;

	// Compute the actual field region.
	object->clip = object->true = region;
	if (object->clip.left < object->parent->clip.left)
		object->clip.left = object->parent->clip.left;
	if (object->clip.top < object->parent->clip.top)
		object->clip.top = object->parent->clip.top;
	if (object->clip.right > object->parent->clip.right)
		object->clip.right = object->parent->clip.right;
	if (object->clip.bottom > object->parent->clip.bottom)
		object->clip.bottom = object->parent->clip.bottom;
	if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
		object->true = region;
	else
	{
		object->true.left = region.left + object->relative.left;
		object->true.top = region.top + object->relative.top;
		object->true.right = region.left + object->relative.right;
		object->true.bottom = region.top + object->relative.bottom;
	}
}

UI_WINDOW_OBJECT *UIW_WINDOW::Subtract(UI_WINDOW_OBJECT *object)
{
	UI_WINDOW_OBJECT *nextObject = object ? object->Next() : NULL;

	// Remove the object from the list.
	if (!object)
		return (nextObject);
	else if (UI_LIST::Index(object) != -1)
	{
		UI_LIST::Subtract(object);
		object->Event(UI_EVENT(S_DEINITIALIZE));

		// Check for active menu object.
		if (current && Current()->userObject == object)
		{
			Current()->userObject = NULL;
			for (UI_WINDOW_OBJECT *tObject = First(); tObject; tObject = tObject->Next())
				if (!FlagSet(tObject->woFlags, WOF_NON_SELECTABLE) &&
					!FlagSet(tObject->woAdvancedFlags, WOAF_NON_CURRENT))
				{
					tObject->woStatus |= WOS_CURRENT;
					Current()->userObject = tObject;
					break;
				}
		}
	}
	else if (support.Index(object) != -1)
	{
		support.Subtract(object);
		object->screenID = 0;
		if (object == current)	// Check for a current support object.
			current = First();
		if (object == vScroll)
			vScroll = NULL;
		else if (object == hScroll)
			hScroll = NULL;
		else if (object == icon)
			icon = NULL;
	}
	object->parent = NULL;

	// Recalculate all other pieces and then return a pointer to the next object.
	Event(UI_EVENT(S_REGION_DEFINE));					// BUG.1233.A
	return (nextObject);
}

