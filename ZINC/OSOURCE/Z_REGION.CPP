//	Zinc Interface Library - Z_REGION.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_dsp.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UI_REGION_ELEMENT & UI_REGION_LIST ---------------------------------

UI_REGION_ELEMENT::UI_REGION_ELEMENT(SCREENID _screenID, const UI_REGION &_region) :
	screenID(_screenID)
{
	region.left = _region.left,
		region.top = _region.top,
		region.right = _region.right,
		region.bottom = _region.bottom;
}

UI_REGION_ELEMENT::UI_REGION_ELEMENT(SCREENID _screenID, int _left, int _top, int _right,
	int _bottom) : screenID(_screenID)
{
	region.left = _left,
		region.top = _top,
		region.right = _right,
		region.bottom = _bottom;
}

UI_REGION_ELEMENT::~UI_REGION_ELEMENT(void)
{
}

UI_REGION_LIST::UI_REGION_LIST(void)
{
}

UI_REGION_LIST::~UI_REGION_LIST(void)
{
}

void UI_REGION_LIST::Split(SCREENID screenID, const UI_REGION &region,
	int allocateBelow)
{
	UI_REGION tRegion, sRegion;
	UI_REGION_ELEMENT *dRegion, *t_dRegion;

	// Split any overlapping regions.
	for (dRegion = First(); dRegion; dRegion = t_dRegion)
	{
		// Preset the previous region element.
		t_dRegion = dRegion->Next();

		// Object encompasses the whole region.
		if (dRegion->region.Encompassed(region))
		{
			UI_LIST::Subtract(dRegion);
			delete dRegion;
		}

		// Object overlaps the region.
		else if (dRegion->region.Overlap(region, tRegion))
		{
			screenID = dRegion->screenID;
			tRegion = dRegion->region;
			sRegion = region;

			// Region is split at a maximum shown by the following set
			// of regions:
			//		ÚÄÄÄÄÄÄÄÄÄ¿		1
		 	//		ÃÄÄÂÄÄÄÂÄÄ´		2,3,4
		 	//		ÃÄÄÁÄÄÄÁÄÄ´		5
		 	//		ÀÄÄÄÄÄÄÄÄÄÙ
		 	//

			// Check for a top region (region 1 above).
			if (region.top > tRegion.top)
				UI_LIST::Add(0, new UI_REGION_ELEMENT(screenID, tRegion.left,
					tRegion.top, tRegion.right, region.top - 1));
			else
				sRegion.top = tRegion.top;

			// Check for a bottom region (region 5 above).
			if (region.bottom < tRegion.bottom)
				UI_LIST::Add(0, new UI_REGION_ELEMENT(screenID, tRegion.left,
					region.bottom + 1, tRegion.right, tRegion.bottom));
			else
				sRegion.bottom = tRegion.bottom;

			// Check for a left region (region 2 above).
			if (region.left > tRegion.left)
				UI_LIST::Add(0, new UI_REGION_ELEMENT(screenID, tRegion.left,
					sRegion.top, region.left - 1, sRegion.bottom));

			// Check for a right region (region 4 above).
			if (region.right < tRegion.right)
				UI_LIST::Add(0, new UI_REGION_ELEMENT(screenID, region.right + 1,
					sRegion.top, tRegion.right, sRegion.bottom));

			// Region 3 is the object's region.
			UI_LIST::Subtract(dRegion);
			delete dRegion;
		}
	}

	// Check for new allocation space.
	if (!First() && allocateBelow)
//		UI_LIST::Add(0, new UI_REGION_ELEMENT(screenID, region.left,		// Bug.1427
//			region.bottom + 1, region.right, region.bottom + 2));
		UI_LIST::Add(0, new UI_REGION_ELEMENT(screenID, region.left,
			region.bottom + 10, region.right, region.bottom + 20));
}
