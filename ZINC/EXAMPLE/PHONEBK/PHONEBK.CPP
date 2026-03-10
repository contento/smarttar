//	PHONEBK.CPP (PHONEBK) - Phone book example program.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <string.h>
#include <ui_win.hpp>

#define PHONE_BOOK_NAME "phone.dat"
const OBJECTID ID_PHONE_BOOK = 20000;

UI_STORAGE *_phoneBook;

EVENT_TYPE Find(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != L_SELECT)
		return (ccode);

	UIW_STRING *name = (UIW_STRING *)object->parent->Information(GET_STRINGID_OBJECT, "NAME");
	UIW_TEXT *address = (UIW_TEXT *)object->parent->Information(GET_STRINGID_OBJECT, "ADDRESS");
	UIW_STRING *workPhone = (UIW_STRING *)object->parent->Information(GET_STRINGID_OBJECT, "WORK_PHONE");
	UIW_STRING *homePhone = (UIW_STRING *)object->parent->Information(GET_STRINGID_OBJECT, "HOME_PHONE");

	_phoneBook->ChDir("~PHONE_BOOK");
//	const char *stringID = _phoneBook->FindFirstObject(name->DataGet());
	char nameBuffer[128];													// BUG.2075
	strcpy(nameBuffer, name->DataGet());
	ui_strupr(nameBuffer);
	while (nameBuffer[strlen(nameBuffer) - 1] == ' ')
		nameBuffer[strlen(nameBuffer) - 1] = 0;
	const char *stringID = _phoneBook->FindFirstObject(nameBuffer);
	if (!stringID)
	{
		// The object was not found in the file.
		address->DataSet("");
		workPhone->DataSet("");
		homePhone->DataSet("");

		// The word was not found.
		object->errorSystem->ReportError(object->windowManager, WOS_NO_STATUS,
			"That name was not found in the phone book.");
		object->Event(UI_EVENT(S_REDISPLAY, 0));
	}
	else
	{
		UI_STORAGE_OBJECT *entry = new UI_STORAGE_OBJECT(*_phoneBook, stringID, ID_PHONE_BOOK, UIS_READ);

		// If the word was found then display the fields.
		if (!entry->objectError)
		{
			// Load the phone information.
			USHORT stringLength;
			char *string;

			// Load the address.
			entry->Load(&stringLength);
			if (stringLength)
			{
				string = new char[stringLength + 1];
				entry->Load(string, 1, stringLength);
				string[stringLength] = '\0';
				address->DataSet(string);
				delete string;
			}
			else
				address->DataSet("");

			// Load the work phone number.
			entry->Load(&stringLength);
			if (stringLength)
			{
				string = new char[stringLength + 1];
				entry->Load(string, 1, stringLength);
				string[stringLength] = '\0';
				workPhone->DataSet(string);
				delete string;
			}
			else
				workPhone->DataSet("");

			//Load the home phone number.
			entry->Load(&stringLength);
			if (stringLength)
			{
				string = new char[stringLength + 1];
				entry->Load(string, 1, stringLength);
				string[stringLength] = '\0';
				homePhone->DataSet(string);
				delete string;
			}
			else
				homePhone->DataSet("");
		}
		delete entry;
	}

	return (ccode);
}

EVENT_TYPE Save(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != L_SELECT)
		return (ccode);

	UIW_STRING *name = (UIW_STRING *)object->parent->Information(GET_STRINGID_OBJECT, "NAME");
	if (!strcmp(name->DataGet(), ""))
	{
		object->errorSystem->ReportError(object->windowManager, WOS_NO_STATUS,
			"A blank phone entry cannot be saved.");
		object->Event(UI_EVENT(S_REDISPLAY, 0));
		return (ccode);
	}
	UIW_TEXT *address = (UIW_TEXT *)object->parent->Information(GET_STRINGID_OBJECT, "ADDRESS");
	UIW_STRING *workPhone = (UIW_STRING *)object->parent->Information(GET_STRINGID_OBJECT, "WORK_PHONE");
	UIW_STRING *homePhone = (UIW_STRING *)object->parent->Information(GET_STRINGID_OBJECT, "HOME_PHONE");

	// Try to find the entry.
	_phoneBook->ChDir("~");
	_phoneBook->MkDir("PHONE_BOOK");
	_phoneBook->ChDir("~PHONE_BOOK");
	
//	const char *stringID = _phoneBook->FindFirstObject(name->DataGet());
	char nameBuffer[128];													// BUG.2075
	strcpy(nameBuffer, name->DataGet());
	ui_strupr(nameBuffer);
	while (nameBuffer[strlen(nameBuffer) - 1] == ' ')
		nameBuffer[strlen(nameBuffer) - 1] = 0;
	const char *stringID = _phoneBook->FindFirstObject(nameBuffer);
	UI_STORAGE_OBJECT *entry;
	if (!stringID)
//		entry = new UI_STORAGE_OBJECT(*_phoneBook, name->DataGet(), ID_PHONE_BOOK, UIS_CREATE | UIS_READWRITE);
		entry = new UI_STORAGE_OBJECT(*_phoneBook, nameBuffer, ID_PHONE_BOOK, UIS_CREATE | UIS_READWRITE);
	else
		entry = new UI_STORAGE_OBJECT(*_phoneBook, stringID, ID_PHONE_BOOK, UIS_READWRITE);

	// If the word was found then display the fields.
	if (!entry->objectError)
	{
		// Store the phone information.
		char *string;

		// Store the address.
		string = address->DataGet();
		entry->Store((USHORT)ui_strlen(string));
		entry->Store(string, 1, ui_strlen(string));

		// Store the work phone number.
		string = workPhone->DataGet();
		entry->Store((USHORT)ui_strlen(string));
		entry->Store(string, 1, ui_strlen(string));

		// Store the home phone number.
		string = homePhone->DataGet();
		entry->Store((USHORT)ui_strlen(string));
		entry->Store(string, 1, ui_strlen(string));
	}
	delete entry;

	return (ccode);
}

EVENT_TYPE Delete(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != L_SELECT)
		return (ccode);

	UIW_STRING *name = (UIW_STRING *)object->parent->Information(GET_STRINGID_OBJECT, "NAME");

	_phoneBook->ChDir("~PHONE_BOOK");
//	const char *stringID = _phoneBook->FindFirstObject(name->DataGet());
	char nameBuffer[128];													// BUG.2075
	strcpy(nameBuffer, name->DataGet());
	ui_strupr(nameBuffer);
	while (nameBuffer[strlen(nameBuffer) - 1] == ' ')
		nameBuffer[strlen(nameBuffer) - 1] = 0;
	const char *stringID = _phoneBook->FindFirstObject(nameBuffer);
	if (stringID)
		_phoneBook->DestroyObject(stringID);
	else
	{
		object->errorSystem->ReportError(object->windowManager, WOS_NO_STATUS,
			"That phone entry was not found in the phone book.");
		object->Event(UI_EVENT(S_REDISPLAY, 0));
	}

	return (ccode);
}

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	// Initialize the error system.
	UI_WINDOW_OBJECT::errorSystem = new UI_ERROR_SYSTEM;

	// Open the phone book.
	_phoneBook = new UI_STORAGE(PHONE_BOOK_NAME, UIS_OPENCREATE | UIS_READWRITE);
	if (_phoneBook->storageError)
		UI_WINDOW_OBJECT::errorSystem->ReportError(windowManager, WOS_NO_STATUS,
			"Error opening %s.", PHONE_BOOK_NAME);
	else
	{
		*windowManager + UI_WINDOW_OBJECT::New("phon_win.dat~PHONE_BOOK");

		// Process user responses.
		UI_APPLICATION::Control();
	}

	// Clean up.
	_phoneBook->Save(1);
 	delete _phoneBook;
	delete UI_WINDOW_OBJECT::errorSystem;

	return (0);
}
