//	Zinc Interface Library Designer - Z_FLAG.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "ui_dsn.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

EVENT_TYPE FlagValidate(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != S_CURRENT)
	{
		int value;
		char hexString[8];
		object->Information(COPY_TEXT, hexString);
		sscanf(hexString, "%x", &value);
		sprintf(hexString, "0x%.4X", value);
		object->Information(SET_TEXT, hexString);
	}
	return 0;
}

UIF_FLAGS FLAG_FIELD::DataGet(void)
{
	UIF_FLAGS flags;
	UIW_STRING::DataGet();
	sscanf(text, "%X", &flags);
	return flags;
}

void FLAG_FIELD::DataSet(UIF_FLAGS flags)
{
   	char hexString[8];
	sprintf(hexString, "0x%.4X", flags);
	UIW_STRING::DataSet(hexString);
}

