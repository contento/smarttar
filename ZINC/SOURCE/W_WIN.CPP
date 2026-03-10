//	Zinc Interface Library - W_WIN.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

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

		UI_EVENT event;
		UI_WINDOW_OBJECT *firstObject = First();

///* START BLOCK COMMENT													// BUG.1445
//**		// Remove any temporary objects if we have a non-temporary window.
//**		if (!FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY))
//**		{
//**			while (firstObject && FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY))
//**			{
//**				UI_WINDOW_MANAGER::Subtract(firstObject);
//**				if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
//**					delete firstObject;
//**				firstObject = First();
//**			}
//**		}
//**		// Remove any temporary objects if the window is already in the list.
//**		else if (UI_LIST::Index(object) != -1)
//**		{
//**			while (firstObject && firstObject != object)
//**			{
//**				UI_WINDOW_MANAGER::Subtract(firstObject);
//**				if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
//**					delete firstObject;
//**				firstObject = First();
//**			}
//**		}
//END BLOCK COMMENT */

	// Remove any temporary objects if we have a non-temporary window.
	if (!FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY))
	{
		while (firstObject && FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY))
		{
			UI_WINDOW_OBJECT *nextObject = firstObject->Next();
			if (!FlagSet(firstObject->woStatus, WOS_WINDOWS_ACTION))
			{
				UI_WINDOW_MANAGER::Subtract(firstObject);
				if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
					delete firstObject;
			}
			firstObject = nextObject;
		}
	}

	// Remove any temporary objects if the window is already in the list.
	else if (UI_LIST::Index(object) != -1)
	{
		while (firstObject && firstObject != object)
		{
			UI_WINDOW_OBJECT *nextObject = firstObject->Next();
			if (!FlagSet(firstObject->woStatus, WOS_WINDOWS_ACTION))
			{
				UI_WINDOW_MANAGER::Subtract(firstObject);
				if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
					delete firstObject;
			}
			firstObject = nextObject;
		}
	}

	// See if the object is already the first on the list.
	if (object == First())
		return (object);

	// Find the maximum update region.
 	UI_REGION updateRegion = { 0x0FFF, 0x0FFF, 0x0000, 0x0000 };
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
		object->Event(event);
	}

	// Bring the object to the front of the object queue.
	if (object != firstObject)
	{
		if (UI_LIST::Index(object) != -1)
			UI_LIST::Subtract(object);
		else
			updateRegion = object->true;
		event.type = S_REGION_DEFINE;
		object->Event(event);
		UI_LIST::Add(firstObject, object);
		event.type = S_NON_CURRENT;
		event.region.left = event.region.top = event.region.right = event.region.bottom = -1;
		if (firstObject && !FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY))
			firstObject->Event(event);
	}
	event.type = S_CURRENT;
	event.region = updateRegion;
	object->Event(event);
		
	// See if the object needs to be re-displayed.
	if (!FlagSet(object->woStatus, WOS_WINDOWS_ACTION))
	{
		object->woStatus |= WOS_WINDOWS_ACTION;
		if (taskID == ID_SCREEN)
		{
//			screenID = taskID = object->screenID;	// Default Windows operation.
			taskID = object->screenID;
			ShowWindow(object->screenID, display->nCmdShow);
		}
		else
		{
			int showState = SW_SHOWNORMAL;
			if (FlagSet(object->woStatus, WOS_MAXIMIZED))
				showState = SW_SHOWMAXIMIZED;
			else if (FlagSet(object->woStatus, WOS_MINIMIZED))
				showState = SW_SHOWMINIMIZED;
			ShowWindow(object->screenID, showState);
		}
		UpdateWindow(object->screenID);
		SetFocus(object->screenID);
		object->woStatus &= ~WOS_WINDOWS_ACTION;
	}
	else
	{
		event.type = S_CURRENT;
		event.region = updateRegion;
		object->Event(event);
	}

	if (FlagSet(object->woAdvancedFlags, WOAF_MODAL))
	{
		for (tObject = object->Next(); tObject; tObject = tObject->Next())
			EnableWindow(tObject->screenID, FALSE);
	}

	// Return a pointer to the object.
	return (object);
}

EVENT_TYPE UI_WINDOW_MANAGER::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *firstObject = First();

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW_MANAGER);
	switch (ccode)
	{
	case S_CLOSE:
	case S_CLOSE_TEMPORARY:
		// Check the validity of removing the window object.
		if (!firstObject)
			return (S_NO_OBJECT);
		else
		{
			EVENT_TYPE returnCode = firstObject->Event(event);
			if (returnCode != S_UNKNOWN)
				return (returnCode);
		}
//		if (screenID == firstObject->screenID)								// BUG.1692
//			ccode = L_EXIT;
//		else if (FlagSet(firstObject->woAdvancedFlags, WOAF_LOCKED))
//			break;
//		else if (!FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY) &&
//			ccode == S_CLOSE_TEMPORARY)
//			break;
		if (FlagSet(firstObject->woAdvancedFlags, WOAF_LOCKED))
			break;
		else if (!FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY) &&
			ccode == S_CLOSE_TEMPORARY)
			break;
		else if (screenID == firstObject->screenID)
//			ccode = L_EXIT;
			return (Event(UI_EVENT(L_EXIT_FUNCTION)));

		// Remove all affected window objects.
		while (firstObject)
		{
			UI_WINDOW_OBJECT *nextObject = (ccode == S_CLOSE &&
				FlagSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY)) ?
				firstObject->Next() : NULL;
			Subtract(firstObject);
			if (!FlagSet(firstObject->woAdvancedFlags, WOAF_NO_DESTROY))
				delete firstObject;
			firstObject = nextObject;
		}
		break;

	case S_ADD_OBJECT:
//		if (FlagSet(woStatus, WOS_WINDOWS_ACTION) &&						//BUG 1273
//			Index((UI_WINDOW_OBJECT *)event.data) != -1)
//		{
			firstObject->woStatus &= ~WOS_CURRENT;
			firstObject = Add((UI_WINDOW_OBJECT *)event.data);
			firstObject->woStatus |= WOS_CURRENT;
//		}
		break;

	case L_EXIT_FUNCTION:
		if (FlagSet(firstObject->woAdvancedFlags, WOAF_MODAL))
			break;
		else if (exitFunction && (*exitFunction)(display, eventManager, this) != L_EXIT)
			break;
		// Continue to L_EXIT.
	case L_EXIT:
		ccode = L_EXIT;
		break;

	case L_PREVIOUS:
	case L_NEXT:
	case L_UP:
	case L_DOWN:
	case L_LEFT:
	case L_RIGHT:
		if (firstObject)
			ccode = firstObject->Event(event);
		break;

	case L_NEXT_WINDOW:
		while (firstObject && FlagsSet(firstObject->woAdvancedFlags, WOAF_TEMPORARY))
			firstObject = firstObject->Next();
		if (firstObject && !FlagSet(firstObject->woAdvancedFlags, WOAF_MODAL) &&
			firstObject != Last())
			UI_WINDOW_MANAGER::Add(Last());
		break;

	case L_HELP:
		// MapEvent returns E_KEY if help context is not general.
		if (UI_EVENT_MAP::MapEvent(eventMapTable, event) != L_HELP)
		{
			if (helpSystem)
				helpSystem->DisplayHelp(windowManager, NO_HELP_CONTEXT);
		}
		else
			ccode = firstObject->Event(UI_EVENT(ccode, 0));
		break;

	default:
		if (event.type == E_MSWINDOWS)
		{
			MSG message = event.message;									// BUG.1247 & 1248
//			HWND mdiClient = NULL;
//			if (!First() || !FlagSet(First()->woAdvancedFlags, WOAF_MDI_OBJECT) ||
//				!TranslateMDISysAccel(*(HWND*)First()->Information(GET_MDIHANDLE, &mdiClient), &message))
//			{
//				if (message.message == WM_KEYDOWN || message.message == WM_KEYUP ||
//					message.message == WM_SYSKEYDOWN || message.message == WM_SYSKEYUP)
//					TranslateMessage(&message);
//				if (message.message == WM_QUIT)
//					ccode = L_EXIT;
//				else
//					ccode = DispatchMessage(&message);
//			}
//			else															// BUG.1525
//			{
//#if defined(WIN32)
//				HWND cWindowHwnd = (HWND)SendMessage(mdiClient, WM_MDIGETACTIVE, 0, 0);
//#else
//				HWND cWindowHwnd = 0x0000FFFFL & SendMessage(mdiClient, WM_MDIGETACTIVE, 0, 0);
//#endif
//				if (First()->screenID != cWindowHwnd)
//				{
//					for (UI_WINDOW_OBJECT *cWindow = ((UIW_WINDOW *)First())->Last();
//						cWindow && cWindow->screenID != cWindowHwnd;
//						cWindow = cWindow->Previous())
//							;
//					((UIW_WINDOW *)First())->current = cWindow;
//				}
//			}

			if (message.hwnd == taskID)										// BUG.task
			{
				switch (message.message)
				{
				case WM_SETFOCUS:
					{
					for (UI_WINDOW_OBJECT *object = windowManager->First();
						object && FlagSet(object->woStatus, WOS_WINDOWS_ACTION);
						object = object->Next())
						;
					if (object)
						SetFocus(object->screenID);
					ccode = 0;
					}
					break;

#if (WINVER > 0x030A)
				case WM_WINDOWPOSCHANGING:
					((WINDOWPOS*)event.message.lParam)->flags |= SWP_NOMOVE;
					ccode = DefWindowProc(message.hwnd, message.message,
						message.wParam, message.lParam);
					break;
#else
				case WM_MOVE:
					MoveWindow(taskID, -32767, -32767, 0, 0, NULL);
					ccode = DefWindowProc(message.hwnd, message.message,
						message.wParam, message.lParam);
					break;
#endif

				case WM_NCDESTROY:
					windowManager->taskID = 0;
					ccode = DefWindowProc(message.hwnd, message.message,
						message.wParam, message.lParam);
					break;

				default:
					ccode = DefWindowProc(message.hwnd, message.message,
						message.wParam, message.lParam);
				}
				return (ccode);
			}
			else
			{
				HWND mdiClient = NULL;
				if (!First() || !FlagSet(First()->woAdvancedFlags, WOAF_MDI_OBJECT) ||
					!TranslateMDISysAccel(*(HWND*)First()->Information(GET_MDIHANDLE, &mdiClient), &message))
				{
					if (message.message == WM_KEYDOWN || message.message == WM_KEYUP ||
						message.message == WM_SYSKEYDOWN || message.message == WM_SYSKEYUP)
						TranslateMessage(&message);
					if (message.message == WM_QUIT)
						ccode = L_EXIT;
					else
						ccode = DispatchMessage(&message);
				}
				else	 													// BUG.1525
				{
#if defined(WIN32)
					HWND cWindowHwnd = (HWND)SendMessage(mdiClient, WM_MDIGETACTIVE, 0, 0);
#else
					HWND cWindowHwnd = 0x0000FFFFL & SendMessage(mdiClient, WM_MDIGETACTIVE, 0, 0);
#endif
					if (First()->screenID != cWindowHwnd)
					{
						for (UI_WINDOW_OBJECT *cWindow = ((UIW_WINDOW *)First())->Last();
							cWindow && cWindow->screenID != cWindowHwnd;
							cWindow = cWindow->Previous())
								;
//						((UIW_WINDOW *)First())->current = cWindow;

						cWindow->woStatus |= WOS_WINDOWS_ACTION;
						cWindow->UI_WINDOW_OBJECT::Event(UI_EVENT(S_ADD_OBJECT));
						cWindow->woStatus &= ~WOS_WINDOWS_ACTION;
					}
				}
			}
		}
		else if (firstObject)
			ccode = firstObject->Event(event);
		else
			ccode = S_NO_OBJECT;
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

	// Update any status minimized window information.
	if (FlagSet(object->woAdvancedFlags, WOAF_MODAL) && object == First())
	{
		for (UI_WINDOW_OBJECT *tObject = object->Next(); tObject; tObject = tObject->Next())
		{
			EnableWindow(tObject->screenID, TRUE);
			if (FlagSet(tObject->woAdvancedFlags, WOAF_MODAL))
				break;
		}
	}

	// Update the windows according to their new list positions.
	UI_EVENT event;
	UI_WINDOW_OBJECT *nextObject = object->Next();
	if (UI_LIST::Index(object) == -1)
		return (object);
	else if (FlagSet(object->woStatus, WOS_WINDOWS_ACTION))
	{
		UI_LIST::Subtract(object);
		if (object->screenID)
		{
			if (object->screenID == screenID)
				eventManager->Put(UI_EVENT(L_EXIT, 0));
			else if (!First())
				eventManager->Put(UI_EVENT(S_NO_OBJECT, 0));
			object->screenID = 0;
			event.type = S_DEINITIALIZE;
			object->Event(event);
		}
	}
	else
	{
		event.type = S_NON_CURRENT;
		event.region.left = event.region.top = event.region.right = event.region.bottom = -1;
		object->Event(event);		// Call any user-functions.
		object->woStatus |= WOS_WINDOWS_ACTION;								// BUG.1445
		if (object->screenID)
			DestroyWindow(object->screenID);	// This re-calls Subtract Internally (see previous else).
		object->woStatus &= ~WOS_WINDOWS_ACTION;
	}

	// Return a pointer to the next object.
	return (nextObject);
}


