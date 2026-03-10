//	Zinc Interface Library Designer - Z_HLPMSG.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define USE_HELP_CONTEXTS
#include <stddef.h>
#include "ui_dsn.hpp"
#include "p_design.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

char *HelpBarMessage(UI_HELP_CONTEXT helpContext)
{
	static struct HELP_PAIR
	{
		UI_HELP_CONTEXT helpContext;
		char *message;
	} helpMessageTable[] =
	{
		{ STRINGID_HELP,				"Enter an appropriate ID for the object" },
		{ HELP_CONTEXT_HELP,			"Enter the name of the help context for the object" },
		{ OBJECT_LIST_HELP,				"Select a child object and its editor will appear" },
		{ FLAGS_LIST_HELP,				"Flags marked with `X' are in effect; select a flag to toggle it" },
		{ OK_BUTTON_HELP,				"Select `OK' to save your changes and close the window" },
		{ CANCEL_BUTTON_HELP,			"Select `Cancel' to close the window without saving changes" },
		{ HELP_BUTTON_HELP,				"Select `Help' to see additional help for this window" },
		{ MIN_ICON_HELP,				"Enter or select an icon for the minimized window" },
		{ NUMBER_FIELD_HELP,			"Enter the number to be displayed" },
		{ RANGE_FIELD_HELP,				"Enter a range for valid user input (e.g., 10..100)" },
		{ USER_FUNCTION_HELP,			"Enter the name of the user function for the object" },
		{ TEXT_FIELD_HELP,				"Enter the text to appear in the field" },
		{ VALUE_FIELD_HELP,				"Enter a value identification for the object" },
		{ BITMAP_FIELD_HELP,			"Enter or select a bitmap to be displayed on the button" },
		{ COMPARE_FUNCTION_HELP,		"Enter the name of the compare function for the object" },
		{ DATE_FIELD_HELP,				"Enter the date to be displayed in the field" },
		{ COMPRESSED_TEXT_HELP,			"Enter the text to initially appear in the field" },
		{ EDIT_MASK_HELP,				"Enter the characters that define the acceptable string format" },
		{ DELETE_TEXT_HELP,				"Enter the literal characters to appear when a character is deleted" },
		{ CELL_WIDTH_HELP,				"Enter the maximum cell width for single list item" },
		{ CELL_HEIGHT_HELP,				"Enter the maximum cell height for a single list item" },
		{ MAX_LENGTH_HELP,				"Enter a number for the maximum characters the field will display" },
		{ TIME_FIELD_HELP,				"Enter a time value to be displayed in the time field" },
		{ ICON_IMAGE_HELP,				"Enter or select the image to be displayed on the icon" },
		{ ICON_TITLE_HELP,				"Enter the title to be displayed underneath the icon" },
    	{ FILE_OPTION_HELP,				"The File options control the general operations of Zinc Designer" },
		{ EDIT_OPTION_HELP,				"The Edit options allow you to edit the objects in the file" },
		{ RESOURCE_OPTION_HELP,			"The Resource options control the creation of resource windows" },
		{ OBJECT_OPTION_HELP,			"The Object options allow you to create objects" },
		{ UTIL_OPTION_HELP,				"The Utility options allow you to create images and help contexts" },
		{ HELP_OPTION_HELP,				"The Help options allow access to all help contexts" },
		{ OBJECT_BAR_HELP,				" (C) 1991-93 Zinc Software Incorporated.  All Rights Reserved." },
		{ FILE_NAME_HELP,				"Enter the name of the file or select it below" },
		{ DRIVES_LIST_HELP,				"Select from the drives available on your system" },
		{ DIRECTORY_LIST_HELP,			"Select from the directories of the current drive" },
		{ FILE_LIST_HELP,				"Select from the .dat files of the current directory" },
		{ RSRC_STRINGID_HELP,			"Enter an ID for the resource or select it from the `Resources' field" },
		{ RESOURCE_LIST_HELP,			"Select from the resources of the current file" },
		{ IMAGE_TYPE_HELP,				"Select an image type--bitmap or icon" },
		{ IMAGE_NAME_HELP,				"Enter the name of the image or select it from below" },
		{ IMAGE_LIST_HELP,				"Select from the images of the current file" },
		{ CONTEXT_NAME_HELP,			"Enter the name of the context or select it from below" },
		{ CONTEXT_LIST_HELP,			"Select from the contexts of the current file" },
		{ IMAGE_WIDTH_HELP,				"The width of the image is in pixels" },
		{ IMAGE_HEIGHT_HELP,			"The height of the image is in pixels" },
		{ TITLE_HELP,					"Enter the title to be shown in the window's title bar"},
		{ FILE_OK_HELP,					"Select OK to initiate the file operation"},
		{ RESOURCE_OK_HELP,				"Select OK to initiate the resource operation"},
		{ IMAGE_OK_HELP,				"Select OK to initiate the image operation"},
		{ CONTEXT_OK_HELP,				"Select OK to initiate the context operation"},
		{ FILE_HELP_BUTTON_HELP,		"Select `Help' to see additional help for this window" },
		{ RESOURCE_HELP_BUTTON_HELP,	"Select `Help' to see additional help for this window" },
		{ IMAGE_HELP_BUTTON_HELP,		"Select `Help' to see additional help for this window" },
		{ CONTEXT_HELP_BUTTON_HELP,		"Select `Help' to see additional help for this window" },
		{ BACKUPS_HELP,					"Enter the number of backup copies desired"},
		{ OPTIONS_HELP,					"Options marked with `X' are in effect; select one to toggle it"},
		{ MINICELL_HELP,				"Enter the coordinates for mini-cell ratios"},
		{ OBJ_BAR_BUTTN_HELP, 			"Select the objects to be displayed in the object bar"},
		{ OBJ_BUTTONS_HELP, 			"Select an option for the presentation of object buttons"},
		{ PREF_OK_HELP,					"Select OK to initiate the default preferences information"},
		{ PREF_HELP_BTTN_HELP,			"Select `Help' to see additional help for this window" },
		{ COMBO_HEIGHT_HELP,			"Enter the height of the drop down list in cells" },
		{ ADD_ITEM_HELP,				"Select `Add Item' to add a new item to the menu" },
		{ 0,							NULL }
	};

	// Search for help context.
	int i = 0;
	do
	{
		if (helpMessageTable[i].helpContext == helpContext)
			return helpMessageTable[i].message;
		++i;
	} while (helpMessageTable[i].helpContext);
	return NULL;
}
