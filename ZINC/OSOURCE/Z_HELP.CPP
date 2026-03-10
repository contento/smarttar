//	Zinc Interface Library - Z_HELP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UI_HELP_SYSTEM -----------------------------------------------------

UI_HELP_SYSTEM::UI_HELP_SYSTEM(char *pathName,
	UI_WINDOW_MANAGER *windowManager, UI_HELP_CONTEXT helpContext) :
	defaultHelpContext(helpContext)
{
#if defined(ZIL_LINKBUG)
	extern void z_phelp_dummy(void);		// Bug fix for broken linkers.
	z_phelp_dummy();
#endif

	// Open the help storage unit.
	storage = new UI_STORAGE(pathName, UIS_READ);
	if (storage->storageError)
	{
		delete storage;
		storage = NULL;
	}

	// Create the help window.
	helpWindow = new UIW_WINDOW(0, 0, 60, 14, WOF_NO_FLAGS, WOAF_NO_DESTROY);
	if (windowManager)
		windowManager->Center(helpWindow);
	UIW_BORDER *border = new UIW_BORDER;
	UIW_MAXIMIZE_BUTTON *maxButton = new UIW_MAXIMIZE_BUTTON;
	UIW_MINIMIZE_BUTTON *minButton = new UIW_MINIMIZE_BUTTON;
	UIW_SYSTEM_BUTTON *sysButton = new UIW_SYSTEM_BUTTON(SYF_GENERIC);
	titleField = new UIW_TITLE(helpPrompts[0]);
	messageField = new UIW_TEXT(0, 0, 0, 0, helpPrompts[1], -1, WNF_NO_FLAGS,
		WOF_NON_FIELD_REGION | WOF_VIEW_ONLY);
	(void)&(*messageField + new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_VERTICAL));
	extern UI_PALETTE_MAP *_helpPaletteMapTable;
	maxButton->paletteMapTable =
		minButton->paletteMapTable = sysButton->paletteMapTable =
		titleField->paletteMapTable = messageField->paletteMapTable =
		_helpPaletteMapTable;
	(void)&(*helpWindow
		+ new UIW_ICON(0, 0, "QUESTION", helpPrompts[2], ICF_MINIMIZE_OBJECT)
		+ border + maxButton + minButton + sysButton + titleField + messageField);
}

UI_HELP_SYSTEM::~UI_HELP_SYSTEM(void)
{
	if (helpWindow->windowManager)
		(void)&(*helpWindow->windowManager - helpWindow);
	if (storage)
		delete storage;
	delete helpWindow;
}

void UI_HELP_SYSTEM::DisplayHelp(UI_WINDOW_MANAGER *windowManager,
	UI_HELP_CONTEXT helpContext)
{
	// Check the help system parameters.
	if (!windowManager || !storage)
		return;
	else if (helpContext == NO_HELP_CONTEXT)
		helpContext = defaultHelpContext;

	// Load in the help context.
	storage->ChDir("~UI_HELP");
	char *helpName = storage->FindFirstID(helpContext);
	if (helpName && strcmp(helpName, ".") && strcmp(helpName, ".."))
	{
		UI_STORAGE_OBJECT hFile(*storage, helpName, helpContext, UIS_READ);
		char *text;
		hFile.Load(&text); titleField->DataSet(text); delete text;
		hFile.Load(&text); messageField->DataSet(text, ui_strlen(text) + 1); delete text;
	}
	else
	{
		titleField->DataSet(helpPrompts[0]);
		messageField->DataSet(helpPrompts[1]);
	}

	// Display the help information.
	if (FlagSet(windowManager->First()->woAdvancedFlags, WOAF_MODAL))
		helpWindow->woAdvancedFlags |= WOAF_MODAL;
	else
		helpWindow->woAdvancedFlags &= ~WOAF_MODAL;
	(void)&(*windowManager + helpWindow);
	if (FlagSet(helpWindow->woStatus, WOS_MINIMIZED))
		helpWindow->Event(UI_EVENT(L_RESTORE, 0x0001));
}
