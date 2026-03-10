#define USE_FILE_ERROR
#define USE_FILE_EDITOR
#define USE_FILE_STATISTICS
#define USE_HELP_CONTEXTS
#include "direct.hpp"
#include "dir_win.hpp"
#include "fedit.hpp"

const long MAX_FILE_LENGTH				= 30000L;
const EVENT_TYPE FILE_OPEN_ERROR		= 10010;
const EVENT_TYPE FILE_LENGTH_ERROR		= 10011;

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	// Initialize the default storage.
	static char *_resourceFile = "dir_win.dat";
	UI_WINDOW_OBJECT::defaultStorage = new UI_STORAGE(_resourceFile, UIS_READ);
	if (!UI_WINDOW_OBJECT::defaultStorage)
	{
		*windowManager + new FILE_ERROR(FILE_OPEN_ERROR, _resourceFile);
		return (0);	// Exit the program.
	}

	// Initialize the help and error systems.
	UI_WINDOW_OBJECT::errorSystem = new UI_ERROR_SYSTEM;
	UI_WINDOW_OBJECT::helpSystem = new UI_HELP_SYSTEM(_resourceFile, windowManager);

	*windowManager + new FILE_EDITOR;

	// Process user responses.
	UI_APPLICATION::Control();

	// Clean up.
	delete UI_WINDOW_OBJECT::defaultStorage;
	delete UI_WINDOW_OBJECT::helpSystem;
	delete UI_WINDOW_OBJECT::errorSystem;

	return (0);
}

// ----- FILE_EDITOR --------------------------------------------------------

const EVENT_TYPE OPTION_NEW				= 10001;
const EVENT_TYPE OPTION_OPEN			= 10002;
const EVENT_TYPE OPTION_SAVE			= 10003;
const EVENT_TYPE OPTION_SAVE_AS			= 10004;
const EVENT_TYPE OPTION_DELETE			= 10005;
const EVENT_TYPE OPTION_STATISTICS		= 10006;
const EVENT_TYPE OPTION_EXIT			= 10007;

const EVENT_TYPE OPTION_COMMANDS		= 10008;
const EVENT_TYPE OPTION_ABOUT			= 10009;

FILE_EDITOR::FILE_EDITOR(char *_pathName) :
	UIW_WINDOW("FILE_EDITOR", defaultStorage)
{
	if (windowManager)
		windowManager->Center(this);
	strcpy(pathName, _pathName ? _pathName : "");

	// Create a directory service object.
	provider = new DIRECTORY_SERVICE(this);
}

FILE_EDITOR::~FILE_EDITOR(void)
{
	// Destory the directory service object.
	if (windowManager && windowManager->Index(provider) != -1)
		*windowManager - provider;
	delete provider;
}

EVENT_TYPE FILE_EDITOR::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case OPTION_SAVE:
		// Save the contents of the file editor text field.
		if (*pathName && Open("w+b"))
		{
			char *buffer;
			object = Get(TEXT_FIELD);
			object->Information(GET_TEXT, &buffer);
			Write(buffer, sizeof(char), strlen(buffer));
			Close();
		}
		else
		{
			provider->DeviceName(pathName);
			*windowManager + new FILE_ERROR(ccode, pathName);
		}
		break;

	case OPTION_NEW:
		// Clear the file editor.
		Information(SET_TEXT, "New File");
		strcpy(pathName, "");
		object = Get(TEXT_FIELD);
		object->Information(SET_TEXT, pathName);
		break;

	case OPTION_OPEN:
	case OPTION_SAVE_AS:
	case OPTION_DELETE:
		// If provider is not active (not in the window manager) then 
		// present the directory service object to the screen.
		if (windowManager->Index(provider) == -1)
		{
			provider->command = ccode;
			*windowManager + provider;
		}
		// Else the directory service has responded with a user request
		// to either open, save or delete a file.
		else
		{
			provider->PathName(pathName);
			// Try to open the file.
			if (ccode == OPTION_OPEN && Open("r+b"))
			{
				provider->DeviceName(pathName);
				Seek(0, SEEK_LAST);
				long length = Tell();
				if (length >= MAX_FILE_LENGTH)
				{
					// The file is larger than the application can handle.
					*windowManager + new FILE_ERROR(FILE_LENGTH_ERROR, pathName);
					strcpy(pathName, ""); // Reset the path-name.
					break;
				}
				*windowManager - provider;
				Seek(0, SEEK_FIRST);
				char *buffer = new char[(int)length+1];
				Read(buffer, sizeof(char), (int)length);
				buffer[(int)length] = NULL;
				Information(SET_TEXT, pathName);
				object = Get(TEXT_FIELD);
				object->Information(SET_TEXT, buffer);
				delete buffer;
				Close();
			}
			// Try saving the file.
			else if (ccode == OPTION_SAVE_AS && *pathName && Open("w+b"))
			{
				*windowManager - provider;
				provider->DeviceName(pathName);
				Information(SET_TEXT, pathName);
				Close();
				Event(OPTION_SAVE);
			}
			// Try deleting the file.
			else if (ccode == OPTION_DELETE && Open("r+b"))
			{
				*windowManager - provider;
				Close();
				Unlink();
			}
			// Report a file error.
			else
			{
				provider->DeviceName(pathName);
				*windowManager + new FILE_ERROR(ccode, pathName);
				strcpy(pathName, ""); // Reset the path-name.
			}
		}
		break;

	case OPTION_STATISTICS:
		// If provider is not active (not in the window manager) then 
		// present the directory service object to the screen.
		if (windowManager->Index(provider) == -1)
		{
			provider->command = ccode;
			*windowManager + provider;
		}
		// Else the directory service has responded with a user request
		// to either get a files statistics.
		else
		{
			// Try getting the file statistics.
			DEVICE_INFO info;
			if (!provider->DeviceInfo(NULL, info))
			{
				*windowManager - provider;
				FILE_STATISTICS *window = new FILE_STATISTICS(info);
				*windowManager + window;
			}
			// Report a file error.
			else
			{
				provider->DeviceName(pathName);
				*windowManager + new FILE_ERROR(ccode, pathName);
			}
		}
		break;

	case OPTION_EXIT:
		// Send a message to exit the application.
		eventManager->Put(L_EXIT);
		break;

	case OPTION_ABOUT:
	case OPTION_COMMANDS:
		// Display the appropriate help context.
		if (helpSystem)
		{
			UI_HELP_CONTEXT helpContext;
			if (ccode == OPTION_ABOUT)
				helpContext = HELP_FILE_ABOUT;
			else
				helpContext = HELP_FILE_COMMANDS;
			helpSystem->DisplayHelp(windowManager, helpContext);
		}
		break;

	default:
		// Let the base window process all other events.
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	return (ccode);
}

// ----- FILE_EDITOR support functions --------------------------------------

int FILE_EDITOR::Close(void)
{
	// Close the file.
	return (fclose(stream));
}

int FILE_EDITOR::Open(const char *mode)
{
	// Open the file.
	if (!pathName || !pathName[0])
		return (FALSE);
	stream = fopen(pathName, mode);
	return (stream ? TRUE : FALSE);
}

unsigned FILE_EDITOR::Read(void *buffer, unsigned size, unsigned count)
{
	// Read data from the file.
	return (fread((char *)buffer, size, count, stream));
}

int FILE_EDITOR::Seek(int offset, SEEK_LOCATION location)
{
	// Seek into the file.
	return (fseek(stream, offset, location));
}

long FILE_EDITOR::Tell(void)
{
	// Give the file offset location.
	return (ftell(stream));
}

int FILE_EDITOR::Unlink(void)
{
	// Delete the file.
	return (unlink(pathName));
}

unsigned FILE_EDITOR::Write(void *buffer, unsigned size, unsigned count)
{
	// Write data to the file.
	return (fwrite((char *)buffer, size, count, stream));
}

// ----- FILE_STATISTICS ----------------------------------------------------

const EVENT_TYPE OPTION_STATISTICS_HELP	= 10000;

FILE_STATISTICS::FILE_STATISTICS(DEVICE_INFO &info) :
	UIW_WINDOW("FILE_STATISTICS", defaultStorage)
{
	woAdvancedFlags |= WOAF_MODAL;
	if (windowManager)
		windowManager->Center(this);

	// Set the field information according to the supplied information.
	UI_WINDOW_OBJECT *object;
	object = Get(NAME_FIELD);
	object->Information(SET_TEXT, info.name);
	object = Get(SIZE_FIELD);
	object->Information(SET_VALUE, &info.size);
	object = Get(DATE_FIELD);
	object->Information(SET_VALUE, &info.date);
	object = Get(TIME_FIELD);
	object->Information(SET_VALUE, &info.time);
}

EVENT_TYPE FILE_STATISTICS::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case OPTION_STATISTICS_HELP:
		// Display the associated help information.
		if (helpSystem)
			helpSystem->DisplayHelp(windowManager, helpContext);
		break;

	default:
		// Let the base window process all other events.
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	return (ccode);
}

// ----- FILE_ERROR ---------------------------------------------------------

FILE_ERROR::FILE_ERROR(EVENT_TYPE errorType, const char *fileName) :
	UIW_WINDOW("FILE_ERROR", defaultStorage)
{
	woAdvancedFlags |= WOAF_MODAL;
	if (windowManager)
	{
		windowManager->Center(this);
		windowManager->errorSystem->Beep();
	}

	// Determine the type of error.
	static UI_ITEM _errorTable[] =
	{
		{ FILE_OPEN_ERROR, NULL, "The file %s could not be found.", 0 },
		{ FILE_LENGTH_ERROR, NULL, "This application can only read files smaller than 30,000 bytes.", 0 },

		{ OPTION_OPEN, NULL, "The file %s could not be opened.", 0 },
		{ OPTION_SAVE, NULL, "No filename has been specified.", 0 },
		{ OPTION_SAVE_AS, NULL, "The file %s could not be saved.", 0 },
		{ OPTION_DELETE, NULL, "The file %s could not be deleted.", 0 },
		{ OPTION_STATISTICS, NULL, "The file %s could not be found.", 0 },

		{ 0, NULL, NULL, 0 }
	};
	for (int i = 0; _errorTable[i].value; i++)
		if (_errorTable[i].value == errorType)
		{
			char errorMessage[256];
			UI_WINDOW_OBJECT *object = Get(ERROR_TEXT);
			sprintf(errorMessage, _errorTable[i].text, fileName ? fileName : "");
			object->Information(SET_TEXT, errorMessage);
			break;
		}
}
