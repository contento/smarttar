//	Zinc Application Framework 3.5 Demonstration - ZINCDEMO.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if defined(_MSC_VER)
#	include <new.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <ui_win.hpp>
#if defined(ZIL_PENDOS)
#	include <ui_pen.hpp>
#endif
#define USE_HELP_CONTEXTS
#include "zincpost.hpp"

UIW_WINDOW *_sampleWindow = NULL;
UIW_WINDOW *demoWindow = NULL;
UIW_TEXT *_information = NULL;
UI_STORAGE *_storage = NULL;
char *message = "Zinc 3.5 Demonstration.";

void HelpInformation(const char *helpName)
{
	if (!_storage)
		return;

	_storage->ChDir("~UI_HELP");
	UI_STORAGE_OBJECT hFile(*_storage, helpName, ID_HELP_CONTEXT, UIS_READ);
	if (!hFile.objectError)
	{
		char *message;					// Read the help context title.
		hFile.Load(&message);
		delete message;
		hFile.Load(&message);			// Read the help context.
 		_information->DataSet(message);
		delete message;
	}
}

EVENT_TYPE About(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != L_SELECT)
		return (ccode);

	// Compute the window region.
	UI_DISPLAY *display = object->display;
	int width = 42;
	int height = display->isText ? 7 : 6;
	int left = (display->columns / display->cellWidth - width) / 2;
	int top = (display->lines / display->cellHeight - height) / 2;

	UIW_WINDOW *window = new UIW_WINDOW(left, top, width, height,
		WOF_NO_FLAGS, WOAF_MODAL | WOAF_DIALOG_OBJECT | WOAF_NO_SIZE);
	*window
		+ new UIW_BORDER
		+ &(*new UIW_SYSTEM_BUTTON
			+ new UIW_POP_UP_ITEM("&Move", MNIF_MOVE)
			+ new UIW_POP_UP_ITEM("&Close\tAlt+F4", MNIF_CLOSE))
		+ new UIW_TITLE("About...");
	if (display->isText)
		*window
			+ new UIW_STRING(2, 1, 32, message, 100, STF_NO_FLAGS, WOF_NON_SELECTABLE | WOF_JUSTIFY_CENTER);
	else
		*window
			+ new UIW_ICON(3, 1, "minIcon")
			+ new UIW_STRING(8, 1, 32, message, 100, STF_NO_FLAGS, WOF_NON_SELECTABLE | WOF_JUSTIFY_CENTER);
	*window
		+ new UIW_BUTTON(15, height - 3, 10, "~OK",
			BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE,
			WOF_JUSTIFY_CENTER, NULL, S_CLOSE);
	object->windowManager->Add(window);
	return (ccode);
}

EVENT_TYPE Exit(UI_DISPLAY *display, UI_EVENT_MANAGER *, UI_WINDOW_MANAGER *windowManager)
{
	// Compute the window region.
	int width = 44;
	int height = display->isText ? 7 : 6;
	int left = (display->columns / display->cellWidth - width) / 2;
	int top = (display->lines / display->cellHeight - height) / 2;

	// Create the exit window.
	char *title = "Exit Demonstration";
	char *message = "This will close the demonstration.";
	UIW_WINDOW *window = new UIW_WINDOW(left, top, width, height,
		WOF_NO_FLAGS, WOAF_MODAL | WOAF_DIALOG_OBJECT | WOAF_NO_SIZE);
	*window
		+ new UIW_BORDER
		+ &(*new UIW_SYSTEM_BUTTON
			+ new UIW_POP_UP_ITEM("&Move", MNIF_MOVE)
			+ new UIW_POP_UP_ITEM("&Close\tAlt+F4", MNIF_CLOSE))
		+ new UIW_TITLE(title);
	if (display->isText)
		*window
			+ new UIW_STRING(4, 1, 36, message, 100, STF_NO_FLAGS, WOF_NON_SELECTABLE | WOF_JUSTIFY_CENTER);
	else
		*window
			+ new UIW_ICON(2, 1, "ASTERISK")
			+ new UIW_STRING(8, 1, 32, message, 100, STF_NO_FLAGS, WOF_NON_SELECTABLE | WOF_JUSTIFY_CENTER);
	*window
		+ new UIW_BUTTON(10, height - 3, 10, "~OK",
			BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE,
			WOF_JUSTIFY_CENTER, NULL, L_EXIT)
		+ new UIW_BUTTON(22, height - 3, 10, "~Cancel",
			BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE,
			WOF_JUSTIFY_CENTER, NULL, S_CLOSE);
	*windowManager + window;
	return (S_CONTINUE);
}

EVENT_TYPE Error(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		demoWindow->helpContext = HELP_ERROR_SYSTEM;
		object->errorSystem->ReportError(object->windowManager, WOS_UNANSWERED,
			"This window is used to present error information.");
	}
	return (ccode);
}

EVENT_TYPE Help(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		EVENT_TYPE value;
		object->Information(GET_VALUE, &value);
		object->helpSystem->DisplayHelp(object->windowManager,
			(UI_HELP_CONTEXT)value);
	}
	return (ccode);
}

EVENT_TYPE ButtonWindow(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		_sampleWindow->Destroy();
		if (_sampleWindow->display->isText)
		{
			*_sampleWindow
	       		+ new UIW_BUTTON(10, 2, 9,  "OK", BTF_NO_TOGGLE | BTF_AUTO_SIZE)
		   		+ new UIW_BUTTON(10, 4, 9,  "Cancel", BTF_NO_TOGGLE | BTF_AUTO_SIZE)
		   		+ new UIW_BUTTON(10, 6, 9,  "Help", BTF_NO_TOGGLE | BTF_AUTO_SIZE);
		}
		else
		{
			*_sampleWindow
				+ new UIW_BUTTON(7, 1, 5, NULL, BTF_NO_TOGGLE,
					WOF_JUSTIFY_CENTER, NULL, 0, "hardDrive")
				+ new UIW_BUTTON(7, 3, 5, NULL, BTF_NO_TOGGLE,
					WOF_JUSTIFY_CENTER, NULL, 0, "softDrive")
				+ new UIW_BUTTON(7, 5, 5, NULL, BTF_NO_TOGGLE,
					WOF_JUSTIFY_CENTER, NULL, 0, "networkDrive")
	       		+ new UIW_BUTTON(20, 1, 8, "OK", BTF_NO_TOGGLE | BTF_AUTO_SIZE)
		   		+ new UIW_BUTTON(20, 3, 8, "Cancel", BTF_NO_TOGGLE | BTF_AUTO_SIZE)
				+ new UIW_BUTTON(20, 5, 10, "Mixed", BTF_NO_TOGGLE | BTF_AUTO_SIZE,
					WOF_NO_FLAGS, NULL, 0, "file");
		}
		_sampleWindow->Event(UI_EVENT(S_REDISPLAY, 0));
		HelpInformation("INFO_BUTTON");
	}
	return (ccode);
}

EVENT_TYPE CheckWindow(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		_sampleWindow->Destroy();
		*_sampleWindow
			+ &(*new UIW_GROUP(8, 1, 22, 5, "Check Box Group", WNF_SELECT_MULTIPLE)
 	    		+ new UIW_BUTTON(2, 1, 18, "Check Box A", BTF_CHECK_BOX, WOF_NO_FLAGS)
	        	+ new UIW_BUTTON(2, 2, 18, "Check Box B", BTF_CHECK_BOX, WOF_NO_FLAGS)
		    	+ new UIW_BUTTON(2, 3, 18, "Check Box C", BTF_CHECK_BOX, WOF_NO_FLAGS)
		   		+ new UIW_BUTTON(2, 4, 18, "Check Box D", BTF_CHECK_BOX, WOF_NO_FLAGS));
		_sampleWindow->Event(UI_EVENT(S_REDISPLAY, 0));
		HelpInformation("INFO_CHECK_BOX");
	}
	return (ccode);
}

EVENT_TYPE ComboWindow(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		_sampleWindow->Destroy();
		*_sampleWindow
			+ &(* new UIW_COMBO_BOX(1, 1, 17, 6)
            	+ new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL)
		    	+ new UIW_STRING(0, 0, 15, "Combo 1")
		    	+ new UIW_STRING(0, 0, 15, "Combo 2")
		    	+ new UIW_STRING(0, 0, 15, "Combo 3")
		    	+ new UIW_STRING(0, 0, 15, "Combo 4")
            	+ new UIW_STRING(0, 0, 15, "Combo 5")   
            	+ new UIW_STRING(0, 0, 15, "Combo 6")   
            	+ new UIW_STRING(0, 0, 15, "Combo 7")   
            	+ new UIW_STRING(0, 0, 15, "Combo 8")   
            	+ new UIW_STRING(0, 0, 15, "Combo 9")   
            	+ new UIW_STRING(0, 0, 15, "Combo 10"));
		*_sampleWindow
			+ &(* new UIW_COMBO_BOX(20, 1, 17, 6, NULL, WNF_NO_WRAP | WNF_BITMAP_CHILDREN)
            	+ new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL)
				+ new UIW_BUTTON(0, 0, 22, "String",   	BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "string")
				+ new UIW_BUTTON(0, 0, 22, "F String", 	BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "fmtstr")
				+ new UIW_BUTTON(0, 0, 22, "Text",	   	BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "text")
				+ new UIW_BUTTON(0, 0, 22, "Date",	   	BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "date")
				+ new UIW_BUTTON(0, 0, 22, "Time",	   	BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "time")
				+ new UIW_BUTTON(0, 0, 22, "Number",   	BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "number")
				+ new UIW_BUTTON(0, 0, 22, "Button",   	BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "button")
				+ new UIW_BUTTON(0, 0, 22, "Radio",		BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "radio")
				+ new UIW_BUTTON(0, 0, 22, "Chk Box",  	BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "check")
				+ new UIW_BUTTON(0, 0, 22, "Hz List",  	BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "hlist")
				+ new UIW_BUTTON(0, 0, 22, "Vt List",  	BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "vlist")
				+ new UIW_BUTTON(0, 0, 22, "Combo",		BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "combo")
				+ new UIW_BUTTON(0, 0, 22, "Hz Scroll",	BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "hscroll")
				+ new UIW_BUTTON(0, 0, 22, "Vt Scroll",	BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "vscroll")
				+ new UIW_BUTTON(0, 0, 22, "Window",   	BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "window")
				+ new UIW_BUTTON(0, 0, 22, "Tool Bar", 	BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "toolbar")
				+ new UIW_BUTTON(0, 0, 22, "Prompt",   	BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "prompt")
				+ new UIW_BUTTON(0, 0, 22, "Group",  	BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "group")
				+ new UIW_BUTTON(0, 0, 22, "Icon",	   	BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "icon"));
		_sampleWindow->Event(UI_EVENT(S_REDISPLAY, 0));
		HelpInformation("INFO_COMBO_BOX");
	}
	return (ccode);
}

EVENT_TYPE DateWindow(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		_sampleWindow->Destroy();
		UI_DATE date;
		*_sampleWindow
           	+ new UIW_PROMPT(1, 1, "Default:")
			+ new UIW_DATE(12, 1, 25, &date, NULL)
           	+ new UIW_PROMPT(1, 2, "Month:")
			+ new UIW_DATE(12, 2, 25, &date, NULL, DTF_ALPHA_MONTH)
           	+ new UIW_PROMPT(1, 3, "Weekday:")
			+ new UIW_DATE(12, 3, 25, &date, NULL, DTF_SHORT_MONTH | DTF_SHORT_DAY)
           	+ new UIW_PROMPT(1, 4, "Slash:")
			+ new UIW_DATE(12, 4, 25, &date, NULL, DTF_SLASH)
           	+ new UIW_PROMPT(1, 5, "Military:")
			+ new UIW_DATE(12, 5, 25, &date, NULL, DTF_MILITARY_FORMAT);
		_sampleWindow->Event(UI_EVENT(S_REDISPLAY, 0));
		HelpInformation("INFO_DATE");
	}
	return (ccode);
}

class EXPORT FONT_STRING : public UIW_STRING
{
public:
	FONT_STRING(int left, int top, int width, char *text, int maxLength,
		LOGICAL_FONT _font, STF_FLAGS stFlags = STF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR) :
		UIW_STRING(left, top, width, text, maxLength, stFlags, woFlags) { font = _font; }
	virtual ~FONT_STRING(void) { };
};


EVENT_TYPE FontSupport(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		_sampleWindow->Destroy();
		*_sampleWindow
           	+ new UIW_PROMPT(1, 1, "System:")
           	+ new FONT_STRING(10, 1, 27, "ABCDEFGHIJKabcdefghijk", -1, FNT_SYSTEM_FONT,
				STF_NO_FLAGS, WOF_NON_SELECTABLE)
           	+ new UIW_PROMPT(1, 3, "Dialog:")
           	+ new FONT_STRING(10, 3, 27, "ABCDEFGHIJKabcdefghijk", -1, FNT_DIALOG_FONT,
				STF_NO_FLAGS, WOF_NON_SELECTABLE)
           	+ new UIW_PROMPT(1, 5, "Small:")
           	+ new FONT_STRING(10, 5, 27, "ABCDEFGHIJKabcdefghijk", -1, FNT_SMALL_FONT,
				STF_NO_FLAGS, WOF_NON_SELECTABLE);
		_sampleWindow->Event(UI_EVENT(S_REDISPLAY, 0));

		HelpInformation("INFO_FONT_SUPPORT");
	}
	return (ccode);
}

EVENT_TYPE FStringWindow(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		_sampleWindow->Destroy();
		*_sampleWindow
           	+ new UIW_PROMPT(1, 2, "Phone #:")
           	+ new UIW_FORMATTED_STRING(10, 2, 27, "8017858900", "LNNNLLNNNLNNNN", "(...) ...-....")
           	+ new UIW_PROMPT(1, 3, "Part #:")
           	+ new UIW_FORMATTED_STRING(10, 3, 27, "ABC1234", "LAAALNNNNL", "[...*0000]")
           	+ new UIW_PROMPT(1, 4, "Serial #:")
           	+ new UIW_FORMATTED_STRING(10, 4, 27, "634922", "LLLLLNNNNLNN", "#ZNC-....-..");

		_sampleWindow->Event(UI_EVENT(S_REDISPLAY, 0));

		EVENT_TYPE value;
		object->Information(GET_VALUE, &value);
		if (value == HELP_FORMATTED_STRING)
			HelpInformation("INFO_FORMATTED_STRING");
		else
			HelpInformation("INFO_PROMPT");
	}
	return (ccode);
}

EVENT_TYPE HzListWindow(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		_sampleWindow->Destroy();
		UIW_HZ_LIST *list = new UIW_HZ_LIST(1, 1, 36, 5, 12, 1, NULL, WNF_NO_WRAP | WNF_BITMAP_CHILDREN);
		*list
       		+ new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_HORIZONTAL)
           	+ new UIW_BUTTON(0, 0, 12, "File.1", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file")
           	+ new UIW_BUTTON(0, 0, 12, "File.2", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file")
           	+ new UIW_BUTTON(0, 0, 12, "File.3", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file")
           	+ new UIW_BUTTON(0, 0, 12, "File.4", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file")
           	+ new UIW_BUTTON(0, 0, 12, "File.5", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file")
           	+ new UIW_BUTTON(0, 0, 12, "File.6", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file")
           	+ new UIW_BUTTON(0, 0, 12, "File.7", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file")
           	+ new UIW_BUTTON(0, 0, 12, "File.8", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file")
           	+ new UIW_BUTTON(0, 0, 12, "File.9", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file")
           	+ new UIW_BUTTON(0, 0, 12, "File.10", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file")
           	+ new UIW_BUTTON(0, 0, 12, "File.11", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file")
           	+ new UIW_BUTTON(0, 0, 12, "File.12", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file")
           	+ new UIW_BUTTON(0, 0, 12, "File.13", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file")
           	+ new UIW_BUTTON(0, 0, 12, "File.14", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file")
           	+ new UIW_BUTTON(0, 0, 12, "File.15", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file")
           	+ new UIW_BUTTON(0, 0, 12, "File.16", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file")
           	+ new UIW_BUTTON(0, 0, 12, "File.17", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file")
           	+ new UIW_BUTTON(0, 0, 12, "File.18", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file")
           	+ new UIW_BUTTON(0, 0, 12, "File.19", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file")
           	+ new UIW_BUTTON(0, 0, 12, "File.20", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "file");
		*_sampleWindow + list;
		_sampleWindow->Event(UI_EVENT(S_REDISPLAY, 0));

		EVENT_TYPE value;
		object->Information(GET_VALUE, &value);
		if (value == HELP_HLIST)
			HelpInformation("INFO_HLIST");
		else
			HelpInformation("INFO_SCROLL_BAR");
	}
	return (ccode);
}

EVENT_TYPE IconWindow(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		_sampleWindow->Destroy();
		*_sampleWindow
			+ new UIW_ICON(4, 2, "desk")
			+ new UIW_ICON(16, 2, "analog")
			+ new UIW_ICON(28, 2, "minIcon");
		_sampleWindow->Event(UI_EVENT(S_REDISPLAY, 0));

		HelpInformation("INFO_ICON");
	}
	return (ccode);
}

EVENT_TYPE MenuWindow(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		_sampleWindow->Destroy();
	    UIW_PULL_DOWN_MENU *menu = new UIW_PULL_DOWN_MENU();
		*menu
	        + &(*new UIW_PULL_DOWN_ITEM("File")
		        + new UIW_POP_UP_ITEM("Option 1", MNIF_NO_FLAGS, BTF_NO_3D)
			   	+ new UIW_POP_UP_ITEM("Option 2", MNIF_NO_FLAGS, BTF_NO_3D)
			   	+ new UIW_POP_UP_ITEM("Option 3", MNIF_NO_FLAGS, BTF_NO_3D))
			+ &(*new UIW_PULL_DOWN_ITEM("Cascade")
		 	  	+ &(*new UIW_POP_UP_ITEM("Option 1", MNIF_NO_FLAGS, BTF_NO_3D)
			     	+ new UIW_POP_UP_ITEM("Option 1.1", MNIF_NO_FLAGS, BTF_NO_3D)
			    	+ &(* new UIW_POP_UP_ITEM("Option 1.2", MNIF_NO_FLAGS, BTF_NO_3D)
		 	         	+ new UIW_POP_UP_ITEM("Option 1.2.1", MNIF_NO_FLAGS, BTF_NO_3D)
				 		+ new UIW_POP_UP_ITEM("Option 1.2.2", MNIF_NO_FLAGS, BTF_NO_3D)
            	   	 	+ new UIW_POP_UP_ITEM("Option 1.2.3", MNIF_NO_FLAGS, BTF_NO_3D)))
			   	+ new UIW_POP_UP_ITEM("Option 2", MNIF_NO_FLAGS, BTF_NO_3D)
			   	+ new UIW_POP_UP_ITEM("Option 3", MNIF_NO_FLAGS, BTF_NO_3D));
		*_sampleWindow
			+ &(*new UIW_WINDOW(1, 1, 36, 5, WOF_NO_FLAGS, WOAF_NO_MOVE | WOAF_NO_SIZE)
				+ new UIW_ICON(0, 0, "minIcon", "Menu Window", ICF_MINIMIZE_OBJECT, WOF_NO_FLAGS)
				+ new UIW_BORDER
				+ new UIW_MAXIMIZE_BUTTON
				+ new UIW_MINIMIZE_BUTTON
				+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
				+ new UIW_TITLE("Menu Window")
				+ menu);

		_sampleWindow->Event(UI_EVENT(S_REDISPLAY, 0));

		EVENT_TYPE value;
		object->Information(GET_VALUE, &value);
		if (value == HELP_WINDOW)
			HelpInformation("INFO_WINDOW");
		else if (value == HELP_PULL_DOWN_MENU)
			HelpInformation("INFO_PULL_DOWN_MENU");
		else if (value == HELP_PULL_DOWN_ITEM)
			HelpInformation("INFO_PULL_DOWN_ITEM");
		else if (value == HELP_POP_UP_MENU)
			HelpInformation("INFO_POP_UP_MENU");
		else
			HelpInformation("INFO_POP_UP_MENU_ITEM");
	}
	return (ccode);
}

EVENT_TYPE NumberWindow(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		_sampleWindow->Destroy();
		UI_BIGNUM number(1234L);
		UI_BIGNUM negNumber(-1234L);
		*_sampleWindow
           	+ new UIW_PROMPT(1, 1, "Default:")
			+ new UIW_BIGNUM(11, 1, 26, &number, NULL, NMF_NO_FLAGS)
           	+ new UIW_PROMPT(1, 2, "No decimal:")
			+ new UIW_BIGNUM(11, 2, 26, &number, NULL, NMF_DECIMAL(0))
           	+ new UIW_PROMPT(1, 3, "Currency:")
			+ new UIW_BIGNUM(11, 3, 26, &number, NULL, NMF_CURRENCY | NMF_COMMAS)
           	+ new UIW_PROMPT(1, 4, "Credit:")
			+ new UIW_BIGNUM(11, 4, 26, &negNumber, NULL, NMF_CREDIT | NMF_COMMAS | NMF_DECIMAL(0))
           	+ new UIW_PROMPT(1, 5, "Percent:")
			+ new UIW_BIGNUM(11, 5, 26, &number, NULL, NMF_PERCENT | NMF_DECIMAL(0));
		_sampleWindow->Event(UI_EVENT(S_REDISPLAY, 0));

		HelpInformation("INFO_BIGNUM");
	}
	return (ccode);
}

EVENT_TYPE RadioWindow(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		_sampleWindow->Destroy();
		*_sampleWindow
			+ &(*new UIW_GROUP(8, 1, 22, 5, "Radio Button Group")
				+ new UIW_BUTTON(2, 1, 18, "Radio Button 1", BTF_RADIO_BUTTON, WOF_NO_FLAGS)
		   		+ new UIW_BUTTON(2, 2, 18, "Radio Button 2", BTF_RADIO_BUTTON, WOF_NO_FLAGS)
		    	+ new UIW_BUTTON(2, 3, 18, "Radio Button 3", BTF_RADIO_BUTTON, WOF_NO_FLAGS)
		   		+ new UIW_BUTTON(2, 4, 18, "Radio Button 4", BTF_RADIO_BUTTON, WOF_NO_FLAGS));
		_sampleWindow->Event(UI_EVENT(S_REDISPLAY, 0));

		EVENT_TYPE value;
		object->Information(GET_VALUE, &value);
		if (value == HELP_GROUP)
			HelpInformation("INFO_GROUP");
		else
			HelpInformation("INFO_RADIO_BUTTON");
	}
	return (ccode);
}

EVENT_TYPE StringWindow(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		_sampleWindow->Destroy();
			*_sampleWindow
           	+ new UIW_PROMPT(1, 2, "Name:")
           	+ new UIW_STRING(10, 2, 27, "Zinc Software Incorporated", 100)
           	+ new UIW_PROMPT(1, 3, "Address:")
           	+ new UIW_STRING(10, 3, 27, "405 South 100 East", 100)
           	+ new UIW_STRING(10, 4, 27, "Pleasant Grove, UT", 100);

		_sampleWindow->Event(UI_EVENT(S_REDISPLAY, 0));

		HelpInformation("INFO_STRING");
	}
	return (ccode);
}

EVENT_TYPE TextWindow(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		_sampleWindow->Destroy();
		*_sampleWindow
			+ &(*new UIW_TEXT(1, 1, 36, 5, 
        		"This is text to demonstrate a \r\n"
				"text field with scroll bars. \r\n"
				"Word wrap can also be used as \r\n"
				"shown below.", 256, WNF_NO_WRAP, WOF_BORDER | WOF_AUTO_CLEAR)
				+ new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_CORNER)
				+ new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL)
				+ new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_HORIZONTAL));
		_sampleWindow->Event(UI_EVENT(S_REDISPLAY, 0));

		HelpInformation("INFO_TEXT");
	}
	return (ccode);
}

EVENT_TYPE TimeWindow(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		_sampleWindow->Destroy();
		UI_TIME time;
		*_sampleWindow
           	+ new UIW_PROMPT(1, 1, "Default:")
			+ new UIW_TIME(12, 1, 25, &time, NULL, TMF_NO_FLAGS)
           	+ new UIW_PROMPT(1, 2, "Hours only:")
			+ new UIW_TIME(12, 2, 25, &time, NULL, TMF_NO_MINUTES)
           	+ new UIW_PROMPT(1, 3, "Seconds:")
			+ new UIW_TIME(12, 3, 25, &time, NULL, TMF_SECONDS)
           	+ new UIW_PROMPT(1, 4, "Hundredths:")
			+ new UIW_TIME(12, 4, 25, &time, NULL, TMF_SECONDS | TMF_HUNDREDTHS)
           	+ new UIW_PROMPT(1, 5, "24 hour:")
			+ new UIW_TIME(12, 5, 25, &time, NULL, TMF_TWENTY_FOUR_HOUR);
		_sampleWindow->Event(UI_EVENT(S_REDISPLAY, 0));

		HelpInformation("INFO_TIME");
	}
	return (ccode);
}

EVENT_TYPE ToolWindow(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		_sampleWindow->Destroy();
		UIW_WINDOW *toolWindow =
			&(*new UIW_WINDOW(1, 1, 36, 5, WOF_NO_FLAGS, WOAF_NO_MOVE | WOAF_NO_SIZE)
				+ new UIW_ICON(0, 0, "minIcon", "Tool Bar Window", ICF_MINIMIZE_OBJECT, WOF_NO_FLAGS)
				+ new UIW_BORDER
				+ new UIW_MAXIMIZE_BUTTON
				+ new UIW_MINIMIZE_BUTTON
				+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
				+ new UIW_TITLE("Tool Bar Window"));
		if (_sampleWindow->display->isText)
		{
			*toolWindow
				+ &(*new UIW_TOOL_BAR(0, 0, 0, 2, WNF_NO_WRAP)
					+ new UIW_STRING(1, 0, 10, "Default", 100)
					+ new UIW_BUTTON(13, 1, 6, "Hard", BTF_AUTO_SIZE,
						WOF_JUSTIFY_CENTER, NULL, 0)
					+ new UIW_BUTTON(20, 1, 6, "Soft", BTF_AUTO_SIZE,
						WOF_JUSTIFY_CENTER, NULL, 0)
					+ new UIW_BUTTON(27, 1, 6, "Net", BTF_AUTO_SIZE,
						WOF_BORDER, NULL, 0));
		}
		else
		{
			*toolWindow
				+ &(*new UIW_TOOL_BAR(0, 0, 0, 0, WNF_NO_WRAP)
					+ new UIW_STRING(2, 0, 10, "Default", 100)
					+ new UIW_BUTTON(15, 0, 5, NULL, BTF_NO_TOGGLE,
						WOF_JUSTIFY_CENTER, NULL, 0, "hardDrive")
					+ new UIW_BUTTON(22, 0, 5, NULL, BTF_NO_TOGGLE,
						WOF_JUSTIFY_CENTER, NULL, 0, "softDrive")
					+ new UIW_BUTTON(29, 0, 5, NULL, BTF_NO_TOGGLE,
						WOF_JUSTIFY_CENTER, NULL, 0, "networkDrive"));
		}
		*_sampleWindow
			+ toolWindow;
		_sampleWindow->Event(UI_EVENT(S_REDISPLAY, 0));

		HelpInformation("INFO_TOOL_BAR");
	}
	return (ccode);
}

EVENT_TYPE VtListWindow(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		_sampleWindow->Destroy();
		UIW_VT_LIST *list = new UIW_VT_LIST(1, 1, 17, 5, NULL, WNF_NO_WRAP);
		*list
           	+ new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 1", 20, STF_NO_FLAGS, WOF_VIEW_ONLY)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 2", 20, STF_NO_FLAGS, WOF_VIEW_ONLY)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 3", 20, STF_NO_FLAGS, WOF_VIEW_ONLY)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 4", 20, STF_NO_FLAGS, WOF_VIEW_ONLY)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 5", 20, STF_NO_FLAGS, WOF_VIEW_ONLY)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 6", 20, STF_NO_FLAGS, WOF_VIEW_ONLY)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 7", 20, STF_NO_FLAGS, WOF_VIEW_ONLY)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 8", 20, STF_NO_FLAGS, WOF_VIEW_ONLY)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 9", 20, STF_NO_FLAGS, WOF_VIEW_ONLY)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 10", 20, STF_NO_FLAGS, WOF_VIEW_ONLY)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 11", 20, STF_NO_FLAGS, WOF_VIEW_ONLY)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 12", 20, STF_NO_FLAGS, WOF_VIEW_ONLY)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 13", 20, STF_NO_FLAGS, WOF_VIEW_ONLY)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 14", 20, STF_NO_FLAGS, WOF_VIEW_ONLY)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 15", 20, STF_NO_FLAGS, WOF_VIEW_ONLY)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 16", 20, STF_NO_FLAGS, WOF_VIEW_ONLY)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 17", 20, STF_NO_FLAGS, WOF_VIEW_ONLY)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 18", 20, STF_NO_FLAGS, WOF_VIEW_ONLY)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 19", 20, STF_NO_FLAGS, WOF_VIEW_ONLY)
           	+ new UIW_STRING(0, 0, 17, "Vt-Item 20", 20, STF_NO_FLAGS, WOF_VIEW_ONLY);
		*_sampleWindow + list;
		list = new UIW_VT_LIST(20, 1, 17, 5, NULL, WNF_NO_WRAP | WNF_BITMAP_CHILDREN);
		*list
           	+ new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL)
			+ new UIW_BUTTON(0, 0, 17, "c:\\", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "parentDirectory")
			+ new UIW_BUTTON(1, 0, 16, "work", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "parentDirectory")
			+ new UIW_BUTTON(1, 0, 16, "docs", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "parentDirectory")
			+ new UIW_BUTTON(1, 0, 15, "compiler", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "parentDirectory")
			+ new UIW_BUTTON(2, 0, 15, "bin", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "childDirectory")
			+ new UIW_BUTTON(2, 0, 15, "lib", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "childDirectory")
			+ new UIW_BUTTON(2, 0, 15, "include", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "childDirectory")
			+ new UIW_BUTTON(2, 0, 15, "util", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "childDirectory")
			+ new UIW_BUTTON(2, 0, 15, "examples", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "childDirectory")
			+ new UIW_BUTTON(2, 0, 15, "tutor", BTF_NO_3D, WOF_NO_FLAGS, NULL, 0, "childDirectory");
		*_sampleWindow + list;
		_sampleWindow->Event(UI_EVENT(S_REDISPLAY, 0));

		EVENT_TYPE value;
		object->Information(GET_VALUE, &value);
		if (value == HELP_VLIST)
			HelpInformation("INFO_VLIST");
		else
			HelpInformation("INFO_SCROLL_BAR");
	}
	return (ccode);
}

static UI_ITEM toolBarItems[] =
{
	{ HELP_STRING,				VOIDF(StringWindow),	"string",		0 },
	{ HELP_FORMATTED_STRING,	VOIDF(FStringWindow),	"fmtstr",		0 },
	{ HELP_TEXT,				VOIDF(TextWindow),		"text",			0 },
	{ HELP_DATE,				VOIDF(DateWindow),		"date",			0 },
	{ HELP_TIME,				VOIDF(TimeWindow),		"time",			0 },
	{ HELP_BIGNUM,				VOIDF(NumberWindow),	"number",		0 },
	{ HELP_BUTTON,				VOIDF(ButtonWindow),	"button",		0 },
	{ HELP_RADIO_BUTTON,		VOIDF(RadioWindow),		"radio",		0 },
	{ HELP_CHECK_BOX,			VOIDF(CheckWindow),		"check",		0 },
	{ HELP_HLIST,				VOIDF(HzListWindow),	"hlist",		0 },
	{ HELP_VLIST,				VOIDF(VtListWindow),	"vlist",		0 },
	{ HELP_COMBO_BOX,			VOIDF(ComboWindow),		"combo",		0 },
	{ HELP_SCROLL_BAR,			VOIDF(HzListWindow),	"hscroll",		0 },
	{ HELP_SCROLL_BAR,			VOIDF(VtListWindow),	"vscroll",		0 },
	{ HELP_WINDOW,				VOIDF(MenuWindow),		"window",		0 },
	{ HELP_PULL_DOWN_MENU,		VOIDF(MenuWindow),		"pllmenu",		0 },
	{ HELP_POP_UP_MENU,			VOIDF(MenuWindow),		"popmenu",		0 },
	{ HELP_TOOL_BAR,			VOIDF(ToolWindow),		"toolbar",		0 },
	{ HELP_PROMPT,				VOIDF(FStringWindow),	"prompt",		0 },
	{ HELP_GROUP,				VOIDF(RadioWindow),		"group",		0 },
	{ HELP_ICON,				VOIDF(IconWindow),		"icon",			0 },
	{ HELP_FONT_SUPPORT,		VOIDF(FontSupport),		"font",			0 },
	{ 0,						NULL,					NULL,			0 }
};

static UI_ITEM listItems[] =
{
	{ 0,						NULL,					"- INPUT OBJECTS -",			0 },
	{ HELP_STRING,				VOIDF(StringWindow),	"  String",						0 },
	{ HELP_FORMATTED_STRING,	VOIDF(FStringWindow),	"  Formatted String",			0 },
	{ HELP_TEXT,				VOIDF(TextWindow),		"  Text",						0 },
	{ HELP_DATE,				VOIDF(DateWindow),		"  Date",						0 },
	{ HELP_TIME,				VOIDF(TimeWindow),		"  Time",						0 },
	{ HELP_BIGNUM,				VOIDF(NumberWindow),	"  Number",						0 },
	{ 0,						NULL,					"- CONTROL OBJECTS -",			0 },
	{ HELP_BUTTON,				VOIDF(ButtonWindow),	"  Button",						0 },
	{ HELP_RADIO_BUTTON,		VOIDF(RadioWindow),		"  Radio-Button",				0 },
	{ HELP_CHECK_BOX,			VOIDF(CheckWindow),		"  Check Box",					0 },
	{ HELP_HLIST,				VOIDF(HzListWindow),	"  Hz List",					0 },
	{ HELP_VLIST,				VOIDF(VtListWindow),	"  Vt List",					0 },
	{ HELP_COMBO_BOX,			VOIDF(ComboWindow),		"  Combo-Box",					0 },
	{ HELP_SCROLL_BAR,			VOIDF(HzListWindow),	"  Hz Scroll",					0 },
	{ HELP_SCROLL_BAR,			VOIDF(VtListWindow),	"  Vt Scroll",					0 },
	{ HELP_WINDOW,				VOIDF(MenuWindow),		"  Window",						0 },
	{ 0,						NULL,					"- MENU OBJECTS -",		 		0 },
	{ HELP_PULL_DOWN_MENU,		VOIDF(MenuWindow),		"  Pull-Down Menu",				0 },
	{ HELP_POP_UP_MENU,			VOIDF(MenuWindow),		"  Pop-Up Menu",				0 },
	{ HELP_TOOL_BAR,			VOIDF(ToolWindow),		"  Tool Bar",					0 },
	{ 0,						NULL,					"- STATIC OBJECTS -",		 	0 },
	{ HELP_PROMPT,				VOIDF(FStringWindow),	"  Prompt",						0 },
	{ HELP_GROUP,				VOIDF(RadioWindow),		"  Group",						0 },
	{ HELP_ICON,				VOIDF(IconWindow),		"  Icon",						0 },
	{ 0,						NULL,					"- OTHER OBJECTS -", 			0 },
	{ HELP_KEYBOARD,			VOIDF(Help),			"  Keyboard",					0 },
	{ HELP_MOUSE,				VOIDF(Help),			"  Mouse",						0 },
	{ HELP_PEN,					VOIDF(Help),			"  Pen",						0 },
	{ HELP_HELP_SYSTEM,			VOIDF(Help),			"  Help System",				0 },
	{ HELP_ERROR_SYSTEM,		VOIDF(Error),			"  Error System",				0 },
	{ HELP_FONT_SUPPORT,		VOIDF(FontSupport),		"  Font Support",				0 },
	{ 0,						NULL,					NULL,							0 }
};

#if defined(ZIL_MSDOS)

// Free-store exception handler.
char *_errorMemory = NULL;

#	if defined(_MSC_VER)
int NewHandler(size_t size);

extern "C" int __cdecl xNewHandler(size_t size)
{
	return NewHandler(size);
}

int NewHandler(size_t size)
#	else
void NewHandler(void)
#	endif
{
	// Free enouph memory for reporting error.
	if (_errorMemory)
	{
		delete _errorMemory;
		_errorMemory = NULL;

		// Report out of memory error.
		UI_WINDOW_OBJECT::errorSystem->Beep();
		UI_WINDOW_OBJECT::errorSystem->ReportError(UI_WINDOW_OBJECT::windowManager,
			WOS_NO_STATUS, "     Not enough memory!\r\n"
			"       Exitng to DOS.");
	}

	// Emergency clean up and exit.
	UI_EVENT_MANAGER *eventManager = UI_WINDOW_OBJECT::eventManager;
	UI_DISPLAY *display = UI_WINDOW_OBJECT::display;
	delete UI_WINDOW_OBJECT::windowManager;
	delete eventManager;
	delete display;
	exit(1);

#	if defined(_MSC_VER)
	return (0);
#	endif
}

main(int argc, char *argv[])
{
	// Set up a free-store exception handler.
	_errorMemory = new char[0x2000];
#	if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
	extern void (*_new_handler)(void);
	_new_handler = NewHandler;
#	elif defined(__ZTC__)
	extern void (*_new_handler)(void);
	_new_handler = NewHandler;
#	elif defined(_MSC_VER)
	_set_new_handler(xNewHandler);
#	endif

	// Set up the storage and display search path.
	UI_PATH searchPath(argv[0], TRUE);

	UI_DISPLAY *display = NULL;
	if (argc <= 1 || (argv[1][1] != 't' && argv[1][1] != 'T'))
	{
		display = new UI_GRAPHICS_DISPLAY;
//		display = new UI_BGI_DISPLAY;
//		display = new UI_FG_DISPLAY;
//		display = new UI_MSC_DISPLAY;

	}
	if (!display || !display->installed)
	{
		if (display)
			delete display;
		display = new UI_TEXT_DISPLAY;
	}	
#elif defined(ZIL_MSWINDOWS)
int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR, int nCmdShow)
{
	// Set up the storage and display search path.
	UI_PATH searchPath;

	// Create the display.
	UI_DISPLAY *display = new UI_MSWINDOWS_DISPLAY(hInstance, hPrevInstance, nCmdShow);
#elif defined(ZIL_OS2)
main(int argc, char *argv[])
{
	// Set up the storage and display search path.
	UI_PATH searchPath(argv[0], TRUE);

	// Create the display.
	UI_DISPLAY *display = new UI_OS2_DISPLAY;
#elif defined(ZIL_MOTIF)
main(int argc, char *argv[])
{
	// Set up the storage and display search path.
	UI_PATH searchPath;

	UI_DISPLAY *display = new UI_MOTIF_DISPLAY(&argc, argv);
#endif

	UI_EVENT_MANAGER *eventManager = new UI_EVENT_MANAGER(display);
	*eventManager
		+ new UID_KEYBOARD
#if defined(ZIL_PENDOS)
		+ new UID_PENDOS
#else
		+ new UID_MOUSE
#endif
		+ new UID_CURSOR;

	UI_WINDOW_MANAGER *windowManager = new UI_WINDOW_MANAGER(display,
		eventManager, Exit);

	UID_KEYBOARD::breakHandlerSet = L_EXIT_FUNCTION;

	// Initialize the default storage.
	UI_STORAGE::searchPath = &searchPath;
	_storage = new UI_STORAGE("zincpost.dat", UIS_READ);
	UI_WINDOW_OBJECT::defaultStorage = _storage;

	// Attach the error and help systems.
	UI_WINDOW_OBJECT::errorSystem = new UI_ERROR_SYSTEM;
	UI_WINDOW_OBJECT::helpSystem = new UI_HELP_SYSTEM("zincpost.dat",
		windowManager, HELP_GENERAL);

	// Check for data file errors.
	if (UI_WINDOW_OBJECT::defaultStorage->storageError)
	{
		UI_WINDOW_OBJECT::errorSystem->ReportError(windowManager,
			WOS_NO_STATUS, "Error opening the data file <ZINCPOST.DAT>.\n");

	 	delete UI_WINDOW_OBJECT::helpSystem;
 		delete UI_WINDOW_OBJECT::errorSystem;
		delete _storage;
		delete windowManager;
		delete eventManager;
		delete display;

		return (1);
	}

	// Create the main menu and items to add to it.
	UIW_PULL_DOWN_MENU *mainMenu = new UIW_PULL_DOWN_MENU;
	UIW_PULL_DOWN_ITEM *fileMenu = new UIW_PULL_DOWN_ITEM("~File");
	UIW_PULL_DOWN_ITEM *objectMenu = new UIW_PULL_DOWN_ITEM("~Object");
	UIW_PULL_DOWN_ITEM *helpMenu = new UIW_PULL_DOWN_ITEM("~Help");

	// Add items to the File drop down menu.
	*fileMenu
		+ new UIW_POP_UP_ITEM("E~xit", MNIF_NO_FLAGS, BTF_SEND_MESSAGE, WOF_NO_FLAGS, NULL, L_EXIT_FUNCTION)
		+ new UIW_POP_UP_ITEM
		+ new UIW_POP_UP_ITEM("~About...", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, About);

	// Create the cascaded menu for Input window objects.
	UIW_POP_UP_ITEM *inputItem = new UIW_POP_UP_ITEM("~Input");
	*inputItem
		+ new UIW_POP_UP_ITEM("~String", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, StringWindow, HELP_STRING)
		+ new UIW_POP_UP_ITEM("~Formatted String", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, FStringWindow, HELP_FORMATTED_STRING)
		+ new UIW_POP_UP_ITEM("Te~xt", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, TextWindow, HELP_TEXT)
		+ new UIW_POP_UP_ITEM
		+ new UIW_POP_UP_ITEM("~Date", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, DateWindow, HELP_DATE)
		+ new UIW_POP_UP_ITEM("~Time", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, TimeWindow, HELP_TIME)
		+ new UIW_POP_UP_ITEM(NULL)
		+ new UIW_POP_UP_ITEM("~Number", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, NumberWindow, HELP_BIGNUM);

	// Create the cascaded menu for Control window objects.
	UIW_POP_UP_ITEM *controlItem = new UIW_POP_UP_ITEM("~Control");
	*controlItem
		+ new UIW_POP_UP_ITEM("~Button", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, ButtonWindow, HELP_BUTTON)
		+ new UIW_POP_UP_ITEM("~Radio Button", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, RadioWindow, HELP_RADIO_BUTTON)
		+ new UIW_POP_UP_ITEM("~Check Box", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, CheckWindow, HELP_CHECK_BOX)
		+ new UIW_POP_UP_ITEM
		+ new UIW_POP_UP_ITEM("~Hz-List", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, HzListWindow, HELP_HLIST)
		+ new UIW_POP_UP_ITEM("~Vt-List", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, VtListWindow, HELP_VLIST)
		+ new UIW_POP_UP_ITEM("C~ombo Box", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, ComboWindow, HELP_COMBO_BOX)
		+ new UIW_POP_UP_ITEM
		+ new UIW_POP_UP_ITEM("H~z-Scroll Bar", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, HzListWindow, HELP_SCROLL_BAR)
		+ new UIW_POP_UP_ITEM("V~t-Scroll Bar", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, VtListWindow, HELP_SCROLL_BAR)
		+ new UIW_POP_UP_ITEM
		+ new UIW_POP_UP_ITEM("Child ~Window", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, MenuWindow, HELP_WINDOW);

	// Create the cascaded menu for Menu window objects.
	UIW_POP_UP_ITEM *menuItem = new UIW_POP_UP_ITEM("~Menu");
	*menuItem
		+ new UIW_POP_UP_ITEM("~Pull-Down Menu", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, MenuWindow, HELP_PULL_DOWN_MENU)
		+ new UIW_POP_UP_ITEM("Pop-Up ~Menu", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, MenuWindow, HELP_POP_UP_MENU)
		+ new UIW_POP_UP_ITEM("Tool ~Bar", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, ToolWindow, HELP_TOOL_BAR);

	// Create the cascaded menu for Static window objects.
	UIW_POP_UP_ITEM *staticItem = new UIW_POP_UP_ITEM("~Static");
	*staticItem
		+ new UIW_POP_UP_ITEM("~Prompt", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, FStringWindow, HELP_PROMPT)
		+ new UIW_POP_UP_ITEM("~Group", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, RadioWindow, HELP_GROUP)
		+ new UIW_POP_UP_ITEM
		+ new UIW_POP_UP_ITEM("~Icon", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, IconWindow, HELP_ICON);

	// Add the cascaded menu items to the Object drop down menu.
	*objectMenu
		+ inputItem
		+ controlItem
		+ menuItem
		+ staticItem;

	// Add items to the Help drop down menu.
	*helpMenu
		+ new UIW_POP_UP_ITEM("~General", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, Help, HELP_GENERAL)
		+ new UIW_POP_UP_ITEM("~Order info", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, Help, HELP_ORDER_INFORMATION)
		+ new UIW_POP_UP_ITEM
		+ new UIW_POP_UP_ITEM("~Screen display", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, Help, HELP_SCREEN_DISPLAY)
		+ new UIW_POP_UP_ITEM("~Event manager", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, Help, HELP_EVENT_MANAGER)
		+ new UIW_POP_UP_ITEM("~Window manager", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, Help, HELP_WINDOW_MANAGER)
		+ new UIW_POP_UP_ITEM("~Application class", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, Help, HELP_APPLICATION)
		+ new UIW_POP_UP_ITEM
		+ new UIW_POP_UP_ITEM("~Zinc Designer", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, Help, HELP_ZINC_DESIGNER)
		+ new UIW_POP_UP_ITEM("~Persistent objects", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, Help, HELP_PERSISTENT_OBJECTS)
		+ new UIW_POP_UP_ITEM("~Class hierarchy", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, Help, HELP_CLASS_HIERARCHY)
		+ new UIW_POP_UP_ITEM
		+ new UIW_POP_UP_ITEM("~Video training", MNIF_NO_FLAGS, BTF_NO_FLAGS, WOF_NO_FLAGS, Help, HELP_VIDEO);

	// Add the pull-down items (with their menus) to the main menu.
	*mainMenu
		+ fileMenu
		+ objectMenu
		+ helpMenu;

	// Create main window and add objects (border, menu, minimize icon, etc.) to it.
	int width = 74;
	int height = display->isText ? 16 : 18;
	int left = (display->columns / display->cellWidth - width) / 2;
	int top = (display->lines / display->cellHeight - height) / 2;
	if (top < 0)
		top = 0;
	demoWindow = new UIW_WINDOW(left, top, width, height, WOF_NO_FLAGS, WOAF_NO_SIZE);
	*demoWindow
		+ new UIW_ICON(0, 0, "minIcon", "Zinc Demo", ICF_MINIMIZE_OBJECT, WOF_NO_FLAGS)
		+ new UIW_BORDER
		+ new UIW_MAXIMIZE_BUTTON
		+ new UIW_MINIMIZE_BUTTON
		+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
		+ new UIW_TITLE(message)
		+ mainMenu;

	// Create the tool bar and add it to the window.
	if (!display->isText && top > 0)
	{
		UIW_TOOL_BAR *toolBar = new UIW_TOOL_BAR(0, 0, 0, 0, WNF_NO_FLAGS,
			WOF_BORDER | WOF_NON_FIELD_REGION, WOAF_NO_FLAGS);
		for (int i = 0; toolBarItems[i].text; i++)
		{
			UIW_BUTTON *button = new UIW_BUTTON(0, 0, 6, NULL,
				BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE,
				WOF_JUSTIFY_CENTER, (USER_FUNCTION)toolBarItems[i].data,
				toolBarItems[i].value, toolBarItems[i].text);
			button->helpContext = (UI_HELP_CONTEXT)toolBarItems[i].value;
			*toolBar + button;
		}
		*demoWindow + toolBar;
	}

	// Create the object list and add it to the window.
	UIW_VT_LIST *sampleList = new UIW_VT_LIST(2, 1, 30, 11, NULL, WNF_NO_WRAP | WNF_BITMAP_CHILDREN);
	*sampleList + new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL);
	for (int i = 0; listItems[i].text; i++)
	{
		if (listItems[i].value)
		{
			UIW_BUTTON *button = new UIW_BUTTON(0, 0, 28, listItems[i].text,
				BTF_RADIO_BUTTON, WOF_NO_FLAGS,
				(USER_FUNCTION)listItems[i].data, listItems[i].value);
			button->helpContext = (UI_HELP_CONTEXT)listItems[i].value;
			*sampleList + button;
		}
		else
			*sampleList + new UIW_STRING(0, 0, 29, listItems[i].text,
				-1, STF_NO_FLAGS, WOF_NON_SELECTABLE);
	}
	*demoWindow + sampleList;

	// Create the sample window area and add it to the window.
	_sampleWindow = new UIW_WINDOW(34, 0, 38, 7, WOF_NO_FLAGS, WOAF_NO_MOVE | WOAF_NO_SIZE);
	_sampleWindow->wnFlags |= WNF_NO_WRAP;
	*demoWindow + _sampleWindow;

	// Create the information text field and add it to the window.
	_information = new UIW_TEXT(35, 7, 36, 5, NULL, 2048, WNF_NO_FLAGS, WOF_BORDER | WOF_VIEW_ONLY);
	*_information + new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL);
	*demoWindow + _information;

	// Add the demonstration window to the window manager (to be displayed).
	*windowManager + demoWindow;

	// This line assigns the exit function to be called before the demo
	// window is closed.  It MUST be after the window is added to windowManager.
	windowManager->screenID = demoWindow->screenID;

	// Initialize the information text field.
	HelpInformation("INFO_GENERAL");

	// Process user events.
	EVENT_TYPE ccode;
	do
	{
		UI_EVENT event;
		eventManager->Get(event);
		ccode = windowManager->Event(event);
	} while (ccode != L_EXIT && ccode != S_NO_OBJECT);

	// Clean up.
 	delete UI_WINDOW_OBJECT::helpSystem;
 	delete UI_WINDOW_OBJECT::errorSystem;
	delete _storage;
	delete windowManager;
	delete eventManager;
	delete display;
#if defined(ZIL_MSDOS)
	if (_errorMemory)
		delete _errorMemory;
#endif

	return (0);
}


