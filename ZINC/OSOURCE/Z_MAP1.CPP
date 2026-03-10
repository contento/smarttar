//	Zinc Interface Library - Z_MAP1.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UI_PALETTE_MAP -----------------------------------------------------

UI_PALETTE *UI_PALETTE_MAP::MapPalette(UI_PALETTE_MAP *mapTable,
	LOGICAL_PALETTE logicalPalette, OBJECTID id1, OBJECTID id2, OBJECTID id3,
	OBJECTID id4, OBJECTID id5)
{
	int level = 10;
	UI_PALETTE *palette = &mapTable[0].palette;
	int id[5];
	id[0] = id1; id[1] = id2; id[2] = id3; id[3] = id4; id[4] = id5;

	// Try to find a matching palette.
	for (UI_PALETTE_MAP *map = mapTable; map->objectID != ID_END; map++)
		if (map->objectID == id1 && FlagsSet(logicalPalette, map->logicalPalette))
			return (&map->palette);
		else
		{
			for (int i = 0; i < 6; i++)
				if (map->objectID == id[i] &&
					map->logicalPalette == PM_ANY && level > 2 * i + 1)
				{
					level = 2 * i + 1;
					palette = &map->palette;
					break;
				}
				else if (map->objectID == id[i] &&
					FlagsSet(logicalPalette, map->logicalPalette) && level > 2 * i)
				{
					level = 2 * i;
					palette = &map->palette;
					break;
				}
		}

	// Return the logical match palette.
	return (palette);
}

