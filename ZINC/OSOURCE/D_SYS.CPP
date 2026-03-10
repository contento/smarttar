//	Zinc Interface Library - D_SYS.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_SYSTEM_BUTTON --------------------------------------------------

UIW_POP_UP_MENU *_genericSystemMenu = NULL;
UIW_POP_UP_MENU *_genericMDIChildSystemMenu = NULL;
int _genericSystemMenuCount = 0;
int _genericMDIChildSystemMenuCount = 0;

static char *_tNormal = "[-]";

static UCHAR _gNormal[29] =
{
	7, 3,
	BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,
	BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,
	BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK
};

EVENT_TYPE UIW_SYSTEM_BUTTON::Event(const UI_EVENT &event)
{
	static initializedTime = FALSE;
	static UI_TIME lastTime;
	if (!initializedTime)
	{
		lastTime.Import();
		initializedTime = TRUE;
	}

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_SYSTEM_BUTTON);
	switch (ccode)
	{
	case S_INITIALIZE:
		// Initialize the generic system button.
		if (FlagSet(syFlags, SYF_GENERIC))
		{
			if (FlagSet(parent->woAdvancedFlags, WOAF_MDI_OBJECT) && parent->parent)
			{																// BUG.268.2
				++_genericMDIChildSystemMenuCount;
				if (!_genericMDIChildSystemMenu)
				{
					_genericMDIChildSystemMenu = new UIW_POP_UP_MENU(0, 0, WNF_NO_FLAGS, WOF_BORDER,
						WOAF_TEMPORARY | WOAF_NO_DESTROY);
					*_genericMDIChildSystemMenu
						+ new UIW_POP_UP_ITEM(sysPrompts[0], MNIF_RESTORE)
						+ new UIW_POP_UP_ITEM(sysPrompts[1], MNIF_MOVE)
						+ new UIW_POP_UP_ITEM(sysPrompts[2], MNIF_SIZE)
						+ new UIW_POP_UP_ITEM(sysPrompts[3], MNIF_MINIMIZE)
						+ new UIW_POP_UP_ITEM(sysPrompts[4], MNIF_MAXIMIZE)
						+ new UIW_POP_UP_ITEM
						+ new UIW_POP_UP_ITEM(sysPrompts[6], MNIF_CLOSE);
				}
			}
			else
			{
				++_genericSystemMenuCount;
				if (!_genericSystemMenu)
				{
					_genericSystemMenu = new UIW_POP_UP_MENU(0, 0, WNF_NO_FLAGS, WOF_BORDER,
						WOAF_TEMPORARY | WOAF_NO_DESTROY);
					*_genericSystemMenu
						+ new UIW_POP_UP_ITEM(sysPrompts[0], MNIF_RESTORE)
						+ new UIW_POP_UP_ITEM(sysPrompts[1], MNIF_MOVE)
						+ new UIW_POP_UP_ITEM(sysPrompts[2], MNIF_SIZE)
						+ new UIW_POP_UP_ITEM(sysPrompts[3], MNIF_MINIMIZE)
						+ new UIW_POP_UP_ITEM(sysPrompts[4], MNIF_MAXIMIZE)
						+ new UIW_POP_UP_ITEM
						+ new UIW_POP_UP_ITEM(sysPrompts[6], MNIF_CLOSE);
				}
			}
		}
		UIW_BUTTON::Event(event);
		break;

	case S_DEINITIALIZE:													// BUG.268.2
		// Check the status of the DOS generic menu.
		if (FlagSet(syFlags, SYF_GENERIC))
		{
			if (FlagSet(parent->woAdvancedFlags, WOAF_MDI_OBJECT) &&
				parent->parent)
			{
				if (!--_genericMDIChildSystemMenuCount)
				{
	 				delete _genericMDIChildSystemMenu;
 					_genericMDIChildSystemMenu = NULL;
				}
			}
			else if (!--_genericSystemMenuCount)
			{
       			delete _genericSystemMenu;
       			_genericSystemMenu = NULL;
			}
		}
		UIW_BUTTON::Event(event);
		break;

	case S_SIZE:
	case S_CREATE:
		UI_WINDOW_OBJECT::Event(event);
		if (display->isText)
		{
			bitmapArray = NULL;
			bitmapWidth = bitmapHeight = 0;
			text = _tNormal;
			true.right = true.left + 2;
			true.bottom = true.top;
		}
		else
		{
			text = NULL;
			UCHAR *bitmap = _gNormal;
			bitmapWidth = bitmap[0];
			bitmapHeight = bitmap[1];
			bitmapArray = &bitmap[2];
			true.bottom = --true.top + (display->cellHeight - display->preSpace - display->postSpace);
			true.right = --true.left + (display->cellHeight - display->preSpace - display->postSpace);
		}
		break;

	case S_RESET_DISPLAY:
		ccode = UIW_BUTTON::Event(event);
		menu.Event(event);
		break;

	case S_ADD_OBJECT:
	case S_SUBTRACT_OBJECT:
		menu.Event(event);
		break;

	case L_BEGIN_SELECT:
		{
		UI_TIME currentTime;
		long elapsedTime = currentTime - lastTime;							// BUG.1228
//		if (Abs(elapsedTime) < doubleClickRate && !parent->parent)	// BUG.internal
		if (Abs(elapsedTime) < doubleClickRate)
		{
			UI_EVENT tEvent = event;
			while (ccode != L_END_SELECT)
			{
				eventManager->Get(tEvent);
				ccode = LogicalEvent(tEvent, ID_SYSTEM_BUTTON);
				// Send all user messages to the window manager.
				if (tEvent.type > 9999)
					windowManager->Event(tEvent);
			}
			if (parent->parent && FlagSet(parent->woAdvancedFlags, WOAF_MDI_OBJECT))	// BUG.internal
			{
				windowManager->Event(UI_EVENT(S_CLOSE_TEMPORARY));
				eventManager->Put(UI_EVENT(S_CLOSE + S_MDICHILD_EVENT));
			}
			else
				eventManager->Put(UI_EVENT(S_CLOSE));
			break;
		}
		lastTime = currentTime;
		}
		// Continue to L_SELECT.
	case L_SELECT:
		if (FlagSet(syFlags, SYF_GENERIC) || menu.First())
		{
			UIW_POP_UP_MENU *sMenu = FlagSet(syFlags, SYF_GENERIC) ?
				((FlagSet(parent->woAdvancedFlags, WOAF_MDI_OBJECT) && parent->parent) ?
				_genericMDIChildSystemMenu : _genericSystemMenu) : &menu;
			if (windowManager->Index(sMenu) != -1)
				break;
			sMenu->parent = this;
	 		sMenu->RegionConvert(sMenu->relative, TRUE);
//			if (FlagSet(parent->woStatus, WOS_MINIMIZED))						// BUG.1373
//				true = parent->true;
//			sMenu->relative.left = true.left;			// BUG.2276
//			sMenu->relative.top = true.bottom;
			if (FlagSet(parent->woStatus, WOS_MINIMIZED))
			{
				sMenu->relative.left = parent->true.left;
				sMenu->relative.top = parent->true.bottom;
			}
			else
			{
				sMenu->relative.left = true.left;
				sMenu->relative.top = true.bottom;
			}
			if (display->isText)
				sMenu->relative.top++;
			if (sMenu->Current())
			{
				sMenu->Current()->woStatus &= ~WOS_CURRENT;
				sMenu->SetCurrent(NULL);
			}
			sMenu->true = sMenu->relative;
			for (UI_WINDOW_OBJECT *object = sMenu->First(); object; object = object->Next())
			{
				switch (object->NumberID())
				{
				case NUMID_OPT_RESTORE:
					if (FlagSet(parent->woStatus, WOS_MAXIMIZED | WOS_MINIMIZED))
						object->woFlags &= ~WOF_NON_SELECTABLE;
					else
						object->woFlags |= WOF_NON_SELECTABLE;
					break;

				case NUMID_OPT_MOVE:
					if (FlagSet(parent->woAdvancedFlags, WOAF_NO_MOVE) ||
						FlagSet(parent->woStatus, WOS_MAXIMIZED))
						object->woFlags |= WOF_NON_SELECTABLE;
					else
						object->woFlags &= ~WOF_NON_SELECTABLE;
					break;

				case NUMID_OPT_SIZE:
					if (FlagSet(parent->woAdvancedFlags, WOAF_NO_SIZE) ||
						FlagSet(parent->woStatus, WOS_MAXIMIZED | WOS_MINIMIZED))
						object->woFlags |= WOF_NON_SELECTABLE;
					else
						object->woFlags &= ~WOF_NON_SELECTABLE;
					break;

				case NUMID_OPT_MINIMIZE:
				case NUMID_OPT_MAXIMIZE:
					{
					object->woFlags &= ~WOF_NON_SELECTABLE;
					if (FlagSet(parent->woStatus, WOS_MAXIMIZED | WOS_MINIMIZED))
					{
						object->woFlags |= WOF_NON_SELECTABLE;
						break;
					}
					for (UI_WINDOW_OBJECT *pobject = (UI_WINDOW_OBJECT *)(((UIW_WINDOW *)parent)->support.First()); pobject; pobject = pobject->Next())	// BUG.1260
						if ((pobject->SearchID() == ID_MAXIMIZE_BUTTON && object->NumberID() == NUMID_OPT_MAXIMIZE) ||
						    (pobject->SearchID() == ID_MINIMIZE_BUTTON && object->NumberID() == NUMID_OPT_MINIMIZE))
							break;
					if (!pobject)
						object->woFlags |= WOF_NON_SELECTABLE;
					}
					break;

				case NUMID_OPT_CLOSE:
//					if (parent->parent || FlagSet(parent->woAdvancedFlags, WOAF_LOCKED))
					if ((parent->parent && !FlagSet(parent->woAdvancedFlags, WOAF_MDI_OBJECT)) ||
						FlagSet(parent->woAdvancedFlags, WOAF_LOCKED))		// BUG.268.2
						object->woFlags |= WOF_NON_SELECTABLE;
					else
						object->woFlags &= ~WOF_NON_SELECTABLE;
					break;
				}
				
				if (!sMenu->Current() && !FlagSet(object->woFlags, WOF_NON_SELECTABLE))
				{
					object->woStatus |= WOS_CURRENT;
					sMenu->SetCurrent(object);
				}
			}
			*windowManager + sMenu;
		}
		else
			UIW_BUTTON::Event(event);
		break;

	case S_DISPLAY_INACTIVE:
	case S_DISPLAY_ACTIVE:
		if (display->isText)
		{
			lastPalette = LogicalPalette(ccode);
			UIW_BUTTON::DrawItem(event, ccode);
			woStatus &= ~WOS_REDISPLAY;
			break;
		}
		else
		{
			UI_WINDOW_OBJECT::Event(event);	// Get the palette and status.
			int i, j;
			UCHAR *bitmap = bitmapArray;
			COLOR foregroundColor = display->MapColor(lastPalette, TRUE);
			for (i = 0; i < bitmapHeight; i++)
				for (j = 0; j < bitmapWidth; j++, bitmap++)
					if (*bitmap != BACKGROUND)
						*bitmap = foregroundColor;
			UIW_BUTTON::Event(event);
		}
		break;

	default:
		ccode = UIW_BUTTON::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}


