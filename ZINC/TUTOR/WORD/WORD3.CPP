//	WORD3.CPP (WORD) - Dictionary tutorial, using the data file.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <string.h>
#include <ui_win.hpp>

#define USE_WINDOW_DICTIONARY
#include "word_win.hpp"
#include "word3.hpp"

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	// Initialize the error system.
	UI_WINDOW_OBJECT::errorSystem = new UI_ERROR_SYSTEM;

	// Create the dictionary window.
	DICTIONARY_WINDOW *dictionary = new DICTIONARY_WINDOW("word.dat");

	if (!FlagSet(dictionary->woStatus, WOS_READ_ERROR))
		*windowManager + dictionary;
	else
	{
		UI_WINDOW_OBJECT::errorSystem->ReportError(windowManager, WOS_NO_STATUS,
			"An error was encountered trying to open word_win.dat");
		return (1);															// BUG.1637
	}

	// Process user responses.
	UI_APPLICATION::Control();

	// Clean up.
	delete UI_WINDOW_OBJECT::errorSystem;

	return (0);
}

DICTIONARY_WINDOW::DICTIONARY_WINDOW(char * dictionaryName) :
	UIW_WINDOW("word_win.dat~WINDOW_DICTIONARY"), dictionary(NULL)
{
	if (FlagSet(woStatus, WOS_READ_ERROR))									// BUG.1637
		return;

	// Set up the pointers to the window fields.
	inputField = (UIW_STRING *)Information(GET_STRINGID_OBJECT, "DCT_INPUT");
	definitionField = (UIW_TEXT *)Information(GET_STRINGID_OBJECT, "DCT_DEFINITION");
	antonymField = (UIW_STRING *)Information(GET_STRINGID_OBJECT, "DCT_ANTONYM");
	synonymField = (UIW_STRING *)Information(GET_STRINGID_OBJECT, "DCT_SYNONYM");

	// Set the user functions to the buttons.
	UIW_BUTTON *button;
	button = (UIW_BUTTON *)Get(DCT_LOOKUP_BUTTON);
	button->userFunction = DICTIONARY_WINDOW::ButtonFunction;

	button = (UIW_BUTTON *)Get(DCT_SAVE_BUTTON);
	button->userFunction = DICTIONARY_WINDOW::ButtonFunction;

	button = (UIW_BUTTON *)Get(DCT_DELETE_BUTTON);
	button->userFunction = DICTIONARY_WINDOW::ButtonFunction;

	// Set up the dictionary.
	dictionary = new DICTIONARY(dictionaryName);
	if (dictionary->storageError)
	{
		dictionary = NULL;
		errorSystem->ReportError(windowManager, WOS_NO_STATUS,
			"The dictionary file could not be opened with read write access.");
	}
	else
	{
		if (dictionary->ChDir("~DICTIONARY"))
		{
			dictionary->ChDir("~");
			dictionary->MkDir("DICTIONARY");
			dictionary->ChDir("~DICTIONARY");
		}
	}
}

EVENT_TYPE DICTIONARY_WINDOW::Event(const UI_EVENT &event)
{
	D_ENTRY *entry;
	EVENT_TYPE ccode = event.type;
	char *inputString = NULL;
	const char *stringID = NULL;

	switch(event.type)
	{
	case L_DCT_DELETE:
		{
		inputString = inputField->DataGet(); ui_strlwr(inputString);
		stringID = dictionary->FindFirstObject(inputString);
		if (stringID)
			dictionary->DestroyObject(stringID);
		else
			errorSystem->ReportError(windowManager, WOS_NO_STATUS,
				"That word was not found in the dictionary and cannot be deleted.");
		}
		break;

	case L_DCT_LOOKUP:
		{
		inputString = inputField->DataGet(); ui_strlwr(inputString);
		stringID = dictionary->FindFirstObject(inputString);
		if (!stringID)
		{
			// The object was not found in the file.
			definitionField->DataSet("");
			antonymField->DataSet("");
			synonymField->DataSet("");

			// The word was not found.
			errorSystem->ReportError(windowManager, WOS_NO_STATUS,
				"That word was not found in the dictionary.");
		}
		else
		{
			entry = D_ENTRY::New(stringID, (UI_STORAGE *)dictionary, UIS_READ);

			// If the word was found then display the fields.
			if (entry->wasLoaded)
			{
				inputField->DataSet(entry->word);
				if (entry->definition)
					definitionField->DataSet(entry->definition);
				else
					definitionField->DataSet("");
				if (entry->antonym)
					antonymField->DataSet(entry->antonym);
				else
					antonymField->DataSet("");
				if (entry->synonym)
					synonymField->DataSet(entry->synonym);
				else
					synonymField->DataSet("");
			}
			delete entry;
		}
		}
		break;

	case L_DCT_SAVE:
		{
		// If the entry exists, modify it.  Otherwise, create it.
		inputString = inputField->DataGet(); ui_strlwr(inputString);
		stringID = dictionary->FindFirstObject(inputString);
		if (!stringID)
			entry = D_ENTRY::New(inputString, (UI_STORAGE *)dictionary, UIS_CREATE | UIS_READWRITE);
		else
			entry = D_ENTRY::New(inputString, (UI_STORAGE *)dictionary, UIS_READWRITE);

		// Don't save an empty field.
		if (strcmp(inputString, ""))
		{
			entry->word = ui_strdup(inputString);
			entry->definition = ui_strdup(definitionField->DataGet());
			entry->antonym = ui_strdup(antonymField->DataGet());
			entry->synonym = ui_strdup(synonymField->DataGet());
			entry->Save();
		}
		else
			errorSystem->ReportError(windowManager, WOS_NO_STATUS,
				"A blank word cannot be saved.");

		delete entry;
		}
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
	};

	return ccode;
}

EVENT_TYPE DICTIONARY_WINDOW::ButtonFunction(UI_WINDOW_OBJECT *object, 
   UI_EVENT &event, EVENT_TYPE ccode)
{
	UI_EVENT tEvent = event;

	if (ccode != L_SELECT)
		return ccode;

	NUMBERID buttonID = object->NumberID();
	switch (buttonID)
	{
	case DCT_DELETE_BUTTON:
		tEvent.type = L_DCT_DELETE;
	   break;

	case DCT_LOOKUP_BUTTON:
		tEvent.type = L_DCT_LOOKUP;
	   break;

	case DCT_SAVE_BUTTON:
		tEvent.type = L_DCT_SAVE;
	   break;

	default:
	   return event.type;
	}

	object->eventManager->Put(tEvent);
	return ccode;
}

D_ENTRY::D_ENTRY(const char *name, UI_STORAGE *file, UIS_FLAGS flags) :
	word(NULL), definition(NULL), antonym(NULL), synonym(NULL)
{
	object = new UI_STORAGE_OBJECT(*file, name, ID_DICTIONARY_ENTRY, flags);

	// Check to see if object was found in the file.
	if (object->objectError)
		wasLoaded = FALSE;
	else
	{
		wasLoaded = TRUE;

		// Load the word.
		USHORT stringLength;
		object->Load(&stringLength);
		if (stringLength)
		{
			word = new char[stringLength+1];
			object->Load(word, 1, stringLength);
			word[stringLength] = '\0';
		}

		// Load the definition.
		object->Load(&stringLength);
		if (stringLength)
		{
			definition = new char[stringLength+1];
			object->Load(definition, 1, stringLength);
			definition[stringLength] = '\0';
		}

		// Load the antonyms.
		object->Load(&stringLength);
		if (stringLength)
		{
			antonym = new char[stringLength+1];
			object->Load(antonym, 1, stringLength);
			antonym[stringLength] = '\0';
		}

		// Load the synonyms.
		object->Load(&stringLength);
		if (stringLength)
		{
			synonym = new char[stringLength+1];
			object->Load(synonym, 1, stringLength);
			synonym[stringLength] = '\0';
		}
	}
}

D_ENTRY::~D_ENTRY(void)
{
	// Free the memory if it was allocated.
	if (word)
		delete word;
	if (definition)
		delete definition;
	if (antonym)
		delete antonym;
	if (synonym)
		delete synonym;
	if (object)
		delete object;
}

D_ENTRY *D_ENTRY::New(const char *name, UI_STORAGE *directory, UIS_FLAGS flags)
{
	// Check for a valid directory and file.
	if (name)
		return (new D_ENTRY(name, directory, flags));
	else
       return NULL;
}

void D_ENTRY::Save(void)
{
	// Store the word.
	object->Store((USHORT)ui_strlen(word));
	object->Store(word, 1, ui_strlen(word));

	// Store the word definition.
	object->Store((USHORT)ui_strlen(definition));
	object->Store(definition, 1, ui_strlen(definition));
  
	// Store the antonyms.
	object->Store((USHORT)ui_strlen(antonym));
	object->Store(antonym, 1, ui_strlen(antonym));

	// Store the synonyms.
	object->Store((USHORT)ui_strlen(synonym));
	object->Store(synonym, 1, ui_strlen(synonym));
}
