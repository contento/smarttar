//	Zinc Interface Library Designer - Z_CONFIG.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ui_dsn.hpp"
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(__DVX__)
#	include <io.h>
#	include <fcntl.h>
#	if defined(_MSC_VER)
#		pragma hdrstop					// Microsoft pre-compiled header pragma.
#	endif
#elif defined(ZIL_POSIX)
#	include <fcntl.h>
#	include <unistd.h>
#endif

#if defined(_SUNOS4)
#	define Read(a,b,c)	read(a, (char *)b, c)
#	define Write(a,b,c)	write(a, (char *)b, c)
#else
#	define Read(a,b,c)	read(a, b, c)
#	define Write(a,b,c)	write(a, b, c)
#endif

#define MAGIC_NUMBER	0x05AF
#define MAJOR_VERSION	3
#define MINOR_VERSION	5

static ZINC_SIGNATURE _signature =
	{ "Zinc Data File Version 3.5\032", MAJOR_VERSION, MINOR_VERSION, MAGIC_NUMBER };

CONFIG_DATA *_config = new CONFIG_DATA;

UI_PALETTE_MAP _textPaletteTable[] =
{
	{ ID_WINDOW_OBJECT, PM_ANY,
		{ ' ', attrib(BLACK, WHITE), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_SOLID_FILL, BLACK, WHITE, BW_BLACK, BW_WHITE, GS_BLACK, GS_WHITE } },

	// End of array.
	{ ID_END, 0, { 0, 0, 0, 0, 0, 0 } }
};

UI_ITEM _toolData[] =
{
	{ D_CREATE_OBJECT +	ID_STRING,				"Z_13", 	"String", 		TRUE },
	{ D_CREATE_OBJECT +	ID_FORMATTED_STRING,	"Z_4",		"F String", 	TRUE },
	{ D_CREATE_OBJECT +	ID_TEXT,				"Z_15",    	"Text", 		TRUE },
	{ D_CREATE_OBJECT +	ID_DATE,				"Z_3",    	"Date", 		TRUE },
	{ D_CREATE_OBJECT +	ID_TIME,				"Z_16",    	"Time", 		TRUE },
	{ D_CREATE_OBJECT +	ID_BIGNUM,				"Z_24", 	"Bignum", 		TRUE },
	{ D_CREATE_OBJECT +	ID_REAL,				"Z_12", 	"Real", 		FALSE },
	{ D_CREATE_OBJECT +	ID_INTEGER,				"Z_6", 		"Integer", 		FALSE },
	{ D_CREATE_OBJECT +	ID_BUTTON,				"Z_2", 		"Button", 		TRUE },
	{ D_CREATE_OBJECT +	ID_RADIO_BUTTON,		"radio",   	"Radio", 		TRUE },
	{ D_CREATE_OBJECT +	ID_CHECK_BOX,			"check",   	"Chk Box", 		TRUE },
	{ D_CREATE_OBJECT +	ID_HZ_LIST,				"Z_1011", 	"Hz-List", 		TRUE },
	{ D_CREATE_OBJECT +	ID_VT_LIST,				"Z_1010",  	"Vt-List", 		TRUE },
	{ D_CREATE_OBJECT +	ID_COMBO_BOX,			"Z_1008",  	"Combo", 		TRUE },
	{ D_CREATE_OBJECT +	ID_HSCROLL,				"Z_1005",	"Hz-Scroll", 	TRUE },
	{ D_CREATE_OBJECT +	ID_VSCROLL,				"vscroll",	"Vt-Scroll", 	TRUE },
	{ D_CREATE_OBJECT +	ID_SCROLL_BAR,			"vscroll", 	"Scroll Bar",	FALSE },
	{ D_CREATE_OBJECT +	ID_WINDOW,				"Z_1007", 	"Window", 		TRUE },
	{ D_CREATE_OBJECT + ID_PULL_DOWN_MENU,		"Z_1004",	"Pull Menu", 	TRUE },
	{ D_CREATE_OBJECT +	ID_TOOL_BAR,			"Z_1009",   "Tool Bar", 	TRUE },
	{ D_CREATE_OBJECT +	ID_PROMPT,				"Z_11", 	"Prompt", 		TRUE },
	{ D_CREATE_OBJECT +	ID_GROUP,				"Z_23",   	"Group", 		TRUE },
	{ D_CREATE_OBJECT +	ID_ICON,				"Z_5",    	"Icon", 		TRUE },
	{ D_CREATE_OBJECT +	ID_DERIVE_OBJECT,		"Z_18",  	"Derive", 		TRUE },
	{ 0,										NULL, 	  	NULL,			TRUE }
};

UI_WINDOW_OBJECT *CreateObject(OBJECTID objectID, UIF_FLAGS flags)
{
	switch (objectID)
	{
	case ID_BORDER:				return (new UIW_BORDER);
	case ID_BUTTON:				return (new UIW_BUTTON(0, 0, 14, "Button"));
	case ID_RADIO_BUTTON:		return (new UIW_BUTTON(0, 0, 16, "Radio-button", BTF_RADIO_BUTTON, WOF_NO_FLAGS));
	case ID_CHECK_BOX:			return (new UIW_BUTTON(0, 0, 13, "Check-box", BTF_CHECK_BOX, WOF_NO_FLAGS));
	case ID_DATE:				return (new UIW_DATE(0, 0, 15, NULL));
	case ID_FORMATTED_STRING:	return (new UIW_FORMATTED_STRING(0, 0, 20, "", "", ""));
 	case ID_ICON:				return (new UIW_ICON(0, 0, "blankIcon", "Icon"));
	case ID_INTEGER:			return (new UIW_INTEGER(0, 0, 15, NULL));
	case ID_MAXIMIZE_BUTTON:	return (new UIW_MAXIMIZE_BUTTON);
	case ID_MINIMIZE_BUTTON:	return (new UIW_MINIMIZE_BUTTON);
	case ID_PROMPT:				return (new UIW_PROMPT(0, 0, "Prompt:"));
	case ID_STRING:				return (new UIW_STRING(0, 0, 20, "String", 20));
	case ID_TEXT:				return (new UIW_TEXT(0, 0, 20, 5, "Text", 100));
	case ID_TIME:				return (new UIW_TIME(0, 0, 15, NULL));
	case ID_TITLE:				return (new UIW_TITLE("Title"));
 	case ID_GROUP:				return (new UIW_GROUP(0, 0, 20, 5, "Group"));
	case ID_BIGNUM:				return (new UIW_BIGNUM(0, 0, 15, NULL));
	case ID_VT_LIST:			return (new UIW_VT_LIST(0, 0, 20, 5));
	case ID_HZ_LIST:			return (new UIW_HZ_LIST(0, 0, 20, 5, 10, 1));
	case ID_COMBO_BOX:			return (new UIW_COMBO_BOX(0, 0, 20, 3, NULL, WNF_NO_WRAP, WOF_VIEW_ONLY));
	case ID_SCROLL_BAR:			return (new UIW_SCROLL_BAR(0, 0, 2, 5, flags));
	case ID_CSCROLL:			return (new UIW_SCROLL_BAR(0, 0, 2, 5, SBF_CORNER, WOF_BORDER));
	case ID_HSCROLL:			return (new UIW_SCROLL_BAR(0, 0, 10, 1, SBF_HORIZONTAL, WOF_BORDER));
//	case ID_VSCROLL:			return (new UIW_SCROLL_BAR(0, 0, 2, 5, SBF_VERTICAL, WOF_BORDER));	// BUG.2037
	case ID_VSCROLL:			return (new UIW_SCROLL_BAR(0, 0, 3, 5, SBF_VERTICAL, WOF_BORDER));
	case ID_WINDOW:				return (UIW_WINDOW::Generic(0, 0, 20, 4, "Title"));
	case ID_PULL_DOWN_MENU:		return (&(*new UIW_PULL_DOWN_MENU + new UIW_PULL_DOWN_ITEM("Item")));
	case ID_PULL_DOWN_ITEM:		return (new UIW_PULL_DOWN_ITEM("Item"));
	case ID_POP_UP_MENU:		return (&(*new UIW_POP_UP_MENU(0, 0, WNF_NO_FLAGS) + new UIW_POP_UP_ITEM("Item")));
	case ID_POP_UP_ITEM:		return (new UIW_POP_UP_ITEM("Item"));
	case ID_REAL:				return (new UIW_REAL(0, 0, 15, NULL));
	case ID_SYSTEM_BUTTON:	   	return (new UIW_SYSTEM_BUTTON(SYF_GENERIC));
	case ID_TOOL_BAR:			return (new UIW_TOOL_BAR(0, 0, 20, 1));
	case ID_DERIVE_OBJECT:		return (new UIW_DERIVE_OBJECT(0, 0, 10, 3));
	}
	return (NULL);
}

void ReadConfiguration(void)
{
	char fileName[128];
	int fileHandle = -1;
	const char *pathName = UI_STORAGE::searchPath->FirstPathName();
	while (pathName && fileHandle < 0)
	{
		strcpy(fileName, pathName);
		if (fileName[ui_strlen(fileName) - 1] != ZIL_DIRSEP)
			strcat(fileName, ZIL_DIRSEPSTR);
		strcat(fileName, "design.cfg");
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
	 	fileHandle = open(fileName, O_BINARY | O_RDWR);
#elif defined(ZIL_UNIX)
	 	fileHandle = open(fileName, O_RDWR);
#endif
		pathName = UI_STORAGE::searchPath->NextPathName();
	}

	if (fileHandle >= 0)
		Read(fileHandle, &_config->signature, sizeof(_config->signature));
	if (fileHandle >= 0 && _config->signature.magicNumber == MAGIC_NUMBER)
	{
		Read(fileHandle, &_config->options, sizeof(_config->options));
		Read(fileHandle, &_config->backups, sizeof(_config->backups));
		Read(fileHandle, &_config->numeratorX, sizeof(_config->numeratorX));
		UI_WINDOW_OBJECT::display->miniNumeratorX = _config->numeratorX;
		Read(fileHandle, &_config->denominatorX, sizeof(_config->denominatorX));
		UI_WINDOW_OBJECT::display->miniDenominatorX = _config->denominatorX;
		Read(fileHandle, &_config->numeratorY, sizeof(_config->numeratorY));
		UI_WINDOW_OBJECT::display->miniNumeratorY = _config->numeratorY;
		Read(fileHandle, &_config->denominatorY, sizeof(_config->denominatorY));
		UI_WINDOW_OBJECT::display->miniDenominatorY = _config->denominatorY;
		Read(fileHandle, &_config->toolBar, sizeof(_config->toolBar));
		TOOL_BUTTON_DATA toolData;
		while(Read(fileHandle, &toolData, sizeof(toolData)) > 0)
			(void *)&(_config->toolList + new TOOL_BUTTON_ELEMENT(toolData));
		close(fileHandle);
	}
	else
	{
		_config->signature = _signature;
		_config->options = OPTION_TOOL_BAR | OPTION_STATUS_BAR |
			OPTION_HELP_BAR | OPTION_MENU_BITMAPS;
		_config->backups = 1;
		_config->numeratorX = (USHORT)UI_WINDOW_OBJECT::display->miniNumeratorX;
		_config->denominatorX = (USHORT)UI_WINDOW_OBJECT::display->miniDenominatorX;
		_config->numeratorY = (USHORT)UI_WINDOW_OBJECT::display->miniNumeratorY;
		_config->denominatorY = (USHORT)UI_WINDOW_OBJECT::display->miniDenominatorY;
		_config->toolBar = TOOL_BITMAP;
		_config->toolList.Destroy();
		for (int i = 0; _toolData[i].value; i++)
		{
			if (_toolData[i].flags)
			{
				TOOL_BUTTON_DATA toolData;
				strcpy(toolData.objectName, _toolData[i].text);
				strcpy(toolData.bitmapName, (char *)_toolData[i].data);
				toolData.createID = (OBJECTID)_toolData[i].value;
				toolData.used = TRUE;
				(void *)&(_config->toolList + new TOOL_BUTTON_ELEMENT(toolData));
			}
		}
	}
}

static void WriteConfiguration(void)
{
	char fileName[128];
	const char *pathName = UI_STORAGE::searchPath->FirstPathName();
	strcpy(fileName, pathName);
	if (fileName[ui_strlen(fileName) - 1] != ZIL_DIRSEP)
		strcat(fileName, ZIL_DIRSEPSTR);
	strcat(fileName, "design.cfg");
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(__DVX__)
 	int fileHandle = open(fileName, O_CREAT | O_BINARY | O_RDWR | O_TRUNC, S_IREAD | S_IWRITE);
#elif defined(ZIL_POSIX)
 	int fileHandle = open(fileName, O_CREAT | O_RDWR | O_TRUNC, S_IREAD | S_IWRITE);
#endif
	if (fileHandle >= 0)
	{
		UI_WINDOW_OBJECT::display->miniNumeratorX = _config->numeratorX;
		UI_WINDOW_OBJECT::display->miniDenominatorX = _config->denominatorX;
		UI_WINDOW_OBJECT::display->miniNumeratorY = _config->numeratorY;
		UI_WINDOW_OBJECT::display->miniDenominatorY = _config->denominatorY;
		Write(fileHandle, &_config->signature, sizeof(_config->signature));
		Write(fileHandle, &_config->options, sizeof(_config->options));
		Write(fileHandle, &_config->backups, sizeof(_config->backups));
		Write(fileHandle, &_config->numeratorX, sizeof(_config->numeratorX));
		Write(fileHandle, &_config->denominatorX, sizeof(_config->denominatorX));
		Write(fileHandle, &_config->numeratorY, sizeof(_config->numeratorY));
		Write(fileHandle, &_config->denominatorY, sizeof(_config->denominatorY));
		Write(fileHandle, &_config->toolBar, sizeof(_config->toolBar));
		for (TOOL_BUTTON_ELEMENT *element = (TOOL_BUTTON_ELEMENT *)_config->toolList.First();
			element; element = (TOOL_BUTTON_ELEMENT *)element->Next())
			Write(fileHandle, &element->data, sizeof(TOOL_BUTTON_DATA));
		close(fileHandle);
	}
}

PREFERENCES_WINDOW::PREFERENCES_WINDOW(void) :
	UIW_WINDOW(UI_WINDOW_OBJECT::display->isText ? "CONFIG_WINDOW_TEXT" : "CONFIG_WINDOW",
		defaultStorage)
{
	windowManager->Center(this);

	backupsField = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "BACKUPS_INTEGER");
	numeratorX = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "NUMERATOR_X");
	denominatorX = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "DENOMINATOR_X");
	numeratorY = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "NUMERATOR_Y");
	denominatorY = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "DENOMINATOR_Y");
	backupsField = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "BACKUPS_INTEGER");
	toolBarButton = (UIW_BUTTON *)Information(GET_STRINGID_OBJECT, "TOOLBAR_BUTTON");
	statusBarButton = (UIW_BUTTON *)Information(GET_STRINGID_OBJECT, "STATUSBAR_BUTTON");
	helpBarButton = (UIW_BUTTON *)Information(GET_STRINGID_OBJECT, "HELPBAR_BUTTON");
	bitmapsButton = (UIW_BUTTON *)Information(GET_STRINGID_OBJECT, "BITMAPS_BUTTON");
	bitmapButton = (UIW_BUTTON *)Information(GET_STRINGID_OBJECT, "BITMAP_BUTTON");
	textButton = (UIW_BUTTON *)Information(GET_STRINGID_OBJECT, "TEXT_BUTTON");
	bothButton = (UIW_BUTTON *)Information(GET_STRINGID_OBJECT, "BOTH_BUTTON");
	objectsList = (UIW_HZ_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
	helpBar = (HELP_BAR *)Information(GET_STRINGID_OBJECT, "HELP_BAR");

	int value = _config->backups; backupsField->DataSet(&value);
	value = _config->numeratorX; numeratorX->DataSet(&value);
	value = _config->denominatorX; denominatorX->DataSet(&value);
	value = _config->numeratorY; numeratorY->DataSet(&value);
	value = _config->denominatorY; denominatorY->DataSet(&value);

	if (_config->toolBar == TOOL_BITMAP)
		bitmapButton->woStatus |= WOS_CURRENT | WOS_SELECTED;
	else if (_config->toolBar == TOOL_TEXT)
		textButton->woStatus |= WOS_CURRENT | WOS_SELECTED;
	else
		bothButton->woStatus |= WOS_CURRENT | WOS_SELECTED;
	if (FlagSet(_config->options, OPTION_TOOL_BAR))
		toolBarButton->woStatus |= WOS_SELECTED;
	if (FlagSet(_config->options, OPTION_STATUS_BAR))
		statusBarButton->woStatus |= WOS_SELECTED;
	if (FlagSet(_config->options, OPTION_HELP_BAR))
		helpBarButton->woStatus |= WOS_SELECTED;
	if (FlagSet(_config->options, OPTION_MENU_BITMAPS))
		bitmapsButton->woStatus |= WOS_SELECTED;

	for (UI_ELEMENT *element = _config->toolList.First(); element; element = element->Next())
	{
		UIW_BUTTON *button = new UIW_BUTTON(0, 0, 30, ((TOOL_BUTTON_ELEMENT *)element)->data.objectName,
			BTF_CHECK_BOX | BTF_AUTO_SIZE | BTF_NO_3D,	WOF_NO_FLAGS);
		if (((TOOL_BUTTON_ELEMENT *)element)->data.used)
 			button->woStatus |= WOS_SELECTED;
		(void *)&(*objectsList + button);
	}
}

EVENT_TYPE PREFERENCES_WINDOW::Event(const UI_EVENT &event)
{
	// Switch on the type of event.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case OPTION_OK:
		{
		int redisplay = FALSE;
		_config->backups = backupsField->DataGet();
		_config->numeratorX = numeratorX->DataGet();
		_config->denominatorX = denominatorX->DataGet();
		_config->numeratorY = numeratorY->DataGet();
		_config->denominatorY = denominatorY->DataGet();
		USHORT oldValue = _config->toolBar;
		if (FlagSet(bitmapButton->woStatus, WOS_SELECTED))
			_config->toolBar = TOOL_BITMAP;
		else if (FlagSet(textButton->woStatus, WOS_SELECTED))
			_config->toolBar = TOOL_TEXT;
		else
			_config->toolBar = TOOL_BITMAP_AND_TEXT;
		if (oldValue != _config->toolBar && FlagSet(toolBarButton->woStatus, WOS_SELECTED))
			redisplay = TRUE;
		oldValue = _config->options & ~OPTION_MENU_BITMAPS;
		_config->options = 0;
		if (FlagSet(toolBarButton->woStatus, WOS_SELECTED))
			_config->options |= OPTION_TOOL_BAR;
		if (FlagSet(statusBarButton->woStatus, WOS_SELECTED))
			_config->options |= OPTION_STATUS_BAR;
		if (FlagSet(helpBarButton->woStatus, WOS_SELECTED))
			_config->options |= OPTION_HELP_BAR;
		if (oldValue != _config->options)
			redisplay = TRUE;
		if (FlagSet(bitmapsButton->woStatus, WOS_SELECTED))
			_config->options |= OPTION_MENU_BITMAPS;

		UI_WINDOW_OBJECT *object = objectsList->First();
		TOOL_BUTTON_ELEMENT *toolElement = (TOOL_BUTTON_ELEMENT *)_config->toolList.First();
		while (object && toolElement)
		{
			if (!toolElement->data.used != !FlagSet(object->woStatus, WOS_SELECTED)
				&& FlagSet(toolBarButton->woStatus, WOS_SELECTED))
				redisplay = TRUE;
			toolElement->data.used = FlagSet(object->woStatus, WOS_SELECTED);
			object = object->Next();
			toolElement = (TOOL_BUTTON_ELEMENT *)toolElement->Next();
		}
		WriteConfiguration();
		eventManager->Put(UI_EVENT(S_CLOSE, 0));
		if (redisplay)
		{
			windowManager->screenID = ID_SCREEN;
			(void *)&(*windowManager - (UIW_WINDOW *)windowManager->Information(GET_STRINGID_OBJECT, "CONTROL_WINDOW"));
			eventManager->Put(UI_EVENT(D_CONFIG_CHANGED, 0));
		}
		}
		break;

	case OPTION_CANCEL:
		eventManager->Put(UI_EVENT(S_CLOSE, 0));
		break;

 	case S_INITIALIZE:
	case S_CREATE:
		ccode = UIW_WINDOW::Event(event);
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		helpBar->Update(Current());
		break;
	}
	return ccode;
}
