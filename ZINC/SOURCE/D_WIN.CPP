//	Zinc Interface Library - D_WIN.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define USE_RAW_KEYS
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UI_WINDOW_MANAGER --------------------------------------------------

UI_WINDOW_OBJECT *UI_WINDOW_MANAGER::Add(UI_WINDOW_OBJECT *object)
{
	// Check for a null object.
	if (!object)
		return (NULL);

	// Remove any temporary objects if we have a non-temporary window.
	UI_EVENT event;
	UI_WINDOW_OBJECT *firstObject = First();
	if (!FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY))
	{
		while (firstObject && FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY))
		{
			UI_WINDOW_MANAGER::Subtract(firstObject);
			if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
				delete firstObject;
			firstObject = First();
		}
	}
	// Remove any temporary objects if the window is already in the list.
	else if (UI_LIST::Index(object) != -1)
	{
		while (firstObject && firstObject != object)
		{
			UI_WINDOW_MANAGER::Subtract(firstObject);
			if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
				delete firstObject;
			firstObject = First();
		}
	}
	// See if the object is already the first on the list.
	if (object == First())
		return (object);

	// Find the maximum update region.
	UI_REGION updateRegion;
	updateRegion.left = updateRegion.top = 0x0FFF;
	updateRegion.right = updateRegion.bottom = 0x0000;
	for (UI_WINDOW_OBJECT *tObject = First(); tObject && tObject != object;
		tObject = tObject->Next())
		if (object->true.Overlap(tObject->true))
		{
			updateRegion.left = Min(updateRegion.left, tObject->true.left);
			updateRegion.top = Min(updateRegion.top, tObject->true.top);
			updateRegion.right = Max(updateRegion.right, tObject->true.right);
			updateRegion.bottom = Max(updateRegion.bottom, tObject->true.bottom);
		}

	// Initialize the object region.
	if (!object->screenID || FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY))
	{
		event.type = S_INITIALIZE;
		object->Event(event);
		event.type = S_CREATE;
		if (!object->screenID)
		{
			object->screenID = ++currentScreenID;
			object->Event(event);
		}
		else
			object->Event(event);

		// Make sure the temporary window is visible on the screen.
		if (FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY))
		{
			if (object->relative.left < 0)
			{
//				object->relative.right += object->relative.left;			// BUG.General
				object->relative.right -= object->relative.left;
				object->relative.left = 0;
			}
			else if (object->relative.right >= display->columns)
			{
				object->relative.left += (display->columns - 1 - object->relative.right);
				object->relative.right = display->columns - 1;
			}
			if (object->relative.bottom >= display->lines && object->parent)
			{
				int height = object->relative.bottom - object->relative.top;
				object->relative.bottom = object->parent->true.top - 1;
				object->relative.top = object->relative.bottom - height;
				if (object->relative.top < 0)
				{
					object->relative.top = 0;
					object->relative.bottom = height;
				}
			}
			event.type = S_CREATE;
			object->Event(event);
		}
		updateRegion = object->true;
	}

	// Bring the object to the front of the object queue.
	eventManager->DeviceState(E_CURSOR, D_OFF);
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	if (object != firstObject)
	{
		if (UI_LIST::Index(object) != -1)
			UI_LIST::Subtract(object);
		else
			updateRegion = object->true;
//		event.type = S_REGION_DEFINE;					// BUG.7158
//		object->Event(event);
		UI_LIST::Add(firstObject, object);
		event.type = S_NON_CURRENT;
		event.region.left = event.region.top = event.region.right = event.region.bottom = -1;
		if (firstObject && !FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY))
			firstObject->Event(event);
		event.type = S_REGION_DEFINE;					// BUG.7158
		object->Event(event);
	}
	event.type = S_CURRENT;
	event.region = updateRegion;
	object->Event(event);

	// Return a pointer to the object.
	return (object);
}

EVENT_TYPE UI_WINDOW_MANAGER::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Check for an empty list.
	UI_WINDOW_OBJECT *firstObject = First();
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW_MANAGER);
	if (!firstObject && ccode == L_EXIT)
		return (L_EXIT);
	else if (!firstObject && ccode != L_EXIT_FUNCTION)
		return (S_NO_OBJECT);

	// Switch on the event type.
	switch (ccode)
	{
	case S_CASCADE:
		{
		UI_EVENT event;
		int column = 0, line = 0;
		display->RegionDefine(ID_SCREEN, 0, 0, display->columns, display->lines);
		// Recompute the object regions.
		for (object = Last(); object; object = object->Previous())
		{
			int width = object->relative.right - object->relative.left + 1;
			int height = object->relative.bottom - object->relative.top + 1;
			if (line + height >= display->lines)
				line = 0;
			object->relative.left = column;
			object->relative.top = line;
			object->relative.right = column + width - 1;
			object->relative.bottom = line + height - 1;
			event.type = S_CREATE;
			object->Event(event);
			event.type = S_REGION_DEFINE;
			object->Event(event);
			column += display->cellWidth * 3;
			line += display->cellHeight * 3 / 2;
		}
		}
		// Continue to S_REDISPLAY.
	case S_REDISPLAY:
		// Redisplay the objects on the screen.
		if (display->isText)
			display->VirtualGet(ID_SCREEN, 0, 0, display->columns, display->lines);
		display->Rectangle(ID_SCREEN, true, display->backgroundPalette, 0, TRUE);
		for (object = Last(); object; object = object->Previous())
			object->Event(UI_EVENT((object == First()) ? S_CURRENT : S_DISPLAY_INACTIVE, 0, object->true));
		if (display->isText)
			display->VirtualPut(ID_SCREEN);
		break;

	case S_RESET_DISPLAY:
		{
		UI_WINDOW_OBJECT *tCurrent = NULL;				// BUG.7212
		if (screenID != ID_SCREEN)
			for (tCurrent = First(); tCurrent && tCurrent->screenID != screenID; tCurrent = tCurrent->Next())
				;	// NOTHING
		while (firstObject && FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY))
		{
			UI_WINDOW_MANAGER::Subtract(firstObject);
			if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
				delete firstObject;
			firstObject = First();
		}
		if (!event.data)
		{
			woStatus &= ~WOS_GRAPHICS;

			// Convert each object's coordinates from graphics.
			for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
				object->Event(event);
			break;
		}
		display = (UI_DISPLAY *)event.data;
		if (!display->isText)
			woStatus |= WOS_GRAPHICS;
		UI_WINDOW_OBJECT *object1 = Last();
		UI_WINDOW_OBJECT *object2;
		first = current = last = NULL;
		currentScreenID = screenID = ID_SCREEN;
		while (object1)
		{
			object2 = object1;
			object1 = object2->Previous();
			UI_WINDOW_MANAGER::Add(object2);
		}
		if (tCurrent)							// BUG.7212
			screenID = tCurrent->screenID;
		eventManager->Put(UI_EVENT(S_REDISPLAY));
		}
		break;

	case S_CLOSE_TEMPORARY:
		if (FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY))
		{
			UI_WINDOW_MANAGER::Subtract(firstObject);
			if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
				delete firstObject;
		}
		else if (event.type != S_CLOSE_TEMPORARY)
			ccode = firstObject->Event(event);
		break;

	case S_CLOSE:
	case L_MOVE:
	case L_SIZE:
	case L_MINIMIZE:
	case L_MAXIMIZE:
	case L_RESTORE:
		while (firstObject && firstObject->parent &&
			FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY))
		{
			UI_WINDOW_MANAGER::Subtract(firstObject);
			if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
				delete firstObject;
			firstObject = First();
		}
		if (!firstObject)
			return (S_NO_OBJECT);
		else if (ccode != S_CLOSE)
			return (firstObject->Event(event));
		else if (screenID != firstObject->screenID)
		{
			// Make sure we can remove the window from the display.
			if (!FlagSet(firstObject->woAdvancedFlags, WOAF_LOCKED))
			{
				UI_WINDOW_MANAGER::Subtract(firstObject);
				if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
					delete firstObject;
				if (!First())
					eventManager->Put(UI_EVENT(S_NO_OBJECT));
			}
			break;
		}
		// Continue to L_EXIT_FUNCTION.
	case L_EXIT_FUNCTION:
		if (FlagSet(firstObject->woAdvancedFlags, WOAF_MODAL))
			break;
		else if (exitFunction && (*exitFunction)(display, eventManager, this) != L_EXIT)
			break;
		// Continue to L_EXIT.
	case L_EXIT:
		ccode = L_EXIT;
		break;

	case L_NEXT_WINDOW:
		while (firstObject && FlagsSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY))
			firstObject = firstObject->Next();
		if (firstObject && !FlagSet(firstObject->woAdvancedFlags, WOAF_MODAL) &&
			firstObject != Last())
			UI_WINDOW_MANAGER::Add(Last());
		break;

	case L_BEGIN_SELECT:
	case L_VIEW:
		while (firstObject)
		{
			UI_WINDOW_OBJECT *nextObject = firstObject->Next();
			if (!FlagSet(firstObject->woFlags, WOF_NON_SELECTABLE) &&
				firstObject->true.Overlap(event.position))
			{
				// Bring the new object to the front.
				if (firstObject != first && ccode == L_BEGIN_SELECT)
				{
					// Check for an overlapping parent.
					if (!FlagSet(firstObject->woStatus, WOS_MINIMIZED) &&
						FlagSet(First()->woAdvancedFlags, WOAF_TEMPORARY) &&
						First()->Next() == firstObject)
					{
						for (UI_WINDOW_OBJECT *object = First()->parent; object;
							object = object->parent)
							if (object == firstObject || !object->true.Overlap(event.position))
								break;
						// Temporary object is direct child of firstObject.
						if (object == firstObject)
							return (firstObject->Event(event));
					}
					UI_WINDOW_MANAGER::Add(firstObject);
				}
				EVENT_TYPE tCode = firstObject->Event(event);
				if (tCode != S_UNKNOWN)
					return (tCode);
			}
			else if (FlagSet(firstObject->woAdvancedFlags, WOAF_MODAL))
			{
				if (ccode == L_BEGIN_SELECT)
					UI_ERROR_SYSTEM::Beep();
				break;
			}
			firstObject = nextObject;
		}
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		break;

	case L_HELP:
		// MapEvent returns E_KEY if help context is not general.
		if (UI_EVENT_MAP::MapEvent(eventMapTable, event) != L_HELP)
		{
			if (helpSystem)
				helpSystem->DisplayHelp(windowManager, NO_HELP_CONTEXT);
			break;
		}
		else
			ccode = firstObject->Event(UI_EVENT(ccode, 0));
		break;

	case S_ADD_OBJECT:							// BUG.1560
		object = (UI_WINDOW_OBJECT *)event.data;
		UI_WINDOW_MANAGER::Add(object);
		break;

	case S_SUBTRACT_OBJECT:							// BUG.1560
		object = (UI_WINDOW_OBJECT *)event.data;
		UI_WINDOW_MANAGER::Subtract(object);
		break;

	default:
		ccode = firstObject->Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

UI_WINDOW_OBJECT *UI_WINDOW_MANAGER::Subtract(UI_WINDOW_OBJECT *object)
{
	// Make sure there is an object to subtract.
	if (!object)
		return (NULL);
	object->woStatus &= ~WOS_CURRENT;

	// Update the windows according to their new list positions.
	UI_EVENT event;
	UI_WINDOW_OBJECT *nextObject = object->Next();
	UI_WINDOW_OBJECT *newCurrent = First();
	if (newCurrent == object)
		newCurrent = newCurrent->Next();
	if (UI_LIST::Index(object) == -1)
		return (object);
	else
	{
		// De-activate the specified object.
		event.type = S_NON_CURRENT;
		event.region.left = event.region.top =
			event.region.right = event.region.bottom = -1;
		object->Event(event);		// Call any user-functions.

		event.type = S_REGION_DEFINE;
		event.region = object->true;

		// Reset the device states.
		eventManager->DeviceState(E_CURSOR, D_OFF);
		eventManager->DeviceState(E_MOUSE, DM_VIEW);

		// Re-define the window regions on the screen.
		display->RegionDefine(ID_SCREEN, 0, 0, display->columns, display->lines);
		for (UI_WINDOW_OBJECT *tObject = Last(); tObject; tObject = tObject->Previous())
			if (tObject != object)
				tObject->Event(event);

		// Refresh the screen and windows affected by the object removal.
		if (display->isText)
			display->VirtualGet(ID_SCREEN, 0, 0, display->columns, display->lines);
		display->Rectangle(ID_SCREEN, event.region, display->backgroundPalette, 0, TRUE);
		for (tObject = Last(); tObject; tObject = tObject->Previous())
		{
			int updateRegion = FALSE;
			if (tObject == object)
				;
			else if (tObject == newCurrent)
			{
				event.type = S_CURRENT;
				tObject->Event(event);
				updateRegion = TRUE;
				break;
			}
			else if (tObject->true.Overlap(object->true))
			{
				if (FlagSet(tObject->woAdvancedFlags, WOAF_TEMPORARY) ||
					(tObject->Previous() && FlagSet(tObject->Previous()->woAdvancedFlags, WOAF_TEMPORARY)))
					event.type = S_CURRENT;
				else
					event.type = S_DISPLAY_INACTIVE;
				tObject->Event(event);
				updateRegion = TRUE;
			}
			if (updateRegion && display->isText) // Fix for shadow border problem.
			{
				event.region.left = Min(event.region.left, tObject->true.left);
				event.region.top = Min(event.region.top, tObject->true.top);
				event.region.right = Max(event.region.right, tObject->true.right);
				event.region.bottom = Max(event.region.bottom, tObject->true.bottom);
				updateRegion = FALSE;
			}
		}
		if (display->isText)
			display->VirtualPut(ID_SCREEN);

		// Remove the window from the manager's list.
		UI_LIST::Subtract(object);
		event.type = S_DEINITIALIZE;
		object->Event(event);
	}

	// Return a pointer to the next object.
	return (nextObject);
}
