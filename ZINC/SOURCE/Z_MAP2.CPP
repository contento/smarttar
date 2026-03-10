//	Zinc Interface Library - Z_MAP2.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_WININPUT			// OS/2 key mapping.
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UI_EVENT_MAP -------------------------------------------------------

EVENT_TYPE UI_EVENT_MAP::MapEvent(UI_EVENT_MAP *mapTable,
	const UI_EVENT &event, OBJECTID id1, OBJECTID id2, OBJECTID id3,
	OBJECTID id4, OBJECTID id5)
{
#if defined(ZIL_MSDOS)
	// See if the type is already mapped.
	EVENT_TYPE logicalValue = event.type;
	EVENT_TYPE eventType = event.type;
	if (logicalValue > E_DEVICE_LAST)
		return (logicalValue);
	USHORT eventRawCode = event.rawCode;
#elif defined(ZIL_MSWINDOWS)
	// See if the type is already mapped.
	EVENT_TYPE logicalValue = event.type;
	EVENT_TYPE eventType = event.message.message;
	USHORT eventRawCode;
	if (logicalValue != E_MSWINDOWS)
		return (logicalValue);
	else if (eventType >= WM_KEYFIRST && eventType <= WM_KEYLAST)
		eventRawCode = event.key.value;
	else if (eventType >= WM_MOUSEFIRST && eventType <= WM_MOUSELAST)
		eventRawCode = event.message.wParam;
	else
		switch (eventType)
		{
		case WM_ACTIVATE:
		case WM_ACTIVATEAPP:
		case WM_ENABLE:
		case WM_MENUCHAR:
		case WM_SYSCHAR:
			eventRawCode = event.message.wParam;
			break;

		case WM_SYSCOMMAND:
			eventRawCode = event.message.wParam & 0xFFF0;
			break;

		default:
			eventRawCode = 0;
			break;
		}
#elif defined(ZIL_OS2)
	// See if the type is already mapped.
	EVENT_TYPE logicalValue = event.type;
	EVENT_TYPE eventType = event.message.msg;
	USHORT eventRawCode = 0;
	if (logicalValue != E_OS2)
		return (logicalValue);
	// Rawcode values are different depending on down and up clicks.
	else if (eventType == WM_CHAR)
//		eventRawCode = event.key.value;
		eventRawCode = LOWORD(event.message.mp2);
	else if (eventType >= WM_MOUSEFIRST && eventType <= WM_MOUSELAST)
		eventRawCode = event.rawCode;
#elif defined(ZIL_MOTIF)
	// See if the type is already mapped.
	EVENT_TYPE logicalValue = event.type;
	EVENT_TYPE eventType = event.message.type;
	if (logicalValue != E_MOTIF)
		return (logicalValue);
	RAW_CODE eventRawCode = event.rawCode;
#endif
	int level = 4;
	OBJECTID id[4];
	id[0] = id2; id[1] = id3; id[2] = id4; id[3] = id5;

	// Try to find a matching event.
	for (UI_EVENT_MAP *map = mapTable; map->objectID != ID_END; map++)
		if (map->rawCode == eventRawCode && map->eventType == eventType &&
			map->modifiers == event.modifiers)
		{
			if (map->objectID == id1)
				return (map->logicalValue);
			for (int i = 0; i < 4; i++)
				if (map->objectID == id[i] && level > i)
				{
					level = i;
					logicalValue = map->logicalValue;
					break;
				}
		}

	// Return the logical match value.
	return (logicalValue);
}
